/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SCAN_QUEUE_WIDGET_H
#define SCAN_QUEUE_WIDGET_H

//---------------------------------------------------------------------------

#include <QWidget>
#include <QTextEdit>
#include <QTableView>
#include <QPushButton>
#include "mvc/BlueskyPlan.h"
#include "mvc/ScanQueueTableModel.h"
#include "mvc/ScanRegionDialog.h"

//---------------------------------------------------------------------------


class ScanQueueWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   ScanQueueWidget(ScanRegionDialog* scan_dialog, QWidget *parent = nullptr);

   /**
    * Destructor.
    */
   ~ScanQueueWidget();

   void updateQueuedItems( std::vector<BlueskyPlan> &finished_plans, std::vector<BlueskyPlan> &queued_plans, BlueskyPlan &running_plan);

   const std::map<QString, QString>& get_uuid_links();

signals:
   void queueNeedsToBeUpdated();

   void onOpenEnv();

   void onCloseEnv();

   void onStartQueue();

   void onStopQueue();

   void onMoveScanRow(int, int);

   void onMoveScanUp(int);

   void onMoveScanDown(int);

   void onRemoveScan(int);

   void onPlanChanged(BlueskyPlan&);

   void onAddScan(BlueskyPlan&);

   void onSetHistory();

   void onClearHistory();

   void onScanProgress(int);

   void planFilenameChanged(QString, QString); 

public slots:
   void newDataArrived(const QString &);

   void scanContextMenu(const QPoint &);

   void on_move_scan_up();

   void on_move_scan_down();

   void on_remove_scan();

   void on_clear_history();

protected:

   void _updateRunningPlanMetaInfo(const QString& msg);
   /**
    * @brief Create layout
    */
   void _createLayout();

   QTextEdit *_te_qs_console;

   QTableView* _scan_queue_table_view;

   ScanQueueTableModel* _scan_queue_table_model;

   QPushButton* _btn_play;
   
   QPushButton* _btn_stop;

   QPushButton* _btn_refresh;

   QPushButton* _btn_open_env;

   QPushButton* _btn_close_env;

   QPushButton* _btn_export_and_clear_history;
   
   QPushButton* _btn_add_scan;

   QPushButton* _btn_set_history;

   QPushButton* _btn_clear_history;

   QAction* _move_scan_up;
   
   QAction* _move_scan_down;

   QAction* _remove_scan;

   ScanRegionDialog* _scan_dialog;
};


//---------------------------------------------------------------------------

#endif /* SCAN_QUEUE_WIDGET_H */

//---------------------------------------------------------------------------
