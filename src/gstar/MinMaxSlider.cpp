/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/MinMaxSlider.h"

#include <cmath>

using namespace gstar;
/*---------------------------------------------------------------------------*/

MinMaxSlider::MinMaxSlider(bool is_glob, QWidget* parent)
: QWidget(parent)
{
	QHBoxLayout* layout = new QHBoxLayout();
	QVBoxLayout* vlay = new QVBoxLayout();
	_min_label = nullptr;
	_max_label = nullptr;

	_min_val = 0;
	_max_val = 100;

	_min_lineedit = new QLineEdit();
	_min_lineedit->setMaximumWidth(35);
	_min_lineedit->setText("0");
	connect(_min_lineedit, &QLineEdit::returnPressed, this, &MinMaxSlider::min_lineedit_changed);

	_min_slider = new QSlider(Qt::Horizontal);
	_min_slider->setTickInterval(1);
	_min_slider->setMinimum(0);
	_min_slider->setMaximum(99);
	_min_slider->setValue(0);
	connect(_min_slider, &QSlider::valueChanged, this, &MinMaxSlider::min_slider_changed);

	_max_slider = new QSlider(Qt::Horizontal);
	_max_slider->setTickInterval(1);
	_max_slider->setMinimum(1);
	_max_slider->setMaximum(100);
	_max_slider->setValue(100);
	connect(_max_slider, &QSlider::valueChanged, this, &MinMaxSlider::max_slider_changed);

	_max_lineedit = new QLineEdit();
	_max_lineedit->setMaximumWidth(35);
	_max_lineedit->setText("100");
	connect(_max_lineedit, &QLineEdit::returnPressed, this, &MinMaxSlider::max_lineedit_changed);

	vlay->addWidget(_min_slider);
	vlay->addWidget(_max_slider);

	if(is_glob)
	{ 
		layout->addWidget(_min_lineedit);
		layout->addWidget(new QLabel("%"));
	}
	else
	{
		QVBoxLayout* vlay2 = new QVBoxLayout();
		_min_label = new QLabel();
		vlay2->addWidget(new QLabel("Min"));
		vlay2->addWidget(_min_lineedit);
		vlay2->addWidget(_min_label);
		layout->addItem(vlay2);
	}
	
	layout->addItem(vlay);

	if (is_glob)
	{
		layout->addWidget(_max_lineedit);
		layout->addWidget(new QLabel("%"));
	}
	else
	{
		QVBoxLayout* vlay2 = new QVBoxLayout();
		_max_label = new QLabel();
		vlay2->addWidget(new QLabel("Max"));
		vlay2->addWidget(_max_lineedit);
		vlay2->addWidget(_max_label);
		layout->addItem(vlay2);
	}
	

	setLayout(layout);
}

/*---------------------------------------------------------------------------*/

MinMaxSlider::~MinMaxSlider()
{

}

/*---------------------------------------------------------------------------*/

void MinMaxSlider::setMinMax(float min, float max)
{
	_min_val = min;
	_max_val = max;
	

	if (max < 10.0)
	{
		//make 128 bins
		_min_slider->setMinimum(0);
		_min_slider->setMaximum(127);
		_min_slider->setValue(0);

		_max_slider->setMinimum(1);
		_max_slider->setMaximum(128);
		_max_slider->setValue(128);
	}
	else
	{
		_min_slider->setMinimum(_min_val);
		_min_slider->setMaximum(_max_val - 1);
		_min_slider->setValue(_min_val);

		_max_slider->setMinimum(_min_val + 1);
		_max_slider->setMaximum(_max_val);
		_max_slider->setValue(_max_val);
	}
	_min_lineedit->setText(QString::number(_min_val));
	_max_lineedit->setText(QString::number(_max_val));

	if (_min_label != nullptr)
	{
		_min_label->setText(QString::number(_min_val));
	}
	if (_max_label != nullptr)
	{
		_max_label->setText(QString::number(_max_val));
	}
}

/*---------------------------------------------------------------------------*/

void MinMaxSlider::min_slider_changed(int val)
{
	QString tval;
	if (_max_slider->value() <= val)
	{
		_max_slider->setValue(val + 1);
	}
	if (_max_val < 10.0)
	{
		qreal _bin_width = (_max_val - _min_val) / 127.0;
		qreal nval = _min_val + (val * _bin_width);
		tval = QString::number(nval);
	}
	else
	{
		tval = QString::number(val);
	}
	_min_lineedit->setText(tval);
	emit min_max_val_changed();
}

/*---------------------------------------------------------------------------*/

void MinMaxSlider::max_slider_changed(int val)
{
	QString tval;
	if (_min_slider->value() >= val)
	{
		_min_slider->setValue(val - 1);
	}
	if (_max_val < 10.0)
	{
		qreal _bin_width = (_max_val - _min_val) / 127.0;
		qreal nval = _min_val + (val * _bin_width);
		tval = QString::number(nval);
	}
	else
	{
		tval = QString::number(val);
	}
	_max_lineedit->setText(tval);
	emit min_max_val_changed();
}

/*---------------------------------------------------------------------------*/

void MinMaxSlider::min_lineedit_changed()
{
	try
	{
		float nval = _min_lineedit->text().toFloat();
		if (_max_val < 10.0)
		{
			nval = std::max(_min_val, (qreal)nval);
			nval = std::min(_max_val, (qreal)nval);
			qreal _bin_width = (_max_val - _min_val) / 127.0;
			int bin = std::floor((nval - _min_val) / _bin_width);
			_min_slider->setValue(bin);
		}
		else
		{
			nval = std::max(_min_val, (qreal)nval);
			nval = std::min(_max_val - 1, (qreal)nval);
			_min_slider->setValue(nval);
		}
	}
	catch (...)
	{

	}
}

/*---------------------------------------------------------------------------*/

void MinMaxSlider::max_lineedit_changed()
{
	try
	{
		float nval = _max_lineedit->text().toFloat();
		if (_max_val < 10.0)
		{
			nval = std::min(_max_val, (qreal)nval);
			nval = std::max(_min_val, (qreal)nval);
			qreal _bin_width = (_max_val - _min_val) / 127.0;
			int bin = std::floor((nval - _min_val) / _bin_width);
			_max_slider->setValue(bin);
		}
		else
		{
			nval = std::min(_max_val, (qreal)nval);
			nval = std::max(_min_val + 1, (qreal)nval);
			_max_slider->setValue(nval);
		}
		
	}
	catch (...)
	{

	}
}

/*---------------------------------------------------------------------------*/

float MinMaxSlider::getUserMin()
{
	if (_max_val < 10.0)
	{
		float _bin_width = (_max_val - _min_val) / 127.0;
		float nval = _min_val + ((float)_min_slider->value() * _bin_width);
		return nval;
	}
	return _min_slider->value(); 
}

/*---------------------------------------------------------------------------*/

float MinMaxSlider::getUserMax()
{
	if (_max_val < 10.0)
	{
		float _bin_width = (_max_val - _min_val) / 127.0;
		float nval = _min_val + ((float)_max_slider->value() * _bin_width);
		return nval;
	}
	return _max_slider->value(); 
}