/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_H5_MODEL_H
#define MAPS_H5_MODEL_H

/*---------------------------------------------------------------------------*/

#include <QString>
#include <QObject>
#include <QImage>
#include <QDir>
#include <hdf5.h>
#include <unordered_map>
#include <vector>
#include <mutex>
#include "core/defines.h"
#include "io/file/hdf5_io.h"
#include "fitting/routines/param_optimized_fit_routine.h"
#include "fitting/optimizers/mpfit_optimizer.h"
#include "fitting/models/gaussian_model.h"
#include "data_struct/stream_block.h"

/*---------------------------------------------------------------------------*/

typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> EMatrixF;

const std::string QUANT_V9_LOC_MATRIX_STR = "XRF_fits_quant";
const std::string QUANT_V9_LOC_NNLS_STR = "XRF_roi_plus_quant";
const std::string QUANT_V9_LOC_ROI_STR = "XRF_roi_quant";

const std::string QUANT_V10_LOC_MATRIX_STR = "Quantification/Calibration/Fitted";
const std::string QUANT_V10_LOC_NNLS_STR = "Quantification/Calibration/NNLS";
const std::string QUANT_V10_LOC_ROI_STR = "Quantification/Calibration/ROI";

const std::string STR_SR_CURRENT_V9 = "SRcurrent";
const std::string STR_US_IC_V9 = "us_ic";
const std::string STR_DS_IC_V9 = "ds_ic";

const std::string STR_ROI_V9 = "XRF_roi";
const std::string STR_ROI_PLUS_V9 = "XRF_roi_plus";
const std::string STR_FITS_V9 = "XRF_fits";

template <typename T_real>
struct Calibration_curve
{
    Calibration_curve()
    {
    }

    Calibration_curve(std::string name)
    {
        scaler_name = name;
    }

    std::string scaler_name;
    //            el_name   value
    std::unordered_map<std::string, T_real> calib_curve;
};

using ArrayDr = data_struct::ArrayTr<double>;

struct Map_ROI
{
    Map_ROI()
    {

    }
    Map_ROI(std::string name_, std::vector<std::pair<unsigned int, unsigned int>> pixel_list_, data_struct::Spectra<double> int_spec_)
    {
        name = name;
        pixel_list = pixel_list_;
        int_spec = int_spec_;
    }
    std::string name;
    std::vector<std::pair<unsigned int, unsigned int>> pixel_list;
    data_struct::Spectra<double> int_spec;
    QColor color;
    int color_alpha;
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

    std::map<std::string, data_struct::ArrayXXr<float>>* getScalers() { return &_scalers; }

    bool is_fully_loaded() { return _is_fully_loaded; }

    //data_struct::Fit_Count_Dict* getAnalyzedCounts(std::string analysis_type);
    void getAnalyzedCounts(std::string analysis_type, data_struct::Fit_Count_Dict<float>& out_counts);

    //const data_struct::Spectra* getIntegratedSpectra() {return &_integrated_spectra;}
    void getIntegratedSpectra(data_struct::Spectra<double>& out_spectra);

    QString getFilePath() { return _filepath; }

    QString getDatasetName() { return _datset_name; }

    QDir getDir() { return _dir; }

    void initialize_from_stream_block(data_struct::Stream_Block<float>* block);

    void update_from_stream_block(data_struct::Stream_Block<float>* block);

    bool is_scalers_loaded() { return _loaded_scalers; }

    bool is_quantification_loaded() { return _loaded_quantification; }

    bool is_scan_loaded() { return _loaded_scan; }

    bool is_integrated_spectra_loaded() { return _loaded_integrated_spectra; }

    bool is_counts_loaded() { return _loaded_counts; }

    std::vector<std::string> getAnalyzedTypes();

    void set_fit_parameters_override(data_struct::Params_Override<double>* override);

    Calibration_curve<double>* get_calibration_curve(std::string analysis_type, std::string scaler_name);

    data_struct::Params_Override<double>* getParamOverride() { return _params_override; }

    bool load_roi(const std::vector<QPoint>& roi_list, data_struct::Spectra<double>& spec);

    std::unordered_map<std::string, ArrayDr*> _fit_int_spec_dict;

    std::unordered_map<std::string, ArrayDr*> _max_chan_spec_dict;

    const data_struct::Scan_Info<double>* getScanInfo() { return &_scan_info; }

    const std::vector<float>& get_x_axis() { return _x_axis; }

    const std::vector<float>& get_y_axis() { return _y_axis; }

    void addRegionLink(QString name, QImage image) { _region_links[name] = image; }

    const std::map<QString, QImage>& regionLinks() const { return _region_links; }

    void clearAllMapRois();

    void appendMapRoi(std::string name, struct Map_ROI roi);

signals:
    void model_data_updated();

    void model_int_spec_updated(bool snip_background);

protected:

    //Version 9
    bool _load_version_9(hid_t maps_grp_id);
    //                                                           scaler_name       
    bool _load_quantification_9_single(hid_t maps_grp_id, std::string path, std::unordered_map<std::string, Calibration_curve<double> >& quant);

    bool _load_quantification_9(hid_t maps_grp_id);

    bool _load_scalers_9(hid_t maps_grp_id);

    bool _load_scan_9(hid_t maps_grp_id);

    bool _load_integrated_spectra_9(hid_t maps_grp_id);

    bool _load_counts_9(hid_t maps_grp_id);

    bool _load_analyzed_counts_9(hid_t analyzed_grp_id, std::string group_name);

    bool _load_roi_9(const std::vector<QPoint>& roi_list, data_struct::Spectra<double>& spec);

    //Version 10

    bool _load_version_10(hid_t file_id, hid_t maps_grp_id);

    bool _load_quantification_10_single(hid_t maps_grp_id, std::string path, std::unordered_map<std::string, Calibration_curve<double> >& quant);

    bool _load_quantification_10(hid_t maps_grp_id);

    bool _load_scalers_10(hid_t maps_grp_id);

    bool _load_scan_10(hid_t maps_grp_id);

    bool _load_integrated_spectra_10(hid_t maps_grp_id);

    bool _load_counts_10(hid_t maps_grp_id);

    bool _load_analyzed_counts_10(hid_t analyzed_grp_id, std::string group_name);

    bool _load_roi_10(const std::vector<QPoint>& roi_list, data_struct::Spectra<double>& spec);

    std::string _analysis_enum_to_str(data_struct::Fitting_Routines val);

    std::map<std::string, data_struct::Fit_Count_Dict<float>*> _analyzed_counts;

    data_struct::Spectra<double> _integrated_spectra;

    data_struct::Params_Override<double>* _params_override;

    std::map<std::string, data_struct::ArrayXXr<float>> _scalers;

private:
    std::mutex _mutex;

    QString _filepath;

    QString _datset_name;

    QDir _dir;

    std::unordered_map<std::string, Calibration_curve<double> > _quant_map_matrix;

    std::unordered_map<std::string, Calibration_curve<double> > _quant_map_nnls;

    std::unordered_map<std::string, Calibration_curve<double> > _quant_map_roi;

    std::unordered_map<std::string, struct Map_ROI > _map_rois;

    std::map<QString, QImage> _region_links;

    data_struct::Scan_Info<double> _scan_info;

    std::vector<float> _x_axis;
    std::vector<float> _y_axis;

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
