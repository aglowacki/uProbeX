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
#include <QShortcut>
#include "core/defines.h"
#include "preferences/Preferences.h"

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
    connect(_scan_queue_table_model, &ScanQueueTableModel::scanFileNameChanged, this, &ScanQueueWidget::planFilenameChanged);
    _scan_queue_table_view = new QTableView();
    _scan_queue_table_view->setModel(_scan_queue_table_model);
    _scan_queue_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    _scan_queue_table_view->setSelectionMode(QAbstractItemView::ContiguousSelection);
    _scan_queue_table_view->setDragEnabled(true);
    _scan_queue_table_view->setAcceptDrops(true);
    _scan_queue_table_view->setDragDropMode(QAbstractItemView::InternalMove);
    _scan_queue_table_view->setDefaultDropAction(Qt::MoveAction);
    _scan_queue_table_view->setDragDropOverwriteMode(false);
    _scan_queue_table_view->setDropIndicatorShown(true);
    _scan_queue_table_view->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    _scan_queue_table_view->setContextMenuPolicy(Qt::CustomContextMenu);

    if(false == Preferences::inst()->getValue(STR_PFR_SHOW_SCAN_QUEUE_HEADER).toBool())
    {
        _scan_queue_table_view->horizontalHeader()->hide();
    }

    connect(_scan_queue_table_view,
          &QTableView::customContextMenuRequested,
          this,
          &ScanQueueWidget::scanContextMenu);
    
    QShortcut* del_shortcut = new QShortcut(QKeySequence(QKeySequence::Delete), _scan_queue_table_view);
    connect(del_shortcut, &QShortcut::activated, this, &ScanQueueWidget::on_remove_scan);

    QShortcut* backsp_shortcut = new QShortcut(QKeySequence(QKeySequence::Backspace), _scan_queue_table_view);
    connect(backsp_shortcut, &QShortcut::activated, this, &ScanQueueWidget::on_remove_scan);
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

    _btn_set_history = new QPushButton("Set History Location");
    connect(_btn_set_history, &QPushButton::pressed, this, &ScanQueueWidget::onSetHistory);

    _btn_clear_history = new QPushButton("Clear History");
    connect(_btn_clear_history, &QPushButton::pressed, this, &ScanQueueWidget::on_clear_history);

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
        auto selected = _scan_queue_table_view->selectionModel()->selectedIndexes();
        if(selected.count() == 1)
        {
            menu.addAction(_move_scan_up);
            menu.addAction(_move_scan_down);
        }
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
        if(selectedIndexes.count() == 1)
        {
            for (int i = selectedIndexes.count() - 1; i >= 0; i--)
            {
                QModelIndex index = selectedIndexes[i];
                emit onMoveScanUp(index.row() - _scan_queue_table_model->get_finished_idx());
            }
        }
    }
}

//---------------------------------------------------------------------------

void ScanQueueWidget::on_move_scan_down()
{
    if (_scan_queue_table_view->selectionModel()->hasSelection())
    {  
        QModelIndexList selectedIndexes = _scan_queue_table_view->selectionModel()->selectedRows();
        if(selectedIndexes.count() == 1)
        {
            for (int i = selectedIndexes.count() - 1; i >= 0; i--)
            {
                QModelIndex index = selectedIndexes[i];
                emit onMoveScanDown(index.row() - _scan_queue_table_model->get_finished_idx());
            }
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

void ScanQueueWidget::setAvailScans(std::map<QString, BlueskyPlan> * avail_scans)
{
     _avail_scans = avail_scans; 
     _scan_dialog.setAvailScans(avail_scans);
     _scan_queue_table_model->setAvailScans(avail_scans);
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
    else if (data.count("Filename:") > 0 )
    {
        _updateRunningPlanMetaInfo(data);
    }
    // if ERROR:bluesky:Run aborted
}

//---------------------------------------------------------------------------

void ScanQueueWidget::_updateRunningPlanMetaInfo(const QString& msg)
{
    const QString fname_tag = "Filename:";
    const QString scan_prog_tag = "Scan_progress:";
    qsizetype idx = msg.indexOf(fname_tag);
    if(idx > 0)
    {
        QString subMsg = msg.mid(idx);
        QStringList slist = subMsg.split(",");
        QString Filename = "";
        for(QString s : slist)
        {
            QString trim_s = s.trimmed();
            qsizetype filenameIdx = trim_s.indexOf(fname_tag);
            qsizetype scanProgIdx = trim_s.indexOf(scan_prog_tag);
            if(filenameIdx != -1)
            {
                QString filename = trim_s.mid(filenameIdx+fname_tag.size());
                _scan_queue_table_model->setRunningPlanMetaData(filename);
            }
            else if(scanProgIdx != -1)
            {
                qsizetype percIdx = trim_s.indexOf("%");
                QString strPerc = trim_s.mid(scanProgIdx+scan_prog_tag.size(), percIdx);
                int perc = strPerc.toInt();
                emit onScanProgress(perc);
            }
        }
    }
}

//---------------------------------------------------------------------------

void ScanQueueWidget::on_clear_history()
{

    _te_qs_console->clear();
    emit onClearHistory();

}

//---------------------------------------------------------------------------
