/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef TIFF_MODEL_H
#define TIFF_MODEL_H

/*---------------------------------------------------------------------------*/

#include <mvc/VLM_Model.h>

#include <QString>
#include <QImageReader>
#include "data_struct/fit_parameters.h"

/*---------------------------------------------------------------------------*/

/**
 * @brief Model to manipulate the image path behavior, such as get
 * and set path.
 */
class TIFF_Model :  public VLM_Model
{

public:

    /**
    * Constructor
    */
    TIFF_Model();

    /**
    * Destructor
    */
    ~TIFF_Model();

    virtual bool load(QString filepath);

    virtual bool loaded();
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
     * @brief getBytes
     * @return
     */
    virtual uchar* getBytes() { return _img.bits(); }

    /**
     * @brief getImage
     * @return
     */
    virtual QImage* getImage() { return &_img; }

protected:

    virtual void _initializeCoordModel();

private:

    /**
    * @brief Compute the image dims
    */
    void computeImageDims();

    QImage _img;

    // if tiff is 32bit float
    data_struct::ArrayXXr<float> _pixel_values;

    bool _loaded;

    /**
    * @brief HDF5 opened by the playback window.
    */
    QString m_pathtiffFile;

};


/*---------------------------------------------------------------------------*/

#endif /* TIFF_Model_H_ */

/*---------------------------------------------------------------------------*/
