#ifndef STR_DEFINES_H
#define STR_DEFINES_H

#include <string>
#include <QString>

const static std::string QUANT_V9_LOC_MATRIX_STR = "XRF_fits_quant";
const static std::string QUANT_V9_LOC_NNLS_STR = "XRF_roi_plus_quant";
const static std::string QUANT_V9_LOC_ROI_STR = "XRF_roi_quant";

const static std::string QUANT_V10_LOC_MATRIX_STR = "Quantification/Calibration/Fitted";
const static std::string QUANT_V10_LOC_NNLS_STR = "Quantification/Calibration/NNLS";
const static std::string QUANT_V10_LOC_ROI_STR = "Quantification/Calibration/ROI";
const static std::string QUANT_V10_NUM_STANDARDS_STR = "Quantification/Number_Of_Standards";
const static std::string QUANT_V10_STANDARD_STR = "Quantification/Standard";

const static std::string STR_SR_CURRENT_V9 = "SRcurrent";
const static std::string STR_US_IC_V9 = "us_ic";
const static std::string STR_DS_IC_V9 = "ds_ic";

const static std::string STR_ROI_V9 = "XRF_roi";
const static std::string STR_ROI_PLUS_V9 = "XRF_roi_plus";
const static std::string STR_FITS_V9 = "XRF_fits";

const static std::string STR_MAPS_ROIS_DIR_NAME = "rois";

const static QString STR_FULL_IMAGE = "Full Image";
const static QString STR_CENTER_2_3_IMAGE = "Center 2/3";
const static QString STR_CENTER_1_3_IMAGE = "Center 1/3";
const static QString STR_CENTER_1_4_IMAGE = "Center 1/4";
const static QString STR_CENTER_1_6_IMAGE = "Center 1/6";

const static QString STR_PROCESS("Per Pixel Process");
const static QString STR_PROCESS_ANALYZED("Per Pixel ReProcess");
const static QString STR_PROCESS_ANALYZED_ALL("Per Pixel ReProcess All");
const static QString STR_BATCH_ROI("Process ROI's");
const static QString STR_H5_EXPORT("hdf5_export");
const static QString STR_GEN_SCAN_AREA("Generate Scan Area");

const static QString QSTR_SlopeX = "SlopeX";
const static QString QSTR_SlopeY = "SlopeY";
const static QString QSTR_InterceptX = "InterceptX";
const static QString QSTR_InterceptY = "InterceptY";
const static QString QSTR_SlopeXY = "SlopeXY";
const static QString QSTR_InterceptXY = "InterceptXY";
const static QString QSTR_SlopeYX = "SlopeYX";
const static QString QSTR_InterceptYX = "InterceptYX";

const static QString STR_KMEANS = QString("KMeans");
const static QString STR_DBSCAN = QString("DBSCAN");
const static QString STR_MANUAL = QString("Manual");
const static QString STR_PLOT_OPTIONS = QString("Plot Options");

const static QString CONST_STATIC_ROI_NAME = QString("ROI");

const static QString STR_GENERAL_FIT = "General";
const static QString STR_HYBRID_FIT = "Hybrid";

#endif