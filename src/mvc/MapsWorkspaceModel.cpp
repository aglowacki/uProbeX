/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsWorkspaceModel.h>
#include <QDebug>


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

    _mda_suffex.append("mda");

    _sws_suffex.append("sws");
}

/*---------------------------------------------------------------------------*/

MapsWorkspaceModel::~MapsWorkspaceModel()
{
    if(_dir != nullptr)
    {
        delete _dir;
    }
}

/*---------------------------------------------------------------------------*/

bool MapsWorkspaceModel::load(QString filepath)
{
    try
    {

        _dir = new QDir(filepath);
        if (!_dir->exists())
        {
            qWarning("Cannot find the example directory");
            return false;
        }

        _is_fit_params_loaded = _load_fit_params();

        _is_mda_loaded = _get_filesnames_in_directory("mda", _mda_suffex, &_mda_fileinfo_list);
        _is_sws_loaded = _get_filesnames_in_directory("vlm", _sws_suffex, &_sws_fileinfo_list);
        _is_imgdat_loaded = _get_filesnames_in_directory("img.dat", _all_h5_suffex, &_h5_fileinfo_list);

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

    return _is_fit_params_loaded && _is_imgdat_loaded;
}

/*---------------------------------------------------------------------------*/

MapsH5Model* MapsWorkspaceModel::getMapsH5Model(QString name)
{
    if(_h5_models.count(name) > 0)
    {
        return _h5_models[name];
    }
    if(_h5_fileinfo_list.count(name) > 0)
    {
        MapsH5Model * model = new MapsH5Model();
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

MDA_Model* MapsWorkspaceModel::get_MDA_Model(QString name)
{
    if(_mda_models.count(name) > 0)
    {
        return _mda_models[name];
    }
    if(_mda_fileinfo_list.count(name) > 0)
    {
        MDA_Model * model = new MDA_Model();
        QFileInfo fileInfo = _mda_fileinfo_list[name];

        if(model->load(fileInfo.absoluteFilePath()))
        {
            _mda_models.insert( {fileInfo.fileName(), model} );
            return model;
        }
        else
        {
            delete model;
        }
    }

    return nullptr;
}

/*---------------------------------------------------------------------------*/

SWSModel* MapsWorkspaceModel::get_SWS_Model(QString name)
{
    if(_sws_models.count(name) > 0)
    {
        return _sws_models[name];
    }
    if(_sws_fileinfo_list.count(name) > 0)
    {
        SWSModel * model = new SWSModel();
        QFileInfo fileInfo = _sws_fileinfo_list[name];
        if(model->load(fileInfo.absoluteFilePath()))
        {
            _sws_models.insert( {fileInfo.fileName(), model} );
            return model;
        }
        else
        {
            delete model;
        }
    }

    return nullptr;
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceModel::unload_H5_Model(QString name)
{
    if(_h5_models.count(name) > 0)
    {
        _h5_models.erase(name);
        MapsH5Model* model = _h5_models[name];
        delete model;
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceModel::unload_MDA_Model(QString name)
{
    if(_mda_models.count(name) > 0)
    {
        _mda_models.erase(name);
        MDA_Model* model = _mda_models[name];
        delete model;
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceModel::unload_SWS_Model(QString name)
{
    if(_sws_models.count(name) > 0)
    {
        _sws_models.erase(name);
        SWSModel* model = _sws_models[name];
        delete model;
    }
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

bool MapsWorkspaceModel::_get_filesnames_in_directory(QString sub_dir_name, QList <QString> suffex, map<QString, QFileInfo> *fileinfo_list)
{
    if(sub_dir_name.length() > 0)
    {
        if (!_dir->cd(sub_dir_name))
        {
            QString warn_msg = "Cannot find the "+sub_dir_name+" directory";
            qWarning(warn_msg.toStdString().c_str());
            return false;
        }
    }
    _dir->setFilter(QDir::Files | QDir::NoSymLinks);

    QFileInfoList list = _dir->entryInfoList();

    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        if (suffex.contains(fileInfo.suffix()))
        {
            fileinfo_list->emplace( fileInfo.fileName(), fileInfo );
        }
    }

    if(sub_dir_name.length() > 0)
    {
        _dir->cd("..");
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
}
