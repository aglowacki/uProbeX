/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/MinMaxSlider.h"

using namespace gstar;
/*---------------------------------------------------------------------------*/

MinMaxSlider::MinMaxSlider(QWidget* parent)
: QWidget(parent)
{
	_min_val = 0;
	_max_val = 100;

	QHBoxLayout* layout = new QHBoxLayout();
	QVBoxLayout* vlay = new QVBoxLayout();
	
	_min_lineedit = new QLineEdit();
	_min_lineedit->setMaximumWidth(25);
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
	_max_lineedit->setMaximumWidth(25);
	_max_lineedit->setText("100");
	connect(_max_lineedit, &QLineEdit::textChanged, this, &MinMaxSlider::max_lineedit_changed);

	vlay->addWidget(_min_slider);
	vlay->addWidget(_max_slider);

	layout->addWidget(_min_lineedit);
	layout->addWidget(new QLabel("%"));
	layout->addItem(vlay);
	layout->addWidget(_max_lineedit);
	layout->addWidget(new QLabel("%"));
	setLayout(layout);
}

/*---------------------------------------------------------------------------*/

MinMaxSlider::~MinMaxSlider()
{

}

/*---------------------------------------------------------------------------*/

void MinMaxSlider::setMin(float min)
{

   

}

/*---------------------------------------------------------------------------*/

void MinMaxSlider::setMax(float max)
{

   
}

/*---------------------------------------------------------------------------*/

void MinMaxSlider::setVal(float val)
{


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