/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/LiveMapsElementsWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "core/defines.h"

/*---------------------------------------------------------------------------*/

LiveMapsElementsWidget::LiveMapsElementsWidget(QString ip, QString port, QWidget* parent) : QWidget(parent)
{

    _streamWorker = nullptr;
    _mapsElementsWidget = nullptr;
    _last_packet = nullptr;
    //_currentModel = new MapsH5Model();
    _currentModel = nullptr;
    _num_images = 0;
    _prev_dataset_name = " ";
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

/*---------------------------------------------------------------------------*/

LiveMapsElementsWidget::~LiveMapsElementsWidget()
{

    if(_currentModel != nullptr)
	{
 	   disconnect(_currentModel.get(),
	            SIGNAL(model_data_updated()),
	            _mapsElementsWidget,
	            SLOT(model_updated()));
	}

    if(_currentModel)
    {
        _currentModel.reset();
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

}

/*---------------------------------------------------------------------------*/

void LiveMapsElementsWidget::createLayout()
{

    QVBoxLayout* layout = new QVBoxLayout();
    QHBoxLayout* hlayout = new QHBoxLayout();
    _btn_update = new QPushButton("Update");
    connect(_btn_update, SIGNAL(released()), this, SLOT(updateIp()));

    hlayout->addWidget(new QLabel("Computer:"));
    hlayout->addWidget(_qline_ip_addr);
    hlayout->addWidget(new QLabel("Port:"));
    hlayout->addWidget(_qline_port);
    hlayout->addWidget(_btn_update);
    layout->addLayout(hlayout);

 //   _textEdit = new QTextEdit(this);
 //   _textEdit->resize(1024, 800);
 //   _textEdit->scrollBarWidgets(Qt::AlignRight);
    _mapsElementsWidget = new MapsElementsWidget(this);
    //_mapsElementsWidget->setModel(_currentModel, nullptr, nullptr);
 //   _mapsElementsWidget->appendTab(_textEdit, "Log");

    connect(_mapsElementsWidget,
            SIGNAL(rangeChanged(int, int)),
            this,
            SLOT(image_changed(int, int)));

	if(_currentModel != nullptr)
	{
	    connect(_currentModel.get(),
	            SIGNAL(model_data_updated()),
	            _mapsElementsWidget,
	            SLOT(model_updated()));
	}
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
        disconnect(_streamWorker, &NetStreamWorker::newData, this, &LiveMapsElementsWidget::newDataArrived);
        _streamWorker->stop();
        _streamWorker->quit();
        _streamWorker->wait();
        delete _streamWorker;
    }
    _streamWorker = new NetStreamWorker(_qline_ip_addr->text(), _qline_port->text(), this);
    connect(_streamWorker, &NetStreamWorker::newData, this, &LiveMapsElementsWidget::newDataArrived);
    _streamWorker->start();
    if(_last_packet != nullptr)
        delete _last_packet;
    _last_packet = nullptr;
}

/*---------------------------------------------------------------------------*/

void LiveMapsElementsWidget::newDataArrived(data_struct::Stream_Block *new_packet)
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
        if(new_packet->dataset_name().compare((_last_packet->dataset_name())) != 0)
        {
            start_new_image = true;
        }
    }
    if(_currentModel == nullptr)
    {
        start_new_image = true;
    }

    if (start_new_image)
    {
        _prev_dataset_name = new_packet->dataset_name();
        if (_currentModel)
        {
            disconnect(_currentModel.get(),
                SIGNAL(model_data_updated()),
                _mapsElementsWidget,
                SLOT(model_updated()));
        }
        _currentModel = std::make_shared<MapsH5Model>();
        _currentModel->initialize_from_stream_block(new_packet);
        _progressBar->setRange(0, new_packet->height() - 1);
        _maps_h5_models.push_back(_currentModel);

		MultiElementsWidget * element_widget = _mapsElementsWidget->multi_element_widget();
		if (element_widget != nullptr)
		{
			int cur = element_widget->getRangeWidgetStartIndex();
			_num_images++;
			element_widget->setNumberOfImages(_num_images);

			if (cur == _num_images - 1)
			{
				_mapsElementsWidget->setModel(_currentModel);
				connect(_currentModel.get(),
					SIGNAL(model_data_updated()),
					_mapsElementsWidget,
					SLOT(model_updated()));
				element_widget->setRangeWidgetStartIndex(_num_images);
			}
		}
	}

    _currentModel->update_from_stream_block(new_packet);
    if(_last_packet != nullptr && _last_packet->row() != new_packet->row())
    {   
        QString str = ">" + QString::number(new_packet->row()) + " " + QString::number(new_packet->col()) + " : " + QString::number(new_packet->height()) + " " + QString::number(new_packet->width()) ;
//        _textEdit->append(str);

		MultiElementsWidget * element_widget = _mapsElementsWidget->multi_element_widget();
		if (element_widget != nullptr)
		{
			element_widget->redrawCounts();
		}
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

/*---------------------------------------------------------------------------*/

void LiveMapsElementsWidget::image_changed(int start, int end)
{
    _mapsElementsWidget->setModel(_maps_h5_models[start-1]);
}
