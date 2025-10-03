/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef uProbeX_H
#define uProbeX_H

//---------------------------------------------------------------------------

#include <QMainWindow>
#include <QApplication>
#include <QMap>
#include <QUuid>
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <preferences/Preferences.h>
#include <mvc/MapsH5Model.h>
#include <mvc/LiveMapsElementsWidget.h>
#include <mvc/MapsWorkspaceController.h>
#include <mvc/SWSModel.h>
#include <mvc/RAW_Model.h>
#include <mvc/TIFF_Model.h>
#include <mvc/VLM_Widget.h>
#include "mvc/UpgradeRoiDialog.h"
#include <mvc/GenScanVlmWidget.h>

class SubWindow;
class AbstractWindowController;



//---------------------------------------------------------------------------

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
    uProbeX(QWidget* parent, Qt::WindowFlags flags = Qt::Widget);

   /**
    * Destructor
    */
   ~uProbeX();

   //Static log windows 
   static QTextEdit * log_textedit;

   void check_and_set_default_pref();
   
signals:

   void loadMapsWorkspace(MapsWorkspaceController*, QString);

private slots:

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
   void make_VLM_Window(QString path, bool newWindow = false);

   /**
    * @brief makeSWSWindow
    * @param swsModel
    */
   void make_VLM_Window(VLM_Model* model);

   /**
    * @brief laodMapsWorkspace
    * @param path
    */
   void onLoadMapsWorkspace(MapsWorkspaceController* controller, QString path);

   /**
    * @brief makeHDFWindow
    * @param path
    * @param newWindow
    */
   void make_HDF_Window(QString path);

   /**
    * @brief makeHDFWindow
    * @param model
    */
   void make_HDF_Window(MapsH5Model* model);

   /**
    * @brief makeMDAWindow
    * @param model
    */
   void make_MDA_Window(RAW_Model* model);

   void open_spectra_file();

   void open_spectra_and_override_file();

   void make_spectra_window(QString path, data_struct::Params_Override<double>* po);
   
   void open_VLM_File();

   /**
    * @brief openMAPSWorkspace
    */
   void openMapsWorkspaceA();

   void openRecentMapsWorkspace();

   void openMapsWorkspace(QString dirName);

   void updateRecentMapsWorkspaces();

   /**
    * @brief openHDFFile
    */
   void open_HDF_File();

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
   void saveAllXMLA();

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
   //void performAutoSave();

   /**
    * @brief cleanUpAutoSafeData
    */
   //void cleanUpAutoSafeData();
   /*
   void savePreferencesXMLData();

   void loadPreferencesXMLData();
   */
   void mapsControllerClosed(MapsWorkspaceController*);

   void upgradeV9Rois();

   void batchPerPixel();

   void BatchExportImages();

   void BatchRoiStats();

   void BatcGenScanVlm();

private:

   /**
    * @brief adjustAutoSaveSettings
    */
   //void adjustAutoSaveSettings();

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
    * @brief Create menu bar
    */
   void createMenuBar();

   /**
    * @brief Default initialization of variables such as setting pointers to nullptr
    */
   void initialize();
   
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

   void setBatchActionsEnabled(bool val);

private:

   /**
    * @brief Menu bar
    */
   QMenuBar* m_menu;

   QMenu* _menu_file;
   QMenu* _menu_recent_workspace;

   QMenu* _menu_stream;

   QMenu* _menu_batch;

   QMenu* _menu_view;
   QMenu* _menu_view_file_top;
   QMenu* _menu_view_file_side;
   QMenu* _menu_view_marker;

   QMenu* _menu_help;

   /**
    * @brief MDI area that serves as a central widget
    */
   QMdiArea* m_mdiArea;

   /**
    * @brief Keeps track of all open MDI child windows for hdf5 play-back.
    */
   QMap<QUuid, SubWindow*> m_subWindows;

   /**
    * @brief timer used to execute autosafe.
    */
   //QTimer* m_autosaveTimer;

   /**
    * @brief _liveMapsViewer
    */
   LiveMapsElementsWidget*  _liveMapsViewer;

   // log dock
   QDockWidget *_log_dock;

   UpgradeRoiDialog _upgradeRoiDialog;

   QAction* _action_per_pixel;

   QAction* _action_export_images;
   
   QAction* _action_roi_stats;

   QAction* _action_gen_scan_vlm;

   PerPixelFitWidget _per_pixel_fit_widget;

   QList<MapsWorkspaceController*> _mapsWorkspaceControllers;

   GenScanVlmWidget _gen_scan_vlm_widget;

};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
