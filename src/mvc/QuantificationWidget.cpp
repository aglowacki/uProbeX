/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/QuantificationWidget.h>
#include "data_struct/element_info.h"

//---------------------------------------------------------------------------

QuantificationWidget::QuantificationWidget(QWidget* parent) : QWidget(parent)
{
    _model = nullptr;
    _calib_curve = nullptr;
    _display_log10 = false;
    _calib_curve_series = new QLineSeries();
    _createLayout();
}

//---------------------------------------------------------------------------

QuantificationWidget::~QuantificationWidget()
{
    
}

//---------------------------------------------------------------------------

void QuantificationWidget::_createLayout()
{
    
    QVBoxLayout* layout = new QVBoxLayout();
    _cb_analysis_types = new QComboBox();
    connect(_cb_analysis_types, &QComboBox::currentTextChanged, this, &QuantificationWidget::update);
    _cb_scalers = new QComboBox();
    connect(_cb_scalers, &QComboBox::currentTextChanged, this, &QuantificationWidget::update);

    _axisYLog10 = new QLogValueAxis();
    _axisYLog10->setTitleText("Energy Calibration");
    //_axisYLog10->setLabelFormat("%.1e");
    _axisYLog10->setBase(10.0);


    _axisX = new QCategoryAxis();
    _axisX->setTitleText("Element");
    //_axisX->setLabelFormat("");
    //_axisX->setTickAnchor(0.0);
    //_axisX->setTickInterval(0.5);
    //_axisX->setTickType(QValueAxis::TicksDynamic);
    //_axisX->setTickCount(20);

    //_top_axis_elements = new QCategoryAxis();
    //_top_axis_elements->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    //_top_axis_elements->setGridLineVisible(false);

    _axisY = new QValueAxis();
    _axisY->setTitleText("Calib");
    //_axisY->setLabelFormat("%i");
    //_axisY->setTickCount(series->count());


    _chart = new QChart();
    _chart->addAxis(_axisX, Qt::AlignBottom);
    //_chart->addAxis(_top_axis_elements, Qt::AlignTop);
    /*
    float ymax = 0;
    float ymin = 0;

    if (_display_log10)
    {
        ymax = _axisYLog10->max();
        ymin = _axisYLog10->min();
        _currentYAxis = _axisYLog10;
    }
    else
    {
        ymax = _axisY->max();
        ymin = _axisY->min();
        _currentYAxis = _axisY;
    }
    */
    //_chart->addAxis(_currentYAxis, Qt::AlignLeft);
    _chart->addAxis(_axisY, Qt::AlignLeft);

    _chartView = new ChartView(_chart);
    
    layout->addWidget(_cb_analysis_types);
    layout->addWidget(_cb_scalers);
    layout->addWidget(_chartView);

    setLayout(layout);

}

//---------------------------------------------------------------------------

void QuantificationWidget::setModel(MapsH5Model* model)
{
    if (_model != model)
    {
        _model = model;
        if (_model != nullptr)
        {
            std::vector<std::string> analysis_types = _model->getAnalyzedTypes();
            _cb_analysis_types->clear();
            for (auto& itr : analysis_types)
            {
                _cb_analysis_types->addItem(itr.c_str());
            }
            QString first_an = analysis_types.front().c_str();

            QStringList quant_scalers = _model->get_calibration_curve_scalers(first_an.toStdString());
            _cb_scalers->clear();
            _cb_scalers->addItems(quant_scalers);
            QString not_used = "";
            update(not_used);
        }
    }
}

//---------------------------------------------------------------------------

void QuantificationWidget::update(const QString& val)
{
    _calib_curve = _model->get_calibration_curve(_cb_analysis_types->currentText().toStdString(), _cb_scalers->currentText().toStdString());
    if (_calib_curve != nullptr)
    {
        _calib_curve_series->detachAxis(_axisX);
        _calib_curve_series->detachAxis(_axisY);
        _chart->removeSeries(_calib_curve_series);
        _calib_curve_series->clear();
        int i = 0;
        float x = 1.0;
        double max_val = 0.0;
        for (int i = 0; i < 91; i++)
        {
            _axisX->append(QString::fromStdString(data_struct::Element_Symbols[i + 1]), x);
            double val = _calib_curve->calib_curve.at(data_struct::Element_Symbols[i + 1]);
            max_val = std::max(max_val, val);
            _calib_curve_series->append(i, val);
            x += 1;
        }
        _chart->addSeries(_calib_curve_series);
        _calib_curve_series->attachAxis(_axisX);
        _calib_curve_series->attachAxis(_axisY);

        _axisY->setRange(0, max_val);
        //_calib_curve_series->attachAxis(_currentYAxis);
    }
    else
    {
        // dispaly label that says no quant data found?
    }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
