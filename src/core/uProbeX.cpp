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
#include <mvc/MapsH5Model.h>
#include <mvc/MapsElementsWidget.h>
#include <mvc/MapsWorkspaceModel.h>
#include <preferences/CoordinateTransformGlobals.h>

#include <Splash.h>

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
    m_lightToMicroCoordModel = NULL;
    m_solver = NULL;
    m_autosaveTimer = NULL;
    m_solverParameterParse = new SolverParameterParse();


    //////// HENKE and ELEMENT INFO /////////////
    std::string element_csv_filename = "../reference/xrf_library.csv";
    std::string element_henke_filename = "../reference/henke.xdr";

    // Use resources from GStar
    Q_INIT_RESOURCE(GStar);

    PythonLoader::inst()->safeCheck();

    initialize();
    /*
    // Splash screen
    Splash splash(this, Qt::Popup, tr("uProbeX"), false);
    splash.show();
    splash.appendMessage(tr("Initializing..."));
    splash.appendMessageAndProcess(tr("Creating GUI..."));
*/
    // Create all actions
    createActions();

    // Create menu
    createMenuBar();

    // Creat MDI window
    m_mdiArea = new QMdiArea();
    m_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setCentralWidget(m_mdiArea);

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


    if (false == io::load_element_info(element_henke_filename, element_csv_filename, data_struct::xrf::Element_Info_Map::inst()) )
    {
        qDebug()<<"Error loading "<< element_henke_filename.c_str() <<" and "<< element_csv_filename.c_str();
    }


    show();
}

/*---------------------------------------------------------------------------*/

uProbeX::~uProbeX()
{


    if(m_solverParameterParse != NULL)
        delete m_solverParameterParse;
    m_solverParameterParse = NULL;

    if(m_lightToMicroCoordModel != NULL)
        delete m_lightToMicroCoordModel;
    m_lightToMicroCoordModel = NULL;

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

        if (h != NULL)
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

    foreach(AbstractWindowController* con, m_subWindows.values())
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

void uProbeX::createActions()
{

    m_preferencesAction = new QAction(tr("Preferences..."), this);
    connect(m_preferencesAction,
            SIGNAL(triggered()),
            this,
            SLOT(showPreferences()));

    m_openSWSAction = new QAction(tr("Open SWS Workspace"), this);
    connect(m_openSWSAction,
            SIGNAL(triggered()),
            this,
            SLOT(openSWSFile()));

    m_openMapsAction = new QAction(tr("Open Maps Workspace"), this);
    connect(m_openMapsAction,
            SIGNAL(triggered()),
            this,
            SLOT(openMapsWorkspace()));

    m_openHDFAction = new QAction(tr("Open HDF5 Analyzed"), this);
    connect(m_openHDFAction,
            SIGNAL(triggered()),
            this,
            SLOT(openHDFFile()));

    m_saveScreenShotAction = new QAction(tr("Save Screen Shot"), this);
    connect(m_saveScreenShotAction,
            SIGNAL(triggered()),
            this,
            SLOT(saveScreenShot()));

    m_saveActivatedXMLAction = new QAction(tr("Save Activated SWS DATA"), this);
    connect(m_saveActivatedXMLAction,
            SIGNAL(triggered()),
            this,
            SLOT(saveActivatedXML()));

    m_saveEnviornmentXMLAction = new QAction(tr("Save preferences"), this);
    connect(m_saveEnviornmentXMLAction,
            SIGNAL(triggered()),
            this,
            SLOT(savePreferencesXMLData()));

    m_loadEnviornmentXMLAction = new QAction(tr("Load preferences"), this);
    connect(m_loadEnviornmentXMLAction,
            SIGNAL(triggered()),
            this,
            SLOT(loadPreferencesXMLData()));

    m_saveAllXMLAction = new QAction(tr("Save All SWS DATA"), this);
    connect(m_saveAllXMLAction,
            SIGNAL(triggered()),
            this,
            SLOT(saveAllXML()));

    m_exitAction = new QAction(tr("Exit"), this);
    connect(m_exitAction, SIGNAL(triggered()), this, SLOT(exitApplication()));

    m_aboutAction = new QAction(tr("About..."), this);
    connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));

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


    ITransformer *lightTransformer = NULL;
    QMap<QString, double> allCoefs;

    if(m_lightToMicroCoordModel != NULL)
    {
        lightTransformer = m_lightToMicroCoordModel->getTransformer();
        if (lightTransformer != NULL)
        {
            delete lightTransformer;
            m_lightToMicroCoordModel->setTransformer(NULL);
        }
        lightTransformer = NULL;
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

        if(lightTransformer == NULL)
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

        if(lightTransformer == NULL)
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
        if(m_lightToMicroCoordModel != NULL)
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
        QMessageBox::critical(NULL, "uProbeX", "Error initializeing Transformer!");
        qDebug()<<"Could not init Transformer\n";
    }

}

/*---------------------------------------------------------------------------*/

void uProbeX::createMenuBar()
{

    // Menu bar
    m_menu = new QMenuBar(this);
    setMenuBar(m_menu);

    // File menu
    m_menuFile = new QMenu(tr("File"));
    m_menuFile->addAction(m_openMapsAction);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_openSWSAction);
    m_menuFile->addAction(m_openHDFAction);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_saveScreenShotAction);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_saveActivatedXMLAction);
    m_menuFile->addAction(m_saveAllXMLAction);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_saveEnviornmentXMLAction);
    m_menuFile->addAction(m_loadEnviornmentXMLAction);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_preferencesAction);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_exitAction);
    m_menu->addMenu(m_menuFile);

    connect(m_menuFile, SIGNAL(aboutToShow()), this, SLOT(menuBarEnable()));

    // Help menu
    m_menuHelp = new QMenu(tr("Help"));
    m_menuHelp->addAction(m_aboutAction);
    m_menu->addMenu(m_menuHelp);

}

/*---------------------------------------------------------------------------*/

void uProbeX::createSolver()
{

    QMap<QString, double> dict_options;

    if(m_solver == NULL)
        m_solver = new Solver();

    int id = m_preferences.readValueKey(Preferences::SolverCheckedID).toInt();

    createLightToMicroCoords(id);

    AbstractSolver *impl = m_solver->getImpl();
    if(impl!=NULL)
    {
        m_solver->setImpl(NULL);
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
            QMessageBox::critical(NULL, "uProbeX", "Error initializeing Python solver,  reverting to NelderMeadSolver");
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

void uProbeX::exitApplication()
{

    // Quit
    QCoreApplication::quit();

}

/*---------------------------------------------------------------------------*/

void uProbeX::initialize()
{

    m_splashAbout = NULL;
    m_mdiArea = NULL;

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
        if (m_autosaveTimer == NULL) {
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
        if (m_autosaveTimer != NULL)
        {
            disconnect(m_autosaveTimer,
                       SIGNAL(timeout()),
                       this,
                       SLOT(performAutoSave()));

            delete m_autosaveTimer;
            m_autosaveTimer = NULL;
        }
    }


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
        SWSModel* swsModel = new SWSModel(path);
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


    SubWindow* w = NULL;
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



    w->setWidget(swsWidget);
    w->resize(950, 700);
    w->setIsAcquisitionWindow(false);
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
    MapsWorkspaceModel* model = new MapsWorkspaceModel();
    bool loaded = false;
    try
    {
        loaded = model->load(path);
    }
    catch(std::string& s)
    {
        // Restore cursor
        //QApplication::restoreOverrideCursor();

        // Display error to user
        QMessageBox::critical(this, tr("uProbeX"),
                              tr("Failed to open maps workspace.\n\n") + tr("Error:  ") +
                              QString(s.c_str()));

        return;
    }

}

/*---------------------------------------------------------------------------*/

void uProbeX::makeHDFWindow(QString path)
{

    MapsH5Model* model = new MapsH5Model();
    bool loaded = false;
    try
    {
        loaded = model->load(path);
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

    if(loaded)
    {
        MapsElementsWidget* widget = new MapsElementsWidget();
        widget->setModel(model);
        //widget->resize(800, 600);




        SubWindow* w = NULL;
        w = new SubWindow(m_mdiArea);
        connect(w,
                SIGNAL(windowClosing(SubWindow*)),
                this,
                SLOT(subWindowClosed(SubWindow*)));


        w->setWidget(widget);
        //w->resize(950, 700);
        w->setIsAcquisitionWindow(false);
        w->setWindowTitle(path);
        w->show();

        //    m_subWindows[w->getUuid()] = tiffController;

        connect(w,
                SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates )),
                widget,
                SLOT(windowChanged(Qt::WindowStates, Qt::WindowStates)));
    }
    else
    {
        QMessageBox::critical(this, tr("uProbeX"),
                              tr("Failed to load HDF5 file.\n\n") + tr("Error:  ") +
                              QString(path.toStdString().c_str()));
    }
}

/*---------------------------------------------------------------------------*/

void uProbeX::menuBarEnable()
{

    if(m_mdiArea->activeSubWindow() != 0)
    {

        m_saveActivatedXMLAction->setEnabled(saveActivatedXmlRequired());
        m_saveAllXMLAction->setEnabled(saveAllXMLRequired());

        m_saveScreenShotAction->setEnabled(true);
    }
    else
    {
        m_saveScreenShotAction->setEnabled(false);
        m_saveActivatedXMLAction->setEnabled(false);
        m_saveAllXMLAction->setEnabled(false);
    }

}

/*---------------------------------------------------------------------------*/

void uProbeX::openAcquisitionData(QString path)
{

    if (path.isEmpty() || path.isNull()) return;

}

/*---------------------------------------------------------------------------*/

void uProbeX::openSWSFile()
{

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open SWS workspace", ".",
                                                    tr("SWS (*.sws *.SWS)"));

    // Dialog returns a null string if user press cancel.
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

    // Dialog returns a null string if user press cancel.
    if (dirName.isNull() || dirName.isEmpty()) return;

    makeMapsWindow(dirName);

}

/*---------------------------------------------------------------------------*/

void uProbeX::openHDFFile()
{

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open HDF5", ".",
                                                    tr("H5 (*.h5 *.h50 *.h51 *.h52 *.h53)"));

    // Dialog returns a null string if user press cancel.
    if (fileName.isNull() || fileName.isEmpty()) return;

    QString filePath = QFileInfo(fileName).canonicalFilePath();

    /*
   QString fileSuffix = QFileInfo(fileName).suffix();

   if (0==fileSuffix.compare("sws")||0==fileSuffix.compare("SWS"))
   {
      // Check if the image has been opened
      if(true == checkSameFileWindow(filePath))
         return;

      makeSWSWindow(filePath, true);

   }
*/
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
        if (w == NULL) return;
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
        if (w == NULL) return;
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
        SWSWidget* widget = NULL;

        foreach(AbstractWindowController* con, m_subWindows.values())
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

    SWSWidget* widget = NULL;

    foreach(AbstractWindowController* con, m_subWindows.values())
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
    SubWindow* w = dynamic_cast<SubWindow*>(m_mdiArea->activeSubWindow());
    if (w != NULL) {
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

    // Dialog returns a null string if user press cancel.
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
    SWSWidget* widget = NULL;

    foreach(AbstractWindowController* con, m_subWindows.values())
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
    SWSWidget* widget = NULL;

    foreach(AbstractWindowController* con, m_subWindows.values())
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
    if (m_splashAbout == NULL) {
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
    m_splashAbout -> appendMessage(tr("Nicholas Schwarz"));
    m_splashAbout -> appendMessage(tr("Ke Yue"));
    m_splashAbout -> appendMessage(tr("Arthur Glowacki"));
    m_splashAbout -> appendMessage(tr("Dariusz Jarosz"));
}

/*---------------------------------------------------------------------------*/

void uProbeX::showPreferences()
{

    QList<gstar::AbstractImageWidget*> widgetList;
    foreach(AbstractWindowController* con, m_subWindows.values())
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

    SWSWidget* widget = NULL;

    foreach(AbstractWindowController* con, m_subWindows.values())
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

    SWSWidget* widget = NULL;

    foreach(AbstractWindowController* con, m_subWindows.values())
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

    if (subWindow == NULL) return;

    QUuid id = subWindow->getUuid();

    QMap<QUuid, AbstractWindowController*>::iterator i = m_subWindows.find(id);
    if (i == m_subWindows.end()) return;

    saveActivatedXML();

    // Found the controller to close.
    AbstractWindowController* absWinCon = i.value();
    if (absWinCon == NULL) return;

    m_subWindows.erase(i);
    delete absWinCon;
    absWinCon = NULL;

}

/*---------------------------------------------------------------------------*/

void uProbeX::updatePreference()
{

    SWSWidget* widget = NULL;

    foreach(AbstractWindowController* con, m_subWindows.values())
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

    SWSWidget* widget = NULL;

    foreach(AbstractWindowController* con, m_subWindows.values())
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
