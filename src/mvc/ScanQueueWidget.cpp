/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ScanQueueWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
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

    _scan_queue_table_model = new ScanQueueTableModel();
    _scan_queue_table_view = new QTableView();
    _scan_queue_table_view->setModel(_scan_queue_table_model);


    layout->addWidget(new QLabel("Running Scan"));
    layout->addWidget(_scan_running_table_view);

    layout->addWidget(new QLabel("Scan Queue"));
    layout->addWidget(_scan_queue_table_view);
    

    _te_qs_console = new QTextEdit(this);
    _te_qs_console->scrollBarWidgets(Qt::AlignRight);
 
    layout->addWidget(new QLabel("QServer Log"));
    layout->addWidget(_te_qs_console);
    setLayout(layout);

}

//---------------------------------------------------------------------------

void ScanQueueWidget::updateQueuedItems( std::vector<BlueskyPlan> &queued_plans, BlueskyPlan &running_plan)
{
    _scan_queue_table_model->setAllData(queued_plans);
    std::vector<BlueskyPlan> runlist;
    if(running_plan.uuid.length() > 0)
    {
        runlist.push_back(running_plan);
    }
    _scan_running_table_model->setAllData(runlist);
}

//---------------------------------------------------------------------------

void ScanQueueWidget::newDataArrived(const QString& data)
{
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