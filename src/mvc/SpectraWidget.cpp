/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/SpectraWidget.h>


#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QDebug>

/*---------------------------------------------------------------------------*/

SpectraWidget::SpectraWidget(QWidget* parent) : QWidget(parent)
{

    createLayout();

}

/*---------------------------------------------------------------------------*/

SpectraWidget::~SpectraWidget()
{


}

/*---------------------------------------------------------------------------*/

void SpectraWidget::createLayout()
{


    QtCharts::QLineSeries *series = new QtCharts::QLineSeries();

    series->append(0, 0);
    series->append(2048, 100);

    _chart = new QtCharts::QChart();
    _chart->legend()->hide();
    _chart->addSeries(series);
    _chart->createDefaultAxes();
    _chart->setTitle("Integrated Spectra");

    _chartView = new QtCharts::QChartView(_chart);
    //_chartView->setRenderHint(QPainter::Antialiasing);

    QLayout* layout = new QHBoxLayout();
    layout->addWidget(_chartView);
    setLayout(layout);

}

void SpectraWidget::set_spectra(data_struct::xrf::Spectra* spectra)
{
    if (spectra == nullptr)
        return;

    _chart->removeAllSeries();

    QtCharts::QLineSeries *series = new QtCharts::QLineSeries();

    for(int i =0; i < spectra->size(); i++)
    {
        series->append(i, std::log10((*spectra)[i]));
    }
    _chart->addSeries(series);
    _chart->createDefaultAxes();
    //_chartView->repaint();
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::windowChanged(Qt::WindowStates oldState,
                                  Qt::WindowStates newState)
{
    /*
   Q_UNUSED(oldState);

   if(Qt::WindowMaximized || Qt::WindowActive == newState)
   {

   }
*/
}

/*---------------------------------------------------------------------------*/
