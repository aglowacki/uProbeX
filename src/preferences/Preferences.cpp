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

/*---------------------------------------------------------------------------*/

Preferences::Preferences()
: QObject(0)
{

   // Create map of value keys to string names
   createValueKeyMap();

}

/*---------------------------------------------------------------------------*/

void Preferences::createValueKeyMap()
{

   // Solver parameter list
   m_mapValueKey.insert(NMCoefficient, "NMCoefficient");
   m_mapValueKey.insert(NMOptions, "NMOptions");

   m_mapValueKey.insert(ProfileList, "ProfileList");
   m_mapValueKey.insert(ProfilePythonSolverName, "ProfilePythonSolverName");
   m_mapValueKey.insert(ProfilePythonCoefficient, "ProfilePythonCoefficient");
   m_mapValueKey.insert(ProfilePythonOptions, "ProfilePythonOptions");

   m_mapValueKey.insert(ProfileIndex, "ProfileIndex");
   m_mapValueKey.insert(PythonSolverName, "PythonSolverName");
   m_mapValueKey.insert(PythonCoefficient, "PythonCoefficient");
   m_mapValueKey.insert(PythonOptions, "PythonOptions");

   m_mapValueKey.insert(SolverCheckedID, "SolverCheckedID");

   m_mapValueKey.insert(FontSize, "FontSize");

   m_mapValueKey.insert(RulerUnitLabel, "RulerUnitLabel");
   m_mapValueKey.insert(RulerUnitX, "RulerUnitX");
   m_mapValueKey.insert(RulerUnitY, "RulerUnitY");

   m_mapValueKey.insert(RegionMenuList, "RegionMenuList");

   m_mapValueKey.insert(MainWindowSavedWidth, "MainWindowSavedWidth");
   m_mapValueKey.insert(MainWindowSavedHeight, "MainWindowSavedHeight");

   m_mapValueKey.insert(WindowTitle, "WindowTitle");

   m_mapValueKey.insert(DecimalPrecision, "DecimalPrecision");

   m_mapValueKey.insert(MicroProbeXPv, "MicroProbeXPv");
   m_mapValueKey.insert(MicroProbeYPv, "MicroProbeYPv");

   m_mapValueKey.insert(AutoSaveOnExit, "AutoSaveOnExit");
   m_mapValueKey.insert(AutoSaveRecoveryEnable, "AutoSaveRecoveryEnable");
   m_mapValueKey.insert(AutoSaveRecoveryEveryMiliseconds, "AutoSaveRecoveryEveryMiliseconds");

   m_mapValueKey.insert(ExportZoomToRegion, "ExportZoomToRegion");
   m_mapValueKey.insert(ExportDirectoryInDataset, "ExportDirectoryInDataset");
   m_mapValueKey.insert(ExportPrintNameOnExportedImage,"ExportPrintNameOnExportedImage");
   m_mapValueKey.insert(ExportPrintPxPyOnExportedImage, "ExportPrintPxPyOnExportedImage");
   m_mapValueKey.insert(ExportPrintWidthHeightOnExportedImage, "ExportPrintWidthHeightOnExportedImage");
   m_mapValueKey.insert(ExportSelectedXmlOption, "ExportSelectedXmlOption");

   m_mapValueKey.insert(LastIP, "LastIP");
   m_mapValueKey.insert(LastPort, "LastPort");
}

/*---------------------------------------------------------------------------*/

QVariant Preferences::readValueKey(Preferences::ValueKey key)
{
   return readValueKey(m_mapValueKey[key]);
}

/*---------------------------------------------------------------------------*/

QVariant Preferences::readValueKey(QString key)
{
   // Settings
   QSettings s(QSettings::IniFormat,
               QSettings::UserScope,
               INI_DIR,
               INI_FILENAME);

   // Read value
   s.beginGroup("Values");
   QVariant value = s.value(key);
   s.endGroup();

   // Return value
   return value;
}

/*---------------------------------------------------------------------------*/

void Preferences::saveValueKey(Preferences::ValueKey key, QVariant value)
{
   // Settings
   QSettings s(QSettings::IniFormat,
               QSettings::UserScope,
               INI_DIR,
               INI_FILENAME);

   // Write value
   s.beginGroup("Values");
   s.setValue(m_mapValueKey[key], value);
   s.endGroup();

   if(key == NMCoefficient
         || key == NMOptions
         || key == PythonCoefficient
         || key == PythonOptions )
   {
        emit preferenceSolverValueChanged();
   }
}

/*---------------------------------------------------------------------------*/

Preferences::ValueKey Preferences::getValueKeyFromString(QString key)
{
   QMapIterator<Preferences::ValueKey, QString> valueKeyIttr(m_mapValueKey);

   while (valueKeyIttr.hasNext())
   {
      valueKeyIttr.next();
      if (valueKeyIttr.value() == key)
      {
         return valueKeyIttr.key();
      }
   }
}

/*---------------------------------------------------------------------------*/

void Preferences::getEnvironmentInfo(QXmlStreamWriter* writer, QString xmlElementName)
{
   writer->writeStartElement(xmlElementName);

   QMapIterator<Preferences::ValueKey, QString> valueKeyIttr(m_mapValueKey);

   while (valueKeyIttr.hasNext())
   {
      valueKeyIttr.next();
      QString keyValue = valueKeyIttr.value();
      QString value = readValueKey(valueKeyIttr.key()).toString();

      writer->writeStartElement("preference");
      writer->writeAttribute("key", keyValue);
      writer->writeCharacters(value);
      writer->writeEndElement();
   }

   writer->writeEndElement();
}

/*---------------------------------------------------------------------------*/

void Preferences::loadThePreferencesIntoEnvironmentXml(QString xmlPath, QString xmlElementName)
{
   readPreferencesFromXml(xmlPath, xmlElementName, true);
   emit preferencesUpdatedInternally();
}

/*---------------------------------------------------------------------------*/

bool Preferences::verifyIsPreferencesDifferentInXml(QString xmlPath, QString xmlElementName)
{
   return readPreferencesFromXml(xmlPath, xmlElementName, false);
}

/*---------------------------------------------------------------------------*/

bool Preferences::readPreferencesFromXml(QString xmlPath, QString xmlElementName, bool applyDatasetPreferences)
{
   bool isConfigDifferent = false;


   // Load in all of the preferences to ensure all of them were loaded
   QStringList preferencesList;
   QMapIterator<Preferences::ValueKey, QString> valueKeyIttr(m_mapValueKey);
   while (valueKeyIttr.hasNext())
   {
      valueKeyIttr.next();
      preferencesList << valueKeyIttr.value();
   }

   QFile* file = new QFile(xmlPath);

   if (!file->exists())
   {
      return false;
   }

   if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
   {
      return false;
   }

   QXmlStreamReader xml(file);
   QStringRef preferencesXMLSection(&xmlElementName);

   while(!xml.atEnd() && !xml.hasError())
   {
      // Read next element
      QXmlStreamReader::TokenType token = xml.readNext();

      // If token is just StartDocument, we'll go to next
      if(token == QXmlStreamReader::StartDocument)
      {
         continue;
      }

      if(token == QXmlStreamReader::StartElement)
      {
         if (xml.name() == preferencesXMLSection)
         {
            token = xml.readNext();
            while (!xml.atEnd() && !xml.hasError())
            {
               QStringRef name = xml.name();

               if(token == QXmlStreamReader::StartElement)
               {
                  QXmlStreamAttributes attributes = xml.attributes();
                  QString settingKey = attributes.value("key").toString();


                  QString actualSettingValue = readValueKey(settingKey).toString();
                  QStringRef storedValue(&actualSettingValue);
                  QString value;

                  token = xml.readNext();
                  if(token == QXmlStreamReader::Characters)
                  {
                     value = xml.text().toString();
                  }

                  if (value != storedValue)
                  {
                     isConfigDifferent = true;
                     if (applyDatasetPreferences)
                     {
                        saveValueKey(getValueKeyFromString(settingKey), value);
                     }
                  }


                  int index = preferencesList.indexOf(settingKey);
                  preferencesList.removeAt(index);
               }
               if (token == QXmlStreamReader::EndElement
                   && name == preferencesXMLSection)
               {
                  // All of the required section was read in.
                  break;
               }
               if (isConfigDifferent && !applyDatasetPreferences) {
                  return isConfigDifferent;
               }

               token = xml.readNext();
            }
            // All of the required section was read in.
            break;
         }
      }
   }

   if (preferencesList.size() > 0)
   {
      isConfigDifferent = true;
   }

   return isConfigDifferent;

}
