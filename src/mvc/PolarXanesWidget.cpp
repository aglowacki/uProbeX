/*-----------------------------------------------------------------------------
 * Copyright (c) 2025, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <QVBoxLayout>
#include <mvc/PolarXanesWidget.h>

//---------------------------------------------------------------------------

PolarXanesWidget::PolarXanesWidget(QWidget* parent) : QWidget(parent)
{
    createLayout();
}

//---------------------------------------------------------------------------

PolarXanesWidget::~PolarXanesWidget()
{

}

//---------------------------------------------------------------------------

void PolarXanesWidget::createLayout()
{
    
    _spectra_widget = new SpectraWidget();

    QLayout* layout = new QVBoxLayout();
    layout->addWidget(_spectra_widget);
    setLayout(layout);

}

//---------------------------------------------------------------------------

void PolarXanesWidget::setIntegratedSpectra(ArrayDr* lhcp_int_spec, ArrayDr* rhcp_int_spec)
{
    //_spectra_widget->append_spectra(name, _fit_int_spec_map.at("Background"), (data_struct::Spectra<double>*) & _ev);
}

//---------------------------------------------------------------------------