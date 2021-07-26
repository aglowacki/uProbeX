/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/MinMaxSlider.h"

using namespace gstar;
/*---------------------------------------------------------------------------*/

MinMaxSlider::MinMaxSlider(bool is_prec, QWidget* parent)
: QWidget(parent)
{
	QHBoxLayout* layout = new QHBoxLayout();
	QVBoxLayout* vlay = new QVBoxLayout();
	
	_min_lineedit = new QLineEdit();
	_min_lineedit->setMaximumWidth(35);
	_min_lineedit->setText("0");
	connect(_min_lineedit, &QLineEdit::textChanged, this, &MinMaxSlider::min_lineedit_changed);

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
	connect(_max_lineedit, &QLineEdit::textChanged, this, &MinMaxSlider::max_lineedit_changed);

	vlay->addWidget(_min_slider);
	vlay->addWidget(_max_slider);

	layout->addWidget(_min_lineedit);
	if (is_prec)
	{
		layout->addWidget(new QLabel("%"));
	}
	layout->addItem(vlay);
	layout->addWidget(_max_lineedit);
	if (is_prec)
	{
		layout->addWidget(new QLabel("%"));
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
	_min_slider->setMinimum(_min_val);
	_min_slider->setMaximum(_max_val -1);
	_min_slider->setValue(_min_val);

	_max_slider->setMinimum(_min_val +1);
	_max_slider->setMaximum(_max_val);
	_max_slider->setValue(_max_val);

	_min_lineedit->setText(QString::number(_min_val));
	_max_lineedit->setText(QString::number(_max_val));
}

/*---------------------------------------------------------------------------*/

void MinMaxSlider::min_slider_changed(int val)
{
	if (_max_slider->value() <= val)
	{
		_max_slider->setValue(val + 1);
	}
	_min_lineedit->setText(QString::number(val));
	emit min_val_changed(val);
}

/*---------------------------------------------------------------------------*/

void MinMaxSlider::max_slider_changed(int val)
{
	if (_min_slider->value() >= val)
	{
		_min_slider->setValue(val - 1);
	}
	_max_lineedit->setText(QString::number(val));
	emit max_val_changed(val);
}

/*---------------------------------------------------------------------------*/

void MinMaxSlider::min_lineedit_changed(QString val)
{
	try
	{
		int nval = val.toInt();
		nval = std::max(_min_val, nval);
		nval = std::min(_max_val - 1, nval);
		_min_slider->setValue(nval);
	}
	catch (...)
	{

	}
}

/*---------------------------------------------------------------------------*/

void MinMaxSlider::max_lineedit_changed(QString val)
{
	try
	{
		int nval = val.toInt();
		nval = std::min(_max_val, nval);
		nval = std::max(_min_val + 1, nval);
		_max_slider->setValue(nval);
	}
	catch (...)
	{

	}
}

/*---------------------------------------------------------------------------*/