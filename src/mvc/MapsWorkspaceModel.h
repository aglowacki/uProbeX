/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_WORKSPACE_MODEL_H
#define MAPS_WORKSPACE_MODEL_H

/*---------------------------------------------------------------------------*/

#include <QString>
#include "MapsH5Model.h"
#include "io/hl_file_io.h"
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

    bool load(QString filepath, bool all=false);

    //bool is_loaded() {return _is_loaded;}

    QString get_directory_name();

    MapsH5Model* getMapsH5Model(QString name);

signals:
    void newFileLoaded(QString);

protected:

    bool _load_fit_params();

    bool _load_img_dat(QList <QString> suffex);

    bool _load_sws_workspaces();

    std::map<QString, MapsH5Model*> _h5_models;

    std::map<int, data_struct::xrf::Params_Override> _fit_params_override_dict;

private:

    QList <QString> _all_h5_suffex;
    QList <QString> _avg_h5_suffex;

    QDir* _dir;

    bool _is_sws_loaded;
    bool _is_fit_params_loaded;
    bool _is_imgdat_loaded;
};


/*---------------------------------------------------------------------------*/

#endif /* MapsWorkspaceModel_H_ */

/*---------------------------------------------------------------------------*/
