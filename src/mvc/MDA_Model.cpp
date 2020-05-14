/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MDA_Model.h>
#include "core/defines.h"

/*----------------src/mvc/MDA_Model.cpp \-----------------------------------------------------------*/

MDA_Model::MDA_Model() : QObject()
{

    _filepath = "";

}

/*---------------------------------------------------------------------------*/

MDA_Model::~MDA_Model()
{

}

/*---------------------------------------------------------------------------*/

bool MDA_Model::load(QString filepath)
{
    try
    {
        _filepath = filepath;
        //_is_loaded = ERROR_LOADING;
        //std::chrono::time_point<std::chrono::system_clock> start, end;
        //start = std::chrono::system_clock::now();
        logW<<" MDA_Model loading "<< filepath.toStdString() << "\n";
        //_integrated_spectra = _mda_io.load_integrated_spectra(_filepath.toStdString());
    }
    catch (std::string& s)
    {
        throw s;
    }
    catch (...)
    {
        throw std::string("Failed to open Maps Analyzed dataset!");
    }

    return true;
}

/*---------------------------------------------------------------------------*/

