/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/RAW_Model.h>
#include "core/defines.h"
#include <QFileInfo>
#include "io/file/hl_file_io.h"
#include "io/file/mca_io.h"

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
	_path = directory;
	_filepath = directory + "/" + filename;
	return load(_filepath);
}

bool RAW_Model::load(QString filename)
{
    try
    {
		QFileInfo finfo = QFileInfo(filename);
        _filepath = filename;
		io::file::MDA_IO mda_io;
        //std::chrono::time_point<std::chrono::system_clock> start, end;
        //start = std::chrono::system_clock::now();
        logI<<" RAW_Model loading "<< _filepath.toStdString() << "\n";

		if (filename.endsWith(".mda"))
		{
			if (mda_io.load_scalers(_filepath.toStdString()) == false)
			{
				return false;
			}
			if (mda_io.get_num_integreated_spectra() == 0) // probably fly scan
			{
				if (_path.endsWith("\\mda") || _path.endsWith("/mda"))
				{
					_path = _path.left(_path.length() - 3);
				}

				for (int i = 0; i < DEFAULT_NUM_DETECTORXS; i++)
				{
					io::load_and_integrate_spectra_volume(_path.toStdString(), filename.toStdString(), i, &(_integrated_spectra_map[i]), getParamOverrideOrAvg(i));
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
		else if (filename.endsWith(".h5") || filename.endsWith(".hdf5") || filename.endsWith(".emd") )
		{
			if (io::get_scalers_and_metadata_h5(_path.toStdString(), filename.toStdString(), &_scan_info))
			{
				// todo: update scaler names from override file

				for (auto &i : _scan_info.meta_info.detectors)
				{
					io::load_and_integrate_spectra_volume(_path.toStdString(), filename.toStdString(), i, &(_integrated_spectra_map[i]), getParamOverrideOrAvg(i));
				}
			}
		}
		else if (filename.endsWith(".csv"))
		{
			if (false == io::file::csv::load_raw_spectra(_filepath.toStdString(), _csv_data))
			{
				return false;
			}
			if (_csv_data.count("Spectrum") > 0)
			{
				_integrated_spectra_map[-1] = _csv_data["Spectrum"];
			}
		}
		else if (finfo.suffix().startsWith("mca"))
		{
			int det = filename[filename.length() - 1].digitValue();
			_integrated_spectra_map[det] = data_struct::Spectra();
			unordered_map<string, string> pv_map;
			io::file::mca::load_integrated_spectra(filename.toStdString(), &_integrated_spectra_map[det], pv_map);
			for (const auto& itr : pv_map)
			{
				data_struct::Extra_PV pv;
				pv.name = itr.first;
				pv.value = itr.second;
				pv.description = "";
				pv.unit = "";					
				_scan_info.extra_pvs.push_back(pv);
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
	else
	{
		_fit_params_override_dict[idx] = new data_struct::Params_Override();
		return _fit_params_override_dict.at(idx);
	}
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