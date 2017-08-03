/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_WORKSPACE_MODEL_H
#define MAPS_WORKSPACE_MODEL_H

/*---------------------------------------------------------------------------*/

#include <QString>
#include "MapsH5Model.h"

/*---------------------------------------------------------------------------*/

/**
 * @brief Model for Maps analyzed hdf5 files
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


private:

    bool _is_loaded;

};


/*---------------------------------------------------------------------------*/

#endif /* MapsWorkspaceModel_H_ */

/*---------------------------------------------------------------------------*/
