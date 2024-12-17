/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/LiveMapsElementsWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include "core/defines.h"

//---------------------------------------------------------------------------

LiveMapsElementsWidget::LiveMapsElementsWidget(QString ip, QString port, QWidget* parent) : QWidget(parent)
{

    _streamWorker = nullptr;
    _qserverComm = nullptr;
    _mapsElementsWidget = nullptr;
    _last_packet = nullptr;
    _qserverComm = nullptr;
    //_currentModel = new MapsH5Model();
    _currentModel = nullptr;
    _num_images = 0;
    _prev_dataset_name = " ";
    _context = new zmq::context_t(1);
    _qserver_ip_addr = new QLineEdit("127.0.0.1");
    _qline_ip_addr = new QLineEdit();
    if(ip.length() > 0)
    {
        _qline_ip_addr->setText(ip);
    }
    _qline_port = new QLineEdit();
    if(port.length() > 0)
    {
        _qline_port->setText(port);
    }
    else
    {
        _qline_port->setText("43434");
    }
    createLayout();

}

//---------------------------------------------------------------------------

LiveMapsElementsWidget::~LiveMapsElementsWidget()
{

    if(_currentModel != nullptr)
	{
 	   disconnect(_currentModel,
	            SIGNAL(model_data_updated()),
	            _mapsElementsWidget,
	            SLOT(model_updated()));
	}

    if(_currentModel != nullptr)
    {
        delete _currentModel;
    }
    _currentModel = nullptr;

    if(_qserverComm != nullptr)
    {
        delete _qserverComm;
    }

    if(_streamWorker != nullptr)
    {
        _streamWorker->stop();
        _streamWorker->quit();
        _streamWorker->wait();
        delete _streamWorker;
    }
    _streamWorker = nullptr;

    if(_mapsElementsWidget != nullptr)
    {
        delete _mapsElementsWidget;
        _mapsElementsWidget = nullptr;
    }

    if (_context != nullptr)
    {
        _context->close();
        delete _context;
    }
    _context = nullptr;

}

//---------------------------------------------------------------------------

void LiveMapsElementsWidget::createLayout()
{

    QVBoxLayout* layout = new QVBoxLayout();
    QHBoxLayout* hlayout = new QHBoxLayout();
    QHBoxLayout* hlayout2 = new QHBoxLayout();
    _btn_update = new QPushButton("Update");
    connect(_btn_update, SIGNAL(released()), this, SLOT(updateIp()));

    hlayout->addWidget(new QLabel("QServer Computer:"));
    hlayout->addWidget(_qserver_ip_addr);
    hlayout2->addWidget(new QLabel("XRF-Maps Computer:"));
    hlayout2->addWidget(_qline_ip_addr);
    hlayout2->addWidget(new QLabel("Port:"));
    hlayout2->addWidget(_qline_port);
    hlayout2->addWidget(_btn_update);
    layout->addLayout(hlayout);
    layout->addLayout(hlayout2);

 //   _textEdit = new QTextEdit(this);
 //   _textEdit->resize(1024, 800);
 //   _textEdit->scrollBarWidgets(Qt::AlignRight);
    _mapsElementsWidget = new MapsElementsWidget(1,1,true,false);
    //_mapsElementsWidget->setTabVisible(1, false);
    _mapsElementsWidget->setTabVisible(2, false);
    _mapsElementsWidget->setTabVisible(3, false);
    _mapsElementsWidget->setTabVisible(4, false);
    _mapsElementsWidget->setTabVisible(5, false);
    //_mapsElementsWidget->setModel(_currentModel, nullptr, nullptr);
 //   _mapsElementsWidget->appendTab(_textEdit, "Log");

    connect(_mapsElementsWidget,
            SIGNAL(rangeChanged(int, int)),
            this,
            SLOT(image_changed(int, int)));

	if(_currentModel != nullptr)
	{
	    connect(_currentModel,
	            SIGNAL(model_data_updated()),
	            _mapsElementsWidget,
	            SLOT(model_updated()));
	}


    _vlm_widget = new VLM_Widget();
    _vlm_widget->setAvailScans(&_avail_scans);
    gstar::CoordinateModel *coord_model = new gstar::CoordinateModel(&_linear_trans);
    _vlm_widget->setCoordinateModel(coord_model);
    _vlm_widget->load_live_coord_settings();
    connect(_vlm_widget, &VLM_Widget::onScanUpdated, this, &LiveMapsElementsWidget::callQueueScan);

    _scan_queue_widget = new ScanQueueWidget();
    _scan_queue_widget->setAvailScans(&_avail_scans);
    connect(_scan_queue_widget, &ScanQueueWidget::queueNeedsToBeUpdated, this, &LiveMapsElementsWidget::getQueuedScans);
    connect(_scan_queue_widget, &ScanQueueWidget::onOpenEnv, this, &LiveMapsElementsWidget::callOpenEnv);
    connect(_scan_queue_widget, &ScanQueueWidget::onCloseEnv, this, &LiveMapsElementsWidget::callCloseEnv);
    connect(_scan_queue_widget, &ScanQueueWidget::onStartQueue, this, &LiveMapsElementsWidget::callStartQueue);
    connect(_scan_queue_widget, &ScanQueueWidget::onStopQueue, this, &LiveMapsElementsWidget::callStopQueue);
    connect(_scan_queue_widget, &ScanQueueWidget::onMoveScanRow, this, &LiveMapsElementsWidget::callMoveScanRow);
    connect(_scan_queue_widget, &ScanQueueWidget::onMoveScanUp, this, &LiveMapsElementsWidget::callMoveScanUp);
    connect(_scan_queue_widget, &ScanQueueWidget::onMoveScanDown, this, &LiveMapsElementsWidget::callMoveScanDown);
    connect(_scan_queue_widget, &ScanQueueWidget::onRemoveScan, this, &LiveMapsElementsWidget::callRemoveScan);
    connect(_scan_queue_widget, &ScanQueueWidget::onPlanChanged, this, &LiveMapsElementsWidget::callUpdatePlan);
    connect(_scan_queue_widget, &ScanQueueWidget::onAddScan, this, &LiveMapsElementsWidget::callQueueScan);
    connect(_scan_queue_widget, &ScanQueueWidget::onSetHistory, this, &LiveMapsElementsWidget::setHistoryLocation);
    connect(_scan_queue_widget, &ScanQueueWidget::onClearHistory, this, &LiveMapsElementsWidget::clearHistory);

    _tab_widget = new QTabWidget();
    _tab_widget->addTab(_mapsElementsWidget, "Counts");
    _tab_widget->addTab(_vlm_widget, "Scan Area");
    _tab_widget->addTab(_scan_queue_widget, "Queue");


    layout->addWidget(_tab_widget);

    _progressBar = new QProgressBar(this);
    layout->addWidget(_progressBar);
    setLayout(layout);

}

//---------------------------------------------------------------------------

void LiveMapsElementsWidget::updateIp()
{
    
    if(_qserverComm != nullptr)
    {
        delete _qserverComm;
    }
    _qserverComm = new BlueskyComm(_context, _qserver_ip_addr->text());

    if(_streamWorker != nullptr)
    {
        disconnect(_streamWorker, &NetStreamWorker::newData, this, &LiveMapsElementsWidget::newDataArrived);
        _streamWorker->stop();
        _streamWorker->quit();
        _streamWorker->wait();
        delete _streamWorker;
    }
    _streamWorker = new NetStreamWorker(_context, _qline_ip_addr->text(), _qline_port->text(), _qserver_ip_addr->text(), this);
    connect(_streamWorker, &NetStreamWorker::newData, this, &LiveMapsElementsWidget::newDataArrived);
    connect(_streamWorker, &NetStreamWorker::newStringData, _scan_queue_widget, &ScanQueueWidget::newDataArrived);
    _streamWorker->start();
    if(_last_packet != nullptr)
        delete _last_packet;
    _last_packet = nullptr;

    updateScansAvailable();
    getQueuedScans();
    callOpenEnv();
}

//---------------------------------------------------------------------------

void LiveMapsElementsWidget::newDataArrived(data_struct::Stream_Block<float>* new_packet)
{
	if (new_packet == nullptr)
	{
		return;
	}
    if (new_packet->is_end_block())
    {
        return;
    }


    bool start_new_image = false;
    if( (new_packet->row() == 0 && new_packet->col() == 0) || _last_packet == nullptr)
    {
        start_new_image = true;
    }
    if(_last_packet != nullptr)
    {
        if( (_last_packet->width() != new_packet->width() || _last_packet->height() != new_packet->height()) )
        {
            start_new_image = true;
        }
		size_t dlen = new_packet->dataset_name->length();
		if (new_packet->dataset_name->at(dlen - 3) != '.' && new_packet->dataset_name->at(dlen - 2) != 'n' && new_packet->dataset_name->at(dlen - 1) != 'c')
		{
			if(new_packet->dataset_name->compare((*_last_packet->dataset_name)) != 0)
			{
			    start_new_image = true;
			}
		}
    }
    if(_currentModel == nullptr)
    {
        start_new_image = true;
    }

    if (start_new_image)
    {
        _prev_dataset_name = *new_packet->dataset_name;
        if (_currentModel != nullptr)
        {
            disconnect(_currentModel,
                SIGNAL(model_data_updated()),
                _mapsElementsWidget,
                SLOT(model_updated()));
        }
        _currentModel = new MapsH5Model();
        _currentModel->initialize_from_stream_block(new_packet);
        _progressBar->setRange(0, new_packet->height() - 1);
        _maps_h5_models.push_back(_currentModel);

        int cur = _mapsElementsWidget->getRangeWidgetStartIndex();
        _num_images++;
        _mapsElementsWidget->setNumberOfImages(_num_images);
        
        if (cur == _num_images - 1)
        {
            _mapsElementsWidget->setModel(_currentModel);
            connect(_currentModel,
                SIGNAL(model_data_updated()),
                _mapsElementsWidget,
                SLOT(model_updated()));
            _mapsElementsWidget->setRangeWidgetStartIndex(_num_images);
        }
    }

    _currentModel->update_from_stream_block(new_packet);
    if(_last_packet != nullptr && _last_packet->row() != new_packet->row())
    {   
        QString str = ">" + QString::number(new_packet->row()) + " " + QString::number(new_packet->col()) + " : " + QString::number(new_packet->height()) + " " + QString::number(new_packet->width()) ;
//        _textEdit->append(str);

        _mapsElementsWidget->redrawCounts();

        _progressBar->setValue(new_packet->row());
        //_textEdit->clear();
        _progressBar->update();
        //cntr = 0;
    }
    if(_last_packet != nullptr)
    {
        delete _last_packet;
    }
    _last_packet = new_packet;
}

//---------------------------------------------------------------------------

void LiveMapsElementsWidget::image_changed(int start, int end)
{
    _mapsElementsWidget->setModel(_maps_h5_models[start-1]);
}

//---------------------------------------------------------------------------

void LiveMapsElementsWidget::updateScansAvailable()
{
    QString msg;
    if(_qserverComm == nullptr)
    {
        updateIp();
    }
    if (false == _qserverComm->get_avail_scans(_avail_scans, msg))
    {
        _scan_queue_widget->newDataArrived( msg );
    }
    else
    {
        _scan_queue_widget->setAvailScans(&_avail_scans);
        _vlm_widget->setAvailScans(&_avail_scans);
    }
}

//---------------------------------------------------------------------------

void LiveMapsElementsWidget::getQueuedScans()
{
    QString msg;
    if(_qserverComm == nullptr)
    {
        updateIp();
    }
    if (false == _qserverComm->get_scan_history(msg, _finished_scans))
    {
        _scan_queue_widget->newDataArrived( msg );
    }
    else
    {
        saveHistory();
    }
    if (false == _qserverComm->get_queued_scans(msg, _queued_scans, _running_scan))
    {
        _scan_queue_widget->newDataArrived( msg );
    }
    _scan_queue_widget->updateQueuedItems(_finished_scans, _queued_scans, _running_scan);
}

//---------------------------------------------------------------------------

void LiveMapsElementsWidget::setHistoryLocation()
{

    QDateTime date = QDateTime::currentDateTime();
    QString formattedTime = date.toString("yyyy_MM_dd_hh_mm_ss");
    QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();
    QString apath = "Scan_History_" + formattedTime + ".json";

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Scan History", apath,
                                                    tr("JSON (*.json);;CSV (*.csv)"));

    if(fileName.length() > 0)
    {
        Preferences::inst()->setValue(STR_SAVE_QSERVER_HISTORY_LOCATION, fileName);
        saveHistory();
    }
}

//---------------------------------------------------------------------------

void LiveMapsElementsWidget::saveHistory()
{
    QString fileName = Preferences::inst()->getValue(STR_SAVE_QSERVER_HISTORY_LOCATION).toString();
    
    if(fileName.length() > 0)
    {
        QFile file(fileName);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            //QMessageBox::warning(nullptr, "Export History", "Could not open the file.");
            logW<<"Could not open QServer history filepath "<<fileName.toStdString()<<"\n";
        }
        QString msg;

        if (fileName.endsWith(".json")) 
        {
             if (false == _qserverComm->get_scan_history(msg, _finished_scans, true))
            {
                //QMessageBox::warning(nullptr, "Export History", "Failed to get scan history from QServer");
                _scan_queue_widget->newDataArrived( msg );
            }
            file.write(msg.toUtf8());
        }
        else if (fileName.endsWith(".csv")) 
        {
            if (false == _qserverComm->get_scan_history(msg, _finished_scans, false))
            {
                //QMessageBox::warning(nullptr, "Export History", "Failed to get scan history from QServer");
                _scan_queue_widget->newDataArrived( msg );
            }
            if(_finished_scans.size() > 0)
            {
                QTextStream out(&file);

                // Write header row
                out << "Name,Type,";
                for(auto & itr: _finished_scans.at(0).parameters)
                {
                    out<<itr.name<<",";
                }
                out << "exit_status,time_start,time_stop,msg\r\n";
                for(auto &itr: _finished_scans)
                {
                    out<<itr.name<<","<<itr.type<<",";
                    for(auto & itr2: itr.parameters)
                    {
                        out<<itr2.default_val<<",";
                    }
                    QDateTime dateTime1 = QDateTime::fromSecsSinceEpoch(itr.result.time_start, Qt::UTC);
                    QDateTime dateTime2 = QDateTime::fromSecsSinceEpoch(itr.result.time_stop, Qt::UTC);
                    out<<itr.result.exit_status<<","<<dateTime1.toString("yyyy-MM-dd hh:mm:ss")<<","<<dateTime2.toString("yyyy-MM-dd hh:mm:ss")<<","<<itr.result.msg<<"\r\n";
                }
            }
        }
        file.close();
    }
}

//---------------------------------------------------------------------------

void LiveMapsElementsWidget::clearHistory()
{

    QString fileName = Preferences::inst()->getValue(STR_SAVE_QSERVER_HISTORY_LOCATION).toString();
    
    // Set history name to new date so we don't lose old history
    if(fileName.length() > 0)
    {
        QFileInfo finfo = QFileInfo(fileName);
        QDir dir = finfo.absoluteDir();
        
        QDateTime date = QDateTime::currentDateTime();
        QString formattedTime = date.toString("yyyy_MM_dd_hh_mm_ss");
        QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();
        QString apath = dir.absolutePath() + QDir::separator() + finfo.baseName() + "_" + formattedTime + finfo.suffix();
        
        Preferences::inst()->setValue(STR_SAVE_QSERVER_HISTORY_LOCATION, apath);
    }

    QString msg;
    if (false == _qserverComm->clear_history(msg))
    {
        QMessageBox::warning(nullptr, "Export History", "Failed to clear scan history from QServer");
        _scan_queue_widget->newDataArrived( msg );
    }

    getQueuedScans();
}

//---------------------------------------------------------------------------

void LiveMapsElementsWidget::callOpenEnv()
{
    QString msg;
    if(_qserverComm == nullptr)
    {
        updateIp();
    }
    if (false == _qserverComm->open_env(msg))
    {
        //_scan_queue_widget->newDataArrived( msg );
    }
    else
    {
        //_scan_queue_widget->updateQueuedItems(_queued_scans, _running_scan);
    }

    _scan_queue_widget->newDataArrived( msg );
}

//---------------------------------------------------------------------------

void LiveMapsElementsWidget::callCloseEnv()
{
    QMessageBox msgBox;
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText("Are you sure you want to close the environment? This will kill the QServer run process. Only do this if you need to restart.");

    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes)
    {
        QString msg;
        if(_qserverComm == nullptr)
        {
            updateIp();
        }
        if (false == _qserverComm->close_env(msg))
        {
            //_scan_queue_widget->newDataArrived( msg );
        }
        else
        {
            //_scan_queue_widget->updateQueuedItems(_queued_scans, _running_scan);
        }

        _scan_queue_widget->newDataArrived( msg );
    }
}

//---------------------------------------------------------------------------

void LiveMapsElementsWidget::callStartQueue()
{
    QString msg;
    if(_qserverComm == nullptr)
    {
        updateIp();
    }
    if (false == _qserverComm->start_queue(msg))
    {
        //_scan_queue_widget->newDataArrived( msg );
    }
    else
    {
        //_scan_queue_widget->updateQueuedItems(_queued_scans, _running_scan);
    }

    _scan_queue_widget->newDataArrived( msg );
}

//---------------------------------------------------------------------------

void LiveMapsElementsWidget::callStopQueue()
{
    QString msg;
    if(_qserverComm == nullptr)
    {
        updateIp();
    }
    if (false == _qserverComm->stop_queue(msg))
    {
        //_scan_queue_widget->newDataArrived( msg );
    }
    else
    {
        //_scan_queue_widget->updateQueuedItems(_queued_scans, _running_scan);
    }

    _scan_queue_widget->newDataArrived( msg );
}

//---------------------------------------------------------------------------

void LiveMapsElementsWidget::callQueueScan(const BlueskyPlan& plan)
{
    QString msg;
    if(_qserverComm == nullptr)
    {
        updateIp();
    }
    if (false == _qserverComm->queue_plan(msg, plan))
    {
        
    }
    else
    {

    }

    _scan_queue_widget->newDataArrived( msg );
    getQueuedScans();
}

//---------------------------------------------------------------------------


void LiveMapsElementsWidget::callMoveScanRow(int srcRow, int destRow)
{
    QString msg;
    if(_qserverComm == nullptr)
    {
        updateIp();
    }
    if (false == _qserverComm->movePlan(msg, srcRow, destRow))
    {
        
    }
    else
    {

    }

    _scan_queue_widget->newDataArrived( msg );
    getQueuedScans();
}

//---------------------------------------------------------------------------

void LiveMapsElementsWidget::callMoveScanUp(int row)
{
    if(row > 0)
    {
        callMoveScanRow(row, row-1);
    }
}

//---------------------------------------------------------------------------

void LiveMapsElementsWidget::callMoveScanDown(int row)
{
    
    callMoveScanRow(row, row+1);

}

//---------------------------------------------------------------------------

void LiveMapsElementsWidget::callRemoveScan(int row)
{
    QString msg;
    if(_qserverComm == nullptr)
    {
        updateIp();
    }
    if (false == _qserverComm->removePlan(msg, row))
    {
        
    }
    else
    {

    }

    _scan_queue_widget->newDataArrived( msg );
    getQueuedScans();
}

//---------------------------------------------------------------------------

void LiveMapsElementsWidget::callUpdatePlan(const BlueskyPlan& plan)
{
    QString msg;
    if(_qserverComm == nullptr)
    {
        updateIp();
    }
    if (false == _qserverComm->update_plan(msg, plan))
    {
        
    }
    else
    {

    }

    _scan_queue_widget->newDataArrived( msg );
    getQueuedScans();
}

//---------------------------------------------------------------------------
