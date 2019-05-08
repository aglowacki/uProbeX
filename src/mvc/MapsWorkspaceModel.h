/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_WORKSPACE_MODEL_H
#define MAPS_WORKSPACE_MODEL_H

/*---------------------------------------------------------------------------*/

#include <QString>
#include "MapsH5Model.h"
#include "SWSModel.h"
#include "MDA_Model.h"
#include "io/file/hl_file_io.h"
#include <map>
#include <QDir>

/*---------------------------------------------------------------------------*/

/**
 * @brief Model
 */
class MapsWorkspaceModel : public QObject
{

    Q_OBJECT

public:

    /**
    * Constructor
    */
    MapsWorkspaceModel();

    /**
    * Destructor
    */
    ~MapsWorkspaceModel();

    bool load(QString filepath);

    //bool is_loaded() {return _is_loaded;}

    QString get_directory_name();

    MapsH5Model* getMapsH5Model(QString name);

    MDA_Model* get_MDA_Model(QString name);

    SWSModel* get_SWS_Model(QString name);

    data_struct::Fit_Parameters* getFitParameters(int idx);

    data_struct::Fit_Element_Map_Dict *getElementToFit(int idx);

    data_struct::Params_Override* getParamOverride(int idx);

    const map<QString, QFileInfo>& get_hdf5_file_list() { return _h5_fileinfo_list; }

    const map<QString, QFileInfo>& get_mda_file_list() { return _mda_fileinfo_list; }

    const map<QString, QFileInfo>& get_sws_file_list() { return _sws_fileinfo_list; }

signals:

    void doneLoading();

    void newFitParamsFileLoaded(int);

protected:

    bool _load_fit_params();

    bool _get_filesnames_in_directory(QString sub_dir_name, QList <QString> suffex, map<QString, QFileInfo> *fileinfo_list);

    std::map<QString, MapsH5Model*> _h5_models;
    std::map<QString, MDA_Model*> _mda_models;
    std::map<QString, SWSModel*> _sws_models;

    std::map<int, data_struct::Params_Override> _fit_params_override_dict;

private:

    QList <QString> _all_h5_suffex;
    QList <QString> _mda_suffex;
    QList <QString> _sws_suffex;

    map<QString, QFileInfo> _h5_fileinfo_list;
    map<QString, QFileInfo> _mda_fileinfo_list;
    map<QString, QFileInfo> _sws_fileinfo_list;

    QDir* _dir;

    bool _is_sws_loaded;
    bool _is_mda_loaded;
    bool _is_fit_params_loaded;
    bool _is_imgdat_loaded;
};


/*---------------------------------------------------------------------------*/

#endif /* MapsWorkspaceModel_H_ */

/*---------------------------------------------------------------------------*/
