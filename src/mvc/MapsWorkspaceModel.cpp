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

    _avg_h5_suffex.append("h5");;
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

bool MapsWorkspaceModel::load(QString filepath, bool all)
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

        _is_sws_loaded = _load_sws_workspaces();


        if (all)
        {
            _is_imgdat_loaded = _load_img_dat(_all_h5_suffex);
        }
        else
        {
            _is_imgdat_loaded = _load_img_dat(_avg_h5_suffex);
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

    return _is_fit_params_loaded && _is_imgdat_loaded;
}

/*---------------------------------------------------------------------------*/

MapsH5Model* MapsWorkspaceModel::getMapsH5Model(QString name)
{
    if(_h5_models.count(name) > 0)
    {
        return _h5_models[name];
    }
    return nullptr;
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
        data_struct::xrf::Params_Override params_override(dataset_dir, detector_num);
        if( io::load_override_params(dataset_dir, detector_num, &params_override) )
        {
            _fit_params_override_dict[detector_num] = params_override;
            emit newFitParamsFileLoaded(detector_num);
        }
    }
    data_struct::xrf::Params_Override params(dataset_dir, -1);
    if( io::load_override_params(dataset_dir, -1, &params) )
    {
        _fit_params_override_dict[-1] = params;
        emit newFitParamsFileLoaded(-1);
    }


    return true;

}

/*---------------------------------------------------------------------------*/

bool MapsWorkspaceModel::_load_img_dat(QList <QString> suffex)
{
    if (!_dir->cd("img.dat"))
    {
        qWarning("Cannot find the \"/tmp\" directory");
        return false;
    }
    _dir->setFilter(QDir::Files | QDir::NoSymLinks);
    //_dir->setSorting(QDir::Size | QDir::Reversed);

    QFileInfoList list = _dir->entryInfoList();
    //std::cout << "     Bytes Filename" << std::endl;
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        if (suffex.contains(fileInfo.suffix()))
        {
            MapsH5Model * model = new MapsH5Model();
            model->load(fileInfo.absoluteFilePath());
            if(model->is_counts_loaded())
            {
                _h5_models.insert( {fileInfo.fileName(), model} );
                emit newAnalyzedH5FileLoaded(fileInfo.fileName());
            }
//            else
//            {
//                delete model;
//            }
        }
    }

    _dir->cd("..");
    return true;
}

/*---------------------------------------------------------------------------*/

bool MapsWorkspaceModel::_load_sws_workspaces()
{
    return false;
}

/*---------------------------------------------------------------------------*/

data_struct::xrf::Fit_Parameters* MapsWorkspaceModel::getFitParameters(int idx)
{
    if(_is_fit_params_loaded && _fit_params_override_dict.count(idx) > 0)
    {
        return &(_fit_params_override_dict[idx].fit_params);
    }
}

/*---------------------------------------------------------------------------*/
data_struct::xrf::Fit_Element_Map_Dict *MapsWorkspaceModel::getElementToFit(int idx)
{
    if(_is_fit_params_loaded && _fit_params_override_dict.count(idx) > 0)
    {
        return &(_fit_params_override_dict[idx].elements_to_fit);
    }
}
