/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef VLM_WIDGET_H
#define VLM_WIDGET_H

//---------------------------------------------------------------------------

#include <gstar/AbstractImageWidget.h>
#include <QApplication>
#include <gstar/AnnotationTreeModel.h>
#include <QXmlStreamWriter>
#include <QFile>
#include <QMap>
#include <QMessageBox>
#include <QMapIterator>
#include <QXmlStreamReader>
#include <QGraphicsView>
#include <QAbstractTableModel>
#include <QPushButton>
#include <QLineEdit>
#include <mvc/VLM_Model.h>

#include <preferences/SolverParameterParse.h>
#include <solver/Solver.h>
#include <solver/NelderMeadSolver.h>
#include <solver/PythonSolver.h>
#include <solver/SV_CoordTransformer.h>
#include <solver/LinearCoordTransformer.h>
#include <solver/PythonTransformer.h>
#include <solver/LinearSolver.h>
#include "mvc/BlueskyPlan.h"
#include <mvc/ScanRegionLinkDialog.h>


class Solver;
class SolverWidget;
class Preferences;
class RegionCaller;

namespace gstar
{
   class CoordinateModel;
   class CoordinateWidget;
   class UProbeRegionGraphicsItem;
}


//---------------------------------------------------------------------------

/**
 * @brief Widget used to display VLM workspaces. Used with VLM_Model.
 */
class VLM_Widget
: public gstar::AbstractImageWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   VLM_Widget(QWidget* parent = nullptr);

   /**
    * Constructor.
    */
   VLM_Widget(QString dataset_name, QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   virtual ~VLM_Widget();

   /**
    * @brief setCoordinateModel: Model is displayed at the bottom of the screen
    * @param model
    */
   virtual void setCoordinateModel(gstar::CoordinateModel *model);

   /**
    * @brief setLightToMicroCoordModel
    * @param model
    */
   void setLightToMicroCoordModel(gstar::CoordinateModel* model);

   /**
    * @brief setMarker
    * @param filepath
    */
 //  void setMarker(QString filepath);

   /**
    * @brief getMarkerCoordinatePoints
    * @param listCoordPoints
    * @return
    */
   bool getMarkerCoordinatePoints(
         QList < QMap<QString,double> >& listCoordPoints);

   /**
    * @brief getModelFileName
    * @return
    */
   QString getModelFileName();

   /**
    * @brief updateContextMenus
    */
   void updateContextMenus();

   /**
    * @brief updateTreeView
    */
   void updateTreeView();

   /**
    * @brief preferenceChanged
    */
   void preferenceChanged();

   /**
    * @brief saveScreenShot
    */
   void saveScreenShot();

   /**
    * @brief get the coordinate info from the widget
    */
   void saveXMLCoordinateInfo();

   /**
    * @brief Save temp file of the current coordinate info.
    */
   void saveTemporaryXMLCoordinateInfo();

   /**
    * @brief cleanUpTemoraryXMLFiles
    */
   void cleanUpTemoraryXMLFiles();

   /**
    * @brief verifySaveIsRequired
    */
   bool verifySaveIsRequired();

   void setModel(VLM_Model* model);

   /**
    * @brief widgetChanged
    */
   void widgetChanged(bool enable);

   /**
    * @brief addMicroProbeRegion
    */
   void addMicroProbeRegion(gstar::UProbeRegionGraphicsItem* annotation);

   void setAvailScans(std::map<QString, BlueskyPlan> * avail_scans);

   void load_live_coord_settings();

   void setEnableChangeBackground(bool val);

public slots:

   /**
    * @brief windowChanged: Accept the window change state
    * @param oldState
    * @param newState
    */
   void windowChanged(Qt::WindowStates oldState, Qt::WindowStates newState);

protected slots:

   /**
    * @brief addCalibration
    */
   void addCalibration();

   /**
    * @brief addTopWindowPoints
    */
   void addTopWindowPoints();

   /**
    * @brief addMicroProbeRegion
    */
   void onAddMicroProbeRegion();

   void onConfigRegionLink();

   /**
    * @brief zoomMicroProbeRegion
    */
   void zoomMicroProbeRegion();

   void onExportSelectedRegionInformation();
   /**
    * @brief exportSelectedRegionInformation
    * @param summaryInformation - When pointer is provided, only simulated output is generated. (must be specified with summaryWarnings)
    * @param summaryWarnings - When pointer is provided, only simulated output is generated. (must be specified with summaryInformation)
    *
    */
   void exportSelectedRegionInformation(QList<QString>* summaryInformation = nullptr, QList<QString>* summaryWarnings = nullptr);

   /**
    * @brief calModelDataChanged
    */
   void calModelDataChanged(const QModelIndex &, const QModelIndex &);

   /**
    * @brief CallPythonFunc
    */
   void CallPythonFunc();

   /**
    * @brief createMicroProbeMenu
    */
   void createMicroProbeMenu();

   /**
    * @brief deleteItem
    */
   virtual void deleteItem();

   /**
    * @brief duplicateItem
    */
   virtual void duplicateItem();

   /**
    * @brief get the marker info from the widget
    */
   void getMarkerInfo(QXmlStreamWriter* xmlWriter);

   /**
    * @brief get the marker info from the widget
    */
   void getRegionMarkerInfo(QXmlStreamWriter* xmlWriter);

   /**
    * @brief grabMicroProbePV
    */
   void grabMicroProbePV();

   /**
    * @brief microModelDataChanged
    * @param
    * @param
    */
   void microModelDataChanged(const QModelIndex &, const QModelIndex &);

   /**
    * @brief cancelUpdatedSolverVariables
    */
   void cancelUpdatedSolverVariables();

   /**
    * @brief runSolver
    */
  // void runSolver();

   /**
    * @brief openSolver
    */
   void openSolver();

   /**
    * @brief tabIndexChanged
    * @param index
    */
   void tabIndexChanged(int index);

   /**
    * @brief treeContextMenu
    * @param pos
    */
   void treeContextMenu(const QPoint& pos);

   /**
    * @brief updatedPixelToLight
    * @param x
    * @param y
    * @param z
    */
   void updatedPixelToLight(double x, double y, double z);

   /**
    * @brief useUpdatedSolverVariables
    * @param vars
    */
   void useUpdatedSolverVariables(const QMap<QString, double> vars);

   void linkRegionToDataset();

   void onUpdateBackgroundImage();

   void onCaptureBackgroundImage();

protected:

   void _init();

   /**
    * @brief callbackPvXUpdatedFloat
    * @param val
    */
   void callbackPvXUpdatedFloat(float val);

   /**
    * @brief callbackPvYUpdatedFloat
    * @param val
    */
   void callbackPvYUpdatedFloat(float val);

   /**
    * @brief callbackPvXUpdatedDouble
    * @param val
    */
   void callbackPvXUpdatedDouble(double val);

   /**
    * @brief callbackPvYUpdatedDouble
    * @param val
    */
   void callbackPvYUpdatedDouble(double val);

   /**
    * @brief createActions
    */
   void createActions();

   /**
    * @brief createCalibrationTab
    */
   void createCalibrationTab();

   /**
    * @brief createMicroProbeTab
    */
   void createMicroProbeTab();

   /**
    * @brief Create layout
    */
   void createLayout();

   /**
    * @brief checkMicroProbePVs
    */
   void checkMicroProbePVs();

   /**
    * @brief displayContextMenu
    * @param parent
    * @param pos
    */
   virtual void displayContextMenu(QWidget* parent, const QPoint& pos);

   /**
    * Parse xml file
    */
   //void parseXML();

   /**
    * Parse marker
    * @param xml
    */
   //QMap<QString, QString> parseMarker(QXmlStreamReader& xml);

   ///**
   // * @brief Parse region marker
   // * @param xml
   // */
   //QMap<QString, QString> parseRegionMarker(QXmlStreamReader& xml);

   /**
    * 
    */
   void restoreMarkerLoaded();

signals:

   /**
    * @brief SolverStart
    */
   void solverStart();

   /**
    * @brief SolverVariableUpdate
    * @param valX
    * @param valY
    */
   void solverVariableUpdate();

   /**
    * @brief CancelSolverVariableUpdate
    */
   void cancelSolverVariableUpdate();

   void onLinkRegionToDataset(QString, QString, QImage);

   void onScanUpdated(const BlueskyPlan &plan); 

private:

   /**
    * @brief saveXMLCoordinateInfo
    * @param savePath
    */
   void saveXMLCoordinateInfo(QString savePath);

   /**
    * @brief exportRegionImage
    * @param region
    * @param filePathToSave
    * @param zoomToRegion
    */
   void exportRegionXMLAndImage(gstar::UProbeRegionGraphicsItem* region,
                                QString filePathToSave,
                                bool zoomToRegion,
                                bool printNameOnImage,
                                bool printPredictedXYOnImage,
                                bool printWidthHeightOnImage,
                                QXmlStreamWriter* xmlStreamWriter = nullptr);

   /**
    * @brief writeXMLSaveData
    * @param device
    */
   void writeXMLSaveData(QIODevice* device);

   /**
    * Location of the dataset that is temporarly saved during runtime.
    *
    * @brief getTemporaryDatasetPath
    * @return
    */
   QString getTemporaryDatasetPath();

   /**
    * Used for getting a selected region graphics item.
    *
    * @brief getSelectedRegion
    * @return nullptr when invalid selection is made.
    */
   gstar::UProbeRegionGraphicsItem* getSelectedRegion();

   void _createLightToMicroCoords(int id);

   void _createSolver();


   VLM_Model* _model;

   /**
    * @brief m_addCalibrationAction
    */
   QAction* m_addCalibrationAction;

   /**
    * @brief m_grabMicroProbePVAction
    */
   QAction* m_grabMicroProbePVAction;

   /**
    * @brief m_addMicroPrboeRegionAction
    */
   QAction* m_addMicroPrboeRegionAction;

   /**
     * @brief m_zoomMicroProbeRegionAction
     */
   QAction* m_zoomMicroProbeRegionAction;

   /**
    * @brief m_exportMicroProbeRegionInfoAction
    */
   QAction* m_exportMicroProbeRegionInfoAction;

   QAction* _linkRegionToDatasetAction;

   /**
    * @brief m_colorValue
    */
   QMap<QString,QString> m_colorValue;

   /**
    * @brief m_lxValue
    */
   QMap<QString,QString> m_lxValue;

   /**
    * @brief m_lyValue
    */
   QMap<QString,QString> m_lyValue;

   /**
    * @brief m_lzValue
    */
   QMap<QString,QString> m_lzValue;

   /**
    * @brief m_markerName
    */
   QMap<QString,QString> m_markerName;

   /**
    * @brief m_mxValue
    */
   QMap<QString,QString> m_mxValue;

   /**
    * @brief m_myValue
    */
   QMap<QString,QString> m_myValue;

   /**
    * @brief m_microProbePvSet
    */
   bool m_microProbePvSet;

   /**
    * @brief m_pxValue
    */
   QMap<QString,QString> m_pxValue;

   /**
    * @brief m_pyValue
    */
   QMap<QString,QString> m_pyValue;

   /**
    * @brief m_btnAddCalibration
    */
   QPushButton* m_btnAddCalibration;

   /**
    * @brief m_btnAddTopWindowPoints
    */
   QPushButton* m_btnAddTopWindowPoints;

   /**
    * @brief m_btnAddBottomWindowPoints
    */
   QPushButton* m_btnAddBottomWindowPoints;

   /**
    * @brief m_btnAddCalibration
    */
   QPushButton* m_btnAddMicroProbe;

   QPushButton* m_btnConfigRegionLink;

   /**
    * @brief m_btnRunSolver
    */
   QPushButton* m_btnRunSolver;

   QPushButton* _btnSetBackground;

   QPushButton* _btnCaptureBackground;

   /**
    * @brief m_calibrationTabWidget
    */
   QWidget* m_calibrationTabWidget;

   /**
    * @brief m_calTreeModel
    */
   gstar::AnnotationTreeModel* m_calTreeModel;

   /**
    * @brief m_calAnnoTreeView
    */
   QTreeView* m_calAnnoTreeView;

   /**
    * @brief m_calSelectionModel
    */
   QItemSelectionModel* m_calSelectionModel;

   /**
    * @brief m_grabbingPvs
    */
   bool m_grabbingPvsX;

   /**
    * @brief m_grabbingPvsY
    */
   bool m_grabbingPvsY;

   /**
    * @brief m_coordTransformer
    */
   Solver* m_solver;

   /**
    * @brief m_solverParameterParse
    */
   SolverParameterParse* m_solverParameterParse;

   /**
    * @brief m_lightToMicroCoordModel
    */
   gstar::CoordinateModel* m_lightToMicroCoordModel;

   /**
    * @brief m_lightToMicroCoordWidget
    */
   gstar::CoordinateWidget* m_lightToMicroCoordWidget;

   /**
    * @brief m_microProbeTabWidget
    */
   QWidget* m_microProbeTabWidget;

   /**
    * @brief m_mpTreeModel
    */
   gstar::AnnotationTreeModel* m_mpTreeModel;

   /**
    * @brief m_mpAnnoTreeView
    */
   QTreeView* m_mpAnnoTreeView;

   /**
    * @brief m_mpSelectionModel
    */
   QItemSelectionModel* m_mpSelectionModel;
   
   /**
    * @brief m_solverWidget
    */
   SolverWidget* m_solverWidget;

   /**
    * @brief m_pathFile
    */
   QString m_pathFile;

   /**
    * @brief m_datasetPath
    */
   QString m_datasetPath;

   /**
    * @brief m_actionMap
    */
   QMap<QAction*, RegionCaller*> m_actionMap;

   std::map<QString, BlueskyPlan> *_avail_scans;

   ScanRegionLinkDialog* _scan_region_link_dialog;

};


//---------------------------------------------------------------------------

#endif /* VLM_Widget_H_ */

//---------------------------------------------------------------------------

