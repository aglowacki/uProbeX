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
	_historgram_widget->setMinimumSize(640, 480);
	_historgram = new HistogramPlot(_historgram_widget);

	_min_max_slider = new MinMaxSlider(false);

	QVBoxLayout* layout = new QVBoxLayout();
	
	 
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