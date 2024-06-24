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
    
    _te_qs_console = new QTextEdit(this);
    _te_qs_console->scrollBarWidgets(Qt::AlignRight);
 
    layout->addWidget(_te_qs_console);
    setLayout(layout);
/*
    QHBoxLayout* hlayout = new QHBoxLayout();
    _btn_update = new QPushButton("Update");
    connect(_btn_update, SIGNAL(released()), this, SLOT(updateIp()));

    hlayout->addWidget(new QLabel("Computer:"));
    hlayout->addWidget(_qline_ip_addr);
    hlayout->addWidget(new QLabel("Port:"));
    hlayout->addWidget(_qline_port);
    hlayout->addWidget(_btn_update);
    layout->addLayout(hlayout);

    _mapsElementsWidget = new MapsElementsWidget(1,1,true,this);
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

    _tab_widget = new QTabWidget();

    _tab_widget->addTab(_mapsElementsWidget, "Counts");
    _tab_widget->addTab(_vlm_widget, "Scan Area");
    //_tab_widget->addTab(_scan_queue, "Queue");


    layout->addWidget(_tab_widget);

    _progressBar = new QProgressBar(this);
    layout->addWidget(_progressBar);
    setLayout(layout);
*/
}

//---------------------------------------------------------------------------

void ScanQueueWidget::newDataArrived(const QString& data)
{
}

//---------------------------------------------------------------------------