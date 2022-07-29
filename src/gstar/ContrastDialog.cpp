/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/ContrastDialog.h"

using namespace gstar;
/*---------------------------------------------------------------------------*/

ContrastDialog::ContrastDialog(QWidget* parent) : QDialog(parent)
{

	_historgram_widget = new QWidget(this);
	_historgram_widget->setMinimumSize(250, 50);
	_historgram = new HistogramPlot(_historgram_widget);

	_min_max_slider = new MinMaxSlider(false);
	connect(_min_max_slider, &MinMaxSlider::min_max_val_changed, this, &ContrastDialog::min_max_updated);
	connect(_min_max_slider, &MinMaxSlider::min_max_val_changed, this, &ContrastDialog::min_max_updated);
	QVBoxLayout* layout = new QVBoxLayout();

	QHBoxLayout* hbox = new QHBoxLayout();
	_btn_accept = new QPushButton("Accept");
	connect(_btn_accept, &QPushButton::pressed, this, &ContrastDialog::on_accepted);
	_btn_cancel = new QPushButton("Cancel");
	connect(_btn_cancel, &QPushButton::pressed, this, &ContrastDialog::on_rejected);
	hbox->addWidget(_btn_accept);
	hbox->addWidget(_btn_cancel);

	layout->addWidget(_historgram_widget);
	layout->addWidget(_min_max_slider);
	layout->addItem(hbox);
	
	setLayout(layout);

	_btn_accept->setAutoDefault(false);
	_btn_cancel->setAutoDefault(false);

}

/*---------------------------------------------------------------------------*/

ContrastDialog::~ContrastDialog()
{

}

void ContrastDialog::set_array(const data_struct::ArrayXXr<float>* arr)
{
	if (arr != nullptr)
	{
		_arr = arr;
		_min_max_slider->setMinMax(_arr->minCoeff(), _arr->maxCoeff());

		data_struct::ArrayTr<float> pts;
		int h = arr->rows();
		int w = arr->cols();
		int t = arr->size();
		pts.resize(t);
		int s = 0;
		for (int i = 0; i < h; i++)
		{
			for (int j = 0; j < w; j++)
			{
				pts(s) = ((*arr)(i, j));
				s++;
			}
		}
		_historgram->updatePoints(pts);
	}
}

/*---------------------------------------------------------------------------*/

void ContrastDialog::min_max_updated()
{
	float minCoef = _min_max_slider->getUserMin();
	float maxCoef = _min_max_slider->getUserMax();
	_historgram->set_min_max_lines(minCoef, maxCoef);
	_historgram_widget->update();
	emit on_min_max_update(minCoef, maxCoef);
}

/*---------------------------------------------------------------------------*/

void ContrastDialog::on_accepted()
{
	min_max_updated();
	emit accepted();
	accept();
}

/*---------------------------------------------------------------------------*/

void ContrastDialog::on_rejected()
{
	emit rejected();
	close();
}

/*---------------------------------------------------------------------------*/