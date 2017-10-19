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
    createLayout();

}

/*---------------------------------------------------------------------------*/

LiveMapsElementsWidget::~LiveMapsElementsWidget()
{

    if(_streamWorker != nullptr)
    {
        _streamWorker->stop();
        _streamWorker->quit();
        _streamWorker->wait();
        delete _streamWorker;
    }
    _streamWorker = nullptr;

}

/*---------------------------------------------------------------------------*/

void LiveMapsElementsWidget::createLayout()
{

    QVBoxLayout* layout = new QVBoxLayout();
    _textEdit = new QTextEdit(this);
    _textEdit->resize(1024, 800);
    _textEdit->scrollBarWidgets(Qt::AlignRight);
    layout->addWidget(_textEdit);
    setLayout(layout);

    _streamWorker = new NetStreamWorker(this);
    connect(_streamWorker, &QThread::finished, _streamWorker, &QObject::deleteLater);
    connect(_streamWorker, &NetStreamWorker::newData, this, &LiveMapsElementsWidget::newDataArrived);
    _streamWorker->start();

}

/*---------------------------------------------------------------------------*/

void LiveMapsElementsWidget::newDataArrived(data_struct::xrf::Stream_Block *new_packet)
{
    QString str = ">" + QString::number(new_packet->row()) + " " + QString::number(new_packet->col()) + "\n\r";

    _textEdit->append(str);
}

/*---------------------------------------------------------------------------*/
