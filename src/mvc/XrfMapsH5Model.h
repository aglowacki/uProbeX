/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef XRF_MAPS_H5_MODEL_H
#define XRF_MAPS_H5_MODEL_H

/*---------------------------------------------------------------------------*/

#include <QString>

/*---------------------------------------------------------------------------*/

/**
 * @brief Model for Maps analyzed hdf5 files
 */
class XrfMapsH5Model
{

public:

    /**
    * Constructor
    */
    XrfMapsH5Model();

    /**
    * Destructor
    */
    ~XrfMapsH5Model();

    bool load(QString filepath);

    bool is_loaded() {return _is_loaded;}

protected:


private:

    bool _is_loaded;

};


/*---------------------------------------------------------------------------*/

#endif /* XrfMapsH5Model_H_ */

/*---------------------------------------------------------------------------*/
