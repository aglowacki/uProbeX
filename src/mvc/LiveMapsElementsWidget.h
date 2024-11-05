/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef LIVE_MAPS_ELEMENTS_WIDGET_H
#define LIVE_MAPS_ELEMENTS_WIDGET_H

//---------------------------------------------------------------------------

#include <QWidget>
#include <QThread>
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

//---------------------------------------------------------------------------


class LiveMapsElementsWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   LiveMapsElementsWidget(QString ip="127.0.0.1", QString port="43434", QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~LiveMapsElementsWidget();

   QString getIpAddress(){return _qline_ip_addr->text();}

   QString getPort(){return _qline_port->text();}

public slots:

   void newDataArrived(data_struct::Stream_Block<float>* new_packet);

   void updateIp();

   void image_changed(int start, int end);

   void updateScansAvailable();

   void getQueuedScans();

   void setHistoryLocation();

   void clearHistory();

   void saveHistory();

   void callOpenEnv();

   void callCloseEnv();

   void callStartQueue();

   void callStopQueue();

   void callMoveScanRow(int, int);

   void callMoveScanUp(int);

   void callMoveScanDown(int);

   void callRemoveScan(int);
   
   void callUpdatePlan(const BlueskyPlan& plan);

   void callQueueScan(const BlueskyPlan& plan);
   
protected:

   /**
    * @brief Create layout
    */
   void createLayout();

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

   int _num_images;
};


//---------------------------------------------------------------------------

#endif /* LiveLiveMapsElementsWidget_H_ */

//---------------------------------------------------------------------------
