/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef LIVE_MAPS_ELEMENTS_WIDGET_H
#define LIVE_MAPS_ELEMENTS_WIDGET_H

//---------------------------------------------------------------------------

#include <QWidget>
#include <QTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QLineEdit>
#include <QTabWidget>
#include "NetStreamWorker.h"
#include "mvc/MapsElementsWidget.h"
#include "mvc/VLM_Widget.h"
#include "mvc/ScanQueueWidget.h"
#include "mvc/BlueskyComm.h"
#include "gstar/Annotation/ScanRegionGraphicsItem.h"

//---------------------------------------------------------------------------


class LiveMapsElementsWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   LiveMapsElementsWidget(QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~LiveMapsElementsWidget();

   QString getQServerIpAddress() {return _qserver_ip_addr->text(); }

   QString getIpAddress(){return _qline_ip_addr->text();}

   QString getPort(){return _qline_port->text();}
   
public slots:

   void newDataArrived(data_struct::Stream_Block<float>* new_packet);

   void updateIp();

   void image_changed(int start, int end);

   void updateScansAvailable();

   void getQueuedScans();

   void setHistoryLocation();

   void incHistoryLocation();

   void clearHistory();

   void saveHistory();

   void callOpenEnv();

   void callCloseEnv();

   void callStartQueue();

   void callStopQueue();

   void callMoveScanRow(int, int);

   void callMoveScanUp(int);

   void callMoveScanDown(int);

   void callRemoveRow(int);
   
   void callUpdatePlan(BlueskyPlan& plan);

   void callQueueScan(BlueskyPlan& plan);
   
   void callRemoveScan(BlueskyPlan plan);

   void callQueueScanRegion(BlueskyPlan& plan, gstar::ScanRegionGraphicsItem* item);

   void callUpdateScanRegion(BlueskyPlan& plan, gstar::ScanRegionGraphicsItem* item);
   
   void setPrograssBarRange(int range){_progressBar->setRange(0,100);}

   void updatePrograssBar(int perc){_progressBar->setValue(perc);}

   void onPlanFilenameChanged(QString uuid, QString filename);

protected:

   /**
    * @brief Create layout
    */
   void createLayout();

   void _load_last_auto_save();
   
   MapsElementsWidget *_mapsElementsWidget;

   VLM_Widget *_vlm_widget;

   ScanQueueWidget* _scan_queue_widget;

   QTabWidget *_tab_widget;

   QTextEdit *_textEdit;

   QProgressBar *_progressBar;

   NetStreamWorker* _streamWorker;

   BlueskyComm* _qserverComm;

   QLineEdit *_qserver_ip_addr;

   QLineEdit *_qline_ip_addr;

   QLineEdit *_qline_port;

   QPushButton *_btn_update;

   std::vector<MapsH5Model*> _maps_h5_models;

   MapsH5Model *_currentModel;

   std::map<QString, BlueskyPlan> _avail_scans;

   std::vector<BlueskyPlan> _queued_scans;

   std::vector<BlueskyPlan> _finished_scans;

   BlueskyPlan _running_scan;

   data_struct::Stream_Block<float>* _last_packet;

   std::string _prev_dataset_name;

   zmq::context_t *_context;

   LinearCoordTransformer _linear_trans;

   int _num_images;

   ScanRegionDialog* _scan_dialog;
};


//---------------------------------------------------------------------------

#endif /* LiveLiveMapsElementsWidget_H_ */

//---------------------------------------------------------------------------
