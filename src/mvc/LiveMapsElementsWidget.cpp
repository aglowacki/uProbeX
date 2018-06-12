/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/LiveMapsElementsWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QDebug>

/*---------------------------------------------------------------------------*/

LiveMapsElementsWidget::LiveMapsElementsWidget(QWidget* parent) : QWidget(parent)
{

    _streamWorker = nullptr;
    _mapsElementsWidget = nullptr;
    _last_row = -1;
    createLayout();

}

/*---------------------------------------------------------------------------*/

LiveMapsElementsWidget::~LiveMapsElementsWidget()
{


    disconnect(&_currentModel,
            SIGNAL(model_data_updated()),
            _mapsElementsWidget,
            SLOT(model_updated()));

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

}

/*---------------------------------------------------------------------------*/

void LiveMapsElementsWidget::createLayout()
{

    QVBoxLayout* layout = new QVBoxLayout();
    QHBoxLayout* hlayout = new QHBoxLayout();
    _qline_ip_addr = new QLineEdit("127.0.0.1");
    _btn_update = new QPushButton("Update");
    connect(_btn_update, SIGNAL(released()), this, SLOT(updateIp()));

    hlayout->addWidget(_qline_ip_addr);
    hlayout->addWidget(_btn_update);
    layout->addLayout(hlayout);

    _textEdit = new QTextEdit(this);
    _textEdit->resize(1024, 800);
    _textEdit->scrollBarWidgets(Qt::AlignRight);
    _mapsElementsWidget = new MapsElementsWidget(this);
    _mapsElementsWidget->setModel(&_currentModel, nullptr, nullptr);
    _mapsElementsWidget->appendTab(_textEdit, "Log");

    connect(&_currentModel,
            SIGNAL(model_data_updated()),
            _mapsElementsWidget,
            SLOT(model_updated()));

    layout->addWidget(_mapsElementsWidget);

    _progressBar = new QProgressBar(this);
    layout->addWidget(_progressBar);
    setLayout(layout);

}

/*---------------------------------------------------------------------------*/

void LiveMapsElementsWidget::updateIp()
{

    if(_streamWorker != nullptr)
    {
        //disconnect(_streamWorker, &QThread::finished, _streamWorker, &QObject::deleteLater);
        disconnect(_streamWorker, &NetStreamWorker::newData, this, &LiveMapsElementsWidget::newDataArrived);
        _streamWorker->stop();
        _streamWorker->quit();
        _streamWorker->wait();
        delete _streamWorker;
    }
    _streamWorker = new NetStreamWorker(_qline_ip_addr->text(), this);
    //connect(_streamWorker, &QThread::finished, _streamWorker, &QObject::deleteLater);
    connect(_streamWorker, &NetStreamWorker::newData, this, &LiveMapsElementsWidget::newDataArrived);
    _streamWorker->start();
    _last_row = -1;
}

/*---------------------------------------------------------------------------*/

void LiveMapsElementsWidget::newDataArrived(data_struct::Stream_Block *new_packet)
{

    if( (new_packet->row() == 0 && new_packet->col() == 0) || _last_row == -1)
    {
        _currentModel.initialize_from_stream_block(new_packet);
        _progressBar->setRange(0, new_packet->height()-1);
    }

    _currentModel.update_from_stream_block(new_packet);
    if(_last_row != new_packet->row())
    {   
        QString str = ">" + QString::number(new_packet->row()) + " " + QString::number(new_packet->col()) + " : " + QString::number(new_packet->height()) + " " + QString::number(new_packet->width()) ;
        _textEdit->append(str);

        _mapsElementsWidget->redrawCounts();

        _progressBar->setValue(new_packet->row());
        //_textEdit->clear();
        _progressBar->update();
        //cntr = 0;
    }
    _last_row = new_packet->row();
    delete new_packet;

}

/*---------------------------------------------------------------------------*/
