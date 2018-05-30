/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SWS_WIDGET_H
#define SWS_WIDGET_H

/*---------------------------------------------------------------------------*/

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

#include <preferences/SolverParameterParse.h>
//#include <SStar.h>


class QAbstractTableModel;
class QPushButton;
class Solver;
class QLineEdit;
class SolverWidget;
class Preferences;
class RegionCaller;

namespace gstar
{
   class CoordinateModel;
   class CoordinateWidget;
   class UProbeRegionGraphicsItem;
}


/*---------------------------------------------------------------------------*/

/**
 * @brief Widget used to display SWS workspaces. Used with SWSModel.
 */
class SWSWidget
: public gstar::AbstractImageWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   SWSWidget(Solver *solver, Preferences* pref, QWidget* parent = NULL);


   /**
    * Destructor.
    */
   ~SWSWidget();

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
   void setMarker(QString filepath);

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

   /**
    * @brief widgetChanged
    */
   void widgetChanged(bool enable);

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
   void addMicroProbeRegion();

   /**
    * @brief zoomMicroProbeRegion
    */
   void zoomMicroProbeRegion();

   /**
    * @brief exportSelectedRegionInformation
    * @param summaryInformation - When pointer is provided, only simulated output is generated. (must be specified with summaryWarnings)
    * @param summaryWarnings - When pointer is provided, only simulated output is generated. (must be specified with summaryInformation)
    *
    */
   void exportSelectedRegionInformation(QList<QString>* summaryInformation = NULL, QList<QString>* summaryWarnings = NULL);

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
    * @brief offsetReturnPressed
    */
   void offsetReturnPressed();

   /**
    * @brief cancelUpdatedSolverVariables
    */
   void cancelUpdatedSolverVariables();

   /**
    * @brief runSolver
    */
   void runSolver();

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

protected:

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
   void parseXML();

   /**
    * Parse marker
    * @param xml
    */
   QMap<QString, QString> parseMarker(QXmlStreamReader& xml);

   /**
    * @brief Parse region marker
    * @param xml
    */
   QMap<QString, QString> parseRegionMarker(QXmlStreamReader& xml);

   /**
    * Restore xml file
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
   void solverVariableUpdate(double valX, double valY);

   /**
    * @brief CancelSolverVariableUpdate
    */
   void cancelSolverVariableUpdate();

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
                                QXmlStreamWriter* xmlStreamWriter = NULL);

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
    * @return null when invalid selection is made.
    */
   gstar::UProbeRegionGraphicsItem* getSelectedRegion();

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

   /**
    * @brief m_markersLoaded
    */
   QList< QMap<QString,QString> > m_markersLoaded;

   /**
    * @brief m_markersLoaded
    */
   QList< QMap<QString,QString> > m_regionMarkersLoaded;

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

   /*
   CAEventHandler* m_pvXHandler;


   CAEventHandler* m_pvYHandler;


   boost::shared_ptr<PV> m_pvX;


   boost::shared_ptr<PV> m_pvY;
    */
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

   /**
    * @brief m_btnRunSolver
    */
   QPushButton* m_btnRunSolver;

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
    * @brief m_preferences
    */
   Preferences* m_preferences;

   /**
    * @brief m_solverWidget
    */
   SolverWidget* m_solverWidget;

   /**
    * @brief m_xOffset
    */
   QLineEdit* m_xOffset;

   /**
    * @brief m_xyffset
    */
   QLineEdit* m_yOffset;

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

};


/*---------------------------------------------------------------------------*/

#endif /* SWSWidget_H_ */

/*---------------------------------------------------------------------------*/

