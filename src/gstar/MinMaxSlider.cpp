/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/MinMaxSlider.h"

#include <cmath>
#include "core/defines.h"

using namespace gstar;
//---------------------------------------------------------------------------

MinMaxSlider::MinMaxSlider(bool round_val, QWidget* parent)
: QWidget(parent)
{
	QHBoxLayout* layout = new QHBoxLayout();
	//layout->setContentsMargins(0, 0, 0, 0);
	QVBoxLayout* vlay = new QVBoxLayout();
	//vlay->setContentsMargins(20, 20, 20, 20);
	_min_label = nullptr;
	_max_label = nullptr;

	_round_val = round_val;
	
	_min_val = 0;
	_max_val = 128;
	_diff_over_128 = 1.0;

	_min_slider = new QSlider(Qt::Horizontal);
	_min_slider->setTickInterval(1);
	_min_slider->setMinimum(0);
	_min_slider->setMaximum(127);
	_min_slider->setValue(0);
	connect(_min_slider, &QSlider::sliderReleased, this, &MinMaxSlider::min_slider_changed);

	_max_slider = new QSlider(Qt::Horizontal);
	_max_slider->setTickInterval(1);
	_max_slider->setMinimum(1);
	_max_slider->setMaximum(128);
	_max_slider->setValue(128);
	connect(_max_slider, &QSlider::sliderReleased, this, &MinMaxSlider::max_slider_changed);

	_min_lineedit = new QLineEdit();
	_min_lineedit->setMaximumWidth(65);
	_min_lineedit->setText("0");
	connect(_min_lineedit, &QLineEdit::returnPressed, this, &MinMaxSlider::min_lineedit_changed);

	_max_lineedit = new QLineEdit();
	_max_lineedit->setMaximumWidth(65);
	_max_lineedit->setText("999999999999999999999999999999");
	connect(_max_lineedit, &QLineEdit::returnPressed, this, &MinMaxSlider::max_lineedit_changed);

	vlay->addWidget(_min_slider);
	vlay->addWidget(_max_slider);

	if(_round_val)
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

	if (_round_val)
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

//---------------------------------------------------------------------------

MinMaxSlider::~MinMaxSlider()
{

}

//---------------------------------------------------------------------------

void MinMaxSlider::setMinMax(float min, float max)
{
	_min_val = min;
	_max_val = max;

	disconnect(_min_lineedit, &QLineEdit::returnPressed, this, &MinMaxSlider::min_lineedit_changed);
	disconnect(_max_lineedit, &QLineEdit::returnPressed, this, &MinMaxSlider::max_lineedit_changed);
	disconnect(_min_slider, &QSlider::sliderReleased, this, &MinMaxSlider::min_slider_changed);
	disconnect(_max_slider, &QSlider::sliderReleased, this, &MinMaxSlider::max_slider_changed);
	disconnect(_min_slider, &QSlider::valueChanged, this, &MinMaxSlider::min_slider_value_changed);
	disconnect(_max_slider, &QSlider::valueChanged, this, &MinMaxSlider::max_slider_value_changed);

	_diff_over_128 = (_max_val - _min_val) / 128.0f;

	if(_min_lineedit->text().toFloat() < _min_val)
	{
		_min_lineedit->setText(QString::number(_min_val));
	}
	if(_max_lineedit->text().toFloat() > _max_val)
	{
		_max_lineedit->setText(QString::number(_max_val));
	}

	if (_min_label != nullptr)
	{
		_min_label->setText(QString::number(_min_val));
	}
	if (_max_label != nullptr)
	{
		_max_label->setText(QString::number(_max_val));
	}
	connect(_min_lineedit, &QLineEdit::returnPressed, this, &MinMaxSlider::min_lineedit_changed);
	connect(_max_lineedit, &QLineEdit::returnPressed, this, &MinMaxSlider::max_lineedit_changed);
	connect(_min_slider, &QSlider::sliderReleased, this, &MinMaxSlider::min_slider_changed);
	connect(_max_slider, &QSlider::sliderReleased, this, &MinMaxSlider::max_slider_changed);
	connect(_min_slider, &QSlider::valueChanged, this, &MinMaxSlider::min_slider_value_changed);
	connect(_max_slider, &QSlider::valueChanged, this, &MinMaxSlider::max_slider_value_changed);
}

//---------------------------------------------------------------------------

void MinMaxSlider::min_slider_value_changed(int val)
{
	QString tval;
	if(_round_val)
	{
		int nval  = _min_val + ((float)val * _diff_over_128);
		tval = QString::number(nval);
	}
	else
	{
 		tval = QString::number(_min_val + ((float)val * _diff_over_128));
	}
	_min_lineedit->setText(tval);
	emit min_max_val_changed(false);
}

//---------------------------------------------------------------------------

void MinMaxSlider::max_slider_value_changed(int val)
{
	QString tval;
	if(_round_val)
	{
		int nval = _min_val + ((float)val * _diff_over_128);
		tval = QString::number(nval);
	}
	else
	{
		tval = QString::number(_min_val + ((float)val * _diff_over_128));
	}
	_max_lineedit->setText(tval);
	emit min_max_val_changed(false);
}

//---------------------------------------------------------------------------

void MinMaxSlider::min_slider_changed()
{
	int val = _min_slider->value();
	QString tval;
	if(_round_val)
	{
		int nval  = _min_val + ((float)val * _diff_over_128);
		tval = QString::number(nval);
	}
	else
	{
 		tval = QString::number(_min_val + ((float)val * _diff_over_128));
	}
	_min_lineedit->setText(tval);
	min_lineedit_changed();
}

//---------------------------------------------------------------------------

void MinMaxSlider::max_slider_changed()
{
	int val = _max_slider->value();
	QString tval;
	if(_round_val)
	{
		int nval = _min_val + ((float)val * _diff_over_128);
		tval = QString::number(nval);
	}
	else
	{
		tval = QString::number(_min_val + ((float)val * _diff_over_128));
	}
	_max_lineedit->setText(tval);
	max_lineedit_changed();
}

//---------------------------------------------------------------------------

void MinMaxSlider::min_lineedit_changed()
{
	try
	{
		float nval = _min_lineedit->text().toFloat();
		
		nval = std::max(_min_val, (qreal)nval);
		nval = std::min(_max_val - _diff_over_128, (qreal)nval);
		_min_lineedit->setText(QString::number(nval));

		int slider_val = (int)((nval - _min_val) / _diff_over_128);
		disconnect(_min_slider, &QSlider::sliderReleased, this, &MinMaxSlider::min_slider_changed);
		_min_slider->setValue(slider_val);
		connect(_min_slider, &QSlider::sliderReleased, this, &MinMaxSlider::min_slider_changed);
		emit min_max_val_changed(true);
	}
	catch (...)
	{

	}
}

//---------------------------------------------------------------------------

void MinMaxSlider::max_lineedit_changed()
{
	try
	{
		float nval = _max_lineedit->text().toFloat();
		
		nval = std::min(_max_val, (qreal)nval);
		nval = std::max(_min_val + _diff_over_128, (qreal)nval);
		_max_lineedit->setText(QString::number(nval));

		int slider_val = (int)((nval - _min_val) / _diff_over_128);
		disconnect(_max_slider, &QSlider::sliderReleased, this, &MinMaxSlider::max_slider_changed);
		_max_slider->setValue(slider_val);
		connect(_max_slider, &QSlider::sliderReleased, this, &MinMaxSlider::max_slider_changed);
		emit min_max_val_changed(true);
		
	}
	catch (...)
	{

	}
}

//---------------------------------------------------------------------------

bool MinMaxSlider::getUserMinMax(float &minCoef, float &maxCoef, float &minPerc, float &maxPerc)
{
	bool isOk1 = false;
	bool isOk2 = false;
	try
	{
		minCoef =_min_lineedit->text().toFloat(&isOk1);
		maxCoef = _max_lineedit->text().toFloat(&isOk2);
		minPerc = (minCoef - _min_val) / ( _max_val - _min_val);
		maxPerc = (maxCoef - _min_val) / ( _max_val - _min_val);
	}
	catch(const std::exception& e)
	{
		logE << e.what() << '\n';
	}
	return isOk1 && isOk2;
}

//---------------------------------------------------------------------------
