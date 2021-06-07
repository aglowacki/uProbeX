/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsWorkspaceModel.h>
#include <QEventLoop>
#include "core/GlobalThreadPool.h"
//                                          confocal,  emd,          gsecars   gsecars
std::vector<std::string> raw_h5_groups = {"2D Scan", "/Data/Image", "xrmmap", "xrfmap" };

/*----------------src/mvc/MapsWorkspaceModel.cpp \-----------------------------------------------------------*/

MapsWorkspaceModel::MapsWorkspaceModel() : QObject()
{

    _is_fit_params_loaded = false;
    _is_imgdat_loaded = false;
    _dir = nullptr;

    _all_h5_suffex.append("h5");
    _all_h5_suffex.append("h50");
    _all_h5_suffex.append("h51");
    _all_h5_suffex.append("h52");
    _all_h5_suffex.append("h53");
    _all_h5_suffex.append("hdf5");

    _mda_suffex.append("mda");
    _raw_suffex.append("h5");
    _raw_suffex.append("hdf5");
    _raw_suffex.append("emd");

    _vlm_suffex.append("sws");
    _vlm_suffex.append("tif");
    _vlm_suffex.append("tiff");
}

/*---------------------------------------------------------------------------*/

MapsWorkspaceModel::~MapsWorkspaceModel()
{
	/*
	for (auto & itr : _h5_models)
	{
		delete itr.second;
	}
	*/
	_h5_models.clear();
	/*
	for (auto & itr : _raw_models)
	{
		delete itr.second;
	}
	*/
	_raw_models.clear();
	/*
	for (auto & itr : _vlm_models)
	{
		delete itr.second;
	}
	*/
	_vlm_models.clear();

    if(_dir != nullptr)
    {
        delete _dir;
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceModel::load(QString filepath)
{
    try
    {

        _dir = new QDir(filepath);
        if (!_dir->exists())
        {
            qWarning("Cannot find the example directory");
            return;
        }

        {
            std::map<int, std::future<bool>> job_queue;
 
            job_queue[0] = Global_Thread_Pool::inst()->enqueue(get_filesnames_in_directory, *_dir, ".", _raw_suffex, &_raw_fileinfo_list, check_raw_h5);
            job_queue[1] = Global_Thread_Pool::inst()->enqueue(get_filesnames_in_directory, *_dir, "mda", _mda_suffex, &_raw_fileinfo_list, check_raw_mda);
            job_queue[2] = Global_Thread_Pool::inst()->enqueue(get_filesnames_in_directory, *_dir, "vlm", _vlm_suffex, &_vlm_fileinfo_list, check_vlm);
            job_queue[3] = Global_Thread_Pool::inst()->enqueue(get_filesnames_in_directory, *_dir, "img.dat", _all_h5_suffex, &_h5_fileinfo_list, check_imgdat_h5);

            _is_fit_params_loaded = _load_fit_params();
            io::populate_netcdf_hdf5_files(filepath.toStdString());

            while (job_queue.size() > 0)
            {
                QCoreApplication::processEvents();
                std::vector<int> to_delete;
                for (auto& itr : job_queue)
                {
                    std::future_status status = itr.second.wait_for(std::chrono::milliseconds(100));

                    if (status == std::future_status::ready)
                    {
                        switch (itr.first)
                        {
                        case 0:
                            _is_raw_loaded = itr.second.get();
                            to_delete.push_back(itr.first);
                            emit doneLoadingRAW();
                            break;
                        case 1:
                            _is_raw_loaded = itr.second.get();
                            to_delete.push_back(itr.first);
                            emit doneLoadingMDA();
                            break;
                        case 2:
                            _is_vlm_loaded = itr.second.get();
                            to_delete.push_back(itr.first);
                            emit doneLoadingVLM();
                            break;
                        case 3:
                            _is_imgdat_loaded = itr.second.get();
                            to_delete.push_back(itr.first);
                            emit doneLoadingImgDat();
                            break;
                        }
                    }
                }
                for (const auto& itr : to_delete)
                {
                    job_queue.erase(itr);
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

    emit doneLoading();

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceModel::reload_raw()
{
    _is_raw_loaded = get_filesnames_in_directory(*_dir, "mda", _mda_suffex, &_raw_fileinfo_list, check_raw_mda);
    emit doneLoadingRAW();
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceModel::reload_analyzed()
{
    _is_imgdat_loaded = get_filesnames_in_directory(*_dir, "img.dat", _all_h5_suffex, &_h5_fileinfo_list, check_imgdat_h5);
    emit doneLoadingImgDat();
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceModel::reload_vlm()
{
    _is_vlm_loaded = get_filesnames_in_directory(*_dir, "vlm", _vlm_suffex, &_vlm_fileinfo_list, check_vlm);
    emit doneLoadingVLM();
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceModel::unload()
{
	/*
    for(auto &itr : _h5_models)
    {
        delete itr.second;
    }
	*/
    _h5_models.clear();
	/*
    for(auto &itr : _raw_models)
    {
        delete itr.second;
    }
	*/
    _raw_models.clear();
	/*
    for(auto &itr : _vlm_models)
    {
        delete itr.second;
    }*/
    _vlm_models.clear();

    _fit_params_override_dict.clear();
    _raw_fileinfo_list.clear();
    _vlm_fileinfo_list.clear();
    _h5_fileinfo_list.clear();

    _is_fit_params_loaded = false;
    _is_raw_loaded = false;
    _is_vlm_loaded = false;
    _is_imgdat_loaded = false;
    //_dir = new QDir(filepath);

    emit doneUnloading();
}

/*---------------------------------------------------------------------------*/

std::shared_ptr<MapsH5Model> MapsWorkspaceModel::get_MapsH5_Model(QString name)
{
    if(_h5_models.count(name) > 0)
    {
        return _h5_models[name];
    }
    if(_h5_fileinfo_list.count(name) > 0)
    {
		std::shared_ptr<MapsH5Model> model(new MapsH5Model());
        QFileInfo fileInfo = _h5_fileinfo_list[name];
        model->load(fileInfo.absoluteFilePath());
        if(model->is_counts_loaded())
        {
            _h5_models.insert( {fileInfo.fileName(), model} );
            return model;
        }
    }

    return nullptr;
}

/*---------------------------------------------------------------------------*/

std::shared_ptr<RAW_Model> MapsWorkspaceModel::get_RAW_Model(QString name)
{
	
    if(_raw_models.count(name) > 0)
    {
        return _raw_models[name];
    }
    if(_raw_fileinfo_list.count(name) > 0)
    {
		std::shared_ptr<RAW_Model> model( new RAW_Model());
        for(auto &itr : _fit_params_override_dict)
        {
            model->setParamOverride(itr.first, &(itr.second));
        }
        QFileInfo fileInfo = _raw_fileinfo_list[name];
        if(model->load(fileInfo.absolutePath(), fileInfo.fileName()))
        {
            _raw_models.insert( {fileInfo.fileName(), model} );
            return model;
        }
        else
        {
            //delete model;
        }
    }
	
    return nullptr;
}

/*---------------------------------------------------------------------------*/

std::shared_ptr<VLM_Model> MapsWorkspaceModel::get_VLM_Model(QString name)
{
    if(_vlm_models.count(name) > 0)
    {
        return _vlm_models[name];
    }
    if(_vlm_fileinfo_list.count(name) > 0)
    {
		std::shared_ptr<VLM_Model> model;
        QFileInfo fileInfo = _vlm_fileinfo_list[name];
        QString ext = fileInfo.suffix().toLower();
        if (ext == "tif" || ext == "tiff")
        {
            model = std::make_shared<TIFF_Model>();
        }
        else if (ext == "sws")
        {
			model = std::make_shared<SWSModel>();
        }
        if(model->load(fileInfo.absoluteFilePath()))
        {
            _vlm_models.insert( {fileInfo.fileName(), model} );
            return model;
        }
        else
        {
            //delete model;
        }
    }

    return nullptr;
}

/*---------------------------------------------------------------------------*/

vector<QString> MapsWorkspaceModel::get_loaded_raw_names()
{
    vector<QString>  ret;
    for (auto& itr : _raw_models)
    {
        ret.push_back(itr.first);
    }
    return ret;
}

/*---------------------------------------------------------------------------*/

vector<QString> MapsWorkspaceModel::get_loaded_h5_names()
{
    vector<QString>  ret;
    for (auto& itr : _h5_models)
    {
        ret.push_back(itr.first);
    }
    return ret;
}

/*---------------------------------------------------------------------------*/

vector<QString> MapsWorkspaceModel::get_loaded_vlm_names()
{
    vector<QString>  ret;
    for (auto& itr : _vlm_models)
    {
        ret.push_back(itr.first);
    }
    return ret;
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceModel::unload_H5_Model(QString name)
{
	/*
    if(_h5_models.count(name) > 0)
    {
        MapsH5Model* model = _h5_models[name];
        _h5_models.erase(name);
        delete model;
    }
	*/
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceModel::unload_RAW_Model(QString name)
{
	/*
    if(_raw_models.count(name) > 0)
    {
        RAW_Model* model = _raw_models[name];
        _raw_models.erase(name);
        delete model;
    }
	*/
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceModel::unload_VLM_Model(QString name)
{
	/*
    if(_vlm_models.count(name) > 0)
    {
        VLM_Model* model = _vlm_models[name];
        _vlm_models.erase(name);
        delete model;
    }
	*/
}

/*---------------------------------------------------------------------------*/

QString MapsWorkspaceModel::get_directory_name()
{
    if( _dir != nullptr)
        return _dir->path();
    return "";
}

/*---------------------------------------------------------------------------*/

bool MapsWorkspaceModel::_load_fit_params()
{

    std::string dataset_dir = _dir->absolutePath().toStdString() + "/";
    for(size_t detector_num = 0; detector_num <= 3; detector_num++)
    {
        data_struct::Params_Override params_override(dataset_dir, detector_num);
        if( io::load_override_params(dataset_dir, detector_num, &params_override) )
        {
            _fit_params_override_dict[detector_num] = params_override;
            emit newFitParamsFileLoaded(detector_num);
        }
    }
    data_struct::Params_Override params(dataset_dir, -1);
    if( io::load_override_params(dataset_dir, -1, &params) )
    {
        _fit_params_override_dict[-1] = params;
        emit newFitParamsFileLoaded(-1);
    }


    return true;

}

/*---------------------------------------------------------------------------*/

bool get_filesnames_in_directory(QDir dir, QString sub_dir_name, QList <QString> suffex, map<QString, QFileInfo> *fileinfo_list, Check_Func_Def chk_func)
{
    if(sub_dir_name.length() > 0)
    {
        if (!dir.cd(sub_dir_name))
        {
            //QString warn_msg = "Cannot find the "+sub_dir_name+" directory";
            //qWarning(warn_msg.toStdString().c_str());
            //logW<< "Cannot find the " << sub_dir_name.toStdString() << " directory";
            return false;
        }
    }
    dir.setFilter(QDir::Files | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        if (suffex.contains(fileInfo.suffix()))
        {
            //if (chk_func(fileInfo)) //TODO: maybe have option for checks 
            {
                fileinfo_list->emplace(fileInfo.fileName(), fileInfo);
            }
        }
    }
    
    if(sub_dir_name.length() > 0 && sub_dir_name != ".")
    {
        dir.cd("..");
    }
    return true;
}

/*---------------------------------------------------------------------------*/

data_struct::Fit_Parameters* MapsWorkspaceModel::getFitParameters(int idx)
{
    if(_is_fit_params_loaded && _fit_params_override_dict.count(idx) > 0)
    {
        return &(_fit_params_override_dict[idx].fit_params);
    }
    return nullptr;
}

/*---------------------------------------------------------------------------*/

data_struct::Params_Override* MapsWorkspaceModel::getParamOverride(int idx)
{
    if(_is_fit_params_loaded && _fit_params_override_dict.count(idx) > 0)
    {
        return &(_fit_params_override_dict[idx]);
    }
    return nullptr;
}

/*---------------------------------------------------------------------------*/

data_struct::Fit_Element_Map_Dict *MapsWorkspaceModel::getElementToFit(int idx)
{
    if(_is_fit_params_loaded && _fit_params_override_dict.count(idx) > 0)
    {
        return &(_fit_params_override_dict[idx].elements_to_fit);
    }
    return nullptr;
}

/*---------------------------------------------------------------------------*/

bool check_raw_mda(QFileInfo fileInfo)
{
    int is_good = io::file::mda_get_multiplied_dims(fileInfo.filePath().toStdString());
    if (is_good > 0)
        return true;
    return false;
}

/*---------------------------------------------------------------------------*/

bool check_raw_h5(QFileInfo fileInfo)
{

    hid_t fid = H5Fopen(fileInfo.filePath().toStdString().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if ( fid < 0)
        return false;

    for(const auto& itr: raw_h5_groups)
    {
        hid_t gid = H5Gopen(fid, itr.c_str(), H5P_DEFAULT);
        if ( gid > -1)
        {
            H5Gclose(gid);
            H5Fclose(fid);
            return true;
        }
    }

    return false;

}

/*---------------------------------------------------------------------------*/

bool check_vlm(QFileInfo fileInfo)
{
    // TODO:
       return false;

}

/*---------------------------------------------------------------------------*/

bool check_imgdat_h5(QFileInfo fileInfo)
{

    hid_t fid = H5Fopen(fileInfo.filePath().toStdString().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if ( fid < 0)
        return false;

    hid_t gid = H5Gopen(fid, "MAPS" , H5P_DEFAULT);
    if ( gid > -1)
    {
        H5Gclose(gid);
        H5Fclose(fid);
        return true;
    }

    return false;

}
