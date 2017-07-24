/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/PreferencesDialog.h>

#include <preferences/Preferences.h>
#include <preferences/PreferencesDisplay.h>
#include <preferences/PreferencesMicroPv.h>
#include <preferences/PreferencesPythonFunc.h>
#include <preferences/PreferencesSolverOption.h>
#include <preferences/PreferencesAutoSave.h>
#include <preferences/PreferencesExport.h>
#include <AbstractImageWidget.h>

#include <QListView>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QPushButton>
#include <QList>
#include <QTableView>
#include <QTreeView>
#include <QPair>
#include <QHeaderView>

#include <QDebug>
/*---------------------------------------------------------------------------*/

PreferencesDialog::PreferencesDialog(QList<gstar::AbstractImageWidget*> windowList,
                                     Solver* solver,
                                     QWidget* parent,
                                     Qt::WindowFlags f)
: QDialog(parent, f)
{

   // Background color
   QPalette pal = this->palette();
   pal.setColor(this->backgroundRole(), Qt::white);
   this->setPalette(pal);

   // Autofill background
   setAutoFillBackground(true);

   // Create contents list
   m_contentsWidget = new QListWidget;
   m_contentsWidget->setViewMode(QListView::ListMode);
   m_contentsWidget->setMaximumWidth(128);
   m_contentsWidget->setMinimumHeight(400);
   m_contentsWidget->setCurrentRow(0);

   // Create contents items
   m_displayButton = new QListWidgetItem(m_contentsWidget);
   m_displayButton->setText(tr("Display Settings"));
   m_displayButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

   m_microPvButton = new QListWidgetItem(m_contentsWidget);
   m_microPvButton->setText(tr("MicroProbe PV's"));
   m_microPvButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

   m_regionTableButton = new QListWidgetItem(m_contentsWidget);
   m_regionTableButton->setText(tr("MicroProbe Region Actions"));
   m_regionTableButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

   m_solverOptionButton = new QListWidgetItem(m_contentsWidget);
   m_solverOptionButton->setText(tr("Solver"));
   m_solverOptionButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

   m_autoSaveOptionsButton = new QListWidgetItem(m_contentsWidget);
   m_autoSaveOptionsButton->setText(tr("Auto Save"));
   m_autoSaveOptionsButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

   m_exportOptionsButton = new QListWidgetItem(m_contentsWidget);
   m_exportOptionsButton->setText("Export");
   m_exportOptionsButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);


   // Create preference pages
   m_pagesWidget = new QStackedWidget;
   m_pagesWidget->setMinimumWidth(500);
   m_displaySettings = new PreferencesDisplay();
   m_pagesWidget->addWidget(m_displaySettings);
   m_microPvSettings = new PreferencesMicroPv();
   m_pagesWidget->addWidget(m_microPvSettings);
   m_pythonFuncWidget = new PreferencesPythonFunc();
   m_pagesWidget->addWidget(m_pythonFuncWidget);
   m_solverOptionWidget = new PreferencesSolverOption(windowList, solver);
   m_pagesWidget->addWidget(m_solverOptionWidget);
   m_autoSaveOptionsWidget = new PreferencesAutoSave();
   m_pagesWidget->addWidget(m_autoSaveOptionsWidget);
   m_exportOptionsWidget = new PreferencesExport();
   m_pagesWidget->addWidget(m_exportOptionsWidget);


   // Configuration layout
   QHBoxLayout* configLayout = new QHBoxLayout;
   configLayout->addWidget(m_contentsWidget);
   configLayout->addWidget(m_pagesWidget);

   // Dialog buttons
   m_btnOK = new QPushButton(tr("OK"));
   m_btnCancel = new QPushButton(tr("Cancel"));

   // Button layout
   QHBoxLayout* btnLayout = new QHBoxLayout();
   btnLayout->setContentsMargins(0, 0, 0, 0);
   btnLayout->addStretch();
   btnLayout->addWidget(m_btnOK);
   btnLayout->addWidget(m_btnCancel);

   // Connect button signals/slots
   connect(m_btnOK, SIGNAL(clicked()), this, SLOT(accept()));
   connect(m_btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

   // Main layout
   m_layout = new QVBoxLayout;
   m_layout->addLayout(configLayout);
   m_layout->addSpacing(12);
   m_layout->addLayout(btnLayout);
   setLayout(m_layout);

   // Set dialog title
   setWindowTitle(tr("Preferences"));

   // Change page signal/slot
   connect(m_contentsWidget,
           SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
           this,
           SLOT(changePage(QListWidgetItem*, QListWidgetItem*)));

}

/*---------------------------------------------------------------------------*/

void PreferencesDialog::accept()
{

   // Accept changes in settings widget
   if (m_preferences != NULL)
   {

      m_preferences->saveValueKey(Preferences::NMCoefficient,
                                 m_solverOptionWidget->getNMCoefficientAttrs());
      m_preferences->saveValueKey(Preferences::NMOptions,
                                  m_solverOptionWidget->getNMOptionAttrs());

      // Profile information
      m_preferences->saveValueKey(Preferences::ProfileList,
                                  m_solverOptionWidget->getPythonSolverPofile());
      m_preferences->saveValueKey(Preferences::ProfilePythonSolverName,
                                  m_solverOptionWidget->getPythonSolverName());
      m_preferences->saveValueKey(Preferences::ProfilePythonCoefficient,
                             m_solverOptionWidget->getPythonCoefficientAttrs());
      m_preferences->saveValueKey(Preferences::ProfilePythonOptions,
                                  m_solverOptionWidget->getPythonOptionAttrs());

      // Current profile information
      m_preferences->saveValueKey(Preferences::ProfileIndex,
                             m_solverOptionWidget->getCurrentIndex());
      m_preferences->saveValueKey(Preferences::PythonSolverName,
                             m_solverOptionWidget->getCurrentFileName());
      m_preferences->saveValueKey(Preferences::PythonCoefficient,
                             m_solverOptionWidget->getCurrentCoefficientAttrs());
      m_preferences->saveValueKey(Preferences::PythonOptions,
                                  m_solverOptionWidget->getCurrentOptionAttrs());

      m_preferences->saveValueKey(Preferences::SolverCheckedID,
                                  m_solverOptionWidget->getCheckedID());

      m_preferences->saveValueKey(Preferences::FontSize,
                                  m_displaySettings->getFontSize());
      m_preferences->saveValueKey(Preferences::WindowTitle,
                                  m_displaySettings->getWindowTitle());
      m_preferences->saveValueKey(Preferences::DecimalPrecision,
                                  m_displaySettings->getDecimalPrecision());

      m_preferences->saveValueKey(Preferences::RegionMenuList,
                                  m_pythonFuncWidget->getGroupStringList());

      m_preferences->saveValueKey(Preferences::MicroProbeXPv,
                                  m_microPvSettings->getMicroProbeXPv());
      m_preferences->saveValueKey(Preferences::MicroProbeYPv,
                                  m_microPvSettings->getMicroProbeYPv());

      m_preferences->saveValueKey(Preferences::AutoSaveRecoveryEnable,
                                  m_autoSaveOptionsWidget->getEnableRecoveryAutoSaveFunctionality());
      m_preferences->saveValueKey(Preferences::AutoSaveRecoveryEveryMiliseconds,
                                  m_autoSaveOptionsWidget->getRecoveryAutoSaveEveryMiliseconds());
      m_preferences->saveValueKey(Preferences::AutoSaveOnExit,
                                  m_autoSaveOptionsWidget->getEnableSaveOnExit());

      m_preferences->saveValueKey(Preferences::ExportZoomToRegion,
                                  m_exportOptionsWidget->getEnableZoomToRegionOnExport());
      m_preferences->saveValueKey(Preferences::ExportDirectoryInDataset,
                                  m_exportOptionsWidget->getExportToDirInDataset());
      m_preferences->saveValueKey(Preferences::ExportPrintNameOnExportedImage,
                                  m_exportOptionsWidget->getPrintNameOnExportedImage());
      m_preferences->saveValueKey(Preferences::ExportPrintPxPyOnExportedImage,
                                  m_exportOptionsWidget->getPrintPyPxOnExportedImage());
      m_preferences->saveValueKey(Preferences::ExportPrintWidthHeightOnExportedImage,
                                  m_exportOptionsWidget->getPrintWidthHeightOnExportedImage());
      m_preferences->saveValueKey(Preferences::ExportSelectedXmlOption,
                                  m_exportOptionsWidget->getSelectedXmlExportOption());
   }
   // Close with accept
   QDialog::accept();

}

/*---------------------------------------------------------------------------*/

void PreferencesDialog::changePage(QListWidgetItem* current,
                                   QListWidgetItem* previous)
{

   if (!current) current = previous;

   // Set visible page
   m_pagesWidget->setCurrentIndex(m_contentsWidget->row(current));

}

/*---------------------------------------------------------------------------*/

void PreferencesDialog::reject()
{

   // Close with reject
   QDialog::reject();

}

/*---------------------------------------------------------------------------*/

void PreferencesDialog::setPreferences(Preferences* preferences, bool passMode)
{

   Q_UNUSED(passMode);

   // Assign preferences object
   m_preferences = preferences;

   // Assign preferences to pages
   if (m_preferences != NULL)
   {
      // Solver check id and general attribute which would be used as default
      m_solverOptionWidget->setCheckedID(
            preferences->readValueKey(Preferences::SolverCheckedID).toInt());

      m_solverOptionWidget->setNMCoefficientAttrs(
            preferences->readValueKey(
                  Preferences::NMCoefficient).toStringList());
      m_solverOptionWidget->setNMOptionAttrs(
            preferences->readValueKey(
                  Preferences::NMOptions).toStringList());

      // Preference for all the profiles
      m_solverOptionWidget->setPythonSolverPofile(
            preferences->readValueKey(
                  Preferences::ProfileList).toStringList());
      m_solverOptionWidget->setPythonSolverName(
            preferences->readValueKey(
                  Preferences::ProfilePythonSolverName).toStringList());
      m_solverOptionWidget->setPythonCoefficientAttrs(
            preferences->readValueKey(
                  Preferences::ProfilePythonCoefficient).toStringList());
      m_solverOptionWidget->setPythonOptionAttrs(
            preferences->readValueKey(
                  Preferences::ProfilePythonOptions).toStringList());

      // Preference info for the current selected profile index
      m_solverOptionWidget->setCurrentIndex(
            preferences->readValueKey(Preferences::ProfileIndex).toInt());
      m_solverOptionWidget->setCurrentFileName(
            preferences->readValueKey(Preferences::PythonSolverName).toString());
      m_solverOptionWidget->setCurrentCoefficientAttrs(
            preferences->readValueKey(
                  Preferences::PythonCoefficient).toStringList());
      m_solverOptionWidget->setCurrentOptionAttrs(
            preferences->readValueKey(
                  Preferences::PythonOptions).toStringList());

      // General window attribute
      m_displaySettings->setFontSize(
         preferences->readValueKey(Preferences::FontSize).toInt());
      m_displaySettings->setWindowTitle(
         preferences->readValueKey(Preferences::WindowTitle).toString());
      m_displaySettings->setDecimalPrecision(
         preferences->readValueKey(Preferences::DecimalPrecision).toInt());

      m_microPvSettings->setMicroProbeXPv(
         preferences->readValueKey(Preferences::MicroProbeXPv).toString());
      m_microPvSettings->setMicroProbeYPv(
         preferences->readValueKey(Preferences::MicroProbeYPv).toString());


      // load region actions
      m_pythonFuncWidget->parseGroupStringList(
         m_preferences->readValueKey(
                  Preferences::RegionMenuList).toStringList());

      // Autosave
      m_autoSaveOptionsWidget->setEnableRecoveryAutoSaveFunctionality(
               m_preferences->readValueKey(
                  Preferences::AutoSaveRecoveryEnable).toBool());
      m_autoSaveOptionsWidget->setRecoveryAutoSaveEveryMiliseconds(
               m_preferences->readValueKey(
                  Preferences::AutoSaveRecoveryEveryMiliseconds).toInt());
      m_autoSaveOptionsWidget->setEnableSaveOnExit(
               m_preferences->readValueKey(
                  Preferences::AutoSaveOnExit).toBool());

      // Export
      m_exportOptionsWidget->setEnabledZoomToRegionOnExport(
               m_preferences->readValueKey(
                  Preferences::ExportZoomToRegion).toBool());
      m_exportOptionsWidget->setExportToDirInDataset(
               m_preferences->readValueKey(
                  Preferences::ExportDirectoryInDataset).toString());
      m_exportOptionsWidget->setPrintNameOnExportedImage(
               m_preferences->readValueKey(
                  Preferences::ExportPrintNameOnExportedImage).toBool());
      m_exportOptionsWidget->setPrintPyPxOnExportedImage(
               m_preferences->readValueKey(
                  Preferences::ExportPrintPxPyOnExportedImage).toBool());
      m_exportOptionsWidget->setPrintWidthHeightOnExportedImage(
               m_preferences->readValueKey(
                  Preferences::ExportPrintWidthHeightOnExportedImage).toBool());
      m_exportOptionsWidget->setSelectedXmlExportOption(
               m_preferences->readValueKey(
                  Preferences::ExportSelectedXmlOption).toInt());

   }

}

/*---------------------------------------------------------------------------*/

