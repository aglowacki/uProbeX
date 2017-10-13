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

    createLayout();

}

/*---------------------------------------------------------------------------*/

LiveMapsElementsWidget::~LiveMapsElementsWidget()
{

    _thread.quit();
    _thread.wait();

}

/*---------------------------------------------------------------------------*/

void LiveMapsElementsWidget::createLayout()
{

    //QLayout* layout = generateDefaultLayout();
    //layout->addWidget(_spectra_widget);
    //setLayout(layout);

    _streamWorker = new NetStreamWorker(this);
    _streamWorker->moveToThread(&_thread);
    connect(&_thread, &QThread::finished, _streamWorker, &QObject::deleteLater);
    connect(this, &LiveMapsElementsWidget::start_listening, _streamWorker, &NetStreamWorker::doWork);
    connect(_streamWorker, &NetStreamWorker::newData, this, &LiveMapsElementsWidget::newDataArrived);
    _thread.start();

}

/*---------------------------------------------------------------------------*/

void LiveMapsElementsWidget::newDataArrived(data_struct::xrf::Stream_Block *new_packet)
{

}

/*---------------------------------------------------------------------------*/
