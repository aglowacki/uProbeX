/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <proc/ColorLevelsProcessStage.h>
#include <proc/Histogram.h>
#include <proc/FinalStage.h>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <QDateTime>

#include <QDebug>
#include <QString>

using gstar::Array;

/*---------------------------------------------------------------------------*/

ColorLevelsProcessStage::ColorLevelsProcessStage()
: ProcessStage(), m_rangeMin(0), m_rangeMax(200), m_imageMin(0), m_imageMax(132)
{

   m_histogram = NULL;
   m_inputBuffer = NULL;
   m_outputBuffer = NULL;

}

/*---------------------------------------------------------------------------*/

ColorLevelsProcessStage::~ColorLevelsProcessStage()
{

   if (m_outputBuffer != NULL)
   {
      delete m_outputBuffer;
      m_outputBuffer = NULL;
   }

}

/*---------------------------------------------------------------------------*/

void ColorLevelsProcessStage::execute()
{

   if (m_inputBuffer == NULL)
      return;

   // Get rank
   unsigned long rank = m_inputBuffer->getRank();
   // Get type
   Array::DataType type = m_inputBuffer->getType();

   // Get dimensions
   unsigned long long dims[2];
   m_inputBuffer->getDims(dims, 2);

   // Check image properties; only process UINT8 and UINT16 images
   if (rank != 2) {
      return;
   }
   if (dims[0] <= 0 || dims[1] <= 0) {
      return;
   }
   if (type != Array::UINT8 && type != Array::UINT16) {
      return;
   }

   setRangeMinMax(type);
   setDataMinMax(m_inputBuffer);

   // Creating the output buffer
   unsigned long long size = dims[0] * dims[1] * 4;
   if (m_outputBuffer == NULL)
   {
      m_outputBuffer = new Array(size);

   }
   else
   {
      // Check if our existing buffer is big enough to hold the new image.
      if (m_outputBuffer->getBytes() < size) {
         delete m_outputBuffer;
         m_outputBuffer = new Array(size);
      }
   }

   // Set output buffer parameters
   m_outputBuffer->setRank(2);
   m_outputBuffer->setType(Array::BGRA32);
   m_outputBuffer->setDimensions(dims, 2);

   if (m_histogram != NULL)  m_histogram->reset();

   // Convert here
   uchar* inBuffer = m_inputBuffer->getBuffer();
   uchar* outBuffer = m_outputBuffer->getBuffer();

   getColorImage(m_ctf, inBuffer, outBuffer, dims[0], dims[1], type);

   // TODO: Refactor next stage code to a parent class.
   // pass the data to next-stage if any
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
         pstage->setInputBuffer(m_outputBuffer);
         pstage->execute();
      }

      break;
   }

   case Stage::STAGE_FINAL:
   {
      FinalStage* fstage = (FinalStage*) next;
      fstage->setInputBuffer(m_outputBuffer);
      fstage->execute();
      break;
   }
   default:
      break;
   }

}

/*---------------------------------------------------------------------------*/

void ColorLevelsProcessStage::getColorImage(ColorTransferFunction ctf,
                                            unsigned char* grayImage,
                                            unsigned char* clrImage,
                                            int width,
                                            int height,
                                            int type)
{

   if (clrImage == NULL || grayImage == NULL) return;

   double max= 0, min = 0;

   ctf.getTotalRange(&min, &max);

   int range = max - min + 1;

   double* redMap = ctf.getRedMap();
   double* greenMap = ctf.getGreenMap();
   double* blueMap = ctf.getBlueMap();

   if (redMap == NULL || blueMap == NULL || greenMap == NULL) return;

   for (unsigned int r = 0 ; r < (unsigned int)height ; r++)
   {
      for (unsigned int c = 0 ; c < (unsigned int)width ; c++)
      {
         int intensity = 0;
         int index = (r * width + c);
         if (type == Array::UINT8) intensity = ((uchar*)grayImage)[index];
         else if (type == Array::UINT16) intensity = ((ushort*)grayImage)[index];

         if (m_histogram != NULL) m_histogram->addIntensity(intensity);

         if (intensity < 0 || intensity >= range || intensity > 65536)
         {
            clrImage[(index*4) + 0] = 0;
            clrImage[(index*4) + 1] = 0;
            clrImage[(index*4) + 2] = 0;
            clrImage[(index*4) + 3] = 0;
         }
         else
         {
            clrImage[(index * 4) + 0] = blueMap[intensity];
            clrImage[(index * 4) + 1] = greenMap[intensity];
            clrImage[(index * 4) + 2] = redMap[intensity];
            clrImage[(index * 4) + 3] = 255;
         }
      }
   }

}

/*---------------------------------------------------------------------------*/

double ColorLevelsProcessStage::getImageMin()
{

   return m_imageMin;

}

/*---------------------------------------------------------------------------*/

double ColorLevelsProcessStage::getImageMax()
{

   return m_imageMax;

}

/*---------------------------------------------------------------------------*/

double ColorLevelsProcessStage::getRangeMin()
{

   return m_rangeMin;

}
/*---------------------------------------------------------------------------*/

int ColorLevelsProcessStage::getBufferIntegerValue(uchar * buffer,
                                                   long offset,
                                                   Array::DataType type)
{

   int value = 0;
   switch(type){
   case Array::UINT8:
      value = ((unsigned char *)buffer)[offset];
      break;
   case Array::UINT16:
      value = ((unsigned short *)buffer)[offset];
      break;
   default:
      break;
   }

   return value;

}

/*---------------------------------------------------------------------------*/

double ColorLevelsProcessStage::getRangeMax()
{

   return m_rangeMax;

}

/*---------------------------------------------------------------------------*/

Array* ColorLevelsProcessStage::getOutputBuffer()
{

   return m_outputBuffer;

}

/*---------------------------------------------------------------------------*/

void ColorLevelsProcessStage::setRangeMinMax(Array::DataType type)
{

   if (type == Array::UINT8)
   {
      m_rangeMin = 0.0;
      m_rangeMax = 255.0;
   }
   else if (type == Array::UINT16)
   {
      m_rangeMin = 0.0;
      m_rangeMax = 65535.0;
   }

}

/*---------------------------------------------------------------------------*/

void ColorLevelsProcessStage::setDataMinMax(Array* input)
{

   m_imageMax = m_rangeMin;
   m_imageMin = m_rangeMax;

   unsigned long rank = input->getRank();
   unsigned long long dims[2];
   input->getDims(dims, rank);

   uchar* inBuffer = input->getBuffer();
   for (unsigned int ii = 0; ii < dims[0]*dims[1]; ii++)
   {
      int val = getBufferIntegerValue(inBuffer, ii, input->getType());
      m_imageMin = (val < m_imageMin) ? val : m_imageMin;
      m_imageMax = (val > m_imageMax) ? val : m_imageMax;
   }

}

/*---------------------------------------------------------------------------*/

void ColorLevelsProcessStage::setColorTransferFunction(
   ColorTransferFunction ctf)
{

   this->m_ctf = ctf;

}

/*---------------------------------------------------------------------------*/

void ColorLevelsProcessStage::setHistogram(Histogram* hist)
{

   m_histogram = hist;

}

/*---------------------------------------------------------------------------*/

void ColorLevelsProcessStage::setInputBuffer(Array* buffer)
{

   m_inputBuffer = buffer;

}

/*---------------------------------------------------------------------------*/

void ColorLevelsProcessStage::update()
{
   
   if (getParentStage() != NULL)
   {
      getParentStage()->update();
   }

}

/*---------------------------------------------------------------------------*/
