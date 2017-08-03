/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsWorkspaceModel.h>
#include <QDebug>

/*----------------src/mvc/MapsWorkspaceModel.cpp \-----------------------------------------------------------*/

MapsWorkspaceModel::MapsWorkspaceModel()
{

    _is_loaded = false;

}

/*---------------------------------------------------------------------------*/

MapsWorkspaceModel::~MapsWorkspaceModel()
{

}

/*---------------------------------------------------------------------------*/

bool MapsWorkspaceModel::load(QString filepath)
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
