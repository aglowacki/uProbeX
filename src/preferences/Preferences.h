/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef TXM_PREFERENCES_H
#define TXM_PREFERENCES_H

/*---------------------------------------------------------------------------*/

#include <QMap>
#include <QString>
#include <QVariant>
#include <QObject>
#include <QXmlStreamWriter>

/*---------------------------------------------------------------------------*/

/**
 * @brief Read and save preferences between application restarts, the vaule key
 * is divided into PVList, EnergyFlux, MirrorEnery, HISTBins, which is
 * corresponding to the preference dialog. Preferences settings are recorded
 * using QSetting class.
 */
class Preferences
: public QObject
{

   Q_OBJECT

public:

   /**
    * List of enumerated values that may be stored in the application's
    * preferences file.
    */
   enum ValueKey {
      NMCoefficient,                 /*!< NMCoefficient. */
      NMOptions,
      ProfileList,
      ProfilePythonSolverName ,/*!< Path to the solver python script */
      ProfilePythonCoefficient,
      ProfilePythonOptions,
      ProfileIndex,
      PythonSolverName,
      PythonCoefficient,
      PythonOptions,
      SolverCheckedID,
      FontSize,                   /*!< Applicaiton wide font size. */
      RulerUnitLabel,             /*!< Ruler unit label. */
      RulerUnitX,                 /*!< Ruler unit x. */
      RulerUnitY,                 /*!< Ruler unit y. */
      RegionMenuList,            /*!< List of region menu items. */
      MainWindowSavedWidth,      /*!< Saved width for main window */
      MainWindowSavedHeight,     /*!< Saved height for main window */
      WindowTitle,                /*!< Title to display for window */
      DecimalPrecision,            /*!< Digits after the decimal point */
      MicroProbeXPv,
      MicroProbeYPv,
      AutoSaveRecoveryEnable,
      AutoSaveRecoveryEveryMiliseconds,
      AutoSaveOnExit,
      ExportZoomToRegion,
      ExportDirectoryInDataset,
      ExportPrintNameOnExportedImage,
      ExportPrintPxPyOnExportedImage,
      ExportPrintWidthHeightOnExportedImage,
      ExportSelectedXmlOption,
   };

   /**
    * Constructor
    */
   Preferences();

   /**
    * Read a ValueKey from persistent storage.
    *
    * @param key - the ValueKey enum for the key to read
    * @return value for given key as a QVariant
    */
   QVariant readValueKey(Preferences::ValueKey key);  

   /**
    * Save a ValueKey to persistent storage.
    *
    * @param key - the ValueKey enum for the key to read
    * @param value - the value to store as a QVariant
    */
   void saveValueKey(Preferences::ValueKey key, QVariant value);

   /**
    * Fills in the environment preferences within the element name provided of the xml writer.
    *
    * @param writer - used to write the preferences to
    * @param xmlElementName - tag within which the preferneces will be written.
    */
   void getEnvironmentInfo(QXmlStreamWriter* writer, QString xmlElementName);

   /**
    * Reads in xml file within xml elelemnt and compares with currently loaded preferences.
    *
    * @param xmlPath
    * @param xmlElementName
    * @return true when preferences are different from environment
    */
   bool verifyIsPreferencesDifferentInXml(QString xmlPath, QString xmlElementName);

   /**
    * Read the xml file and load the current environment with the preferences stored in file.
    *
    * @param xmlPath
    * @param xmlElementName
    */
   void loadThePreferencesIntoEnvironmentXml(QString xmlPath, QString xmlElementName);

signals:

   /**
    * preferenceChanged
    */
   void preferenceSolverValueChanged();

   /**
     * when preferences are changed programatically and not from within the preferences dialog.
     */
   void preferencesUpdatedInternally();

private:

   /**
    * Read value for a given string key.
    *
    * @param key
    * @return
    */
   QVariant readValueKey(QString key);

   /**
    * Reads the xml preferences and checks differenes or applies the preferences to environment.
    *
    * @param xmlPath
    * @param xmlElementName
    *
    * @return true when the preferences in xml are different from stored.
    */
   bool readPreferencesFromXml(QString xmlPath, QString xmlElementName, bool applyDatasetPreferences);

   /**
    * Fetch the valuekey enum value by using the string representation of the key.
    *
    * @param key - string representation of the key.
    * @return enum representation of the key.
    */
   Preferences::ValueKey getValueKeyFromString(QString key);

   /**
    * Helper method to create value-key map
    */
   void createValueKeyMap();

   /**
    * Map storing value keys and their text descriptions
    */
   QMap<Preferences::ValueKey, QString> m_mapValueKey;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
