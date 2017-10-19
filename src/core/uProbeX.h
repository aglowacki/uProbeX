/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef uProbeX_H
#define uProbeX_H

/*---------------------------------------------------------------------------*/

#include <QMainWindow>

#include <preferences/Preferences.h>
#include <preferences/SolverParameterParse.h>
#include <CoordinateModel.h>
#include <mvc/MapsH5Model.h>
#include <mvc/LiveMapsElementsWidget.h>
#include <QDesktopWidget>
#include <QApplication>
#include <QMap>
#include <QUuid>
#include <thread>

class SubWindow;
class AbstractWindowController;

class QAction;
class QActionGroup;
class QDockWidget;
class QMenu;
class QMenuBar;
class QToolBar;
class QMdiArea;
class QMdiSubWindow;
class Solver;

namespace gstar
{

  class Splash;

}

/*---------------------------------------------------------------------------*/

/**
 * @brief The main uProbeX user interface window. Main application class
 */
class uProbeX
: public QMainWindow
{

   Q_OBJECT

public:

   /**
    * Constructor. See Qt documentation for QMainWindow.
    */
    uProbeX(QWidget* parent, Qt::WindowFlags flags = 0);

   /**
    * Destructor
    */
   ~uProbeX();

private slots:

   /**
    * @brief cancelSolverVariableUpdate
    */
   void cancelSolverVariableUpdate();

   /**
    * @brief Reimplemented from QMainWindow. See Qt documentation.
    * @param event
    */
   void closeEvent(QCloseEvent* event);

   /**
    * @brief Handle application closing task. Triggered from the exit menu as well.
    */
   void exitApplication();

   /**
    * @brief openLiveStreamViewer
    */
   void openLiveStreamViewer();

   /**
    * @brief makeSWSWindow
    * @param path
    * @param newWindow
    */
   void makeSWSWindow(QString path, bool newWindow = false);

   /**
    * @brief makeMAPSWindow
    * @param path
    */
   void makeMapsWindow(QString path);

   /**
    * @brief makeHDFWindow
    * @param path
    * @param newWindow
    */
   void makeHDFWindow(QString path);

   /**
    * @brief makeHDFWindow
    * @param model
    */
   void makeHDFWindow(MapsH5Model* model);

   void makeFitSpectraWindow(data_struct::xrf::Fit_Parameters* fit_params,
                             data_struct::xrf::Fit_Element_Map_Dict *elements_to_fit,
                             MapsH5Model* model);

   /**
    * @brief Open the data file at the given path. Triggered by acquisition controller
    *
    * @param path - Path to the data file, can be empty. 
    */
   void openAcquisitionData(QString path);

   /**
    * @brief Open the SWS workspace. Triggered from the menu.
    */
   void openSWSFile();

   /**
    * @brief openMAPSWorkspace
    */
   void openMapsWorkspace();

   /**
    * @brief openHDFFile
    */
   void openHDFFile();

   /**
    * @brief Save the screen shot. Triggered from the menu.
    */
   void saveScreenShot();

   /**
    * @brief saveActivatedXML
    */
   void saveActivatedXML();

   /**
    * @brief saveAllXML
    */
   void saveAllXML();

   /**
    * @brief Show about dialog.Triggered from the menu.
    */
   void showAbout();

   /**
    * @brief Show the preferences dialog. Triggered from the menu.
    */
   void showPreferences();

   /**
    * @brief processPreferencesUpdate
    */
   void processPreferencesUpdate();

   /**
    * @brief solverStart
    */
   void solverStart();

   /**
    * @brief solverEnd
    */
   void solverEnd();

   /**
    * @brief solverVariableUpdate
    * @param valX
    * @param valY
    */
   void solverVariableUpdate(double valX, double valY);

   /**
    * @brief Handle event when a child MDI window is closed.
    * @param subWindow
    */
   void subWindowClosed(SubWindow* subWindow);

   /**
    * @brief updatePreference
    */
   void updatePreference();

   /**
    * @brief menuBarEnable
    */
   void menuBarEnable();

   /**
    * @brief perform autosave for the open datasets.
    */
   void performAutoSave();

   /**
    * @brief cleanUpAutoSafeData
    */
   void cleanUpAutoSafeData();

   /**
    * @brief savePreferencesDataInXML
    */
   void savePreferencesXMLData();

   /**
    * @brief loadPreferencesXMLData
    */
   void loadPreferencesXMLData();

private:

   /**
    * @brief adjustAutoSaveSettings
    */
   void adjustAutoSaveSettings();

   /**
    * @briefAdjust displays settings (Font type, size etc).
    *
    * The settings are read from the preferences object (if  exists)
    */
   void adjustDisplaySettings();      

   /**
    * @brief checkSameFileWindow
    * @param filePath
    * @return ture if two window open same image
    */
   bool checkSameFileWindow(QString& filePath);

   /**
    * @brief Create menu actions
    */
   void createActions();

   /**
    * @brief createLightToMicroCoords
    * @param id
    */
   void createLightToMicroCoords(int id);

   /**
    * @brief Create menu bar
    */
   void createMenuBar();

   /**
    * @brief createSolver
    */
   void createSolver();

   /**
    * @brief Default initialization of variables such as setting pointers to NULL
    */
   void initialize();

   /**
    * @brief saveSolver
    */
   //void saveSolver();

   /**
    * @brief saveXYToCoefficient
    * @param valX
    * @param valY
    * @param coefList
    * @param newAttrs
    */
   void saveXYToCoefficient(double& valX,
                            double& valY,
                            QStringList& coefList,
                            QStringList& newAttrs );

   /**
    * @brief updateContextMenus
    */
   void updateContextMenus();

   /**
    * @brief saveAllXML
    */
   void saveAllXML(bool verifyWithUser);

   /**
    * @brief saveAllXMLRequired
    * @return
    */
   bool saveAllXMLRequired();

   /**
    * @brief saveActiveXmlRequired
    * @return
    */
   bool saveActivatedXmlRequired();

private:   

   gstar::CoordinateModel* m_lightToMicroCoordModel;

   /**
    * @brief Show preferences dialog action
    */
   QAction* m_preferencesAction;

   /**
    * @brief Open sws workspace action
    */
   QAction* m_openSWSAction;

   /**
    * @brief m_openMAPSAction
    */
   QAction* m_openMapsAction;

   /**
    * @brief m_openHDFAction
    */
   QAction* m_openHDFAction;

   /**
    * @brief Open sws workspace action
    */
   QAction* m_saveScreenShotAction;

   /**
    * @brief m_saveActivatedXMLAction
    */
   QAction* m_saveActivatedXMLAction;

   /**
    * @brief m_saveEnviornmentXMLAction
    */
   QAction* m_saveEnviornmentXMLAction;

   /**
    * @brief m_loadEnviornmentXMLAction
    */
   QAction* m_loadEnviornmentXMLAction;

   /**
    * @brief Om_saveXMLAction
    */
   QAction* m_saveAllXMLAction;

   /**
    * @brief Exit action
    */
   QAction* m_exitAction;

   /**
    * @brief m_openStreamAction
    */
   QAction* m_openStreamAction;


   /**
    * @brief About action
    */
   QAction* m_aboutAction;

   /**
    * @brief Menu bar
    */
   QMenuBar* m_menu;

   /**
    * @brief File menu
    */
   QMenu* m_menuFile;

   /**
    * @brief Stream menu
    */
   QMenu* m_menuStream;


   /**
    * @brief Help menu
    */
   QMenu* m_menuHelp;

   /**
    * @brief Preferences
    */
   Preferences m_preferences;

   /**
    * @brief Splash screen
    */
   gstar::Splash* m_splashAbout;

   /**
    * @brief MDI area that serves as a central widget
    */
   QMdiArea* m_mdiArea;

   /**
    * @brief m_solver
    */
   Solver *m_solver;

   /**
    * @brief m_solverParameterParse
    */
   SolverParameterParse* m_solverParameterParse;

   /**
    * @brief Keeps track of all open MDI child windows for hdf5 play-back.
    */
   QMap<QUuid, AbstractWindowController*> m_subWindows;

   std::thread* _load_maps_workspace_thread;

   /**
    * @brief timer used to execute autosafe.
    */
   QTimer* m_autosaveTimer;

   LiveMapsElementsWidget*  _liveMapsViewer;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
