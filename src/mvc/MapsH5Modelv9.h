/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_H5_MODEL_V9_H
#define MAPS_H5_MODEL_V9_H

/*---------------------------------------------------------------------------*/

#include <QString>

/*---------------------------------------------------------------------------*/

/**
 * @brief Model for Maps analyzed hdf5 files
 */
class MapsH5Modelv9
{

public:

    /**
    * Constructor
    */
    MapsH5Modelv9();

    /**
    * Destructor
    */
    ~MapsH5Modelv9();

    bool load(QString filepath);

    bool is_loaded() {return _is_loaded;}

protected:


private:

    bool _is_loaded;

};


/*---------------------------------------------------------------------------*/

#endif /* MapsH5Modelv9_H_ */

/*---------------------------------------------------------------------------*/
