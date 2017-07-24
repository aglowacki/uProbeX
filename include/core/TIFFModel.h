/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef TIFF_MODEL_H
#define TIFF_MODEL_H

/*---------------------------------------------------------------------------*/

#include <core/AbstractWindowModel.h>

#include <QString>

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_array.hpp>

#include <DStar.h>

//GStar
#include <Array.h>

/*---------------------------------------------------------------------------*/

/**
 * @brief Model to manipulate the image path behavior, such as get
 * and set path.
 */
class TIFFModel
:  public AbstractWindowModel
{

public:

   /**
    * Constructor
    */
   TIFFModel(QString filepath, std::string datapath="/exchange/data");

   /**
    * Destructor
    */
   ~TIFFModel();

   /**
    * @brief  Get data type
    */
   virtual int getDataType();

   /**
    * @brief  Get tiff path
    */
   QString getDataPath();

   /**
    * @brief  Get image dims
    * @param imageIndex
    */
   virtual int getImageDims(int imageIndex);

   /**
    * @brief
    */
   dstar::Selection getImageSelection();

   /**
    * @brief  Get total number of images in the data-set.
    */
   virtual int getNumberOfImages();

   /**
    * @brief Get total number of images in the output buffer.
    */
   gstar::Array* getOutputBuffer();

   /**
    * @brief Get the total number of dimensions in the dataset.
    */
   virtual int getRank();

   /**
    * @brief Get recorded image data
    * @param imageNum
    * @param imageIndex
    * @param heightIndex
    * @param widthIndex
    */
   virtual boost::shared_array<char> getReorderedImageData(int imageNum,
      int imageIndex,
      int heightIndex,
      int widthIndex);

private:

   /**
    * @brief Compute the image dims
    */
   void computeImageDims();

   /**
    * @brief Read tiff data to properties
    * @param group
    * @param properties
    */
   void readTIFFDataToProperties(
            boost::shared_ptr<const dstar::AbstractData> group,
            std::vector<std::pair<std::string, std::string> > &properties);

   /**
    * @brief Search tiff group for properties
    * @param group
    * @param properties
    */
   void searchTIFFGroupsForProperties(
            boost::shared_ptr<const dstar::AbstractGroup> group,
            std::vector<std::pair<std::string, std::string> > &properties);

   /**
    * @brief Search tiff path
    * @param group
    * @param search
    */
   boost::shared_ptr<const dstar::AbstractObject> searchTIFFPath(
      boost::shared_ptr<dstar::AbstractGroup> group, std::string search);

   /**
    * @brief HDF5 opened by the playback window.
    */
   QString m_pathtiffFile;

   /**
    * @brief HDF5 object
    */
   boost::shared_ptr<dstar::TIFFFile> m_tiffFile;

   /**
    * @brief Hdf data.
    */
   boost::shared_ptr<dstar::TIFFData> m_tiffData;

   /**
    * @brief Data.
    */
   gstar::Array* m_data;

   /**
    * @brief Total number of images in the HDF5 dataset.
    */
   unsigned long m_imageCount;

   /**
    * @brief Index in HDF5 dataset for the current image.
    */
   unsigned long m_imageIndex;

   /**
    * @brief Image width;
    */
   unsigned long m_imageWidth;

   /**
    * @brief Image height
    */
   unsigned long m_imageHeight;

   /**
    * @brief Length of image in total number of bytes.
    */
   unsigned long m_imageLenBytes;

   /**
    * @brief Full selection.
    */
   dstar::Selection m_fullSelection;

   /**
    * @brief Current selection.
    */
   dstar::Selection m_currSelection;

   /**
    * @brief Rank.
    */
   unsigned long m_rank;

   /**
    * @brief Mutex.
    */
   boost::mutex m_mutex;

   /**
    * @brief The dimension information for hdf dataset.
    */
   unsigned long long* m_dims;

};


/*---------------------------------------------------------------------------*/

#endif /* TIFFMODEL_H_ */

/*---------------------------------------------------------------------------*/
