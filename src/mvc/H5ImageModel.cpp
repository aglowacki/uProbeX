/*-----------------------------------------------------------------------------
 * Copyright (c) 2025, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/H5ImageModel.h>

/*----------------src/mvc/H5ImageModel.cpp \-----------------------------------------------------------*/

H5ImageModel::H5ImageModel() : QObject()
{

    _filepath = "";

}

//---------------------------------------------------------------------------

H5ImageModel::~H5ImageModel()
{

}

//---------------------------------------------------------------------------

bool H5ImageModel::load(QString filepath)
{
    _filepath = filepath;
    hid_t file_id = H5Fopen(filepath.toStdString().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if(file_id < 0)
    {
        logE<<"Error opening file "<<filepath.toStdString() << "\n";
        return false;
    }

    hid_t data_id = H5Dopen(file_id, STR_DATA_PATH.c_str(), H5P_DEFAULT);
    if(data_id < 0)
    {
        logE<<"Error opening path "<<STR_DATA_PATH << "\n";
        H5Fclose(file_id);
        return false;
    }

    hid_t x_id = H5Dopen(file_id, STR_MOTOR_X_PATH.c_str(), H5P_DEFAULT);
    if(x_id < 0)
    {
        logE<<"Error opening path "<<STR_MOTOR_X_PATH << "\n";
        H5Dclose(data_id);
        H5Fclose(file_id);
    }
    
    hid_t y_id = H5Dopen(file_id, STR_MOTOR_Y_PATH.c_str(), H5P_DEFAULT);
    if(y_id < 0)
    {
        logE<<"Error opening path "<<STR_MOTOR_Y_PATH << "\n";
        H5Dclose(x_id);
        H5Dclose(data_id);
        H5Fclose(file_id);
    }


    hid_t data_space_id = H5Dget_space(data_id);
    hid_t x_space_id = H5Dget_space(x_id);
    hid_t y_space_id = H5Dget_space(y_id);
    
    
    hsize_t max_size = 3;
    int status_n = H5Sget_simple_extent_dims(data_space_id, &_data_dims_in[0], &max_size);
    if (status_n > -1 )
    { 
        _data.resize(_data_dims_in[1] * _data_dims_in[2]);
        hid_t error = H5Dread(data_id, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, (void*)&_data[0]);    
          
        if(error > 0)
        {
            logW<<"Could not read image\n";
        }
    }
    else
    {
        logW<<"Could not read image dims\n";
    }

    hid_t error = H5Dread(x_id, H5T_NATIVE_FLOAT, x_space_id, x_space_id, H5P_DEFAULT, &_x_axis);
    if (error > 0)
    {
        logW << "Could not load x_axis\n";
    }
    error = H5Dread(y_id, H5T_NATIVE_FLOAT, y_space_id, y_space_id, H5P_DEFAULT, &_y_axis);
    if (error > 0)
    {
        logW << "Could not load y_axis\n";
    }
    

    H5Sclose(data_space_id);
    H5Sclose(x_space_id);
    H5Sclose(y_space_id);
    H5Dclose(x_id);
    H5Dclose(y_id);
    H5Dclose(data_id);
    H5Fclose(file_id);

    return true;
}

//---------------------------------------------------------------------------

QPixmap H5ImageModel::gen_pixmap()
{
    QImage image(_data.data(), _data_dims_in[2], _data_dims_in[1], QImage::Format_Grayscale8);
    return QPixmap::fromImage(image.convertToFormat(QImage::Format_RGB32));
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------