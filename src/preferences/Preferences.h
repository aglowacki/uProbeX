/*-----------------------------------------------------------------------------
 * Copyright (c) 2020, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PREFERENCES_H
#define PREFERENCES_H

/*---------------------------------------------------------------------------*/

#include <QString>
#include <QVariant>
#include <unordered_map>
#include <mutex>

/*---------------------------------------------------------------------------*/

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
#define STR_PRF_LastIP "LastIP"
#define STR_PRF_LastPort "LastPort"
#define STR_PFR_LOG_10 "Log10YAxis"
#define STR_PFR_DETAILED_FIT_SPEC "DetailedFitSpectra"

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
   QVariant getValue(QString key);  

   /**
    * Save a ValueKey to persistent storage.
    *
    * @param key - the ValueKey enum for the key to read
    * @param value - the value to store as a QVariant
    */
   void setValue(QString key, QVariant value);

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

    std::unordered_map<QString, QVariant> _pref_map {
        {STR_PRF_NMCoefficient, QVariant()},
        {STR_PRF_NMOptions, QVariant()},
        {STR_PRF_ProfileList, QVariant()},
        {STR_PRF_ProfilePythonSolverName, QVariant()},
        {STR_PRF_ProfilePythonCoefficient, QVariant()},
        {STR_PRF_ProfilePythonOptions, QVariant()},
        {STR_PRF_ProfileIndex, QVariant()},
        {STR_PRF_PythonSolverName, QVariant()},
        {STR_PRF_PythonCoefficient, QVariant()},
        {STR_PRF_PythonOptions, QVariant()},
        {STR_PRF_SolverCheckedID, QVariant()},
        {STR_PRF_FontSize, QVariant(14)},
        {STR_PRF_RulerUnitLabel, QVariant()},
        {STR_PRF_RulerUnitX, QVariant()},
        {STR_PRF_RulerUnitY, QVariant()},
        {STR_PRF_RegionMenuList, QVariant()},
        {STR_PRF_MainWindowSavedWidth, QVariant(1024)},
        {STR_PRF_MainWindowSavedHeight, QVariant(768)},
        {STR_PRF_WindowTitle, QVariant()},
        {STR_PRF_DecimalPrecision, QVariant()},
        {STR_PRF_MicroProbeXPv, QVariant()},
        {STR_PRF_MicroProbeYPv, QVariant()},
        {STR_PRF_AutoSaveRecoveryEnable, QVariant()},
        {STR_PRF_AutoSaveRecoveryEveryMiliseconds, QVariant()},
        {STR_PRF_AutoSaveOnExit, QVariant()},
        {STR_PRF_ExportZoomToRegion, QVariant()},
        {STR_PRF_ExportDirectoryInDataset, QVariant()},
        {STR_PRF_ExportPrintNameOnExportedImage, QVariant()},
        {STR_PRF_ExportPrintPxPyOnExportedImage, QVariant()},
        {STR_PRF_ExportPrintWidthHeightOnExportedImage, QVariant()},
        {STR_PRF_ExportSelectedXmlOption, QVariant()},
        {STR_PRF_LastIP, QVariant()},
        {STR_PRF_LastPort, QVariant()},
        {STR_PFR_LOG_10, QVariant(true)},
        {STR_PFR_DETAILED_FIT_SPEC, QVariant()}
    };


};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
