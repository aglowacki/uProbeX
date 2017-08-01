/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_MODEL_H
#define MAPS_MODEL_H

/*---------------------------------------------------------------------------*/

#include <mvc/AbstractWindowModel.h>

#include <QString>
#include "io/file/hdf5_io.h"

/*---------------------------------------------------------------------------*/

/**
 * @brief Model for Maps analyzed hdf5 files
 */
class MapsModel : public AbstractWindowModel
{

public:

    /**
    * Constructor
    */
    MapsModel(QString filepath);

    /**
    * Destructor
    */
    ~MapsModel();

protected:


private:


};


/*---------------------------------------------------------------------------*/

#endif /* MapsModel_H_ */

/*---------------------------------------------------------------------------*/
