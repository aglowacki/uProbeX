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
#include <gstar/AbstractImageWidget.h>

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

/*---------------------------------------------------------------------------*/

PreferencesDialog::PreferencesDialog(QList<gstar::AbstractImageWidget*> windowList,
                                     QWidget* parent,
                                     Qt::WindowFlags f)
: QDialog(parent, f)
{

   // Background color
   //QPalette pal = this->palette();
   //pal.setColor(this->backgroundRole(), Qt::white);
   //this->setPalette(pal);

   // Autofill background
//   setAutoFillBackground(true);

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

   //m_solverOptionButton = new QListWidgetItem(m_contentsWidget);
   //m_solverOptionButton->setText(tr("Solver"));
   //m_solverOptionButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

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
   m_solverOptionWidget = nullptr;
   //m_solverOptionWidget = new PreferencesSolverOption(windowList);
   //m_pagesWidget->addWidget(m_solverOptionWidget);
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

   setPreferences(false);

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
   
    if (m_solverOptionWidget != nullptr)
    {
        Preferences::inst()->setValue(STR_PRF_NMCoefficient, m_solverOptionWidget->getNMCoefficientAttrs());
        Preferences::inst()->setValue(STR_PRF_NMOptions, m_solverOptionWidget->getNMOptionAttrs());
        Preferences::inst()->setValue(STR_PRF_SolverCheckedID, m_solverOptionWidget->getCheckedID());
    }
/*
    // Profile information
    Preferences::inst()->setValue(STR_PRF_ProfileList,
                                m_solverOptionWidget->getPythonSolverPofile());
    Preferences::inst()->setValue(STR_PRF_ProfilePythonSolverName,
                                m_solverOptionWidget->getPythonSolverName());
    Preferences::inst()->setValue(STR_PRF_ProfilePythonCoefficient,
                            m_solverOptionWidget->getPythonCoefficientAttrs());
    Preferences::inst()->setValue(STR_PRF_ProfilePythonOptions,
                                m_solverOptionWidget->getPythonOptionAttrs());

    // Current profile information
    Preferences::inst()->setValue(STR_PRF_ProfileIndex,
                            m_solverOptionWidget->getCurrentIndex());
    Preferences::inst()->setValue(STR_PRF_PythonSolverName,
                            m_solverOptionWidget->getCurrentFileName());
    Preferences::inst()->setValue(STR_PRF_PythonCoefficient,
                            m_solverOptionWidget->getCurrentCoefficientAttrs());
    Preferences::inst()->setValue(STR_PRF_PythonOptions,
                                m_solverOptionWidget->getCurrentOptionAttrs());
*/
    
    if (m_displaySettings != nullptr)
    {
        Preferences::inst()->setValue(STR_PRF_FontSize, m_displaySettings->getFontSize());
        Preferences::inst()->setValue(STR_PRF_WindowTitle, m_displaySettings->getWindowTitle());
        Preferences::inst()->setValue(STR_PRF_DecimalPrecision, m_displaySettings->getDecimalPrecision());
        Preferences::inst()->setValue(STR_PFR_USE_DARK_THEME, m_displaySettings->getUseDarkTheme());
    }
    Preferences::inst()->setValue(STR_PRF_RegionMenuList,m_pythonFuncWidget->getGroupStringList());

    Preferences::inst()->setValue(STR_PRF_MicroProbeXPv,m_microPvSettings->getMicroProbeXPv());
    Preferences::inst()->setValue(STR_PRF_MicroProbeYPv,m_microPvSettings->getMicroProbeYPv());

    Preferences::inst()->setValue(STR_PRF_AutoSaveRecoveryEnable,m_autoSaveOptionsWidget->getEnableRecoveryAutoSaveFunctionality());
    Preferences::inst()->setValue(STR_PRF_AutoSaveRecoveryEveryMiliseconds,m_autoSaveOptionsWidget->getRecoveryAutoSaveEveryMiliseconds());
    Preferences::inst()->setValue(STR_PRF_AutoSaveOnExit,m_autoSaveOptionsWidget->getEnableSaveOnExit());

    Preferences::inst()->setValue(STR_PRF_ExportZoomToRegion,m_exportOptionsWidget->getEnableZoomToRegionOnExport());
    Preferences::inst()->setValue(STR_PRF_ExportDirectoryInDataset,m_exportOptionsWidget->getExportToDirInDataset());
    Preferences::inst()->setValue(STR_PRF_ExportPrintNameOnExportedImage,m_exportOptionsWidget->getPrintNameOnExportedImage());
    Preferences::inst()->setValue(STR_PRF_ExportPrintPxPyOnExportedImage,m_exportOptionsWidget->getPrintPyPxOnExportedImage());
    Preferences::inst()->setValue(STR_PRF_ExportPrintWidthHeightOnExportedImage,m_exportOptionsWidget->getPrintWidthHeightOnExportedImage());
    Preferences::inst()->setValue(STR_PRF_ExportSelectedXmlOption,m_exportOptionsWidget->getSelectedXmlExportOption());
   
    Preferences::inst()->save();
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

void PreferencesDialog::setPreferences(bool passMode)
{

   Q_UNUSED(passMode);

   // Assign preferences object

   // Assign preferences to pages
   
    // Solver check id and general attribute which would be used as default
   if (m_solverOptionWidget != nullptr)
   {
       m_solverOptionWidget->setCheckedID(
           Preferences::inst()->getValue(STR_PRF_SolverCheckedID).toInt());

       m_solverOptionWidget->setNMCoefficientAttrs(
           Preferences::inst()->getValue(STR_PRF_NMCoefficient).toStringList());
       m_solverOptionWidget->setNMOptionAttrs(
           Preferences::inst()->getValue(STR_PRF_NMOptions).toStringList());
   }
/*
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
        Preferences::inst()->getValue(STR_PRF_ProfileIndex).toInt());
    m_solverOptionWidget->setCurrentFileName(
        Preferences::inst()->getValue(STR_PRF_PythonSolverName).toString());
    m_solverOptionWidget->setCurrentCoefficientAttrs(
        preferences->readValueKey(
                Preferences::PythonCoefficient).toStringList());
    m_solverOptionWidget->setCurrentOptionAttrs(
        preferences->readValueKey(
                Preferences::PythonOptions).toStringList());
*/
    // General window attribute
    m_displaySettings->setFontSize(Preferences::inst()->getValue(STR_PRF_FontSize).toInt());
    m_displaySettings->setWindowTitle(Preferences::inst()->getValue(STR_PRF_WindowTitle).toString());
    m_displaySettings->setDecimalPrecision(Preferences::inst()->getValue(STR_PRF_DecimalPrecision).toInt());

    m_microPvSettings->setMicroProbeXPv(Preferences::inst()->getValue(STR_PRF_MicroProbeXPv).toString());
    m_microPvSettings->setMicroProbeYPv(Preferences::inst()->getValue(STR_PRF_MicroProbeYPv).toString());


    // load region actions
    m_pythonFuncWidget->parseGroupStringList(Preferences::inst()->getValue(STR_PRF_RegionMenuList).toStringList());

    // Autosave
    m_autoSaveOptionsWidget->setEnableRecoveryAutoSaveFunctionality(Preferences::inst()->getValue(STR_PRF_AutoSaveRecoveryEnable).toBool());
    m_autoSaveOptionsWidget->setRecoveryAutoSaveEveryMiliseconds(Preferences::inst()->getValue(STR_PRF_AutoSaveRecoveryEveryMiliseconds).toInt());
    m_autoSaveOptionsWidget->setEnableSaveOnExit(Preferences::inst()->getValue(STR_PRF_AutoSaveOnExit).toBool());

    // Export
    m_exportOptionsWidget->setEnabledZoomToRegionOnExport(Preferences::inst()->getValue(STR_PRF_ExportZoomToRegion).toBool());
    m_exportOptionsWidget->setExportToDirInDataset(Preferences::inst()->getValue(STR_PRF_ExportDirectoryInDataset).toString());
    m_exportOptionsWidget->setPrintNameOnExportedImage(Preferences::inst()->getValue(STR_PRF_ExportPrintNameOnExportedImage).toBool());
    m_exportOptionsWidget->setPrintPyPxOnExportedImage(Preferences::inst()->getValue(STR_PRF_ExportPrintPxPyOnExportedImage).toBool());
    m_exportOptionsWidget->setPrintWidthHeightOnExportedImage(Preferences::inst()->getValue(STR_PRF_ExportPrintWidthHeightOnExportedImage).toBool());
    m_exportOptionsWidget->setSelectedXmlExportOption(Preferences::inst()->getValue(STR_PRF_ExportSelectedXmlOption).toInt());

   

}

/*---------------------------------------------------------------------------*/

