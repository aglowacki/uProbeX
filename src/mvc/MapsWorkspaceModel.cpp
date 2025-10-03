/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsWorkspaceModel.h>
#include <QEventLoop>
#include "core/GlobalThreadPool.h"
#include <QRegularExpression>
#include "io/file/hdf5_io.h"
#include "preferences/Preferences.h"
//#include "io/file/aps/aps_roi.h"
//                                          confocal,  emd,          gsecars   gsecars
std::vector<std::string> raw_h5_groups = {"2D Scan", "/Data/Image", "xrmmap", "xrfmap" };

//---------------------------------------------------------------------------

bool check_raw_mda(QFileInfo fileInfo)
{
    int is_good = io::file::mda_get_multiplied_dims(fileInfo.filePath().toStdString());
    if (is_good > 0)
        return true;
    return false;
}

//---------------------------------------------------------------------------

bool check_raw_h5(QFileInfo fileInfo)
{

    hid_t fid = H5Fopen(fileInfo.filePath().toStdString().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if (fid < 0)
        return false;

    for (const auto& itr : raw_h5_groups)
    {
        hid_t gid = H5Gopen(fid, itr.c_str(), H5P_DEFAULT);
        if (gid > -1)
        {
            H5Gclose(gid);
            H5Fclose(fid);
            return true;
        }
    }

    return false;

}

//---------------------------------------------------------------------------

bool check_region_link(QFileInfo fileInfo)
{
    // TODO:
    return true;

}


//---------------------------------------------------------------------------

bool check_roi(QFileInfo fileInfo)
{
    // TODO:
    return true;

}

//---------------------------------------------------------------------------

bool check_vlm(QFileInfo fileInfo)
{
    // TODO:
    return true;

}

//---------------------------------------------------------------------------

bool check_imgdat_h5(QFileInfo fileInfo)
{

    hid_t fid = H5Fopen(fileInfo.filePath().toStdString().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if (fid < 0)
        return false;

    hid_t gid = H5Gopen(fid, "MAPS", H5P_DEFAULT);
    if (gid > -1)
    {
        H5Gclose(gid);
        H5Fclose(fid);
        return true;
    }

    return false;

}


/*----------------src/mvc/MapsWorkspaceModel.cpp \-----------------------------------------------------------*/

MapsWorkspaceModel::MapsWorkspaceModel() : QObject()
{

    _is_fit_params_loaded = false;
    _is_imgdat_loaded = false;
    _is_rois_loaded = false;
    _is_vlm_loaded = false;
    _is_raw_loaded = false;
    _dir = nullptr;

    _all_h5_suffex.append("h5");
    _all_h5_suffex.append("hdf5");

    _mda_suffex.append("mda");
    _mda_suffex.append("mca");
    _mda_suffex.append("h5");
    for (int i = 0; i < MAX_DETECTORS; i++)
    {
        _all_h5_suffex.append(QString("h5") + QString::number(i));
        _mda_suffex.append(QString("mca") + QString::number(i));
    }
    
    _raw_suffex.append("h5");
    _raw_suffex.append("hdf5");
    _raw_suffex.append("emd");

    _vlm_suffex.append("sws");
    _vlm_suffex.append("tif");
    _vlm_suffex.append("tiff");

    _all_roi_suffex.append("r0i");

    _all_region_links_suffex.append("tif");
}

//---------------------------------------------------------------------------

MapsWorkspaceModel::~MapsWorkspaceModel()
{
	for (auto & itr : _h5_models)
	{
		delete itr.second;
	}
	_h5_models.clear();

	for (auto & itr : _raw_models)
	{
		delete itr.second;
	}
	_raw_models.clear();

	for (auto & itr : _vlm_models)
	{
		delete itr.second;
	}
	_vlm_models.clear();

    if(_dir != nullptr)
    {
        delete _dir;
    }
}

//---------------------------------------------------------------------------

void MapsWorkspaceModel::load(QString filepath)
{
    try
    {
        std::vector<std::regex> ignore_dir_list = { std::regex("img.dat.*"), std::regex("mda.*"), std::regex("output.*"), std::regex("rois"), std::regex("flyXRF"), std::regex("XRF"), std::regex("[vV][lL][mM]") };

        _dir = new QDir(filepath);
        if (!_dir->exists())
        {
            qWarning("Cannot find the example directory");
            return;
        }

        {
            std::map<int, std::future<bool>> job_queue;
            // get list of directories in the root
            std::string dataset_dir = filepath.toStdString();

            // if check for esrf
            if (Preferences::inst()->getValue(STR_SEARCH_SUB_DIR_FOR_DATASETS).toBool())
            {
                std::vector<std::string> root_dir_list = io::file::File_Scan::inst()->find_all_dirs(dataset_dir + DIR_END_CHAR, ignore_dir_list, false);
                for (const auto& itr : root_dir_list)
                {
                    get_filesnames_in_directory(*_dir, QString(itr.c_str()), _raw_suffex, &_raw_fileinfo_list, check_raw_h5, true);
                    QCoreApplication::processEvents();
                }
            }
            job_queue[0] = Global_Thread_Pool::inst()->enqueue(get_filesnames_in_directory, *_dir, ".", _raw_suffex, &_raw_fileinfo_list, check_raw_h5, false);
            job_queue[1] = Global_Thread_Pool::inst()->enqueue(get_filesnames_in_directory, *_dir, "mda", _mda_suffex, &_raw_fileinfo_list, check_raw_mda, false);
            job_queue[2] = Global_Thread_Pool::inst()->enqueue(get_filesnames_in_directory, *_dir, "vlm", _vlm_suffex, &_vlm_fileinfo_list, check_vlm, false);
            job_queue[3] = Global_Thread_Pool::inst()->enqueue(get_filesnames_in_directory, *_dir, "img.dat", _all_h5_suffex, &_h5_fileinfo_list, check_imgdat_h5, false);
            job_queue[4] = Global_Thread_Pool::inst()->enqueue(get_filesnames_in_directory, *_dir, "rois", _all_roi_suffex, &_roi_fileinfo_list, check_roi, false);
            job_queue[5] = Global_Thread_Pool::inst()->enqueue(get_filesnames_in_directory, *_dir, "VLM/region_links", _all_region_links_suffex, &_region_links_fileinfo_list, check_region_link, false);

            _is_fit_params_loaded = _load_fit_params();
            

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
                        case 4:
                            _is_rois_loaded = itr.second.get();
                            to_delete.push_back(itr.first);
                            break;
                        default:
                            to_delete.push_back(itr.first);
                            break;
                        }
                    }
                }
                for (const auto& itr : to_delete)
                {
                    job_queue.erase(itr);
                }   
            }
            // wait for all datasets to finish loading and then update roi's
            _roi_files_to_num();
            emit doneLoadingROIS();
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

//---------------------------------------------------------------------------

void MapsWorkspaceModel::reload_raw()
{
    _is_raw_loaded = get_filesnames_in_directory(*_dir, "mda", _mda_suffex, &_raw_fileinfo_list, check_raw_mda, false);
    emit doneLoadingRAW();
}

//---------------------------------------------------------------------------

void MapsWorkspaceModel::reload_analyzed()
{
    _is_imgdat_loaded = get_filesnames_in_directory(*_dir, "img.dat", _all_h5_suffex, &_h5_fileinfo_list, check_imgdat_h5, false);
    emit doneLoadingImgDat();
}

//---------------------------------------------------------------------------

void MapsWorkspaceModel::reload_vlm()
{
    _is_vlm_loaded = get_filesnames_in_directory(*_dir, "vlm", _vlm_suffex, &_vlm_fileinfo_list, check_vlm, false);
    emit doneLoadingVLM();
}

//---------------------------------------------------------------------------

void MapsWorkspaceModel::reload_roi()
{
    _roi_fileinfo_list.clear();
    get_filesnames_in_directory(*_dir, "rois", _all_roi_suffex, &_roi_fileinfo_list, check_roi, false);
}

//---------------------------------------------------------------------------

void MapsWorkspaceModel::reload_region_link()
{
    _region_links_fileinfo_list.clear();
    get_filesnames_in_directory(*_dir, "VLM/region_links", _all_region_links_suffex, &_region_links_fileinfo_list, check_region_link, false);
}

//---------------------------------------------------------------------------

void MapsWorkspaceModel::unload()
{

    for(auto &itr : _h5_models)
    {
        delete itr.second;
    }
    _h5_models.clear();

    for(auto &itr : _raw_models)
    {
        delete itr.second;
    }
    _raw_models.clear();

    for(auto &itr : _vlm_models)
    {
        delete itr.second;
    }
    _vlm_models.clear();

    _fit_params_override_dict.clear();
    _raw_fileinfo_list.clear();
    _vlm_fileinfo_list.clear();
    _h5_fileinfo_list.clear();
    _roi_fileinfo_list.clear();

    _is_fit_params_loaded = false;
    _is_raw_loaded = false;
    _is_vlm_loaded = false;
    _is_imgdat_loaded = false;
    _is_rois_loaded = false;
    //_dir = new QDir(filepath);

    emit doneUnloading();
}

//---------------------------------------------------------------------------

void MapsWorkspaceModel::_load_region_links(QString name, MapsH5Model* model)
{
    for (auto& itr : _region_links_fileinfo_list)
    {
        QStringList flist = itr.second.baseName().split("@");
        if (flist.count() > 1)
        {
            if (flist[0] == name)
            {
                QImage image;
                if (image.load(itr.second.absoluteFilePath()))
                {
                    model->addRegionLink(flist[1], image.mirrored());
                }
                else
                {
                    logW << "Error loading region link " << itr.second.absoluteFilePath().toStdString() << " for " << name.toStdString() << "\n";
                }
            }
        }
    }

}

//---------------------------------------------------------------------------

MapsH5Model* MapsWorkspaceModel::get_MapsH5_Model(QString name)
{
    if(_h5_models.count(name) > 0)
    {
        return _h5_models[name];
    }
    if(_h5_fileinfo_list.count(name) > 0)
    {
        MapsH5Model * model = new MapsH5Model();
        QFileInfo fileInfo = _h5_fileinfo_list[name];
        if (model->load(fileInfo.absoluteFilePath()))
        {
            QApplication::processEvents();
            // Load region links
            _load_region_links(fileInfo.baseName(), model);
            
            //can not load from here because ifstream.open locks up in thread
            // Load ROI's
            //_load_rois(fileInfo.baseName(), model);
        }
        //_roi_fileinfo_list.count()
        if(model->is_counts_loaded())
        {
            _h5_models.insert( {fileInfo.fileName(), model} );
            return model;
        }
    }

    return nullptr;
}

//---------------------------------------------------------------------------

RAW_Model* MapsWorkspaceModel::get_RAW_Model(QString name)
{
	
    if(_raw_models.count(name) > 0)
    {
        return _raw_models[name];
    }
    if(_raw_fileinfo_list.count(name) > 0)
    {
        RAW_Model * model = new RAW_Model();
        for(auto &itr : _fit_params_override_dict)
        {
            model->setParamOverride(itr.first, &(itr.second));
        }
        QFileInfo fileInfo = _raw_fileinfo_list[name];
        if(io::file::File_Scan::inst()->get_total_count() == 0)
		{
			io::file::File_Scan::inst()->populate_netcdf_hdf5_files(fileInfo.absolutePath().toStdString());
		}
        if(model->load(fileInfo.absolutePath(), fileInfo.fileName()))
        {
            _raw_models.insert( {fileInfo.fileName(), model} );
            return model;
        }
        else
        {
            delete model;
        }
    }
	
    return nullptr;
}

//---------------------------------------------------------------------------

VLM_Model* MapsWorkspaceModel::get_VLM_Model(QString name)
{
    if(_vlm_models.count(name) > 0)
    {
        return _vlm_models[name];
    }
    if(_vlm_fileinfo_list.count(name) > 0)
    {
        VLM_Model * model = nullptr;
        QFileInfo fileInfo = _vlm_fileinfo_list[name];
        QString ext = fileInfo.suffix().toLower();
        if (ext == "tif" || ext == "tiff")
        {
            model = new TIFF_Model();
        }
        else if (ext == "sws")
        {
            model = new SWSModel();
        }
        if (model != nullptr)
        {
            if (model->load(fileInfo.absoluteFilePath()))
            {
                _vlm_models.insert({ fileInfo.fileName(), model });
                return model;
            }
            else
            {
                delete model;
            }
        }
    }

    return nullptr;
}

//---------------------------------------------------------------------------

std::vector<QString> MapsWorkspaceModel::get_loaded_raw_names()
{
    std::vector<QString>  ret;
    for (auto& itr : _raw_models)
    {
        ret.push_back(itr.first);
    }
    return ret;
}

//---------------------------------------------------------------------------

std::vector<QString> MapsWorkspaceModel::get_loaded_h5_names()
{
    std::vector<QString>  ret;
    for (auto& itr : _h5_models)
    {
        ret.push_back(itr.first);
    }
    return ret;
}

//---------------------------------------------------------------------------

std::vector<QString> MapsWorkspaceModel::get_loaded_vlm_names()
{
    std::vector<QString>  ret;
    for (auto& itr : _vlm_models)
    {
        ret.push_back(itr.first);
    }
    return ret;
}

//---------------------------------------------------------------------------

QStringList MapsWorkspaceModel::get_raw_names_as_qstringlist()
{
    QStringList  ret;
    for (auto& itr : _raw_fileinfo_list)
    {
        ret.push_back(itr.first);
    }
    return ret;
}

//---------------------------------------------------------------------------

QStringList MapsWorkspaceModel::get_h5_names_as_qstringlist()
{
    QStringList  ret;
    for (auto& itr : _h5_fileinfo_list)
    {
        ret.push_back(itr.first);
    }
    return ret;
}

//---------------------------------------------------------------------------

QStringList MapsWorkspaceModel::get_vlm_names_as_qstringlist()
{
    QStringList  ret;
    for (auto& itr : _vlm_fileinfo_list)
    {
        ret.push_back(itr.first);
    }
    return ret;
}

//---------------------------------------------------------------------------

void MapsWorkspaceModel::unload_H5_Model(QString name)
{
    if(_h5_models.count(name) > 0)
    {
        MapsH5Model* model = _h5_models[name];
        _h5_models.erase(name);
        delete model;
    }
}

//---------------------------------------------------------------------------

void MapsWorkspaceModel::unload_all_H5_Model()
{
    for(auto &itr : _h5_models)
    {
        delete itr.second;
    }
    _h5_models.clear();
}

//---------------------------------------------------------------------------

void MapsWorkspaceModel::unload_RAW_Model(QString name)
{
    if(_raw_models.count(name) > 0)
    {
        RAW_Model* model = _raw_models[name];
        _raw_models.erase(name);
        delete model;
    }
}

//---------------------------------------------------------------------------

void MapsWorkspaceModel::unload_all_RAW_Model()
{
    for(auto &itr : _raw_models)
    {
        delete itr.second;
    }
    _raw_models.clear();
}

//---------------------------------------------------------------------------

void MapsWorkspaceModel::unload_VLM_Model(QString name)
{
    if(_vlm_models.count(name) > 0)
    {
        VLM_Model* model = _vlm_models[name];
        _vlm_models.erase(name);
        delete model;
    }
}

//---------------------------------------------------------------------------

void MapsWorkspaceModel::unload_all_VLM_Model()
{
    for(auto &itr : _vlm_models)
    {
        delete itr.second;
    }
    _vlm_models.clear();
}

//---------------------------------------------------------------------------

QString MapsWorkspaceModel::get_directory_name()
{
    if( _dir != nullptr)
    {
        return _dir->path();
    }
    return "";
}

//---------------------------------------------------------------------------

bool MapsWorkspaceModel::_load_fit_params()
{

    std::string dataset_dir = _dir->absolutePath().toStdString() + "/";
    for(size_t detector_num = 0; detector_num <= MAX_DETECTORS; detector_num++)
    {
        data_struct::Params_Override<double> params_override(dataset_dir, detector_num);
        if( io::file::load_override_params(dataset_dir, detector_num, &params_override) )
        {
            _fit_params_override_dict[detector_num] = params_override;
            emit newFitParamsFileLoaded(detector_num);
        }
    }
    data_struct::Params_Override<double> params(dataset_dir, -1);
    if( io::file::load_override_params(dataset_dir, -1, &params) )
    {
        _fit_params_override_dict[-1] = params;
        emit newFitParamsFileLoaded(-1);
    }


    return true;

}

//---------------------------------------------------------------------------

bool get_filesnames_in_directory(QDir dir, QString sub_dir_name, QList <QString> suffex, std::map<QString, QFileInfo> *fileinfo_list, Check_Func_Def chk_func, bool prepend_sub_dir)
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
                if (prepend_sub_dir)
                {
                    fileinfo_list->emplace(sub_dir_name + QDir::separator() + fileInfo.fileName(), fileInfo);
                }
                else
                {
                    fileinfo_list->emplace(fileInfo.fileName(), fileInfo);
                }
            }
        }
    }
    
    if(sub_dir_name.length() > 0 && sub_dir_name != ".")
    {
        dir.cd("..");
    }
    return true;
}

//---------------------------------------------------------------------------

data_struct::Fit_Parameters<double>* MapsWorkspaceModel::getFitParameters(int idx)
{
    if(_is_fit_params_loaded && _fit_params_override_dict.count(idx) > 0)
    {
        return &(_fit_params_override_dict[idx].fit_params);
    }
    return nullptr;
}

//---------------------------------------------------------------------------

data_struct::Params_Override<double>* MapsWorkspaceModel::getParamOverride(int idx)
{
    if(_is_fit_params_loaded && _fit_params_override_dict.count(idx) > 0)
    {
        return &(_fit_params_override_dict[idx]);
    }
    return nullptr;
}

//---------------------------------------------------------------------------

data_struct::Fit_Element_Map_Dict<double>* MapsWorkspaceModel::getElementToFit(int idx)
{
    if(_is_fit_params_loaded && _fit_params_override_dict.count(idx) > 0)
    {
        return &(_fit_params_override_dict[idx].elements_to_fit);
    }
    return nullptr;
}

//---------------------------------------------------------------------------

void MapsWorkspaceModel::_roi_files_to_num()
{
    for(auto itr : _roi_fileinfo_list)
    {
        _roi_count_map[itr.second.baseName()] = 0;
        QFile roiFile(itr.second.absoluteFilePath());
        if (!roiFile.open(QIODevice::ReadOnly))
        {
            continue;
        }

        QJsonObject rootJson = QJsonDocument::fromJson(roiFile.readAll()).object();
        roiFile.close();

        if (rootJson.contains(STR_MAPS_ROIS.c_str()) && rootJson[STR_MAPS_ROIS.c_str()].isArray())
        {
            QJsonArray rois = rootJson[STR_MAPS_ROIS.c_str()].toArray();
            _roi_count_map[itr.second.baseName()] = rois.size();
        }
    }
}

//---------------------------------------------------------------------------