/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef RAW_H5_MODEL_H
#define RAW_H5_MODEL_H

//---------------------------------------------------------------------------

#include <QString>
#include <QObject>
#include <hdf5.h>
#include <unordered_map>
#include <vector>
#include "io/file/hdf5_io.h"
#include "fitting/routines/param_optimized_fit_routine.h"
#include "fitting/models/gaussian_model.h"
#include "data_struct/stream_block.h"

//---------------------------------------------------------------------------
/**
 * @brief Model for Maps analyzed hdf5 files
 */
class RawH5Model : public QObject
{

    Q_OBJECT

public:

    /**
    * Constructor
    */
    RawH5Model();

    /**
    * Destructor
    */
    ~RawH5Model();

    void clear_analyzed_counts();

    bool load(QString filepath);

    std::vector<std::string> count_names();

    bool is_fully_loaded() {return _is_fully_loaded;}

    bool _is_fully_loaded;
};


//---------------------------------------------------------------------------

#endif /* RawH5Model_H_ */

//---------------------------------------------------------------------------
