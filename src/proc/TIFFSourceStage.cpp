/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/
#include <core/AbstractWindowModel.h>
#include <proc/TIFFSourceStage.h>
#include <proc/ProcessStage.h>
#include <proc/FinalStage.h>

#include <Array.h>

#include <QDateTime>
#include <QDebug>

using gstar::Array;

using boost::mutex;
using boost::unique_lock;
using boost::shared_ptr;
using boost::shared_array;
using boost::dynamic_pointer_cast;
using boost::const_pointer_cast;


#include <string>
#include <queue>
#include <stdio.h>

/*---------------------------------------------------------------------------*/

TIFFSourceStage::TIFFSourceStage(AbstractWindowModel* tiffModel)
{

   m_tiffModel = tiffModel;

   // TODO input dimension of the image externally
   // For now we assume that dims[1] = height dims[2] = width
   //    and dims[0] = #of images.

   m_imageDimIndex = 0;
   m_heightDimIndex = 1;
   m_widthDimIndex  = 2;

   m_imageCount  = m_tiffModel->getImageDims(m_imageDimIndex);
   m_imageHeight = m_tiffModel->getImageDims(m_heightDimIndex);
   m_imageWidth  = m_tiffModel->getImageDims(m_widthDimIndex);


   int pixelByteSize = m_tiffModel->getPixelByteSize() / 8;
   Array::DataType arrayType = Array::INVALID;

   switch (pixelByteSize)
   {
   case 1:
      m_imageLenBytes = m_imageWidth * m_imageHeight;
      arrayType = Array::UINT8;
      break;
   case 2:
      arrayType = Array::UINT16;
      m_imageLenBytes = m_imageWidth * m_imageHeight * 2;
      break;
   case 4:
      arrayType = Array::BGRA32;
      m_imageLenBytes = m_imageWidth * m_imageHeight * 4;
      break;
   }

   if (arrayType == Array::INVALID)
   {
      throw std::string(
        "Image data type doesn't match. Only 8, 16, and 32 bit images are supported!");
   }

   m_data = new Array(m_imageLenBytes);
   m_data->setRank(2);
   m_data->setType(arrayType);
   unsigned long long dims2[2];
   dims2[0] = m_imageHeight;
   dims2[1] = m_imageWidth;
   m_data->setDimensions(dims2, 2);

//   m_currSelection = m_tiffModel->getImageSelection();
//   m_fullSelection = m_currSelection;


}

/*---------------------------------------------------------------------------*/

TIFFSourceStage::~TIFFSourceStage()
{

   if (m_data != NULL)
   {
      delete m_data;
      m_data = NULL;
   }

}
/*---------------------------------------------------------------------------*/

void TIFFSourceStage::computeImageDims()
{

   m_imageWidth = m_tiffModel->getImageDims(m_widthDimIndex);
   m_imageHeight = m_tiffModel->getImageDims(m_heightDimIndex);
   m_imageCount = m_tiffModel->getImageDims(m_imageDimIndex);

   // Check the array type to allocate the buffer it needs.
   Array::DataType arrayType = m_data->getType();

   switch (arrayType)
   {
   case Array::UINT8:
      m_imageLenBytes = m_imageWidth * m_imageHeight;
      break;
   case Array::UINT16:
      m_imageLenBytes = m_imageWidth * m_imageHeight*2;
      break;
   case Array::BGRA32:
      m_imageLenBytes = m_imageWidth * m_imageHeight*4;
      break;
   default:
      break;
   }

   if (m_imageIndex > m_imageCount)
      m_imageIndex = 1;

   //Re-allocate memory if image size increase due to change in dimension.
   // TODO: If user accidently pick the image index dimension and it is huge
   // we might end up too much memory.

   if (m_data->getBytes() < m_imageLenBytes)
   {
      delete m_data;
      m_data = new Array(m_imageLenBytes);
      m_data->setRank(2);
      // We don't expect the type of change beyond construction.
      m_data->setType(arrayType);
   }

   unsigned long long dims2[2];
   dims2[0] = m_imageHeight;
   dims2[1] = m_imageWidth;
   m_data->setDimensions(dims2, 2);

}

/*---------------------------------------------------------------------------*/

void TIFFSourceStage::execute()
{

   //TODO: We might not need to re-compute dimensions all the time.
   computeImageDims();

   if (m_imageIndex > m_imageCount) return;
/*
   shared_array<char> arr = m_tiffModel->getReorderedImageData(m_imageIndex,
                                                              m_imageDimIndex,
                                                              m_heightDimIndex,
                                                              m_widthDimIndex);
*/
   uchar* dst = m_data->getBuffer();

   uchar* src = m_tiffModel->getBytes();

   memcpy(dst, src, m_imageLenBytes);

   // TODO: Refactor next stage code to a parent class.
   // Pass the data to next-stage if any
   Stage* next = this->getNextStage();
   if (next == NULL)
      return;

   int t = next->getStageType();

   switch (t)
   {
   case Stage::STAGE_PROCESS:
   {
      ProcessStage* pstage = dynamic_cast<ProcessStage*> (next);
      if (pstage != NULL)
      {
         pstage->setInputBuffer(m_data);
         pstage->execute();
      }

      break;
   }

   case Stage::STAGE_FINAL:
   {
      FinalStage* fstage = (FinalStage*) next;
      fstage->setInputBuffer(m_data);
      fstage->execute();
      break;
   }
   default:
      break;
   }

}

/*---------------------------------------------------------------------------*/
unsigned long TIFFSourceStage::getCurrentImageIndex()
{

   return m_imageIndex;

}

/*---------------------------------------------------------------------------*/

int TIFFSourceStage::getHeightDimension()
{

   return m_heightDimIndex;

}

/*---------------------------------------------------------------------------*/

int TIFFSourceStage::getWidthDimension()
{

   return m_widthDimIndex;

}

/*---------------------------------------------------------------------------*/

void TIFFSourceStage::run()
{

   m_imageIndex = m_imageCount;

   execute();

   while (true)
   {
      // Check for interruption point
      try
      {
         //boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
         interuptionPoint();

      } catch (const boost::thread_interrupted& e)
      {
         break;
      }

       //TODO: We might need the code below, when trying to load the
       //image using the thread.
      {
         boost::unique_lock < boost::mutex > lock(m_mutex);
         m_newImageIndex.wait(lock);

         {
            boost::this_thread::disable_interruption di;
            execute();
         }

      }
   }

}

/*---------------------------------------------------------------------------*/

void TIFFSourceStage::setHeightDimension(int h)
{

   {
      boost::unique_lock<mutex> lock(m_mutex);

      m_heightDimIndex = h;

      if (m_heightDimIndex == m_widthDimIndex)
      {
         m_widthDimIndex = 3 - (m_heightDimIndex + m_imageDimIndex);
      }

      m_imageDimIndex = 3 - (m_heightDimIndex + m_widthDimIndex);
   }

   m_newImageIndex.notify_one();

}

/*---------------------------------------------------------------------------*/

void TIFFSourceStage::setImageIndex(int index)
{

   {
      boost::unique_lock<mutex> lock(m_mutex);

      if ((unsigned long)index > m_imageCount) return;

      m_imageIndex = index;

   }

   m_newImageIndex.notify_one();

}

/*---------------------------------------------------------------------------*/

void TIFFSourceStage::setWidthDimension(int w)
{

   {
      boost::unique_lock<mutex> lock(m_mutex);

      m_widthDimIndex = w;

      if (m_heightDimIndex == m_widthDimIndex)
      {
         m_heightDimIndex = 3 - (m_widthDimIndex + m_imageDimIndex);
      }

      m_imageDimIndex = 3 - (m_heightDimIndex + m_widthDimIndex);
   }

   m_newImageIndex.notify_one();

}


/*---------------------------------------------------------------------------*/

void TIFFSourceStage::update()
{
  
   boost::unique_lock<mutex> lock(m_mutex);

   m_newImageIndex.notify_one();

}

/*---------------------------------------------------------------------------*/

