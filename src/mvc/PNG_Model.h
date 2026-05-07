/*-----------------------------------------------------------------------------
 * Copyright (c) 2026, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PNG_MODEL_H
#define PNG_MODEL_H

//---------------------------------------------------------------------------

#include <mvc/VLM_Model.h>

#include <QString>
#include <QImageReader>
#include "data_struct/fit_parameters.h"

//---------------------------------------------------------------------------

/**
 * @brief Model to manipulate the image path behavior, such as get
 * and set path.
 */
class PNG_Model :  public VLM_Model
{

public:

    /**
    * Constructor
    */
    PNG_Model();

    /**
    * Destructor
    */
    ~PNG_Model();

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

    bool save_img(QString filename);

    void set_background_img(QImage img) {_img = img;}

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
    QString m_pathFile;

};


//---------------------------------------------------------------------------

#endif /* PNG_MODEL_H_ */

//---------------------------------------------------------------------------
