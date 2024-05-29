/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/Preferences.h>

#include <QSettings>
#include <QStringList>
#include <QFile>
#include <QMessageBox>

#define INI_FILENAME "uProbeX"
#define INI_DIR "UChicagoArgonneLLC"

Preferences* Preferences::_this_inst(nullptr);
std::mutex Preferences::_mutex;

/*---------------------------------------------------------------------------*/

Preferences::Preferences()
{
    _pref_map = {
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
        {STR_PFR_DETAILED_FIT_SPEC, QVariant()},
        {STR_GRID_ROWS, QVariant()},
        {STR_GRID_COLS, QVariant()},
        {STR_PRF_SHOW_DATASET_ON_FILE_SELECT, QVariant()},
        {STR_SEARCH_SUB_DIR_FOR_DATASETS, QVariant()},
        {STR_GEOMETRY, QVariant()},
        {STR_WINDOWSTATE, QVariant()},
        {STR_PRF_FILE_SIZE, QVariant()},
        {STR_PRF_STRICT_REGEX, QVariant()}
    };
   load();
}

/*---------------------------------------------------------------------------*/

Preferences::~Preferences()
{
    save();
}

/*---------------------------------------------------------------------------*/

Preferences* Preferences::inst()
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_this_inst == nullptr)
    {
        _this_inst = new Preferences();
    }
    return _this_inst;
}

/*---------------------------------------------------------------------------*/

QVariant Preferences::getValue(QString key) const
{

    if (_pref_map.count(key.toStdString()) > 0)
    {
        return _pref_map.at(key.toStdString());
    }
    return QVariant();

}

/*---------------------------------------------------------------------------*/

void Preferences::setValue(QString key, QVariant value)
{

    _pref_map[key.toStdString()] = value;

}

/*---------------------------------------------------------------------------*/

void Preferences::load()
{
    QSettings s(QSettings::IniFormat,
        QSettings::UserScope,
        INI_DIR,
        INI_FILENAME);

    // Write value
    s.beginGroup("Values");
    for(const auto &itr : s.allKeys())
    {
        _pref_map[itr.toStdString()] = s.value(itr);
    }
    s.endGroup();
}

/*---------------------------------------------------------------------------*/

void Preferences::save()
{
    QSettings s(QSettings::IniFormat,
        QSettings::UserScope,
        INI_DIR,
        INI_FILENAME);

    // Write value
    s.beginGroup("Values");
    for (const auto& itr : _pref_map)
    {
        s.setValue(QString(itr.first.c_str()), itr.second);
    }
    s.endGroup();
}

/*---------------------------------------------------------------------------*/
