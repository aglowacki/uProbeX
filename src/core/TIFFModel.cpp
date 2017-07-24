/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <core/TIFFModel.h>

#include <string>
#include <queue>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

using boost::shared_array;
using boost::const_pointer_cast;
using boost::dynamic_pointer_cast;
using boost::lexical_cast;

using gstar::Array;

using dstar::Selection;
using dstar::TIFFFile;
using dstar::TIFFData;
using dstar::AbstractData;
using dstar::AbstractGroup;
using dstar::AbstractObject;

using std::vector;
using std::pair;
using std::string;

/*---------------------------------------------------------------------------*/

TIFFModel::TIFFModel(QString filepath, std::string datapath)
{

   Q_UNUSED(datapath);

   m_pathtiffFile = filepath;

   try
   {

      m_tiffData = boost::shared_ptr<TIFFData>(
               new TIFFData(m_pathtiffFile.toStdString()));
      m_rank = 3;

      int type = m_tiffData->getType();
      Array::DataType arrayType = Array::INVALID;

      m_rank = m_tiffData->getRank();

      if (m_rank < 3)
      {
         throw std::string("Number of dimension in image data are less than 3");
      }

      m_dims = new unsigned long long[m_rank];
      for (unsigned long d = 0 ; d < m_rank ; d++)
      {
         m_dims[d] = 0;
      }

      m_tiffData->getDims(m_dims, m_rank);

      if (m_dims[0] <= 0 || m_dims[1] <= 0 || m_dims <= 0)
      {
         throw std::string("The image data is not valid!");
      }

      // TODO input dimension of the image externally
      // For now we assume that dims[1] = height dims[2] = width
      //    and dims[0] = #of images.

      m_imageCount  = m_dims[0];
      m_imageHeight = m_dims[1];
      m_imageWidth  = m_dims[2];


      switch (type)
      {
      case dstar::Char:
         m_imageLenBytes = m_imageWidth * m_imageHeight;
         arrayType = Array::UINT8;
         break;
      case dstar::UnsignedChar:
         m_imageLenBytes = m_imageWidth * m_imageHeight;
         arrayType = Array::UINT8;
         break;
      case dstar::Short:
         arrayType = Array::UINT16;
         m_imageLenBytes = m_imageWidth * m_imageHeight * 2;
         break;
      case dstar::UnsignedShort:
         arrayType = Array::UINT16;
         m_imageLenBytes = m_imageWidth * m_imageHeight * 2;
         break;
      case dstar::Int:
      case dstar::UnsignedInt:
         arrayType = Array::BGRA32;
         m_imageLenBytes = m_imageWidth * m_imageHeight * 4;
         break;
      }

      if (arrayType == Array::INVALID)
      {
         throw std::string(
 "Image data type doesn't match. Only 8, 16, and 32 bit images are supported!");
      }

      if (m_tiffData->getTileWidth()!=0)
      {
         throw std::string(
           "Image type doesn't match. Only scrip based images are supported!");
      }

      m_data = new Array(m_imageLenBytes);
      m_data->setRank(2);
      m_data->setType(arrayType);
      unsigned long long dims2[2];
      dims2[0] = m_imageHeight;
      dims2[1] = m_imageWidth;
      m_data->setDimensions(dims2, 2);

      m_currSelection = m_tiffData->getSelection();
      m_fullSelection = m_currSelection;

   }
   catch (std::string& s)
   {
      throw s;
   }
   catch (...)
   {
      throw std::string("Failed to open TIFF file!");
   }

}

/*---------------------------------------------------------------------------*/

TIFFModel::~TIFFModel()
{

   if (m_data != NULL)
   {
      delete m_data;
      m_data = NULL;
   }

   if (m_dims != NULL) {
      delete [] m_dims;
      m_dims = NULL;
   }
}

/*---------------------------------------------------------------------------*/

int TIFFModel::getDataType()
{

   return m_tiffData->getType();

}

/*---------------------------------------------------------------------------*/

QString TIFFModel::getDataPath()
{

   return m_pathtiffFile;

}

/*---------------------------------------------------------------------------

TIFFPropertyModel* TIFFModel::getTIFFPropertyModel()
{
   std::vector< std::pair<std::string, std::string> > properties;

   boost::shared_ptr<const AbstractGroup> rootGroup =
            dynamic_pointer_cast<const AbstractGroup>(m_hdfFile);

   searchHDFGroupsForProperties(rootGroup, properties);
   HDF5PropertyModel* propModel = new HDF5PropertyModel(NULL);
   propModel->setProperties(properties);
   return propModel;
}*/

/*---------------------------------------------------------------------------*/

int TIFFModel::getImageDims(int imageIndex)
{

   return m_dims[imageIndex];

}

/*---------------------------------------------------------------------------*/

Selection TIFFModel::getImageSelection()
{

   return m_tiffData->getSelection();

}

/*---------------------------------------------------------------------------*/

int TIFFModel::getNumberOfImages()
{

   return m_imageCount;

}

/*---------------------------------------------------------------------------*/

gstar::Array* TIFFModel::getOutputBuffer()
{

   return m_data;

}

/*---------------------------------------------------------------------------*/

int TIFFModel::getRank()
{

   return m_rank;

}

/*---------------------------------------------------------------------------*/

shared_array<char> TIFFModel::getReorderedImageData(
         int imageNum,
         int imageIndex,
         int heightIndex,
         int widthIndex)
{

   // Select the starting index for the image index dimensions.
   m_currSelection.setDims(m_dims[imageIndex],
                           m_dims[heightIndex],
                           m_dims[widthIndex]);
   m_currSelection.setCount(1, m_dims[heightIndex], m_dims[widthIndex]);
   m_currSelection.setStart(imageNum-1, (unsigned long)imageIndex);
   m_currSelection.setStart((unsigned long long)0, (unsigned long)heightIndex);
   m_currSelection.setStart((unsigned long long)0, (unsigned long) widthIndex);

   m_tiffData->setSelection(m_currSelection);


   shared_array<char> arr = m_tiffData->getData();

   return arr;

}

/*---------------------------------------------------------------------------*/

