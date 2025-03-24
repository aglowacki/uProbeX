/*-----------------------------------------------------------------------------
 * Copyright (c) 2025, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef H5_IMAGE_MODEL_H
#define H5_IMAGE_MODEL_H

//---------------------------------------------------------------------------

#include <QString>
#include <QObject>
#include <QImage>
#include <QDir>
#include <QPixmap>
#include <hdf5.h>
#include <unordered_map>
#include <vector>
#include <mutex>
#include "core/defines.h"
//#include "data_struct/fit_parameters.h"

//---------------------------------------------------------------------------

const std::string STR_DATA_PATH = "/entry/data/data";
const std::string STR_MOTOR_X_PATH = "/entry/instrument/NDAttributes/MotorX";
const std::string STR_MOTOR_Y_PATH = "/entry/instrument/NDAttributes/MotorY";

/**
 * @brief Model for area detector image hdf5 files
 */
class H5ImageModel : public QObject
{

    Q_OBJECT

public:

    /**
    * Constructor
    */
    H5ImageModel();

    /**
    * Destructor
    */
    ~H5ImageModel();

    bool load(QString filepath);

    QPixmap gen_pixmap();

private:

    QString _filepath;

    std::vector<unsigned char> _data;
    hsize_t _data_dims_in[3];
    //data_struct::ArrayXXr<int> _image_data;
    float _x_axis;
    float _y_axis;

};


//---------------------------------------------------------------------------

#endif /* H5ImageModel_H_ */

//---------------------------------------------------------------------------
