/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_H5_MODEL_H
#define MAPS_H5_MODEL_H

/*---------------------------------------------------------------------------*/

#include <QString>
#include <QObject>
#include <hdf5.h>
#include <unordered_map>
#include <vector>
#include "io/file/hdf5_io.h"
#include "fitting/routines/param_optimized_fit_routine.h"
#include "fitting/optimizers/mpfit_optimizer.h"
#include "fitting/models/gaussian_model.h"
#include "data_struct/stream_block.h"

/*---------------------------------------------------------------------------*/

typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> EMatrixF;

/**
 * @brief Model for Maps analyzed hdf5 files
 */
class MapsH5Model : public QObject
{

    Q_OBJECT

public:

    /**
    * Constructor
    */
    MapsH5Model();

    /**
    * Destructor
    */
    ~MapsH5Model();

    void clear_analyzed_counts();

    bool load(QString filepath);

    std::vector<std::string> count_names();

    bool is_fully_loaded() {return _is_fully_loaded;}

    data_struct::Fit_Count_Dict* getAnalyzedCounts(std::string analysis_type);

    const data_struct::Spectra* getIntegratedSpectra() {return &_integrated_spectra;}

    QString getFilePath() { return _filepath; }

    QString getDatasetName() { return _datset_name; }

    void initialize_from_stream_block(data_struct::Stream_Block* block);

    void update_from_stream_block(data_struct::Stream_Block* block);

    bool is_scalers_loaded() { return _loaded_scalers; }

    bool is_quantification_loaded() { return _loaded_quantification; }

    bool is_scan_loaded() { return _loaded_scan; }

    bool is_integrated_spectra_loaded() { return _loaded_integrated_spectra; }

    bool is_counts_loaded() { return _loaded_counts; }

    std::vector<std::string> getAnalyzedTypes();

    void set_fit_parameters_override(data_struct::Params_Override* override);

    data_struct::Params_Override* getParamOverride(){return _params_override;}

signals:
    void model_data_updated();

    void model_int_spec_updated(bool snip_background);

protected:

    //Version 9
    bool _load_version_9(hid_t maps_grp_id);

    bool _load_quantification_9(hid_t maps_grp_id);

    bool _load_scalers_9(hid_t maps_grp_id);

    bool _load_scan_9(hid_t maps_grp_id);

    bool _load_integrated_spectra_9(hid_t maps_grp_id);

    bool _load_counts_9(hid_t maps_grp_id);

    bool _load_analyzed_counts_9(hid_t analyzed_grp_id, std::string group_name);

    //Version 10

    bool _load_version_10(hid_t file_id, hid_t maps_grp_id);

    bool _load_quantification_10(hid_t maps_grp_id);

    bool _load_scalers_10(hid_t maps_grp_id);

    bool _load_scan_10(hid_t maps_grp_id);

    bool _load_integrated_spectra_10(hid_t maps_grp_id);

    bool _load_counts_10(hid_t maps_grp_id);

    bool _load_analyzed_counts_10(hid_t analyzed_grp_id, std::string group_name);

    std::string _analysis_enum_to_str(int val);

    std::unordered_map<std::string, data_struct::Fit_Count_Dict*> _analyzed_counts;

    data_struct::Spectra _integrated_spectra;

    data_struct::Params_Override* _params_override;

private:

    QString _filepath;

    QString _datset_name;

    bool _initialized_by_stream_block;

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
