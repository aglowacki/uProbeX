/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/SpectraWidget.h>


#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QDebug>

#include <math.h>

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
    QtCharts::QLineSeries* series = new QtCharts::QLineSeries();

    series->append(0, 0);
    series->append(2048, 100);

    _chart = new QtCharts::QChart();
    //_chart->legend()->hide();
    _chart->addSeries(series);
    _chart->createDefaultAxes();
    //_chart->setTitle("Integrated Spectra");

    _chartView = new QtCharts::QChartView(_chart);
    //_chartView->setRenderHint(QPainter::Antialiasing);

    QLayout* layout = new QHBoxLayout();
    layout->addWidget(_chartView);
    setLayout(layout);
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::append_spectra(QString name, data_struct::xrf::Spectra* spectra)
{
    if (spectra == nullptr)
        return;

    if(_display_log10)
        _spectra_map[name] = std::log10((std::valarray<float>)*spectra);
    else
        _spectra_map[name] = (std::valarray<float>)*spectra;

    _update_series();
}

void SpectraWidget::remove_spectra(QString name)
{
    if(_spectra_map.count(name) > 0)
        _spectra_map.erase(name);
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
    for(auto& itr : _spectra_map)
    {
        if(_display_log10)
        {
            itr.second = std::pow(10.0f, itr.second);
        }
        else
        {
            itr.second = std::log10(itr.second);
        }
    }

    _display_log10 = !_display_log10;
    _action_check_log10->setChecked(_display_log10);
    _update_series();
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::_update_series()
{
    _chart->removeAllSeries();
    for(auto& itr : _spectra_map)
    {
        QtCharts::QLineSeries *series = new QtCharts::QLineSeries();
        series->setName(itr.first);
        for(unsigned int i =0; i < itr.second.size(); i++)
        {
            if(std::isnan(itr.second[i]) || std::isinf(itr.second[i]))
            {
                itr.second[i] = 0.00000001f;
            }
            series->append(i, itr.second[i]);

        }
        _chart->addSeries(series);
    }
    _chart->createDefaultAxes();
}

/*---------------------------------------------------------------------------*/
