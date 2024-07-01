/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/RAW_Model.h>
#include "core/defines.h"
#include <QFileInfo>
#include "io/file/hl_file_io.h"
#include "io/file/mca_io.h"

#define DEFAULT_NUM_DETECTORXS 8

/*----------------src/mvc/RAW_Model.cpp \-----------------------------------------------------------*/

RAW_Model::RAW_Model() : QObject()
{

    _path = "";
    _filepath = "";
	_scan_info.meta_info.requested_cols = 1;
	_scan_info.meta_info.requested_rows = 1;

}

//---------------------------------------------------------------------------

RAW_Model::~RAW_Model()
{

}

//---------------------------------------------------------------------------

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
		io::file::MDA_IO<double> mda_io;
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
					if (false == io::file::load_and_integrate_spectra_volume(_path.toStdString(), finfo.fileName().toStdString(), i, &(_integrated_spectra_map[i]), getParamOverrideOrAvg(i)))
					{
						_integrated_spectra_map.erase(i);
					}
				}
			}
			else
			{
				// copy from mda_io to this model
				for (int i = 0; i < mda_io.get_num_integreated_spectra(); i++)
				{
					if (mda_io.get_integrated_spectra(i)->size() > 0)
					{
						_integrated_spectra_map[i] = (data_struct::Spectra<double>) * (mda_io.get_integrated_spectra(i));
					}
				}
				mda_io.unload_int_spectra();
			}

			_scan_info = *(mda_io.get_scan_info());
		}
		else if (filename.endsWith(".h5") || filename.endsWith(".hdf5") || filename.endsWith(".emd") )
		{
			if (_path.isEmpty())
			{
				_path = finfo.path();
				filename = finfo.fileName();
			}
			
			if (io::file::get_scalers_and_metadata_h5(_path.toStdString(), filename.toStdString(), &_scan_info))
			{
				// todo: update scaler names from override file

				for (auto& i : _scan_info.meta_info.detectors)
				{
					io::file::load_and_integrate_spectra_volume(_path.toStdString(), filename.toStdString(), i, &(_integrated_spectra_map[i]), getParamOverrideOrAvg(i));
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
			_integrated_spectra_map[det] = data_struct::Spectra<double>();
			std::unordered_map<std::string, double> pv_map;
			io::file::mca::load_integrated_spectra(filename.toStdString(), &_integrated_spectra_map[det], pv_map);
			// add fit params
			data_struct::Params_Override<double>* po = nullptr;
			if (_fit_params_override_dict.count(det) > 0)
			{
				po = _fit_params_override_dict.at(det);
			}

			if (po == nullptr)
			{
				po = new data_struct::Params_Override<double>();
				_fit_params_override_dict[det] = po;
			}
			
			for (const auto& itr : pv_map)
			{
				data_struct::Extra_PV pv;
				pv.name = itr.first;
				pv.value = std::to_string(itr.second);
				pv.description = "";
				pv.unit = "";					
				_scan_info.extra_pvs.push_back(pv);
				if (pv.name == "CAL_OFFSET")
				{
					po->fit_params.add_parameter(data_struct::Fit_Param<double>(STR_ENERGY_OFFSET, itr.second));
				}
				if (pv.name == "CAL_SLOPE")
				{
					po->fit_params.add_parameter(data_struct::Fit_Param<double>(STR_ENERGY_SLOPE, itr.second));
				}
				if (pv.name == "CAL_QUAD")
				{
					po->fit_params.add_parameter(data_struct::Fit_Param<double>(STR_ENERGY_QUADRATIC, itr.second));
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

//---------------------------------------------------------------------------

data_struct::Params_Override<double>* RAW_Model::getParamOverride(int idx)
{
    if (_fit_params_override_dict.count(idx) > 0)
    {
        return _fit_params_override_dict.at(idx);
    }
	else
	{
		_fit_params_override_dict[idx] = new data_struct::Params_Override<double>();
		return _fit_params_override_dict.at(idx);
	}
}

//---------------------------------------------------------------------------

data_struct::Params_Override<double>* RAW_Model::getParamOverrideOrAvg(int idx)
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

//---------------------------------------------------------------------------

unsigned int RAW_Model::getNumIntegratedSpectra()
{
	return _integrated_spectra_map.size();
}

//---------------------------------------------------------------------------

std::vector<unsigned int> RAW_Model::getDetectorKeys()
{

	std::vector<unsigned int> keys;
	for (auto& itr : _integrated_spectra_map)
	{
		keys.push_back(itr.first);
	}
	return keys;

}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

data_struct::Spectra<double>* RAW_Model::getIntegratedSpectra(unsigned int det)
{
	if (_integrated_spectra_map.count(det) > 0)
	{
		return &(_integrated_spectra_map.at(det));
	}
	return nullptr;
}

//---------------------------------------------------------------------------

data_struct::Scan_Info<double>* RAW_Model::getScanInfo()
{
	return &_scan_info;
}

//---------------------------------------------------------------------------