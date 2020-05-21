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
        
        //std::chrono::time_point<std::chrono::system_clock> start, end;
        //start = std::chrono::system_clock::now();
        logW<<" MDA_Model loading "<< filepath.toStdString() << "\n";
        if (_mda_io.load_scalers(filepath.toStdString()) == false)
        {
            return false;
        }
        data_struct::Scan_Info *scan_info = _mda_io.get_scan_info();
        if (scan_info != nullptr)
        {
            _cols = scan_info->meta_info.requested_cols;
            _rows = scan_info->meta_info.requested_rows;
        }
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

data_struct::Params_Override* MDA_Model::getParamOverride(int idx)
{
    if (_fit_params_override_dict.count(idx) > 0)
    {
        return _fit_params_override_dict.at(idx);
    }
    return nullptr;
}

/*---------------------------------------------------------------------------*/