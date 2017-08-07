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
class MapsWorkspaceModel
{

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

    bool is_loaded() {return _is_loaded;}

protected:

    bool _load_fit_params();

    bool _load_img_dat();


    std::map<QString, MapsH5Model*> _h5_models;

    std::map<int, data_struct::xrf::Params_Override> _fit_params_override_dict;

private:

    QDir* _dir;

    bool _is_loaded;

};


/*---------------------------------------------------------------------------*/

#endif /* MapsWorkspaceModel_H_ */

/*---------------------------------------------------------------------------*/
