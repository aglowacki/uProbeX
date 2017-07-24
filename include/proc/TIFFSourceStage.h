/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef TIFF_SOURCE_STAGE_H
#define TIFF_SOURCE_STAGE_H

/*---------------------------------------------------------------------------*/

#include <proc/Thread.h>
#include <proc/SourceStage.h>

#include <QObject>

#include <boost/shared_ptr.hpp>
#include <boost/thread/condition_variable.hpp>

class AbstractWindowModel;

namespace gstar 
{
    class Array;
}

/*---------------------------------------------------------------------------*/

/**
 * @brief Source stage for processing image, where additional information
 * could be got, such as dimension , number of images.
 */
class TIFFSourceStage
: public QObject, public SourceStage, public Thread
{

   Q_OBJECT

public:
   /**
    * Constructor.
    */
   TIFFSourceStage(AbstractWindowModel* tiffModel);

   /**
    * Destructor.
    */
   ~TIFFSourceStage();

   /**
    * Execute.
    */
   void execute();

   /**
    * Get the index of current image loaded by the pipeline
    *
    * @return image number.
    */
   unsigned long getCurrentImageIndex();


   /**
    * Get the dimension index for image height.
    */
   int getHeightDimension();

   /**
    * Get the dimension index for image width.
    */
   int getWidthDimension();

  /**
    * Run.
    */
   void run();

   /**
    * Set height dimension index.
    */
   void setHeightDimension(int h);

   /**
    * Set the current image index to be loaded by the pipeline.
    */
   void setImageIndex(int index);

   /**
    * Set width dimension index.
    */
   void setWidthDimension(int w);

   /**
    * Force an update of the image data through the pipe-line 
    */
   void update();

private:
   /**
    * Compute the image dimension.
    */
   void computeImageDims();

private:

   /**
    * File path.
    */
   std::string m_filePath;

   /**
    * Data.
    */
   gstar::Array* m_data;

   /**
    * @brief m_tiffModel
    */
   AbstractWindowModel* m_tiffModel;

   /**
    * Total number of images in the dataset.
    */
   unsigned long m_imageCount;

   /**
    * Index in dataset for the current image.
    */
   unsigned long m_imageIndex;

   /**
    * Image width;
    */
   unsigned long m_imageWidth;

   /**
    * Image height
    */
   unsigned long m_imageHeight;

   /**
    * Length of image in total number of bytes.
    */
   unsigned long m_imageLenBytes;

    /*
    * New image index.
    */
   boost::condition_variable m_newImageIndex;

   /**
    * Mutex.
    */
   boost::mutex m_mutex;

   /**
    * The dimension information for dataset.
    */
//   unsigned long long* m_dims;

   /**
    * Image dimension index.
    */
   int m_imageDimIndex;

   /**
    * Width dimension index.
    */
   int m_widthDimIndex;

   /**
    * Height dimension index.
    */
   int m_heightDimIndex;
};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
