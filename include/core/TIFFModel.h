/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef TIFF_MODEL_H
#define TIFF_MODEL_H

/*---------------------------------------------------------------------------*/

#include <core/AbstractWindowModel.h>

#include <QString>
#include <QImageReader>

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_array.hpp>

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
    TIFFModel(QString filepath, std::string datapath);

    /**
    * Destructor
    */
    ~TIFFModel();

    /**
    * @brief getPixelByteSize
    * @return
    */
    virtual int getPixelByteSize();

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
    * @brief  Get total number of images in the data-set.
    */
    virtual int getNumberOfImages();

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

    virtual uchar* getBytes() { return _img.bits(); }

private:

    /**
    * @brief Compute the image dims
    */
    void computeImageDims();

    QImage _img;

    /**
    * @brief HDF5 opened by the playback window.
    */
    QString m_pathtiffFile;

};


/*---------------------------------------------------------------------------*/

#endif /* TIFFMODEL_H_ */

/*---------------------------------------------------------------------------*/
