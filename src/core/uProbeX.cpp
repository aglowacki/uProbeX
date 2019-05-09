/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <core/uProbeX.h>

#include <mvc/AbstractWindowController.h>
#include <core/SubWindow.h>
#include <mvc/SWSModel.h>
#include <mvc/TIFFController.h>
#include <mvc/SWSWidget.h>
#include <preferences/PreferencesDialog.h>
#include <QFileDialog>
#include <solver/CoordinateTransformer.h>
#include <solver/PythonTransformer.h>
#include <mvc/MapsElementsWidget.h>
#include <mvc/MapsWorkspaceWidget.h>
#include <mvc/MapsWorkspaceModel.h>
#include <mvc/FitSpectraWidget.h>
#include <mvc/MDA_Widget.h>
#include <preferences/CoordinateTransformGlobals.h>

#include <gstar/Splash.h>

#include <solver/Solver.h>
#include <solver/NelderMeadSolver.h>
#include <solver/PythonSolver.h>

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


#include <stdio.h>
#include <QDebug>
using gstar::Splash;
using gstar::ImageViewWidget;
using gstar::CoordinateModel;
using gstar::ITransformer;


static const int ID_NELDER_MEAD = 0;
static const int ID_PYTHON = 1;
static const QString PREFERENCES_XML_SECTION_NAME = "preferences";

/*---------------------------------------------------------------------------*/

uProbeX::uProbeX(QWidget* parent, Qt::WindowFlags flags) : QMainWindow(parent, flags)
{
    m_lightToMicroCoordModel = nullptr;
    m_solver = nullptr;
    m_autosaveTimer = nullptr;
    m_solverParameterParse = new SolverParameterParse();
    _load_maps_workspace_thread = nullptr;
    _liveMapsViewer = nullptr;
    _maps_workspace_dock = nullptr;

    //////// HENKE and ELEMENT INFO /////////////
    std::string element_csv_filename = "../reference/xrf_library.csv";
    std::string element_henke_filename = "../reference/henke.xdr";

    // Use resources from GStar
    //Q_INIT_RESOURCE(GStar);

    PythonLoader::inst()->safeCheck();

    initialize();

    // Create menu
    createMenuBar();

    int w = m_preferences.readValueKey(Preferences::MainWindowSavedWidth).toInt();
    int h = m_preferences.readValueKey(Preferences::MainWindowSavedHeight).toInt();

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
        qDebug()<<"Error loading "<< element_henke_filename.c_str() <<" and "<< element_csv_filename.c_str();
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

    if(_liveMapsViewer != nullptr)
    {
        QString strIp = _liveMapsViewer->getIpAddress();
        QString strPort = _liveMapsViewer->getPort();
        m_preferences.saveValueKey(Preferences::LastIP, strIp);
        m_preferences.saveValueKey(Preferences::LastPort, strPort);
        delete _liveMapsViewer;
        _liveMapsViewer = nullptr;
    }

    if(m_solverParameterParse != nullptr)
        delete m_solverParameterParse;
    m_solverParameterParse = nullptr;

    if(m_lightToMicroCoordModel != nullptr)
        delete m_lightToMicroCoordModel;
    m_lightToMicroCoordModel = nullptr;

    disconnect(&m_preferences,
               SIGNAL(preferenceSolverValueChanged()),
               this,
               SLOT(updatePreference()));

    m_preferences.saveValueKey(Preferences::MainWindowSavedWidth, width());
    m_preferences.saveValueKey(Preferences::MainWindowSavedHeight, height());

    // Close all open HDF5 files.
    QMapIterator<QUuid, AbstractWindowController*> i(m_subWindows);
    while (i.hasNext())
    {
        QUuid id = i.next().key();

        AbstractWindowController* h = m_subWindows[id];

        if (h != nullptr)
        {
            delete h;
        }

    }

}

/*---------------------------------------------------------------------------*/

void uProbeX::adjustDisplaySettings()
{

    // Adjust font size
    int fontSize = m_preferences.readValueKey(Preferences::FontSize).toInt();
    //TODO Remove hard-coded values.
    if (fontSize > 6 && fontSize < 40)
    {
        QFont font("Sans", fontSize);
        QApplication::setFont(font);
    }

}

/*---------------------------------------------------------------------------*/

void uProbeX::cancelSolverVariableUpdate()
{

    solverEnd();

}

/*---------------------------------------------------------------------------*/

bool uProbeX::checkSameFileWindow(QString& filePath)
{

    for(AbstractWindowController* con : m_subWindows.values())
    {
        AbstractWindowModel* windowModel = con->getModel();
        QString modelPath = windowModel->getDataPath();
        if(modelPath == filePath)
        {
            QMessageBox::critical(0, "SWS Window",
                                  "You already have opened this image!!");
            return true;
        }
    }

    return false;

}

/*---------------------------------------------------------------------------*/

void uProbeX::closeEvent(QCloseEvent* event)
{

    Q_UNUSED(event);

    bool saveWithoutPrompt = m_preferences.readValueKey(Preferences::AutoSaveOnExit).toBool();
    saveAllXML(!saveWithoutPrompt);

    cleanUpAutoSafeData();

    // Quit
    exitApplication();

}

/*---------------------------------------------------------------------------*/

void uProbeX::createLightToMicroCoords(int id)
{

    /*
   map["2xfm_x"] = -64.1418;
   map["2xfm_y"] = -41.8941;
   map["angle_alpha"] = 1.19;
   map["offset_a"] = 10.27;
   map["offset_b"] = 2.13;
   map["offset_c"] = 0.0;
   map["offset_d"] = 0.0;
   map["omega"] = 90.0;
   map["omega_prime"] = 90.04;
   map["scaling_XFM_X"] = 1.03;
   map["scaling_XFM_Y"] = 1.0;
   map["z_offset"] = 0.0;
   map["z_lin_x"] = -0.18;
   map["z_lin_y"] = 0.02;
   */


    ITransformer *lightTransformer = nullptr;
    QMap<QString, double> allCoefs;

    if(m_lightToMicroCoordModel != nullptr)
    {
        lightTransformer = m_lightToMicroCoordModel->getTransformer();
        if (lightTransformer != nullptr)
        {
            delete lightTransformer;
            m_lightToMicroCoordModel->setTransformer(nullptr);
        }
        lightTransformer = nullptr;
    }

    if(id == ID_NELDER_MEAD)
    {
        QStringList coefList = m_preferences.readValueKey(
                    Preferences::NMCoefficient).toStringList();
        if(false == m_solverParameterParse -> parseSolverCoefList(coefList))
        {
            allCoefs = CoordinateTransformGlobals::generateMap();
        }
        else
        {
            m_solverParameterParse -> getTransform(allCoefs);
        }

        if(lightTransformer == nullptr)
        {
            lightTransformer = new CoordinateTransformer();
        }
    }
    else
    {
        QStringList coefList = m_preferences.readValueKey(
                    Preferences::PythonCoefficient).toStringList();

        if(false == m_solverParameterParse -> parseSolverCoefList(coefList))
        {
            m_preferences.saveValueKey(Preferences::SolverCheckedID, 0);
            createSolver();
            return;
        }
        m_solverParameterParse -> getTransform(allCoefs);

        if(lightTransformer == nullptr)
        {
            QString pythonFileName = m_preferences.readValueKey(
                        Preferences::PythonSolverName).toString();
            if(pythonFileName.isEmpty())
            {
                QMessageBox::critical(0, "Error",
                                      "Must have a python script having a transform function, using default transformer right now.");
                m_preferences.saveValueKey(Preferences::SolverCheckedID, 0);
                createSolver();
                return;
            }
            QFileInfo fileInfo = QFileInfo(pythonFileName);
            lightTransformer = new PythonTransformer(
                        fileInfo.path(),
                        fileInfo.baseName(),
                        QString("my_transform"));
        }
    }

    if(lightTransformer->Init(allCoefs))
    {
        if(m_lightToMicroCoordModel != nullptr)
        {
            m_lightToMicroCoordModel->setTransformer(lightTransformer);
        }
        else
        {
            m_lightToMicroCoordModel = new gstar::CoordinateModel(lightTransformer);
        }
    }
    else
    {
        QMessageBox::critical(nullptr, "uProbeX", "Error initializeing Transformer!");
        qDebug()<<"Could not init Transformer\n";
    }

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
    m_menu->addMenu(m_menuHelp);

}

/*---------------------------------------------------------------------------*/

void uProbeX::createSolver()
{

    QMap<QString, double> dict_options;

    if(m_solver == nullptr)
        m_solver = new Solver();

    int id = m_preferences.readValueKey(Preferences::SolverCheckedID).toInt();

    createLightToMicroCoords(id);

    AbstractSolver *impl = m_solver->getImpl();
    if(impl!=nullptr)
    {
        m_solver->setImpl(nullptr);
        delete impl;
    }

    if(id == ID_NELDER_MEAD)
    {
        NelderMeadSolver* nm = new NelderMeadSolver();

        //todo read options and set them
        QStringList optionList = m_preferences.readValueKey(
                    Preferences::NMOptions).toStringList();
        if(false == m_solverParameterParse -> parseSolverOptionList(optionList))
        {
            qDebug()<<"Error reading options for NM solver";
            // Initialize with the default option
            dict_options = nm ->getOptions();
        }
        else
        {
            m_solverParameterParse -> getOptions(dict_options);
        }

        m_solver->setImpl(nm);
    }
    else
    {
        PythonSolver *ps = new PythonSolver();

        QString pythonFileName = m_preferences.readValueKey(
                    Preferences::PythonSolverName).toString();
        QFileInfo fileInfo = QFileInfo(pythonFileName);

        QStringList optionList = m_preferences.readValueKey(
                    Preferences::PythonOptions).toStringList();

        if(pythonFileName.isEmpty()
                || false == m_solverParameterParse -> parseSolverOptionList(optionList)
                || false == ps->initialPythonSolver(fileInfo.path(),
                                                    fileInfo.baseName(),
                                                    QString("my_solver")))
        {
            qDebug()<<"Error reading options for python solver, reverting to NelderMeadSolver";
            QMessageBox::critical(nullptr, "uProbeX", "Error initializeing Python solver,  reverting to NelderMeadSolver");
            m_preferences.saveValueKey(Preferences::SolverCheckedID, 0);
            createSolver();
            return;
        }
        m_solverParameterParse -> getOptions(dict_options);
        m_solver->setImpl(ps);

    }
    ITransformer *trans = m_lightToMicroCoordModel->getTransformer();
    m_solver->setAllCoef(trans->getAllCoef());
    m_solver->setOptions(dict_options);
    m_solver->setTransformer(trans);

}

/*---------------------------------------------------------------------------*/

void uProbeX::openLiveStreamViewer()
{
    QString strIp = m_preferences.readValueKey(Preferences::LastIP).toString();
    QString strPort = m_preferences.readValueKey(Preferences::LastPort).toString();

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
        m_preferences.saveValueKey(Preferences::LastIP, strIp);
        m_preferences.saveValueKey(Preferences::LastPort, strPort);
        delete _liveMapsViewer;
        _liveMapsViewer = nullptr;
    }
    // Quit
    QCoreApplication::quit();

}

/*---------------------------------------------------------------------------*/

void uProbeX::initialize()
{

    m_splashAbout = nullptr;
    m_mdiArea = nullptr;

    connect(&m_preferences,
            SIGNAL(preferenceSolverValueChanged()),
            this,
            SLOT(updatePreference()));

    connect(&m_preferences,
            SIGNAL(preferencesUpdatedInternally()),
            this,
            SLOT(processPreferencesUpdate()));


}

/*---------------------------------------------------------------------------*/

void uProbeX::adjustAutoSaveSettings()
{
    if (m_preferences.readValueKey(Preferences::AutoSaveRecoveryEnable).toBool())
    {
        if (m_autosaveTimer == nullptr) {
            m_autosaveTimer = new QTimer(this);

            connect(m_autosaveTimer,
                    SIGNAL(timeout()),
                    this,
                    SLOT(performAutoSave()));
        }

        int time = m_preferences.readValueKey(Preferences::AutoSaveRecoveryEveryMiliseconds).toInt();

        // Enforce a minimum time of 1 second.
        if (time < 1000)
        {
            time = 1000;
            m_preferences.saveValueKey(Preferences::AutoSaveRecoveryEveryMiliseconds, time);
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

/*---------------------------------------------------------------------------*/

void uProbeX::makeSWSWindow(SWSModel* swsModel)
{
    SWSWidget* swsWidget = new SWSWidget(m_solver, &m_preferences);
    swsWidget->resize(800, 600);
    swsWidget->updateFrame(swsModel->getImage());
    swsWidget->setCoordinateModel(swsModel->getCoordModel());
    swsWidget->setLightToMicroCoordModel(m_lightToMicroCoordModel);
    //swsWidget->setMarker(path);

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

    connect(w,
            SIGNAL(windowClosing(SubWindow*)),
            this,
            SLOT(subWindowClosed(SubWindow*)));


    m_mdiArea->addSubWindow(w);
    w->setWidget(swsWidget);
    w->resize(950, 700);
    //w->setIsAcquisitionWindow(false);
    w->setWindowTitle(swsModel->getFilePath());
    w->show();

    //m_subWindows[w->getUuid()] = tiffController;

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
    SWSWidget* swsWidget = new SWSWidget(m_solver, &m_preferences);
    swsWidget->resize(800, 600);

    try
    {
        //this->setCursor(Qt::WaitCursor);
        SWSModel* swsModel = new SWSModel();
        swsModel->load(path);
        if(swsModel->tiffLoaded())
        {

            swsWidget->updateFrame(swsModel->getImage());
            swsWidget->setCoordinateModel(swsModel->getCoordModel());
            swsWidget->setLightToMicroCoordModel(m_lightToMicroCoordModel);
            swsWidget->setMarker(path);
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
    else
    {

    }

    connect(w,
            SIGNAL(windowClosing(SubWindow*)),
            this,
            SLOT(subWindowClosed(SubWindow*)));


    m_mdiArea->addSubWindow(w);
    w->setWidget(swsWidget);
    w->resize(950, 700);
    //w->setIsAcquisitionWindow(false);
    w->setWindowTitle(path);
    w->show();

    //m_subWindows[w->getUuid()] = tiffController;

    connect(w,
            SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates )),
            swsWidget,
            SLOT(windowChanged(Qt::WindowStates, Qt::WindowStates)));

}

/*---------------------------------------------------------------------------*/

void uProbeX::makeMapsWindow(QString path)
{

    if(_maps_workspace_dock != nullptr)
    {
        // TODO: unload old space before loading new one.
        return;
    }


    MapsWorkspaceModel* model = new MapsWorkspaceModel();
    MapsWorkspaceWidget *widget = new MapsWorkspaceWidget();
    widget->setLabelWorkspacePath(path);

    widget->setModel(model);
    //widget->resize(800, 600);

    //connect(widget, SIGNAL(selectedAnalyzedH5(MapsH5Model*)),
    //        this, SLOT(makeHDFWindow(MapsH5Model*)));

    connect(widget, SIGNAL(showFitSpecWindow(MapsH5Model*,
                                             data_struct::Fit_Parameters*,
                                             data_struct::Fit_Element_Map_Dict*)),
            this, SLOT(makeHDFWindow(MapsH5Model*,
                                     data_struct::Fit_Parameters*,
                                     data_struct::Fit_Element_Map_Dict*)));


    connect(widget, SIGNAL(show_MDA_Window(MDA_Model*)),
            this, SLOT(makeMDAWindow(MDA_Model*)));

    connect(widget, SIGNAL(show_SWS_Window(SWSModel*)),
            this, SLOT(makeSWSWindow(SWSModel*)));

/*
    SubWindow* w = nullptr;
    w = new SubWindow(m_mdiArea);
    connect(w,
            SIGNAL(windowClosing(SubWindow*)),
            this,
            SLOT(subWindowClosed(SubWindow*)));


    m_mdiArea->addSubWindow(w);

    w->setWidget(widget);
    w->resize(300, 800);
    w->setIsAcquisitionWindow(false);
    w->setWindowTitle(path);
    w->show();

    connect(w,
            SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates )),
            widget,
            SLOT(windowChanged(Qt::WindowStates, Qt::WindowStates)));
*/

    _maps_workspace_dock = new QDockWidget(tr("MAPS Workspace"), this);
    _maps_workspace_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    _maps_workspace_dock->setWidget(widget);
    addDockWidget(Qt::LeftDockWidgetArea, _maps_workspace_dock);

    // TODO: this should be changed. will cause the first not to load
    // if loading 2 workspaces right after another
    if(_load_maps_workspace_thread != nullptr)
    {
        _load_maps_workspace_thread->join();
        delete _load_maps_workspace_thread;
    }
    _load_maps_workspace_thread = new std::thread( [model, path]()
    {
        try
        {
            model->load(path);
        }
        catch(std::string& s)
        {
            qDebug()<<"Failed to open maps workspace.\n\n"<<QString(s.c_str());
        }
    });

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
    widget->setModel(model, nullptr, nullptr);
    //widget->resize(800, 600);


    SubWindow* w = nullptr;
    w = new SubWindow(m_mdiArea);
    connect(w,
            SIGNAL(windowClosing(SubWindow*)),
            this,
            SLOT(subWindowClosed(SubWindow*)));


    m_mdiArea->addSubWindow(w);

    w->setWidget(widget);
    w->resize(950, 700);
    w->setIsAcquisitionWindow(false);
    w->setWindowTitle(model->getFilePath());
    w->show();

    //    m_subWindows[w->getUuid()] = tiffController;

    connect(w,
            SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates )),
            widget,
            SLOT(windowChanged(Qt::WindowStates, Qt::WindowStates)));


}

/*---------------------------------------------------------------------------*/

void uProbeX::makeHDFWindow(MapsH5Model* model,
                            data_struct::Fit_Parameters* fit_params,
                            data_struct::Fit_Element_Map_Dict *elements_to_fit)
{
    MapsElementsWidget* widget = new MapsElementsWidget();
    widget->setModel(model, fit_params, elements_to_fit);
    //widget->resize(800, 600);


    SubWindow* w = nullptr;
    w = new SubWindow(m_mdiArea);
    connect(w,
            SIGNAL(windowClosing(SubWindow*)),
            this,
            SLOT(subWindowClosed(SubWindow*)));


    m_mdiArea->addSubWindow(w);

    w->setWidget(widget);
    w->resize(950, 700);
    w->setIsAcquisitionWindow(false);
    w->setWindowTitle(model->getFilePath());
    w->show();

    //    m_subWindows[w->getUuid()] = tiffController;

    connect(w,
            SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates )),
            widget,
            SLOT(windowChanged(Qt::WindowStates, Qt::WindowStates)));


}

/*---------------------------------------------------------------------------*/

void uProbeX::makeMDAWindow(MDA_Model* model)
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
    w->resize(950, 700);
    w->setIsAcquisitionWindow(false);
    w->setWindowTitle(model->getFilePath());
    w->show();

    //    m_subWindows[w->getUuid()] = tiffController;

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

void uProbeX::openAcquisitionData(QString path)
{

    if (path.isNull() || path.isEmpty()) return;

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

    makeMapsWindow(dirName);

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

void uProbeX::saveXYToCoefficient(double& valX,
                                  double& valY,
                                  QStringList& coefList,
                                  QStringList& newAttrs)
{

    for(int i=0; i<coefList.size(); i++)
    {
        QString attr = coefList.at(i);
        QStringList l = attr.split(",");
        if (l.size() != 4)  continue;

        if(l.at(0) == "m2xfm_x" )
        {
            newAttrs.append(QString("%1,%2,%3,%4").arg(l.at(0))
                            .arg(QString::number(valX))
                            .arg(l.at(2))
                            .arg(l.at(3)));
        }
        else if(l.at(0) == "m2xfm_y")
        {
            newAttrs.append(QString("%1,%2,%3,%4").arg(l.at(0))
                            .arg(QString::number(valY))
                            .arg(l.at(2))
                            .arg(l.at(3)));
        }
        else
        {
            newAttrs.append(attr);
        }
    }

}

/*---------------------------------------------------------------------------*/

void uProbeX::saveScreenShot()
{

    if(m_mdiArea->activeSubWindow() != 0)
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

    if(m_mdiArea->activeSubWindow() != 0)
    {
        SubWindow* w = dynamic_cast<SubWindow*>(m_mdiArea->activeSubWindow());
        if (w == nullptr) return;
        gstar::AbstractImageWidget* imageWidget = dynamic_cast<gstar::AbstractImageWidget*>(w->widget());
        SWSWidget* swsWidget;
        if(typeid(*imageWidget) == typeid(SWSWidget))
        {
            swsWidget = dynamic_cast<SWSWidget*>(imageWidget);
        } else {
            QMessageBox::critical(this, tr("Warning"),
                                  tr("No SWS Window Activated\n\n"));
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

        for(AbstractWindowController* con : m_subWindows.values())
        {
            gstar::AbstractImageWidget* w = con->getWidget();
            if(typeid(*w) == typeid(SWSWidget))
            {
                widget = (SWSWidget*)w;
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

    for(AbstractWindowController* con : m_subWindows.values())
    {
        gstar::AbstractImageWidget* w = con->getWidget();
        if(typeid(*w) == typeid(SWSWidget))
        {
            widget = (SWSWidget*)w;
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
        QMessageBox::warning(0, "Read only", "The file is in read only mode");
    }

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.setAutoFormattingIndent(true);

    xmlWriter.writeStartDocument();

    m_preferences.getEnvironmentInfo(&xmlWriter, PREFERENCES_XML_SECTION_NAME);

    xmlWriter.writeEndDocument();
}

/*---------------------------------------------------------------------------*/

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
        QMessageBox::information(0, "Preferences Loaded", "Preferences loaded currently are the same as in preferences file.");
    }
}

/*---------------------------------------------------------------------------*/

void uProbeX::performAutoSave()
{
    SWSWidget* widget = nullptr;

    for(AbstractWindowController* con : m_subWindows.values())
    {
        gstar::AbstractImageWidget* w = con->getWidget();
        if(typeid(*w) == typeid(SWSWidget))
        {
            widget = (SWSWidget*)w;
            widget->saveTemporaryXMLCoordinateInfo();
        }
    }
}

/*---------------------------------------------------------------------------*/

void uProbeX::cleanUpAutoSafeData() {
    SWSWidget* widget = nullptr;

    for(AbstractWindowController* con : m_subWindows.values())
    {
        gstar::AbstractImageWidget* w = con->getWidget();
        if(typeid(*w) == typeid(SWSWidget))
        {
            widget = (SWSWidget*)w;
            widget->cleanUpTemoraryXMLFiles();
        }
    }
}

/*---------------------------------------------------------------------------*/

void uProbeX::showAbout()
{

    // New about screen
    if (m_splashAbout == nullptr) {
        m_splashAbout = new Splash(this, Qt::Popup, "uProbeX", true);
    }

    // Show and add message
    m_splashAbout -> setWindowModality(Qt::ApplicationModal);
    m_splashAbout -> show();
    m_splashAbout -> clear();
    m_splashAbout -> appendMessage(tr("Created by"));
    m_splashAbout -> appendMessage(tr("Software Services Group"));
    m_splashAbout -> appendMessage(tr("Advanced Photon Source"));
    m_splashAbout -> appendMessage(tr("Argonne National Laboratory"));
    m_splashAbout -> appendMessage(tr(""));
    m_splashAbout -> appendMessage(tr("Copyright (c) 2014"));
    m_splashAbout -> appendMessage(tr("UChicago Argonne, LLC"));
    m_splashAbout -> appendMessage(tr(""));
    m_splashAbout -> appendMessage(tr("Credits"));
    m_splashAbout -> appendMessage(tr("Arthur Glowacki"));
    m_splashAbout -> appendMessage(tr("Dariusz Jarosz"));
    m_splashAbout -> appendMessage(tr("Nicholas Schwarz"));
    m_splashAbout -> appendMessage(tr("Ke Yue"));

}

/*---------------------------------------------------------------------------*/

void uProbeX::showPreferences()
{

    QList<gstar::AbstractImageWidget*> widgetList;
    for(AbstractWindowController* con : m_subWindows.values())
    {
        gstar::AbstractImageWidget* w = con->getWidget();
        if(typeid(*w) == typeid(SWSWidget))
        {
            widgetList.append(w);
        }
    }

    // Show preferences dialog with current settings
    PreferencesDialog* dialog = new PreferencesDialog(widgetList,
                                                      m_solver,
                                                      this,
                                                      Qt::Dialog);

    // Based on passive or active mode to show different preference dialog
    dialog->setPreferences(&m_preferences, false);

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
    QString title = m_preferences.readValueKey(
                Preferences::WindowTitle).toString();
    if(title.isEmpty())
        title = "uProbeX";
    setWindowTitle(title);

    adjustDisplaySettings();

    adjustAutoSaveSettings();

    createSolver();

    updateContextMenus();

    updatePreference();
}

/*---------------------------------------------------------------------------*/

void uProbeX::solverStart()
{

    SWSWidget* widget = nullptr;

    for(AbstractWindowController* con : m_subWindows.values())
    {
        gstar::AbstractImageWidget* w = con->getWidget();
        if(typeid(*w) == typeid(SWSWidget))
        {
            widget = (SWSWidget*)w;
            widget->widgetChanged(false);
        }
    }

}

/*---------------------------------------------------------------------------*/

void uProbeX::solverEnd()
{

    SWSWidget* widget = nullptr;

    for(AbstractWindowController* con : m_subWindows.values())
    {
        gstar::AbstractImageWidget* w = con->getWidget();
        if(typeid(*w) == typeid(SWSWidget))
        {
            widget = (SWSWidget*)w;
            widget->widgetChanged(true);
        }
    }

}

/*---------------------------------------------------------------------------*/

void uProbeX::solverVariableUpdate(double valX, double valY)
{

    int id = m_preferences.readValueKey(Preferences::SolverCheckedID).toInt();

    if (id == 0)
    {
        QStringList coefList = m_preferences.readValueKey(
                    Preferences::NMCoefficient).toStringList();
        QStringList newAttrs;

        saveXYToCoefficient(valX, valY, coefList, newAttrs);

        m_preferences.saveValueKey(Preferences::NMCoefficient,
                                   newAttrs);
    }
    else
    {
        QStringList coefList = m_preferences.readValueKey(
                    Preferences::PythonCoefficient).toStringList();
        QStringList newAttrs;

        saveXYToCoefficient(valX, valY, coefList, newAttrs);

        m_preferences.saveValueKey(Preferences::PythonCoefficient,
                                   newAttrs);
    }
    solverEnd();

}

/*---------------------------------------------------------------------------*/

void uProbeX::subWindowClosed(SubWindow* subWindow)
{

    if (subWindow == nullptr) return;

    QUuid id = subWindow->getUuid();

    QMap<QUuid, AbstractWindowController*>::iterator i = m_subWindows.find(id);
    if (i == m_subWindows.end()) return;

    saveActivatedXML();

    // Found the controller to close.
    AbstractWindowController* absWinCon = i.value();
    if (absWinCon == nullptr) return;

    m_subWindows.erase(i);
    delete absWinCon;
    absWinCon = nullptr;

}

/*---------------------------------------------------------------------------*/

void uProbeX::updatePreference()
{

    SWSWidget* widget = nullptr;

    for(AbstractWindowController* con : m_subWindows.values())
    {
        gstar::AbstractImageWidget* w = con->getWidget();
        if(typeid(*w) == typeid(SWSWidget))
        {
            widget = (SWSWidget*)w;
            widget->preferenceChanged();
        }
    }

}

/*---------------------------------------------------------------------------*/

void uProbeX::updateContextMenus()
{

    SWSWidget* widget = nullptr;

    for(AbstractWindowController* con : m_subWindows.values())
    {
        gstar::AbstractImageWidget* w = con->getWidget();
        if(typeid(*w) == typeid(SWSWidget))
        {
            widget = (SWSWidget*)w;
            widget->updateContextMenus();
        }
    }

}

/*---------------------------------------------------------------------------*/
