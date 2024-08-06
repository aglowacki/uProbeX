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

public slots:
   void newDataArrived(const QString &);

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
   
};


//---------------------------------------------------------------------------

#endif /* LiveLiveMapsElementsWidget_H_ */

//---------------------------------------------------------------------------
