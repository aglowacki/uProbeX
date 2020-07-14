/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MDA_Model.h>
#include "core/defines.h"
#include "io/file/hl_file_io.h"

#define DEFAULT_NUM_DETECTORXS 4

/*----------------src/mvc/MDA_Model.cpp \-----------------------------------------------------------*/

MDA_Model::MDA_Model() : QObject()
{

    _path = "";
    _filepath = "";

}

/*---------------------------------------------------------------------------*/

MDA_Model::~MDA_Model()
{

}

/*---------------------------------------------------------------------------*/

bool MDA_Model::load(QString directory, QString filename)
{
    try
    {
        _path = directory;
        _filepath = directory + "/" + filename;
        
        //std::chrono::time_point<std::chrono::system_clock> start, end;
        //start = std::chrono::system_clock::now();
        logW<<" MDA_Model loading "<< _filepath.toStdString() << "\n";
        if (_mda_io.load_scalers(_filepath.toStdString()) == false)
        {
            return false;
        }
        if (_mda_io.get_num_integreated_spectra() == 0) // probably fly scan
        {
            if (directory.endsWith("\\mda") || directory.endsWith("/mda"))
            {
                directory = directory.left(directory.length() - 3);
            }
            
            for (int i = 0; i < DEFAULT_NUM_DETECTORXS; i++)
            {
                data_struct::Spectra integrated_spectra;

                if (io::load_and_integrate_spectra_volume(directory.toStdString(), filename.toStdString(), i, &integrated_spectra, nullptr))
                {
                    _mda_io.append_integrated_spectra(i, (data_struct::ArrayXr*)&integrated_spectra);
                }
            }
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