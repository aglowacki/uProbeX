/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ScanQueueWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QDockWidget>
#include "core/defines.h"

//---------------------------------------------------------------------------

ScanQueueWidget::ScanQueueWidget(QWidget *parent) : QWidget(parent)
{

    _createLayout();

}

//---------------------------------------------------------------------------

ScanQueueWidget::~ScanQueueWidget()
{

}

//---------------------------------------------------------------------------

void ScanQueueWidget::_createLayout()
{

    QVBoxLayout* layout = new QVBoxLayout();
    
    _scan_running_table_model = new ScanQueueTableModel();
    _scan_running_table_view = new QTableView();
    _scan_running_table_view->setFixedHeight(100);
    _scan_running_table_view->setModel(_scan_running_table_model);
    _scan_running_table_view->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
   // _scan_running_table_view->horizontalHeader()->resizeSections(QHeaderView::Interactive);

    _scan_queue_table_model = new ScanQueueTableModel();
    _scan_queue_table_view = new QTableView();
    _scan_queue_table_view->setModel(_scan_queue_table_model);
    _scan_queue_table_view->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
   // _scan_queue_table_view->horizontalHeader()->resizeSections(QHeaderView::Interactive);


    QSize btn_size(33,33);

    _btn_play = new QPushButton();
    _btn_play->setFixedSize(btn_size);
    _btn_play->setIcon(QIcon(":images/start.png"));
    _btn_play->setToolTip("Start Queue");
    connect(_btn_play, &QPushButton::pressed, this, &ScanQueueWidget::onStartQueue);

    _btn_stop = new QPushButton();
    _btn_stop->setIcon(QIcon(":images/stop.png"));
    _btn_stop->setFixedSize(btn_size);
    _btn_stop->setToolTip("Stop Queue");
    connect(_btn_stop, &QPushButton::pressed, this, &ScanQueueWidget::onStopQueue);

    _btn_refresh = new QPushButton();
    _btn_refresh->setIcon(QIcon(":images/refresh.png"));
    _btn_refresh->setFixedSize(btn_size);
    _btn_refresh->setToolTip("Manual Refresh Queue");
    connect(_btn_refresh, &QPushButton::pressed, this, &ScanQueueWidget::queueNeedsToBeUpdated);

    _btn_open_env = new QPushButton("Open Env");
    _btn_open_env->setToolTip("Open Environment");
    connect(_btn_open_env, &QPushButton::pressed, this, &ScanQueueWidget::onOpenEnv);

    _btn_close_env = new QPushButton("Close Env");
    _btn_close_env->setToolTip("Close Environment");
    connect(_btn_close_env, &QPushButton::pressed, this, &ScanQueueWidget::onCloseEnv);

    QGridLayout *grid = new QGridLayout();
    grid->addWidget(_btn_play, 0,0);
    grid->addWidget(_btn_stop,0,1);
    grid->addWidget(_btn_refresh,0,2);
    grid->addWidget(_btn_open_env,0,3);
    grid->addWidget(_btn_close_env,0,4);
    grid->addItem(new QSpacerItem(999,10), 0,6);

    layout->addItem(grid);

    _te_qs_console = new QTextEdit(this);
    _te_qs_console->scrollBarWidgets(Qt::AlignRight);
    _te_qs_console->setReadOnly(true);
    
    QDockWidget *dock_running = new QDockWidget("Running Scan", this);
    dock_running->setWidget(_scan_running_table_view);
    layout->addWidget(dock_running);

    QDockWidget *dock_queue = new QDockWidget("Scan Queue", this);
    dock_queue->setWidget(_scan_queue_table_view);
    layout->addWidget(dock_queue);

    QDockWidget *dock_log = new QDockWidget("QServer Log", this);
    dock_log->setWidget(_te_qs_console);
    layout->addWidget(dock_log);

    setLayout(layout);

}

//---------------------------------------------------------------------------

void ScanQueueWidget::updateQueuedItems( std::vector<BlueskyPlan> &queued_plans, BlueskyPlan &running_plan)
{
    _scan_queue_table_model->setAllData(queued_plans);
    _scan_queue_table_view->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    std::vector<BlueskyPlan> runlist;
    if(running_plan.uuid.length() > 0)
    {
        runlist.push_back(running_plan);
    }
    _scan_running_table_model->setAllData(runlist);
    _scan_running_table_view->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

//---------------------------------------------------------------------------

void ScanQueueWidget::newDataArrived(const QString& data)
{
    //_te_qs_console->insertPlainText(data);
    _te_qs_console->append(data);
    if (data.count("Item added: success=True item_type='plan'") > 0
    || data.count("Plan failed:") > 0
    || data.count("The plan was exited") > 0
    || data.count("Removing item from the queue") > 0
    || data.count("Starting the plan") > 0
    || data.count("Queue is empty") > 0)
    {
        emit queueNeedsToBeUpdated();
    }
    // if ERROR:bluesky:Run aborted
}

//---------------------------------------------------------------------------
