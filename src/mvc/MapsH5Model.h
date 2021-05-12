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
#include "core/defines.h"
#include "io/file/hdf5_io.h"
#include "fitting/routines/param_optimized_fit_routine.h"
#include "fitting/optimizers/mpfit_optimizer.h"
#include "fitting/models/gaussian_model.h"
#include "data_struct/stream_block.h"

/*---------------------------------------------------------------------------*/

typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> EMatrixF;

const string QUANT_V9_LOC_MATRIX_STR = "XRF_fits_quant";
const string QUANT_V9_LOC_NNLS_STR = "XRF_roi_plus_quant";
const string QUANT_V9_LOC_ROI_STR = "XRF_roi_quant";

const string QUANT_V10_LOC_MATRIX_STR = "Quantification/Calibration/Fitted";
const string QUANT_V10_LOC_NNLS_STR = "Quantification/Calibration/NNLS";
const string QUANT_V10_LOC_ROI_STR = "Quantification/Calibration/ROI";

const string STR_SR_CURRENT_V9 = "SRcurrent";
const string STR_US_IC_V9 = "us_ic";
const string STR_DS_IC_V9 = "ds_ic";

const string STR_ROI_V9 = "XRF_roi";
const string STR_ROI_PLUS_V9 = "XRF_roi_plus";
const string STR_FITS_V9 = "XRF_fits";

struct Calibration_curve
{
    Calibration_curve()
    {
    }

    Calibration_curve(string name)
    {
        scaler_name = name;
    }

    string scaler_name;
    //            el_name   value
    unordered_map<string, real_t> calib_curve;
};

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

    std::unordered_map<std::string, data_struct::ArrayXXr>* getScalers() { return &_scalers; }

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

    Calibration_curve* get_calibration_curve(string analysis_type, string scaler_name);

    data_struct::Params_Override* getParamOverride(){return _params_override;}

    bool load_roi(const std::vector<QPoint> &roi_list, data_struct::Spectra &spec);

	std::unordered_map<std::string, data_struct::ArrayXr*> _fit_int_spec_dict;

    const data_struct::Scan_Info* getScanInfo() { return &_scan_info; }

signals:
    void model_data_updated();

    void model_int_spec_updated(bool snip_background);

protected:

    //Version 9
    bool _load_version_9(hid_t maps_grp_id);
    //                                                           scaler_name       
    bool _load_quantification_9_single(hid_t maps_grp_id, std::string path, std::unordered_map<std::string, Calibration_curve >& quant);

    bool _load_quantification_9(hid_t maps_grp_id);

    bool _load_scalers_9(hid_t maps_grp_id);

    bool _load_scan_9(hid_t maps_grp_id);

    bool _load_integrated_spectra_9(hid_t maps_grp_id);

    bool _load_counts_9(hid_t maps_grp_id);

    bool _load_analyzed_counts_9(hid_t analyzed_grp_id, std::string group_name);

    bool _load_roi_9(const std::vector<QPoint> &roi_list, data_struct::Spectra &spec);

    //Version 10

    bool _load_version_10(hid_t file_id, hid_t maps_grp_id);

    bool _load_quantification_10_single(hid_t maps_grp_id, std::string path, std::unordered_map<std::string, Calibration_curve >& quant);

    bool _load_quantification_10(hid_t maps_grp_id);

    bool _load_scalers_10(hid_t maps_grp_id);

    bool _load_scan_10(hid_t maps_grp_id);

    bool _load_integrated_spectra_10(hid_t maps_grp_id);

    bool _load_counts_10(hid_t maps_grp_id);

    bool _load_analyzed_counts_10(hid_t analyzed_grp_id, std::string group_name);

    bool _load_roi_10(const std::vector<QPoint> &roi_list, data_struct::Spectra &spec);

    std::string _analysis_enum_to_str(data_struct::Fitting_Routines val);

    std::unordered_map<std::string, data_struct::Fit_Count_Dict*> _analyzed_counts;

    data_struct::Spectra _integrated_spectra;

    data_struct::Params_Override* _params_override;

    std::unordered_map<std::string, data_struct::ArrayXXr> _scalers;

private:

    QString _filepath;

    QString _datset_name;

    std::unordered_map<std::string, Calibration_curve > _quant_map_matrix;

    std::unordered_map<std::string, Calibration_curve > _quant_map_nnls;

    std::unordered_map<std::string, Calibration_curve > _quant_map_roi;

    data_struct::Scan_Info _scan_info;

    float _version;

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
