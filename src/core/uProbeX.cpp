/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <QAction>
#include <QActionGroup>
#include <QDateTime>
#include <QDockWidget>
#include <QFormLayout>
#include <QLabel>
#include <QMenuBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QFileInfo>

#include <core/uProbeX.h>
#include <mvc/AbstractWindowController.h>
#include <core/SubWindow.h>
#include <mvc/SWSModel.h>
#include <mvc/TIFFController.h>
#include <mvc/SWSWidget.h>
#include <preferences/PreferencesDialog.h>
#include <QFileDialog>
#include <mvc/MapsElementsWidget.h>
#include <mvc/MapsWorkspaceModel.h>
#include <mvc/FitSpectraWidget.h>
#include <mvc/MDA_Widget.h>

#include <gstar/Splash.h>

#include <stdio.h>

static const QString PREFERENCES_XML_SECTION_NAME = "preferences";

QTextEdit * uProbeX::log_textedit = 0;

/*---------------------------------------------------------------------------*/

uProbeX::uProbeX(QWidget* parent, Qt::WindowFlags flags) : QMainWindow(parent, flags)
{
	
	log_textedit = new QTextEdit();
    //m_autosaveTimer = nullptr;
    _liveMapsViewer = nullptr;
	
    //////// HENKE and ELEMENT INFO /////////////
    std::string element_csv_filename = "../reference/xrf_library.csv";
    std::string element_henke_filename = "../reference/henke.xdr";

    PythonLoader::inst()->safeCheck();

    initialize();

	_log_dock = new QDockWidget("Log", this);
	_log_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	_log_dock->setWidget(log_textedit);
	this->addDockWidget(Qt::BottomDockWidgetArea, _log_dock);

    // Create menu
    createMenuBar();

    int w = Preferences::inst()->getValue(STR_PRF_MainWindowSavedWidth).toInt();
    int h = Preferences::inst()->getValue(STR_PRF_MainWindowSavedHeight).toInt();

    if(w > 0 && h > 0)
    {
        resize(w, h);
    }
    else
    {
        resize(sizeHint());
    }


    // Update preferences; also creates solver
    processPreferencesUpdate();


    if (false == io::load_element_info(element_henke_filename, element_csv_filename ) )
    {
        QString msg = QString("Error loading ");
        msg += QString(element_henke_filename.c_str());
        msg += QString(" and ");
        msg += QString(element_csv_filename.c_str());

        QMessageBox::critical(this, "Warning", msg);
    }


    // Creat MDI window
    m_mdiArea = new QMdiArea();

    m_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdiArea->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setCentralWidget(m_mdiArea);

    show();
}

/*---------------------------------------------------------------------------*/

uProbeX::~uProbeX()
{
	log_textedit = nullptr;
    if(_liveMapsViewer != nullptr)
    {
        QString strIp = _liveMapsViewer->getIpAddress();
        QString strPort = _liveMapsViewer->getPort();
        Preferences::inst()->setValue(STR_PRF_LastIP, strIp);
        Preferences::inst()->setValue(STR_PRF_LastPort, strPort);
        delete _liveMapsViewer;
        _liveMapsViewer = nullptr;
    }

    Preferences::inst()->setValue(STR_PRF_MainWindowSavedWidth, width());
    Preferences::inst()->setValue(STR_PRF_MainWindowSavedHeight, height());

    // Close all open HDF5 files
    foreach(auto& i , m_subWindows.values())
    {
        if (i != nullptr)
        {
            delete i;
        }
    }
    m_subWindows.clear();

}

/*---------------------------------------------------------------------------*/

void uProbeX::adjustDisplaySettings()
{

    // Adjust font size
    int fontSize = Preferences::inst()->getValue(STR_PRF_FontSize).toInt();
    //TODO Remove hard-coded values.
    if (fontSize > 6 && fontSize < 40)
    {
        QFont font("Sans", fontSize);
        QApplication::setFont(font);
    }

}

/*---------------------------------------------------------------------------*/

bool uProbeX::checkSameFileWindow(QString& filePath)
{

    for(SubWindow* con : m_subWindows.values())
    {
        gstar::AbstractImageWidget* w = static_cast<gstar::AbstractImageWidget*>(con->widget());
        if(typeid(*w) == typeid(SWSWidget))
        {
            SWSWidget* widget = static_cast<SWSWidget*>(w);
            if(widget->getModelFileName()  == filePath)
            {
                QMessageBox::critical(nullptr, "SWS Window",
                                      "You already have opened this image!!");
                return true;
            }
        }
    }

    return false;

}

/*---------------------------------------------------------------------------*/

void uProbeX::closeEvent(QCloseEvent* event)
{

    Q_UNUSED(event);

    bool saveWithoutPrompt = Preferences::inst()->getValue(STR_PRF_AutoSaveOnExit).toBool();
    saveAllXML(!saveWithoutPrompt);

    cleanUpAutoSafeData();

    // Quit
    exitApplication();

}

/*---------------------------------------------------------------------------*/

void uProbeX::createMenuBar()
{

    QAction * action;
    // Menu bar
    m_menu = new QMenuBar(this);
    setMenuBar(m_menu);

    // File menu
    m_menuFile = new QMenu(tr("File"));
    action = m_menuFile->addAction("Open Maps Workspace");
    connect(action, SIGNAL(triggered()), this, SLOT(openMapsWorkspace()));
    m_menuFile->addSeparator();
    action = m_menuFile->addAction("Open SWS Workspace");
    connect(action, SIGNAL(triggered()), this, SLOT(openSWSFile()));
    action = m_menuFile->addAction("Open HDF5 Analyzed");
    connect(action, SIGNAL(triggered()), this, SLOT(openHDFFile()));
    m_menuFile->addSeparator();
    action = m_menuFile->addAction("Save Screen Shot");
    connect(action, SIGNAL(triggered()), this, SLOT(saveScreenShot()));
    m_menuFile->addSeparator();
    action = m_menuFile->addAction("Save Activated SWS DATA");
    connect(action, SIGNAL(triggered()), this, SLOT(saveActivatedXML()));
    action = m_menuFile->addAction("Save All SWS DATA");
    connect(action, SIGNAL(triggered()), this, SLOT(saveAllXML()));
    m_menuFile->addSeparator();
    action = m_menuFile->addAction("Save preferences");
    connect(action, SIGNAL(triggered()), this, SLOT(savePreferencesXMLData()));
    action = m_menuFile->addAction("Load preferences");
    connect(action, SIGNAL(triggered()), this, SLOT(loadPreferencesXMLData()));
    m_menuFile->addSeparator();
    action = m_menuFile->addAction("Preferences");
    connect(action, SIGNAL(triggered()), this, SLOT(showPreferences()));
    m_menuFile->addSeparator();
    action = m_menuFile->addAction("Exit");
    connect(action, SIGNAL(triggered()), this, SLOT(exitApplication()));
    m_menu->addMenu(m_menuFile);

    connect(m_menuFile, SIGNAL(aboutToShow()), this, SLOT(menuBarEnable()));

    // Stream menu
    m_menuStream = new QMenu(tr("Live Stream"));
    action = m_menuStream->addAction("Open Live Stream Viewer");
    connect(action, SIGNAL(triggered()), this, SLOT(openLiveStreamViewer()));
    m_menu->addMenu(m_menuStream);

    // Help menu
    m_menuHelp = new QMenu(tr("Help"));
    action = m_menuHelp->addAction("About");
    connect(action, SIGNAL(triggered()), this, SLOT(showAbout()));
	m_menuHelp->addAction(_log_dock->toggleViewAction());
    m_menu->addMenu(m_menuHelp);

}

/*---------------------------------------------------------------------------*/

void uProbeX::openLiveStreamViewer()
{
    QString strIp = Preferences::inst()->getValue(STR_PRF_LastIP).toString();
    QString strPort = Preferences::inst()->getValue(STR_PRF_LastPort).toString();

    if (_liveMapsViewer == nullptr)
    {
        _liveMapsViewer = new LiveMapsElementsWidget(strIp, strPort);
    }
    _liveMapsViewer->show();
}

/*---------------------------------------------------------------------------*/

void uProbeX::exitApplication()
{
    if(_liveMapsViewer != nullptr)
    {
        QString strIp = _liveMapsViewer->getIpAddress();
        QString strPort = _liveMapsViewer->getPort();
        Preferences::inst()->setValue(STR_PRF_LastIP, strIp);
        Preferences::inst()->setValue(STR_PRF_LastPort, strPort);
        delete _liveMapsViewer;
        _liveMapsViewer = nullptr;
    }
    // Quit
    QCoreApplication::quit();

}

/*---------------------------------------------------------------------------*/

void uProbeX::initialize()
{

    m_mdiArea = nullptr;

}

/*---------------------------------------------------------------------------*/
/*
void uProbeX::adjustAutoSaveSettings()
{
    if (Preferences::inst()->getValue(STR_PRF_AutoSaveRecoveryEnable).toBool())
    {
        if (m_autosaveTimer == nullptr) {
            m_autosaveTimer = new QTimer(this);

            connect(m_autosaveTimer,
                    SIGNAL(timeout()),
                    this,
                    SLOT(performAutoSave()));
        }

        int time = Preferences::inst()->getValue(STR_PRF_AutoSaveRecoveryEveryMiliseconds).toInt();

        // Enforce a minimum time of 1 second.
        if (time < 1000)
        {
            time = 1000;
            Preferences::inst()->setValue(STR_PRF_AutoSaveRecoveryEveryMiliseconds, time);
        }

        m_autosaveTimer->start(time);
    }
    else
    {
        //  autosafe functionality is disabled.
        if (m_autosaveTimer != nullptr)
        {
            disconnect(m_autosaveTimer,
                       SIGNAL(timeout()),
                       this,
                       SLOT(performAutoSave()));

            delete m_autosaveTimer;
            m_autosaveTimer = nullptr;
        }
    }
}
*/
/*---------------------------------------------------------------------------*/

void uProbeX::makeSWSWindow(SWSModel* swsModel)
{
    SWSWidget* swsWidget = new SWSWidget();
    swsWidget->resize(1027, 768);
	swsWidget->setModel(swsModel);
   
    connect(swsWidget,
            SIGNAL(solverStart()),
            this,
            SLOT(solverStart()));

    connect(swsWidget,
            SIGNAL(solverVariableUpdate(double, double)),
            this,
            SLOT(solverVariableUpdate(double, double)));

    connect(swsWidget,
            SIGNAL(cancelSolverVariableUpdate()),
            this,
            SLOT(cancelSolverVariableUpdate()));


    SubWindow* w = new SubWindow(m_mdiArea);
    connect(w,
            SIGNAL(windowClosing(SubWindow*)),
            this,
            SLOT(subWindowClosed(SubWindow*)));


    m_mdiArea->addSubWindow(w);
    w->setWidget(swsWidget);
    w->resize(1024, 768);
    w->setWindowTitle(swsModel->getFilePath());
    w->showMaximized();

    m_subWindows[w->getUuid()] = w;

    connect(w,
            SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates )),
            swsWidget,
            SLOT(windowChanged(Qt::WindowStates, Qt::WindowStates)));
}

/*---------------------------------------------------------------------------*/

void uProbeX::makeSWSWindow(QString path, bool newWindow)
{

    if (newWindow == false) return;
    // Create general window view widget and image model
    SWSWidget* swsWidget = new SWSWidget();
    swsWidget->resize(800, 600);

    try
    {
        //this->setCursor(Qt::WaitCursor);
        SWSModel* swsModel = new SWSModel();
        swsModel->load(path);
        if(swsModel->tiffLoaded())
        {
			swsWidget->setModel(swsModel);
        }
        else
        {
            delete swsModel;
            delete swsWidget;
            return;
        }
    }
    catch(std::string& s)
    {
        delete swsWidget;
        //delete tiffController;

        // Restore cursor
        QApplication::restoreOverrideCursor();

        // Display error to user
        QMessageBox::critical(this, tr("uProbeX"),
                              tr("Failed to open recorded SWS file.\n\n") + tr("Error:  ") +
                              QString(s.c_str()));

        return;
    }

    connect(swsWidget,
            SIGNAL(solverStart()),
            this,
            SLOT(solverStart()));

    connect(swsWidget,
            SIGNAL(solverVariableUpdate(double, double)),
            this,
            SLOT(solverVariableUpdate(double, double)));

    connect(swsWidget,
            SIGNAL(cancelSolverVariableUpdate()),
            this,
            SLOT(cancelSolverVariableUpdate()));


    SubWindow* w = nullptr;
    if (newWindow == true)
    {
        w = new SubWindow(m_mdiArea);
        connect(w,
                SIGNAL(windowClosing(SubWindow*)),
                this,
                SLOT(subWindowClosed(SubWindow*)));

    }

    m_mdiArea->addSubWindow(w);
    w->setWidget(swsWidget);
    w->resize(1024, 768);
    w->setWindowTitle(path);
    w->showMaximized();

    m_subWindows[w->getUuid()] = w;

    connect(w,
            SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates )),
            swsWidget,
            SLOT(windowChanged(Qt::WindowStates, Qt::WindowStates)));

}

/*---------------------------------------------------------------------------*/

void uProbeX::onLoadMapsWorkspace(MapsWorkspaceController* controller, QString path)
{

    controller->setWorkingDir(path);

}

/*---------------------------------------------------------------------------*/

void uProbeX::mapsControllerClosed(MapsWorkspaceController* controller)
{
    if (controller != nullptr)
    {
        delete controller;
    }
    controller = nullptr;
}

/*---------------------------------------------------------------------------*/

void uProbeX::makeHDFWindow(QString path)
{

    MapsH5Model* model = new MapsH5Model();
    try
    {
        model->load(path);
    }
    catch(std::string& s)
    {
        // Restore cursor
        //QApplication::restoreOverrideCursor();

        // Display error to user
        QMessageBox::critical(this, tr("uProbeX"),
                              tr("Failed to open recorded HDF5 file.\n\n") + tr("Error:  ") +
                              QString(s.c_str()));

        return;
    }
    makeHDFWindow(model);
}

/*---------------------------------------------------------------------------*/

void uProbeX::makeHDFWindow(MapsH5Model* model)
{
    MapsElementsWidget* widget = new MapsElementsWidget();
    widget->setModel(model);
    //widget->resize(800, 600);


    SubWindow* w = nullptr;
    w = new SubWindow(m_mdiArea);
    connect(w,
            SIGNAL(windowClosing(SubWindow*)),
            this,
            SLOT(subWindowClosed(SubWindow*)));


    m_mdiArea->addSubWindow(w);

    w->setWidget(widget);
    w->resize(1024, 768);
    w->setWindowTitle(model->getFilePath());
    w->showMaximized();

    m_subWindows[w->getUuid()] = w;

    connect(w,
            SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates )),
            widget,
            SLOT(windowChanged(Qt::WindowStates, Qt::WindowStates)));


}

/*---------------------------------------------------------------------------*/

void uProbeX::makeMDAWindow(RAW_Model* model)
{
    MDA_Widget* widget = new MDA_Widget();
    widget->setModel(model);

    SubWindow* w = nullptr;
    w = new SubWindow(m_mdiArea);
    connect(w,
            SIGNAL(windowClosing(SubWindow*)),
            this,
            SLOT(subWindowClosed(SubWindow*)));


    m_mdiArea->addSubWindow(w);

    w->setWidget(widget);
    w->resize(1024, 768);
    w->setWindowTitle(model->getFilePath());
    w->show();

    m_subWindows[w->getUuid()] = w;

    connect(w,
            SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates )),
            widget,
            SLOT(windowChanged(Qt::WindowStates, Qt::WindowStates)));
}

/*---------------------------------------------------------------------------*/

void uProbeX::menuBarEnable()
{

//    if(m_mdiArea->activeSubWindow() != 0)
//    {

//        m_saveActivatedXMLAction->setEnabled(saveActivatedXmlRequired());
//        m_saveAllXMLAction->setEnabled(saveAllXMLRequired());

//        m_saveScreenShotAction->setEnabled(true);
//    }
//    else
//    {
//        m_saveScreenShotAction->setEnabled(false);
//        m_saveActivatedXMLAction->setEnabled(false);
//        m_saveAllXMLAction->setEnabled(false);
//    }

}

/*---------------------------------------------------------------------------*/

void uProbeX::openSWSFile()
{

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open SWS workspace", ".",
                                                    tr("SWS (*.sws *.SWS)"));

    // Dialog returns a nullptr string if user press cancel.
    if (fileName.isNull() || fileName.isEmpty()) return;

    QString filePath = QFileInfo(fileName).canonicalFilePath();


    QString fileSuffix = QFileInfo(fileName).suffix();

    if (0==fileSuffix.compare("sws")||0==fileSuffix.compare("SWS"))
    {
        // Check if the image has been opened
        if(true == checkSameFileWindow(filePath))
            return;

        makeSWSWindow(filePath, true);

    }

}

/*---------------------------------------------------------------------------*/

void uProbeX::openMapsWorkspace()
{

    QString dirName = QFileDialog::getExistingDirectory(this, "Open Maps workspace", ".");

    // Dialog returns a nullptr string if user press cancel.
    if (dirName.isNull() || dirName.isEmpty()) return;

    QDir dir(dirName);
    dir.mkdir("img.dat");
    dir.mkdir("output");

    MapsWorkspaceController *mapsWorkspaceController = new MapsWorkspaceController(this);
    connect(mapsWorkspaceController, SIGNAL(controllerClosed(MapsWorkspaceController*)), this, SLOT(mapsControllerClosed(MapsWorkspaceController*)));

    mapsWorkspaceController->setWorkingDir(dirName);

}

/*---------------------------------------------------------------------------*/

void uProbeX::openHDFFile()
{

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open HDF5", ".",
                                                    tr("H5 (*.h5 *.h50 *.h51 *.h52 *.h53)"));

    // Dialog returns a nullptr string if user press cancel.
    if (fileName.isNull() || fileName.isEmpty()) return;

    QString filePath = QFileInfo(fileName).canonicalFilePath();

    makeHDFWindow(filePath);

}

/*---------------------------------------------------------------------------*/

void uProbeX::saveScreenShot()
{

    if(m_mdiArea->activeSubWindow() != nullptr)
    {
        SubWindow* w = dynamic_cast<SubWindow*>(m_mdiArea->activeSubWindow());
        if (w == nullptr) return;
        QUuid id = w->getUuid();

        if(m_subWindows.contains(id))
        {
            gstar::AbstractImageWidget* imageWidget = dynamic_cast<gstar::AbstractImageWidget*>(w->widget());

            if(typeid(*imageWidget) == typeid(SWSWidget))
            {
                SWSWidget* swsWidget = dynamic_cast<SWSWidget*>(imageWidget);
                swsWidget->saveScreenShot();
            }
            else
            {
                QMessageBox::critical(this, tr("Warning"),
                                      tr("Not SWS Window Activated\n\n"));
            }
        }
    }

}

/*---------------------------------------------------------------------------*/

void uProbeX::saveActivatedXML()
{

    if(m_mdiArea->activeSubWindow() != nullptr)
    {
        SubWindow* w = dynamic_cast<SubWindow*>(m_mdiArea->activeSubWindow());
        if (w == nullptr) return;
        gstar::AbstractImageWidget* imageWidget = dynamic_cast<gstar::AbstractImageWidget*>(w->widget());
        SWSWidget* swsWidget;
        if(typeid(*imageWidget) == typeid(SWSWidget))
        {
            swsWidget = dynamic_cast<SWSWidget*>(imageWidget);
        } else {

            return;
        }

        if (saveActivatedXmlRequired()) {
            QUuid id = w->getUuid();

            if(m_subWindows.contains(id))
            {
                QMessageBox msgBox;
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::No);
                msgBox.setIcon(QMessageBox::Question);
                msgBox.setText("Do you want to save the activated SWS workspace?");
                int ret = msgBox.exec();

                if (ret == QMessageBox::Yes)
                {
                    swsWidget->saveXMLCoordinateInfo();
                }
            }
        }
        swsWidget->cleanUpTemoraryXMLFiles();
    }

}

/*---------------------------------------------------------------------------*/

void uProbeX::saveAllXML()
{
    saveAllXML(true);
}

/*---------------------------------------------------------------------------*/

void uProbeX::saveAllXML(bool verifyWithUser)
{
    return;
    int ret;

    if (saveAllXMLRequired() == false) {
        return;
    }

    if (verifyWithUser) {
        QMessageBox msgBox;
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText("Do you want to save all SWS workspace?");

        ret = msgBox.exec();
    } else {
        ret = QMessageBox::Yes;
    }

    if (ret == QMessageBox::Yes)
    {
        SWSWidget* widget = nullptr;

        for(SubWindow* con : m_subWindows.values())
        {
            gstar::AbstractImageWidget* w = static_cast<gstar::AbstractImageWidget*>(con->widget());
            if(typeid(*w) == typeid(SWSWidget))
            {
                widget = static_cast<SWSWidget*>(w);
                widget->saveXMLCoordinateInfo();
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

bool uProbeX::saveAllXMLRequired()
{
return false;
    SWSWidget* widget = nullptr;

    for(SubWindow* con : m_subWindows.values())
    {
        gstar::AbstractImageWidget* w = static_cast<gstar::AbstractImageWidget*>(con->widget());
        if(typeid(*w) == typeid(SWSWidget))
        {
            widget = static_cast<SWSWidget*>(w);
            bool saveRequired = widget->verifySaveIsRequired();
            if (saveRequired)
            {
                return true;
            }
        }
    }

    return false;
}

/*---------------------------------------------------------------------------*/

bool uProbeX::saveActivatedXmlRequired()
{
    return false;
    SubWindow* w = dynamic_cast<SubWindow*>(m_mdiArea->activeSubWindow());
    if (w != nullptr) {
        gstar::AbstractImageWidget* imageWidget = dynamic_cast<gstar::AbstractImageWidget*>(w->widget());
        if(typeid(*imageWidget) == typeid(SWSWidget))
        {
            SWSWidget* swsWidget = dynamic_cast<SWSWidget*>(imageWidget);
            return swsWidget->verifySaveIsRequired();
        }
    }
    return false;
}

/*---------------------------------------------------------------------------*/
/*
void uProbeX::savePreferencesXMLData()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Save XML Preferences", ".",
                                                    tr("XML (*.xml *.XML)"));

    if (!fileName.endsWith(".xml") || fileName.endsWith(".XML")) {
        fileName += ".xml";
    }

    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(nullptr, "Read only", "The file is in read only mode");
    }

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.setAutoFormattingIndent(true);

    xmlWriter.writeStartDocument();

    m_preferences.getEnvironmentInfo(&xmlWriter, PREFERENCES_XML_SECTION_NAME);

    xmlWriter.writeEndDocument();
}

/*---------------------------------------------------------------------------

void uProbeX::loadPreferencesXMLData()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open XML Preferences", ".",
                                                    tr("XML (*.xml *.XML)"));

    // Dialog returns a nullptr string if user press cancel.
    if (fileName.isNull() || fileName.isEmpty()) return;

    QString filePath = QFileInfo(fileName).canonicalFilePath();

    // Verify preferences stored in dataset

    bool envDifferentInDataset = m_preferences.verifyIsPreferencesDifferentInXml(fileName, PREFERENCES_XML_SECTION_NAME);
    if (envDifferentInDataset)
    {
        QMessageBox msgBox;
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText("Would you like to load in the preferences from xml file?");

        int ret = msgBox.exec();

        if (ret == QMessageBox::Yes)
        {
            m_preferences.loadThePreferencesIntoEnvironmentXml(fileName, PREFERENCES_XML_SECTION_NAME);
        }
    }
    else
    {
        QMessageBox::information(nullptr, "Preferences Loaded", "Preferences loaded currently are the same as in preferences file.");
    }
}

/*---------------------------------------------------------------------------*/

void uProbeX::performAutoSave()
{
    SWSWidget* widget = nullptr;

    for(SubWindow* con : m_subWindows.values())
    {
        gstar::AbstractImageWidget* w = dynamic_cast<gstar::AbstractImageWidget*>(con->widget());
        if(typeid(*w) == typeid(SWSWidget))
        {
            widget = static_cast<SWSWidget*>(w);
            widget->saveTemporaryXMLCoordinateInfo();
        }
    }
}

/*---------------------------------------------------------------------------*/

void uProbeX::cleanUpAutoSafeData() {
    SWSWidget* widget = nullptr;

    for(SubWindow* con : m_subWindows.values())
    {
        gstar::AbstractImageWidget* w = dynamic_cast<gstar::AbstractImageWidget*>(con->widget());
        if(typeid(*w) == typeid(SWSWidget))
        {
            widget = static_cast<SWSWidget*>(w);
            widget->cleanUpTemoraryXMLFiles();
        }
    }
}

/*---------------------------------------------------------------------------*/

void uProbeX::showAbout()
{
    gstar::Splash* splashAbout = new gstar::Splash(this, Qt::Popup, "uProbeX", true);
    // Show and add message
    splashAbout->setWindowModality(Qt::ApplicationModal);
    splashAbout->clear();
    splashAbout->appendMessage(tr("Created by"));
    splashAbout->appendMessage(tr("Software Services Group"));
    splashAbout->appendMessage(tr("Advanced Photon Source"));
    splashAbout->appendMessage(tr("Argonne National Laboratory"));
    splashAbout->appendMessage(tr(""));
    splashAbout->appendMessage(tr("Copyright (c) 2014"));
    splashAbout->appendMessage(tr("UChicago Argonne, LLC"));
    splashAbout->appendMessage(tr(""));
    splashAbout->appendMessage(tr("Credits"));
    splashAbout->appendMessage(tr("Arthur Glowacki"));
    splashAbout->appendMessage(tr("Dariusz Jarosz"));
    splashAbout->appendMessage(tr("Nicholas Schwarz"));
    splashAbout->appendMessage(tr("Ke Yue"));
    splashAbout->exec();
}

/*---------------------------------------------------------------------------*/

void uProbeX::showPreferences()
{

    QList<gstar::AbstractImageWidget*> widgetList;
    for(SubWindow* con : m_subWindows.values())
    {
        gstar::AbstractImageWidget* w = static_cast<gstar::AbstractImageWidget*>(con->widget());
        if(typeid(*w) == typeid(SWSWidget))
        {
            widgetList.append(w);
        }
    }

    // Show preferences dialog with current settings
    PreferencesDialog* dialog = new PreferencesDialog(widgetList,
                                                      this,
                                                      Qt::Dialog);

    // Update with current settings
    if (dialog->exec() == QDialog::Accepted)
    {
        processPreferencesUpdate();
    }

    // Delete dialog
    delete dialog;

}

/*---------------------------------------------------------------------------*/

void uProbeX::processPreferencesUpdate()
{
    QString title = Preferences::inst()->getValue(STR_PRF_WindowTitle).toString();
    if(title.isEmpty())
        title = "uProbeX";
    setWindowTitle(title);

    adjustDisplaySettings();

    //adjustAutoSaveSettings();

    updateContextMenus();

    updatePreference();
}

/*---------------------------------------------------------------------------*/

void uProbeX::subWindowClosed(SubWindow* subWindow)
{

    if (subWindow == nullptr) return;

    QUuid id = subWindow->getUuid();

    auto i = m_subWindows.find(id);
    if (i == m_subWindows.end())
        return;

    saveActivatedXML();

    // Found the controller to close.
    auto absWinCon = i.value();
    if (absWinCon == nullptr)
        return;

    m_subWindows.erase(i);
    ////delete absWinCon;
    absWinCon = nullptr;

}

/*---------------------------------------------------------------------------*/

void uProbeX::updatePreference()
{

    SWSWidget* widget = nullptr;

    for(SubWindow* con : m_subWindows.values())
    {
        gstar::AbstractImageWidget* w = static_cast<gstar::AbstractImageWidget*>(con->widget());
        if(typeid(*w) == typeid(SWSWidget))
        {
            widget = static_cast<SWSWidget*>(w);
            widget->preferenceChanged();
        }
    }

}

/*---------------------------------------------------------------------------*/

void uProbeX::updateContextMenus()
{

    SWSWidget* widget = nullptr;

    for(SubWindow* con : m_subWindows.values())
    {
        gstar::AbstractImageWidget* w = static_cast<gstar::AbstractImageWidget*>(con->widget());
        if(typeid(*w) == typeid(SWSWidget))
        {
            widget = static_cast<SWSWidget*>(w);
            widget->updateContextMenus();
        }
    }

}

/*---------------------------------------------------------------------------*/
