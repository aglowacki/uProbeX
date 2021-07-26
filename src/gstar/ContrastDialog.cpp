/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/ContrastDialog.h"

using namespace gstar;
/*---------------------------------------------------------------------------*/

ContrastDialog::ContrastDialog(const data_struct::ArrayXXr* arr, QWidget* parent) : QDialog(parent)
{
	_historgram_widget = new QWidget(this);
	_historgram_widget->setMinimumSize(640, 480);
	_historgram = new HistogramPlot(_historgram_widget);

	_min_max_slider = new MinMaxSlider(false);

	QVBoxLayout* layout = new QVBoxLayout();

	if (arr != nullptr)
	{
		_arr = arr;
		_min_max_slider->setMinMax(_arr->minCoeff(), _arr->maxCoeff());
	}

	layout->addWidget(_historgram_widget);
	layout->addWidget(_min_max_slider);
	setLayout(layout);

}

/*---------------------------------------------------------------------------*/

ContrastDialog::~ContrastDialog()
{

}

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/