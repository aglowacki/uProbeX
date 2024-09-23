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
//#include "mvc/DragDropTableView.h"

//---------------------------------------------------------------------------


class ScanQueueWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   ScanQueueWidget(QWidget *parent = nullptr);

   /**
    * Destructor.
    */
   ~ScanQueueWidget();

   void updateQueuedItems( std::vector<BlueskyPlan> &queued_plans, BlueskyPlan &running_plan);

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

public slots:
   void newDataArrived(const QString &);

   void scanContextMenu(const QPoint &);

   void on_move_scan_up();

   void on_move_scan_down();

   void on_remove_scan();

protected:

   /**
    * @brief Create layout
    */
   void _createLayout();

   QTextEdit *_te_qs_console;

   QTableView* _scan_queue_table_view;

   ScanQueueTableModel* _scan_queue_table_model;

   QTableView* _scan_running_table_view;

   ScanQueueTableModel* _scan_running_table_model;

   QPushButton* _btn_play;
   
   QPushButton* _btn_stop;

   QPushButton* _btn_refresh;

   QPushButton* _btn_open_env;

   QPushButton* _btn_close_env;
   
   QAction* _move_scan_up;
   
   QAction* _move_scan_down;

   QAction* _remove_scan;
};


//---------------------------------------------------------------------------

#endif /* SCAN_QUEUE_WIDGET_H */

//---------------------------------------------------------------------------
