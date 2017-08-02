/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_MODEL_H
#define MAPS_MODEL_H

/*---------------------------------------------------------------------------*/

#include <QString>
#include "io/file/hdf5_io.h"

/*---------------------------------------------------------------------------*/

/**
 * @brief Model for Maps analyzed hdf5 files
 */
class MapsModel
{

public:

    /**
    * Constructor
    */
    MapsModel();

    /**
    * Destructor
    */
    ~MapsModel();

    bool load(QString filepath);

    bool is_loaded() {return _is_loaded;}

protected:


private:

    bool _is_loaded;

};


/*---------------------------------------------------------------------------*/

#endif /* MapsModel_H_ */

/*---------------------------------------------------------------------------*/
