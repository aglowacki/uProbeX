/*-----------------------------------------------------------------------------
 * Copyright (c) 2020, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PREFERENCES_H
#define PREFERENCES_H

//---------------------------------------------------------------------------

#include <QString>
#include <QVariant>
#include <unordered_map>
#include <mutex>

// Fix open dialog not working for MacOS when building bundle. 
// Tried entitlements but still didn't work with native dialog
#ifdef __APPLE__
    #define FILE_DIALOG_OPTIONS QFileDialog::DontUseNativeDialog
#else
    #define FILE_DIALOG_OPTIONS QFileDialog::Option(0)
#endif

//---------------------------------------------------------------------------
#define STR_SCAN_TYPE "Scan Type"
#define STR_Region_Box_Top_Y "Region Box Top Y"
#define STR_Region_Box_Left_X "Region Box Left X"
#define STR_Region_Box_Right_X "Region Box Right X"
#define STR_Region_Box_Bottom_Y "Region Box Bottom Y"
#define STR_Region_Box_Center_X "Region Box Center X"
#define STR_Region_Box_Center_Y "Region Box Center Y"
#define STR_Region_Box_Width "Region Box Width"
#define STR_Region_Box_Height "Region Box Height"

#define STR_AUTO_SAVE_SCAN_REGION_NAME "scan_regions_auto_save.json"
#define STR_SCAN_REGIONS " ScanRegions"
#define STR_REGION_PLAN " ScanRegionPlan"
#define STR_SCAN_REGION_PLAN_NAME "ScanRegionPlanName"
#define STR_SCAN_REGION_PLAN_UUID "ScanRegionPlanUUID"
#define STR_SCAN_REGION_PLAN_TYPE "ScanRegionPlanType"
#define STR_SCAN_REGION_PLAN_RESULT "ScanRegionPlanResult"
#define STR_SCAN_REGION_PLAN_DESC "ScanRegionPlanDesc"
#define STR_SCAN_REGION_PLAN_FILENAME "ScanRegionPlanFilename"
#define STR_SCAN_REGION_PLAN_MODULE "ScanRegionPlanModule"
#define STR_SCAN_REGION_PLAN_USER "ScanRegionPlanUser"
#define STR_SCAN_REGION_PLAN_PARAMS "ScanRegionPlanParameters"

#define STR_LIVE_BACKGROUND_PATH "LiveBackgroundPath"
#define STR_LIVE_BACKGROUND_ELEMENT "LiveBackgroundElement"

#define STR_SCAN_REGION_PLAN_PARAM_NAME "Name"
#define STR_SCAN_REGION_PLAN_PARAM_KIND "Kind"
#define STR_SCAN_REGION_PLAN_PARAM_VAL "Value"
#define STR_SCAN_REGION_PLAN_PARAM_DESC "Description"

#define STR_SCAN_REGION_PLAN_RESULT_EXIT_STATUS "ExitStatus"
#define STR_SCAN_REGION_PLAN_RESULT_RUN_UUIDS "RunUUIDs"
#define STR_SCAN_REGION_PLAN_RESULT_TIME_START "TimeStart"
#define STR_SCAN_REGION_PLAN_RESULT_TIME_STOP "TimeStop"
#define STR_SCAN_REGION_PLAN_RESULT_MSG "Msg"
#define STR_SCAN_REGION_PLAN_RESULT_TRACEBACK "Traceback"

#define STR_PRF_NMCoefficient "NMCoefficient"
#define STR_PRF_NMOptions "NMOptions"
#define STR_PRF_ProfileList "ProfileList"
#define STR_PRF_ProfilePythonSolverName "ProfilePythonSolverName"
#define STR_PRF_ProfilePythonCoefficient "ProfilePythonCoefficient"
#define STR_PRF_ProfilePythonOptions "ProfilePythonOptions"
#define STR_PRF_ProfileIndex "ProfileIndex"
#define STR_PRF_PythonSolverName "PythonSolverName"
#define STR_PRF_PythonCoefficient "PythonCoefficient"
#define STR_PRF_PythonOptions "PythonOptions"
#define STR_PRF_SolverCheckedID "SolverCheckedID"
#define STR_PRF_FontSize "FontSize"
#define STR_PRF_RulerUnitLabel "RulerUnitLabel"
#define STR_PRF_RulerUnitX "RulerUnitX"
#define STR_PRF_RulerUnitY "RulerUnitY"
#define STR_PRF_RegionMenuList "RegionMenuList"
#define STR_PRF_MainWindowSavedWidth "MainWindowSavedWidth"
#define STR_PRF_MainWindowSavedHeight "MainWindowSavedHeight"
#define STR_PRF_WindowTitle "WindowTitle"
#define STR_PRF_DecimalPrecision "DecimalPrecision"
#define STR_PRF_MicroProbeXPv "MicroProbeXPv"
#define STR_PRF_MicroProbeYPv "MicroProbeYPv"
#define STR_PRF_AutoSaveRecoveryEnable "AutoSaveRecoveryEnable"
#define STR_PRF_AutoSaveRecoveryEveryMiliseconds "AutoSaveRecoveryEveryMiliseconds"
#define STR_PRF_AutoSaveOnExit "AutoSaveOnExit"
#define STR_PRF_ExportZoomToRegion "ExportZoomToRegion"
#define STR_PRF_ExportDirectoryInDataset "ExportDirectoryInDataset"
#define STR_PRF_ExportPrintNameOnExportedImage "ExportPrintNameOnExportedImage"
#define STR_PRF_ExportPrintPxPyOnExportedImage "ExportPrintPxPyOnExportedImage"
#define STR_PRF_ExportPrintWidthHeightOnExportedImage "ExportPrintWidthHeightOnExportedImage"
#define STR_PRF_ExportSelectedXmlOption "ExportSelectedXmlOption"
#define STR_PRF_LastQServerIP "LastQServerIP"
#define STR_PRF_LastIP "LastIP"
#define STR_PRF_LastPort "LastPort"
#define STR_PFR_LOG_10 "Log10YAxis"
#define STR_PFR_DETAILED_FIT_SPEC "DetailedFitSpectra"
#define STR_PFR_SHOW_FIT_INT_MATRIX "ShowFitIntMatrix"
#define STR_PFR_SHOW_FIT_INT_NNLS "ShowFitIntNNLS"
#define STR_PFR_SHOW_MAX_CHAN_SPEC "ShowMaxChanSpectra"
#define STR_GRID_ROWS "GridRows"
#define STR_GRID_COLS "GridCols"
#define STR_COLORMAP "ColorMap"
#define STR_COLORMAP_GRAY "Grayscale"
#define STR_COLORMAP_HEAT "Heatmap"
#define STR_RECENT_MAPS_WORKSPACES "Recent_MAPS_Workspaces"
#define STR_LOG_SCALE_COLOR "Color_Scale_Log"
#define STR_DISPLAY_COLOR_LEDGEND "Display_Color_Ledgend"
#define STR_INVERT_Y_AXIS "Invert_Y_Axis"
#define STR_PFR_THEME "Theme"
#define STR_PFR_SPECTRA_BLACK_BG "SPECTRA_BLACK_BG"
#define STR_PRF_ScatterPlot_NumWindows "ScatterPlot_NumWindows"
#define STR_PRF_ScatterPlot_Log10 "ScatterPlot_Log10"
#define STR_PFR_MARKER_SHAPE "ScatterPlotMarkerShape"
#define STR_PFR_SCATTER_DARK_BACKGROUND "ScatterPlotDarkBackground"
#define STR_PRF_ScatterPlot_GridLines "ScatterPlotGridLines"
#define STR_PRF_ScatterPlot_Size "ScatterPlotSize"
#define STR_DEFAULT "Default"
#define STR_PRF_SHOW_DATASET_ON_FILE_SELECT "ShowDatasetOnFileSelect" // if you select file name on left side, it will change the current viewed dataset to that if it is loaded
#define STR_SEARCH_SUB_DIR_FOR_DATASETS "SearchSubDirForDatasets"
#define STR_GEOMETRY "Geometry"
#define STR_WINDOWSTATE "WindowState"
#define STR_MAPS_WIDGET_GEOMETRY "MapsWidgetGeometry"
#define STR_COUNTS_DOCK "CountsDock"
#define STR_INTSPEC_DOCK "IntSpecDock"
#define STR_POLAR_DOCK "PolarXanesDock"
#define STR_QUANT_DOCK "QuantDock"
#define STR_COLOR_DOCK "ColorDock"
#define STR_SCATTER_DOCK "ScatterDock"
#define STR_EXTRA_DOCK "ExtraDock"
#define STR_PRF_STRICT_REGEX "StrictRegEx"
#define STR_PRF_FILE_SIZE "FileSize"
#define STR_PRF_SHOW_DATASET_ON_LOAD "ShowOnLoad"
#define STR_PREF_RADIO_LOAD_SELECTED_OPTION "Load_Selected_Opt"
#define STR_PREF_SPRECTRA_CONTROLS_HORIZONTAL_OPTION "Spectra_Controls_Horizontal"
#define STR_SAVE_QSERVER_HISTORY_LOCATION "Save_Qserver_History_Location"
#define STR_PRF_LiveCoefficientNames "LiveCoefficientName"
#define STR_PRF_LiveCoefficientVals "LiveCoefficientVals"
#define STR_PREF_LAST_SCAN_LINK_SELECTED "Last_Scan_Link_Selected"
#define STR_PFR_SHOW_SCAN_QUEUE_HEADER "Show_Scan_Queue_Header"
#define STR_PREF_SCAN_LINK_PROFILES  "Scan_Link_Profiles"
#define STR_PFR_USE_OPENGL "Use_OpenGL"
#define STR_PRF_COMPACT_COUNTS_VIEW "Counts_Compact_View"
#define STR_PFR_SELECTED_ELEMENTS "Selected_Elements"
#define STR_PREF_SAVED_CONTRAST "Saved_Contrast"


/**
 * @brief Read and save preferences between application restarts, the vaule key
 * is divided into PVList, EnergyFlux, MirrorEnery, HISTBins, which is
 * corresponding to the preference dialog. Preferences settings are recorded
 * using QSetting class.
 */
class Preferences 
{

public:


   static Preferences* inst();

   ~Preferences();

   /**
    * Read a ValueKey from persistent storage.
    *
    * @param key - the ValueKey enum for the key to read
    * @return value for given key as a QVariant
    */
   QVariant getValue(QString key) const;  

   /**
    * Save a ValueKey to persistent storage.
    *
    * @param key - the ValueKey enum for the key to read
    * @param value - the value to store as a QVariant
    */
   void setValue(QString key, QVariant value);

   void checkAndSet(QString key, QVariant value);

   /**
   * load from file
   */
   void load();

   /**
   * save to file
   */
   void save();


private:

    /**
     * Constructor
     */
    Preferences();

    static Preferences* _this_inst;

    static std::mutex _mutex;

    std::unordered_map<std::string, QVariant> _pref_map;
};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
