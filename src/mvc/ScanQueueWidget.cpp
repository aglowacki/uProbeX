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
#include <QMessageBox>
#include <QMenu>
#include <QTableWidget>
#include "core/defines.h"

//---------------------------------------------------------------------------

ScanQueueWidget::ScanQueueWidget(QWidget *parent) : QWidget(parent)
{

    _avail_scans = nullptr;
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

    _scan_queue_table_model = new ScanQueueTableModel();
    connect(_scan_queue_table_model, &ScanQueueTableModel::moveScanRow, this, &ScanQueueWidget::onMoveScanRow);
    connect(_scan_queue_table_model, &ScanQueueTableModel::planChanged, this, &ScanQueueWidget::onPlanChanged);
    _scan_queue_table_view = new QTableView();
    _scan_queue_table_view->setModel(_scan_queue_table_model);
    _scan_queue_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    _scan_queue_table_view->setSelectionMode(QAbstractItemView::SingleSelection);
    _scan_queue_table_view->setDragEnabled(true);
    _scan_queue_table_view->setAcceptDrops(true);
    _scan_queue_table_view->setDragDropMode(QAbstractItemView::InternalMove);
    _scan_queue_table_view->setDefaultDropAction(Qt::MoveAction);
    _scan_queue_table_view->setDragDropOverwriteMode(false);
    _scan_queue_table_view->setDropIndicatorShown(true);
    _scan_queue_table_view->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    _scan_queue_table_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_scan_queue_table_view,
          &QTableView::customContextMenuRequested,
          this,
          &ScanQueueWidget::scanContextMenu);
    //_scan_queue_table_view->selectionModel()
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

    _btn_add_scan = new QPushButton("Add Scan");
    connect(_btn_add_scan, &QPushButton::pressed, &_scan_dialog, &ScanRegionDialog::show);

    _btn_add_batch_scan = new QPushButton("Add Batch Scan");
    _btn_add_batch_scan->setEnabled(false);
    connect(_btn_add_batch_scan, &QPushButton::pressed, &_scan_dialog, &ScanRegionDialog::show);

    _btn_set_history = new QPushButton("Set History Location");
    connect(_btn_set_history, &QPushButton::pressed, this, &ScanQueueWidget::onSetHistory);

    _btn_clear_history = new QPushButton("Clear History");
    connect(_btn_clear_history, &QPushButton::pressed, this, &ScanQueueWidget::onClearHistory);

    _scan_dialog.setRegionNameVisible(false);
    connect(&_scan_dialog, &ScanRegionDialog::ScanUpdated, this, &ScanQueueWidget::onAddScan);

    _move_scan_up = new QAction("Move Scan Up", this);
    connect(_move_scan_up,
            &QAction::triggered,
            this,
            &ScanQueueWidget::on_move_scan_up);

    _move_scan_down = new QAction("Move Scan Down", this);
    connect(_move_scan_down,
            &QAction::triggered,
            this,
            &ScanQueueWidget::on_move_scan_down);

    _remove_scan = new QAction("Remove Scan", this);
    connect(_remove_scan,
            &QAction::triggered,
            this,
            &ScanQueueWidget::on_remove_scan);
   
    

    QGridLayout *grid = new QGridLayout();
    grid->addWidget(_btn_play, 0,0);
    grid->addWidget(_btn_stop,0,1);
    grid->addWidget(_btn_refresh,0,2);
    grid->addWidget(_btn_open_env,0,3);
    grid->addWidget(_btn_close_env,0,4);
    grid->addWidget(_btn_add_scan,0,6);
    grid->addWidget(_btn_add_batch_scan,0,7);
    grid->addWidget(_btn_set_history,0,8);
    grid->addWidget(_btn_clear_history,0,9);
    grid->addItem(new QSpacerItem(999,10), 0,10);

    layout->addItem(grid);

    _te_qs_console = new QTextEdit(this);
    _te_qs_console->scrollBarWidgets(Qt::AlignRight);
    _te_qs_console->setReadOnly(true);

    QTabWidget* tabWidget = new QTabWidget();

    QDockWidget *dock_queue = new QDockWidget("Scan Queue", this);
    dock_queue->setWidget(_scan_queue_table_view);
    tabWidget->addTab(dock_queue, "Scan Queue");

    QDockWidget *dock_log = new QDockWidget("QServer Log", this);
    dock_log->setWidget(_te_qs_console);
    tabWidget->addTab(dock_log, "Scan Log");

    layout->addWidget(tabWidget);

    setLayout(layout);

}

//---------------------------------------------------------------------------

void ScanQueueWidget::scanContextMenu(const QPoint& pos)
{
    QMenu menu(_scan_queue_table_view);
    if (_scan_queue_table_view->selectionModel()->hasSelection())
    {   
        menu.addAction(_move_scan_up);
        menu.addAction(_move_scan_down);
        menu.addAction(_remove_scan);
    
        QAction* result = menu.exec(_scan_queue_table_view->viewport()->mapToGlobal(pos));
        if (result == nullptr)
        {
            //m_selectionModel->clearSelection();
        }
        
    }
}

//---------------------------------------------------------------------------

void ScanQueueWidget::on_move_scan_up()
{
    if (_scan_queue_table_view->selectionModel()->hasSelection())
    {  
        QModelIndexList selectedIndexes = _scan_queue_table_view->selectionModel()->selectedRows();
        for (int i = selectedIndexes.count() - 1; i >= 0; i--)
        {
            QModelIndex index = selectedIndexes[i];
            emit onMoveScanUp(index.row() - _scan_queue_table_model->get_finished_idx());
        }
    }
}

//---------------------------------------------------------------------------

void ScanQueueWidget::on_move_scan_down()
{
    if (_scan_queue_table_view->selectionModel()->hasSelection())
    {  
        QModelIndexList selectedIndexes = _scan_queue_table_view->selectionModel()->selectedRows();
        for (int i = selectedIndexes.count() - 1; i >= 0; i--)
        {
            QModelIndex index = selectedIndexes[i];
            emit onMoveScanDown(index.row() - _scan_queue_table_model->get_finished_idx());
        }
    }
}

//---------------------------------------------------------------------------

void ScanQueueWidget::on_remove_scan()
{
    if (_scan_queue_table_view->selectionModel()->hasSelection())
    {  
        QModelIndexList selectedIndexes = _scan_queue_table_view->selectionModel()->selectedRows();

        //display message box
        QMessageBox msgBox;
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setText("Are you sure you want to delete the selected items?");
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes)
        {
            for (int i = selectedIndexes.count() - 1; i >= 0; i--)
            {
                QModelIndex index = selectedIndexes[i];
                emit onRemoveScan(index.row() - _scan_queue_table_model->get_finished_idx());
            }
        }
        
    }
}

//---------------------------------------------------------------------------

void ScanQueueWidget::updateQueuedItems(std::vector<BlueskyPlan> &finished_plans, std::vector<BlueskyPlan> &queued_plans, BlueskyPlan &running_plan)
{
    _scan_queue_table_model->setAllData(finished_plans, queued_plans, running_plan);   
    _scan_queue_table_view->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    if( running_plan.uuid.length() > 0)
    {
        _btn_close_env->setEnabled(false);
    }
    else
    {
        _btn_close_env->setEnabled(true);
    }
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
    || data.count("Queue is ") > 0
    //|| data.count("Queue is empty") > 0
    //|| data.count("Queue is stopped") > 0
    )
    
    {
        emit queueNeedsToBeUpdated();
    }
    // if ERROR:bluesky:Run aborted
}

//---------------------------------------------------------------------------
