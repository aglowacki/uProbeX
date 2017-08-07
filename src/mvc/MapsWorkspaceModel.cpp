/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsWorkspaceModel.h>
#include <QDebug>


/*----------------src/mvc/MapsWorkspaceModel.cpp \-----------------------------------------------------------*/

MapsWorkspaceModel::MapsWorkspaceModel()
{

    _is_loaded = false;
    _dir = nullptr;

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

        _is_loaded &= _load_fit_params();

        _is_loaded &= _load_img_dat();
    }
    catch (std::string& s)
    {
        throw s;
    }
    catch (...)
    {
        throw std::string("Failed to open Maps Analyzed dataset!");
    }

    return _is_loaded;
}


bool MapsWorkspaceModel::_load_fit_params()
{

    std::string dataset_dir = _dir->absolutePath().toStdString() + "/";
    for(size_t detector_num = 0; detector_num <= 3; detector_num++)
    {
        data_struct::xrf::Params_Override params_override(dataset_dir, detector_num);
        if( io::load_override_params(dataset_dir, detector_num, &params_override) )
        {
            _fit_params_override_dict[detector_num] = params_override;
        }
    }
    data_struct::xrf::Params_Override params(dataset_dir, -1);
    if( io::load_override_params(dataset_dir, -1, &params) )
    {
        _fit_params_override_dict[-1] = params;
    }


    return true;

}

bool MapsWorkspaceModel::_load_img_dat()
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
        if (fileInfo.suffix() == "h5")
        {
            MapsH5Model * model = new MapsH5Model();
            model->load(fileInfo.absoluteFilePath());
            if(model->is_loaded() )
            {
                _h5_models.insert( {fileInfo.fileName(), model} );
            }
            else
            {
                delete model;
            }
        }
        //std::cout << qPrintable(QString("%1 %2").arg(fileInfo.size(), 10).arg(fileInfo.fileName()));
        //std::cout << std::endl;
    }

    _dir->cd("..");
    return true;
}
