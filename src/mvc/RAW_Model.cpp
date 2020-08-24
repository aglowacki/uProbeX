/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/RAW_Model.h>
#include "core/defines.h"
#include "io/file/hl_file_io.h"

#define DEFAULT_NUM_DETECTORXS 4

/*----------------src/mvc/RAW_Model.cpp \-----------------------------------------------------------*/

RAW_Model::RAW_Model() : QObject()
{

    _path = "";
    _filepath = "";

}

/*---------------------------------------------------------------------------*/

RAW_Model::~RAW_Model()
{

}

/*---------------------------------------------------------------------------*/

bool RAW_Model::load(QString directory, QString filename)
{
    try
    {
        _path = directory;
        _filepath = directory + "/" + filename;
        
        //std::chrono::time_point<std::chrono::system_clock> start, end;
        //start = std::chrono::system_clock::now();
        logW<<" RAW_Model loading "<< _filepath.toStdString() << "\n";

		if (filename.endsWith(".mda"))
		{
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
					data_struct::Spectra *integrated_spectra = _mda_io.gen_integrated_spectra(i);
					io::load_and_integrate_spectra_volume(directory.toStdString(), filename.toStdString(), i, integrated_spectra, nullptr);
				}
			}

			data_struct::Scan_Info *scan_info = _mda_io.get_scan_info();
			if (scan_info != nullptr)
			{
				_cols = scan_info->meta_info.requested_cols;
				_rows = scan_info->meta_info.requested_rows;
			}
		}
		if (filename.endsWith(".h5"))
		{
			data_struct::Spectra* integrated_spectra = _mda_io.gen_integrated_spectra(0);
			io::load_and_integrate_spectra_volume(directory.toStdString(), filename.toStdString(), 0, integrated_spectra, nullptr);
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

data_struct::Params_Override* RAW_Model::getParamOverride(int idx)
{
    if (_fit_params_override_dict.count(idx) > 0)
    {
        return _fit_params_override_dict.at(idx);
    }
    return nullptr;
}

/*---------------------------------------------------------------------------*/

unsigned int RAW_Model::getNumIntegratedSpectra()
{
	return _mda_io.get_num_integreated_spectra(); 
}

/*---------------------------------------------------------------------------*/

data_struct::ArrayXr* RAW_Model::getIntegratedSpectra(unsigned int det) 
{
	return _mda_io.get_integrated_spectra(det); 
}

/*---------------------------------------------------------------------------*/

data_struct::Scan_Info* RAW_Model::getScanInfo()
{
	return _mda_io.get_scan_info(); 
}

/*---------------------------------------------------------------------------*/