/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_H5_MODEL_H
#define MAPS_H5_MODEL_H

//---------------------------------------------------------------------------

#include <QString>
#include <QObject>
#include <QImage>
#include <QDir>
#include <QPixmap>
#include <hdf5.h>
#include <unordered_map>
#include <vector>
#include <mutex>
#include "core/defines.h"
#include "io/file/hdf5_io.h"
#include "fitting/routines/param_optimized_fit_routine.h"
#include "fitting/models/gaussian_model.h"
#include "data_struct/stream_block.h"

//---------------------------------------------------------------------------

#define MAX_DETECTORS 30

const std::string QUANT_V9_LOC_MATRIX_STR = "XRF_fits_quant";
const std::string QUANT_V9_LOC_NNLS_STR = "XRF_roi_plus_quant";
const std::string QUANT_V9_LOC_ROI_STR = "XRF_roi_quant";

const std::string QUANT_V10_LOC_MATRIX_STR = "Quantification/Calibration/Fitted";
const std::string QUANT_V10_LOC_NNLS_STR = "Quantification/Calibration/NNLS";
const std::string QUANT_V10_LOC_ROI_STR = "Quantification/Calibration/ROI";
const std::string QUANT_V10_NUM_STANDARDS_STR = "Quantification/Number_Of_Standards";
const std::string QUANT_V10_STANDARD_STR = "Quantification/Standard";

const std::string STR_SR_CURRENT_V9 = "SRcurrent";
const std::string STR_US_IC_V9 = "us_ic";
const std::string STR_DS_IC_V9 = "ds_ic";

const std::string STR_ROI_V9 = "XRF_roi";
const std::string STR_ROI_PLUS_V9 = "XRF_roi_plus";
const std::string STR_FITS_V9 = "XRF_fits";

const std::string STR_MAPS_ROIS_DIR_NAME = "rois";

const QString STR_FULL_IMAGE = "Full Image";
const QString STR_CENTER_2_3_IMAGE = "Center 2/3";
const QString STR_CENTER_1_3_IMAGE = "Center 1/3";
const QString STR_CENTER_1_4_IMAGE = "Center 1/4";
const QString STR_CENTER_1_6_IMAGE = "Center 1/6";

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
    Map_ROI(std::string name_, QColor color_, int color_alpha_, std::vector<std::pair<int, int>> pixel_list_, std::string filename, data_struct::Spectra<double> int_spec_)
    {
        name = name_;
        color = color_;
        color_alpha = color_alpha_;
        pixel_list = pixel_list_;
        int_spec[filename] = int_spec_;
    }
    std::string name;
    std::vector<std::pair<int, int>> pixel_list;
    // file name (usually detector)  and spectra
    std::map<std::string, data_struct::Spectra<double> > int_spec;
    QColor color;
    int color_alpha;
};

struct GenerateImageProp
{
    GenerateImageProp()
    {
        selected_colormap = nullptr;
        normalizer = nullptr;
        calib_curve = nullptr;
        log_color = false;
        show_legend = false;
        invert_y = false;
        global_contrast = true;
        contrast_min = 0.;
        contrast_max = 1.;
    }

    std::string analysis_type;
    std::string element;
    bool log_color;
    bool show_legend;
    bool global_contrast;
    QString contrast_limits;
    bool invert_y;
    float contrast_min;
    float contrast_max;
    QVector<QRgb> *selected_colormap;
    const data_struct::ArrayXXr<float>*  normalizer;
    const Calibration_curve<double>* calib_curve;
};

void gen_insert_order_list(std::vector<std::string> &all_lines);
void gen_insert_order_lists(std::vector<std::string> &element_lines, std::vector<std::string> &scalers_to_add_first, std::vector<std::string>& final_counts_to_add_before_scalers);

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

    const std::map<std::string, data_struct::ArrayXXr<float>>* getScalers() const;

    bool is_fully_loaded() { return _is_fully_loaded; }

    bool getAnalyzedCounts(const std::string& analysis_type, data_struct::Fit_Count_Dict<float>& out_counts) const;

    bool getCountsOrScaler(const std::string& analysis_type, const std::string name, ArrayXXr<float>* out_counts) const;

    bool getAnalyzedQuantified(const std::string& analysis_type, const std::string& quant_type, data_struct::Fit_Count_Dict<float>& out_counts);

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

    const data_struct::Fit_Parameters<double>& getFileFitParams() { return _file_fit_params; }

    std::unordered_map<std::string, ArrayDr*> _fit_int_spec_dict;

    std::unordered_map<std::string, ArrayDr*> _max_chan_spec_dict;

    const data_struct::Scan_Info<double>* getScanInfo() { return &_scan_info; }

    const std::vector<float>& get_x_axis() { return _x_axis; }

    const std::vector<float>& get_y_axis() { return _y_axis; }

    void addRegionLink(QString name, QImage image) { _region_links[name] = image; }

    const std::map<QString, QImage>& regionLinks() const { return _region_links; }

    void clearAllMapRois();

    void appendMapRoi(std::string name, struct Map_ROI roi);

    void saveAllRoiMaps();

    void saveAllRoiMaps(QString save_dir);

    void loadAllRoiMaps();

    const std::unordered_map<std::string, struct Map_ROI >& get_map_rois() { return _map_rois; }

    void generateNameLists(QString analysis_type, std::vector<std::string>& names);

    QStringList get_calibration_curve_scalers(std::string analysis_type);

    const std::vector<data_struct::Quantification_Standard<double> >& quant_standards() { return _quant_standards; }

    const std::unordered_map < std::string, Element_Quant<double>*>& get_quant_fit_info(std::string analysis_type, std::string scaler_name);

    static bool load_x_y_motors_only(QString filepath, data_struct::ArrayTr<float> &x_arr, data_struct::ArrayTr<float> &y_arr);

    //bool gen_image(const std::string analysis_type, const std::string element, bool log_color, bool show_legend, QImage& image, gstar::CountsLookupTransformer* counts_lookup, gstar::CountsStatsTransformer* counts_stats);
    //QImage gen_image(const GenerateImageProp& props, ArrayXXr<float>& normalized);

    QPixmap gen_pixmap(const GenerateImageProp& props,  ArrayXXr<float>& normalized) const;

    bool load_pixel_spectra(const std::pair<int, int>& point, ArrayDr& spectra);

    bool is_polar_xanes_scan();

    bool get_copy_lhcp_rhcp_spectra(data_struct::Spectra<double>& out_lhcp_spectra, data_struct::Spectra<double>& out_rhcp_spectra);

    const data_struct::Spectra<double>* get_lhcp_spectra() {return & _lhcp_integrated_spectra; }

    const data_struct::Spectra<double>* get_rhcp_spectra() {return & _rhcp_integrated_spectra; }

    const std::string& get_polarity_pattern() { return _polarity_pattern_str; }
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

    //Version 10

    bool _load_version_10(hid_t file_id, hid_t maps_grp_id);

    bool _load_quantifier(hid_t grp_id, std::string str_quantifier, std::unordered_map<std::string, Calibration_curve<double> >& quant, std::map<std::string, std::unordered_map<std::string, Element_Quant<double>*>>& e_quants);
    
    bool _load_quantification_10_single(hid_t maps_grp_id, std::string path, std::unordered_map<std::string, Calibration_curve<double> >& quant, std::map<std::string, std::unordered_map<std::string, Element_Quant<double>*>> &e_quants);

    bool _load_quantification_standard_10(hid_t maps_grp_id);

    bool _load_quantification_10(hid_t maps_grp_id);

    bool _load_scalers_10(hid_t maps_grp_id);

    bool _load_scan_10(hid_t maps_grp_id);

    bool _load_integrated_spectra_10(hid_t maps_grp_id);

    bool _load_counts_10(hid_t maps_grp_id);

    bool _load_analyzed_counts_10(hid_t analyzed_grp_id, std::string group_name);

    bool _load_fit_parameters_10(hid_t maps_grp_id);

    bool _load_interferometer_10(hid_t maps_grp_id);

    void _genreate_maps_from_interferometer();
    
    std::string _analysis_enum_to_str(data_struct::Fitting_Routines val);

    std::map<std::string, data_struct::Fit_Count_Dict<float>*> _analyzed_counts;

    data_struct::Spectra<double> _integrated_spectra;

    data_struct::Spectra<double> _lhcp_integrated_spectra;

    data_struct::Spectra<double> _rhcp_integrated_spectra;

    data_struct::ArrayTr<double> _energy_array;

    data_struct::Params_Override<double>* _params_override;

    std::map<std::string, data_struct::ArrayXXr<float>> _scalers;

    data_struct::Fit_Parameters<double> _file_fit_params;

private:
    static std::mutex _mutex;

    QString _filepath;

    QString _datset_name;

    QDir _dir;

    std::string _scan_type_as_str;

    std::string _polarity_pattern_str;

    std::unordered_map<std::string, Calibration_curve<double> > _quant_map_matrix;

    std::unordered_map<std::string, Calibration_curve<double> > _quant_map_nnls;

    std::unordered_map<std::string, Calibration_curve<double> > _quant_map_roi;

    std::unordered_map<std::string, struct Map_ROI > _map_rois;

    std::map<QString, QImage> _region_links;

    data_struct::Scan_Info<double> _scan_info;

    std::vector<float> _x_axis;

    std::vector<float> _y_axis;

    std::vector<data_struct::Quantification_Standard<double> > _quant_standards;

    data_struct::ArrayXXr<float> _interferometer_arr;

    unsigned int _requested_rows;

    unsigned int _requested_cols;

    //  proc_type          quantifier            element    quant_prop
    std::map<std::string, std::map<std::string, std::unordered_map<std::string, Element_Quant<double>*>>> _all_element_quants;

    float _version;

    bool _initialized_by_stream_block;

    bool _loaded_scalers;

    bool _loaded_quantification;

    bool _loaded_scan;

    bool _loaded_integrated_spectra;

    bool _loaded_counts;

    bool _is_fully_loaded;
};


//---------------------------------------------------------------------------

#endif /* MapsH5Model_H_ */

//---------------------------------------------------------------------------
