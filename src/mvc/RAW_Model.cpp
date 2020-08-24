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
	_scan_info.meta_info.requested_cols = 1;
	_scan_info.meta_info.requested_rows = 1;

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
		io::file::MDA_IO mda_io;
        //std::chrono::time_point<std::chrono::system_clock> start, end;
        //start = std::chrono::system_clock::now();
        logW<<" RAW_Model loading "<< _filepath.toStdString() << "\n";

		if (filename.endsWith(".mda"))
		{
			if (mda_io.load_scalers(_filepath.toStdString()) == false)
			{
				return false;
			}
			if (mda_io.get_num_integreated_spectra() == 0) // probably fly scan
			{
				if (directory.endsWith("\\mda") || directory.endsWith("/mda"))
				{
					directory = directory.left(directory.length() - 3);
				}

				for (int i = 0; i < DEFAULT_NUM_DETECTORXS; i++)
				{
					io::load_and_integrate_spectra_volume(directory.toStdString(), filename.toStdString(), i, &(_integrated_spectra_map[i]), getParamOverrideOrAvg(i));
				}
			}
			else
			{
				// copy from mda_io to this model
				for (int i = 0; i < mda_io.get_num_integreated_spectra(); i++)
				{
					_integrated_spectra_map[i] = (data_struct::Spectra)*(mda_io.get_integrated_spectra(i));
				}
				mda_io.unload_int_spectra();
			}

			_scan_info = *(mda_io.get_scan_info());
		}
		if (filename.endsWith(".h5"))
		{
			if (io::get_scalers_and_metadata_h5(directory.toStdString(), filename.toStdString(), &_scan_info))
			{
				// todo: update scaler names from override file

				for (auto &i : _scan_info.meta_info.detectors)
				{
					io::load_and_integrate_spectra_volume(directory.toStdString(), filename.toStdString(), i, &(_integrated_spectra_map[i]), getParamOverrideOrAvg(i));
				}
			}
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

data_struct::Params_Override* RAW_Model::getParamOverrideOrAvg(int idx)
{
	if (_fit_params_override_dict.count(idx) > 0)
	{
		return _fit_params_override_dict.at(idx);
	}
	else
	{
		if (_fit_params_override_dict.count(-1) > 0)
		{
			return _fit_params_override_dict.at(-1);
		}
	}
	return nullptr;
}

/*---------------------------------------------------------------------------*/

unsigned int RAW_Model::getNumIntegratedSpectra()
{
	return _integrated_spectra_map.size();
}

/*---------------------------------------------------------------------------*/

std::vector<unsigned int> RAW_Model::getDetectorKeys()
{

	std::vector<unsigned int> keys;
	for (auto& itr : _integrated_spectra_map)
	{
		keys.push_back(itr.first);
	}
	return keys;

}

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

data_struct::Spectra* RAW_Model::getIntegratedSpectra(unsigned int det) 
{
	if (_integrated_spectra_map.count(det) > 0)
	{
		return &(_integrated_spectra_map.at(det));
	}
	return nullptr;
}

/*---------------------------------------------------------------------------*/

data_struct::Scan_Info* RAW_Model::getScanInfo()
{
	return &_scan_info;
}

/*---------------------------------------------------------------------------*/