/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ABSTRACT_WINDOW_MODEL
#define ABSTRACT_WINDOW_MODEL

/*---------------------------------------------------------------------------*/

#include <QList>
#include <QImage>

/*---------------------------------------------------------------------------*/

/**
 * @brief abstract window model for the CTF, histogram.
 */
class AbstractWindowModel
{

public:

   /**
    * Constructor
    */
   AbstractWindowModel();

   /**
    * Destructor
    */
   ~AbstractWindowModel();

   /**
    * Get the list of the points of the histogram
    */
   QList<int> getHistogram();

   /**
    * Get the data type from the image read in
    */
   virtual int getPixelByteSize() = 0;

   /**
    * @brief getNumberOfImages
    * @return
    */
   virtual int getNumberOfImages() = 0;

   /**
    * @brief getRank
    * @return
    */
   virtual int getRank() = 0;

   /**
    * @brief getDataPath
    * @return
    */
   virtual QString getDataPath() = 0;

   /**
    * @brief getImageDims
    * @param imageDim
    * @return
    */
   virtual int getImageDims(int imageDim) = 0;

   virtual uchar* getBytes() = 0;

   /**
    * Set the list of the points of the histogram
    */
   void setHistogram(QList<int> hist);

   /**
    * @brief getImage
    * @return
    */
   virtual QImage* getImage() = 0;

protected:

   /**
    * List of points for the histogram
    */
   QList<int> m_histogram;

};

/*---------------------------------------------------------------------------*/

#endif /* ABSTRACTWINDOWMODEL_H_ */

/*---------------------------------------------------------------------------*/
