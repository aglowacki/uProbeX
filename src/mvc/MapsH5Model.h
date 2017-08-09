/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_H5_MODEL_H
#define MAPS_H5_MODEL_H

/*---------------------------------------------------------------------------*/

#include <QString>
#include <hdf5.h>
#include <unordered_map>
#include <vector>
#include "XrfAnalyzedCounts.h"
#include "io/file/hdf5_io.h"

/*---------------------------------------------------------------------------*/


/**
 * @brief Model for Maps analyzed hdf5 files
 */
class MapsH5Model
{

public:

    /**
    * Constructor
    */
    MapsH5Model();

    /**
    * Destructor
    */
    ~MapsH5Model();

    bool load(QString filepath);

    bool is_fully_loaded() {return _is_fully_loaded;}

    XrfAnalyzedCounts* getAnalyzedCounts(std::string analysis_type);

    data_struct::xrf::Spectra* getIntegratedSpectra() {return &_integrated_spectra;}

    QString getFilePath() { return _filepath; }

protected:

    bool _load_version_9(hid_t maps_grp_id);

    bool _load_version_10(hid_t file_id, hid_t maps_grp_id);

    bool _load_quantification_10(hid_t maps_grp_id);

    bool _load_scalers_10(hid_t maps_grp_id);

    bool _load_scan_10(hid_t maps_grp_id);

    bool _load_integrated_spectra_10(hid_t maps_grp_id);

    bool _load_counts_10(hid_t maps_grp_id);

    bool _load_analyzed_counts(hid_t analyzed_grp_id, std::string group_name);

    std::unordered_map<std::string, XrfAnalyzedCounts*> _analyzed_counts;

    data_struct::xrf::Spectra _integrated_spectra;

private:

    QString _filepath;

    bool _loaded_scalers;
    bool _loaded_quantification;
    bool _loaded_scan;
    bool _loaded_integrated_spectra;
    bool _loaded_counts;

    bool _is_fully_loaded;

};


/*---------------------------------------------------------------------------*/

#endif /* MapsH5Model_H_ */

/*---------------------------------------------------------------------------*/
