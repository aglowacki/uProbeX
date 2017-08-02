/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsModel.h>
#include <QDebug>

/*----------------src/mvc/MapsModel.cpp \-----------------------------------------------------------*/

MapsModel::MapsModel()
{

    _is_loaded = false;

}

/*---------------------------------------------------------------------------*/

MapsModel::~MapsModel()
{

}

/*---------------------------------------------------------------------------*/

bool MapsModel::load(QString filepath)
{
    try
    {
        //io::file::hdf5_io::inst()->load_analyzed(
    }
    catch (std::string& s)
    {
        throw s;
    }
    catch (...)
    {
        throw std::string("Failed to open Maps Analyzed dataset!");
    }

    return _is_loaded;
}
