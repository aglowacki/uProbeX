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

    _display_log10 = true;
    createLayout();

    _action_check_log10 = new QAction("Toggle Log10", this);
    _action_check_log10->setCheckable(true);
    _action_check_log10->setChecked(_display_log10);
    connect(_action_check_log10, SIGNAL(triggered()), this, SLOT(_check_log10()));

    _contextMenu = new QMenu(("Context menu"), this);
    _contextMenu->addAction(_action_check_log10);

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));
}

/*---------------------------------------------------------------------------*/

SpectraWidget::~SpectraWidget()
{


}

/*---------------------------------------------------------------------------*/

void SpectraWidget::createLayout()
{
    _series = new QtCharts::QLineSeries();

    _series->append(0, 0);
    _series->append(2048, 100);

    _chart = new QtCharts::QChart();
    _chart->legend()->hide();
    _chart->addSeries(_series);
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

    if(_display_log10)
        _spectra = std::log10((std::valarray<float>)*spectra);

    _update_series();
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::ShowContextMenu(const QPoint &pos)
{
    _contextMenu->exec(mapToGlobal(pos));
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::_check_log10()
{

    if(_display_log10)
    {
        _spectra = std::pow(10.0f, _spectra);
    }
    else
    {
        _spectra = std::log10(_spectra);
    }
    _display_log10 = !_display_log10;
    _action_check_log10->setChecked(_display_log10);
    _update_series();
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::_update_series()
{
    _chart->removeAllSeries();
    _series = new QtCharts::QLineSeries();

    for(int i =0; i < _spectra.size(); i++)
    {
        _series->append(i, _spectra[i]);
    }
    _chart->addSeries(_series);
    _chart->createDefaultAxes();
}

/*---------------------------------------------------------------------------*/
