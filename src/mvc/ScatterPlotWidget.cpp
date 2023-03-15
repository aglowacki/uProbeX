/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ScatterPlotWidget.h>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QLabel>
#include "preferences/Preferences.h"

 //---------------------------------------------------------------------------

ScatterPlotView::ScatterPlotView(bool display_log10, QWidget* parent) : QWidget(parent)
{

    _model = nullptr;
    _curAnalysis = QString(STR_FIT_NNLS.c_str());

    _axisXLog10 = new QtCharts::QLogValueAxis();
    _axisXLog10->setTitleText("");
    _axisXLog10->setLabelFormat("%.1e");
    _axisXLog10->setBase(10.0);

    _axisX = new QtCharts::QValueAxis();
    _axisX->setTitleText("");
    _axisX->setLabelFormat("%f");
    
    _axisYLog10 = new QtCharts::QLogValueAxis();
    _axisYLog10->setTitleText("");
    _axisYLog10->setLabelFormat("%.1e");
    _axisYLog10->setBase(10.0);

    _axisY = new QtCharts::QValueAxis();
    _axisY->setTitleText("");
    _axisY->setLabelFormat("%f");

    _chart = new QtCharts::QChart();

    _cb_x_axis_element = new QComboBox();
    _cb_x_axis_element->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(_cb_x_axis_element, qOverload<const QString&>(&QComboBox::currentIndexChanged), this, &ScatterPlotView::onNameChange);
    _cb_y_axis_element = new QComboBox();
    _cb_y_axis_element->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(_cb_y_axis_element, qOverload<const QString&>(&QComboBox::currentIndexChanged), this, &ScatterPlotView::onNameChange);

    _chartView = new QtCharts::QChartView(_chart);

    //setRenderHint(QPainter::Antialiasing);
    _scatter_series = new QtCharts::QScatterSeries();
    _scatter_series->setMarkerShape(QtCharts::QScatterSeries::MarkerShapeCircle);
    //_scatter_series->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    _scatter_series->setMarkerSize(1.0);
    _scatter_series->setUseOpenGL(true);
    _chart->addSeries(_scatter_series);

    if (display_log10)
    {
        _chart->addAxis(_axisXLog10, Qt::AlignBottom);
        _chart->addAxis(_axisYLog10, Qt::AlignLeft);
    }
    else
    {
        _chart->addAxis(_axisX, Qt::AlignBottom);
        _chart->addAxis(_axisY, Qt::AlignLeft);
    }


    QHBoxLayout* hbox = new QHBoxLayout();
    hbox->addWidget(new QLabel("X Axis"));
    hbox->addWidget(_cb_x_axis_element);
    hbox->addWidget(new QLabel("Y Axis"));
    hbox->addWidget(_cb_y_axis_element);
    //hbox->addItem(new QSpacerItem(9999, 40, QSizePolicy::Maximum));

    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->addItem(hbox);
    vbox->addWidget(_chartView);

    setLayout(vbox);

}
 
//---------------------------------------------------------------------------

ScatterPlotView::~ScatterPlotView()
{

}

//---------------------------------------------------------------------------

void ScatterPlotView::setLog10(int val)
{
    
    if (val == 1)
    {
        _scatter_series->detachAxis(_axisX);
        _scatter_series->detachAxis(_axisY);
        _chart->removeAxis(_axisX);
        _chart->removeAxis(_axisY);

        _scatter_series->attachAxis(_axisXLog10);
        _scatter_series->attachAxis(_axisYLog10);

        _chart->addAxis(_axisXLog10, Qt::AlignBottom);
        _chart->addAxis(_axisYLog10, Qt::AlignLeft);

    }
    else
    {
        _scatter_series->detachAxis(_axisXLog10);
        _scatter_series->detachAxis(_axisYLog10);

        _chart->removeAxis(_axisXLog10);
        _chart->removeAxis(_axisYLog10);

        _scatter_series->attachAxis(_axisX);
        _scatter_series->attachAxis(_axisY);

        _chart->addAxis(_axisX, Qt::AlignBottom);
        _chart->addAxis(_axisY, Qt::AlignLeft);
    }
    _updatePlot();

}

//---------------------------------------------------------------------------

void ScatterPlotView::onNameChange(QString name)
{
    _updatePlot();
}

//---------------------------------------------------------------------------

void ScatterPlotView::setModel(MapsH5Model* model)
{
    _model = model;
    _updateNames();
}

//---------------------------------------------------------------------------

void ScatterPlotView::setAnalysisType(QString name)
{
    _curAnalysis = name;
    _updateNames();
}

//---------------------------------------------------------------------------

void ScatterPlotView::_updateNames()
{
    if (_model != nullptr)
    {
        std::vector<std::string> map_names;
        _model->generateNameLists(_curAnalysis, map_names);
        _cb_x_axis_element->clear();
        _cb_y_axis_element->clear();

        _cb_x_axis_element->addItem(" ");
        _cb_y_axis_element->addItem(" ");
        for (auto& itr : map_names)
        {
            _cb_x_axis_element->addItem(QString(itr.c_str()));
            _cb_y_axis_element->addItem(QString(itr.c_str()));
        }
    }
}

//---------------------------------------------------------------------------

void ScatterPlotView::updateMarkerSize(qreal val)
{
    _scatter_series->setMarkerSize(val);
    _updatePlot();
}

//---------------------------------------------------------------------------

void ScatterPlotView::_updatePlot()
{
    std::string xName = _cb_x_axis_element->currentText().toStdString();
    std::string yName = _cb_y_axis_element->currentText().toStdString();


    if (_model != nullptr)
    {
        data_struct::Fit_Count_Dict<float> fit_counts;
        _model->getAnalyzedCounts(_curAnalysis.toStdString(), fit_counts);
        std::map<std::string, data_struct::ArrayXXr<float>>* scalers = _model->getScalers();

        data_struct::ArrayXXr<float> x_map;
        data_struct::ArrayXXr<float> y_map;

        bool foundX = false;
        bool foundY = false;

        int xCnt = fit_counts.count(xName);
        if (xCnt != 0)
        {
            x_map = fit_counts.at(xName);
            foundX = true;
        }
        else
        {
            xCnt = scalers->count(xName);
            if (xCnt != 0)
            {
                x_map = scalers->at(xName);
                foundX = true;
            }
        }


        int yCnt = fit_counts.count(yName);
        if (yCnt != 0)
        {
            y_map = fit_counts.at(yName);
            foundY = true;
        }
        else
        {
            yCnt = scalers->count(yName);
            if (yCnt != 0)
            {
                y_map = scalers->at(yName);
                foundY = true;
            }
        }

        if (foundX && foundY)
        {
            _chart->removeSeries(_scatter_series);
            _scatter_series->clear();

            for (int y = 0; y < x_map.rows(); y++)
            {
                for (int x = 0; x < x_map.cols(); x++)
                {
                    _scatter_series->append(x_map(y,x), y_map(y,x));
                }
                
            }
            _chart->addSeries(_scatter_series);

            _axisX->setTitleText(_cb_x_axis_element->currentText());
            _axisY->setTitleText(_cb_y_axis_element->currentText());
            _axisXLog10->setTitleText(_cb_x_axis_element->currentText());
            _axisYLog10->setTitleText(_cb_y_axis_element->currentText());
            QString sname = _cb_x_axis_element->currentText() + "/" + _cb_y_axis_element->currentText();
            _scatter_series->setName(sname);
            _chart->zoomReset();
        }
    }

}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ScatterPlotWidget::ScatterPlotWidget(QWidget* parent) : QWidget(parent)
{
    _createLayout();
}

/*---------------------------------------------------------------------------*/

ScatterPlotWidget::~ScatterPlotWidget()
{
    Preferences::inst()->setValue(STR_PRF_ScatterPlot_NumWindows, _plot_view_list.size());
    Preferences::inst()->setValue(STR_PRF_ScatterPlot_Log10, _ck_display_log10->isChecked());
}

/*---------------------------------------------------------------------------*/

void ScatterPlotWidget::_createLayout()
{

    _subPlotLayout = new QHBoxLayout();

    // read pref to get number of windows
    int num_wins = Preferences::inst()->getValue(STR_PRF_ScatterPlot_NumWindows).toInt();
    bool _display_log10 = Preferences::inst()->getValue(STR_PRF_ScatterPlot_Log10).toBool();

    _ck_display_log10 = new QCheckBox("Display log10");
    _ck_display_log10->setChecked(_display_log10);
    connect(_ck_display_log10, &QCheckBox::stateChanged, this, &ScatterPlotWidget::set_log10);

    if (num_wins < 1)
    {
        num_wins = 1;
    }

    for (int i = 0; i < num_wins; i++)
    {
        _plot_view_list.push_back(new ScatterPlotView(_display_log10, nullptr));
        _subPlotLayout->addWidget(_plot_view_list[i]);
    }

    _sp_maker_size = new QSpinBox();
    _sp_maker_size->setRange(1, 100);
    _sp_maker_size->setSingleStep(1.0);
    _sp_maker_size->setValue(1);
    connect(_sp_maker_size, qOverload<int>(&QSpinBox::valueChanged), this, &ScatterPlotWidget::updateMarkerSize);


    QHBoxLayout* options_layout = new QHBoxLayout();
    options_layout->addWidget(_ck_display_log10);
    options_layout->addWidget(_sp_maker_size);

 //   options_layout->addItem(new QSpacerItem(9999, 40, QSizePolicy::Maximum));

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->addItem(_subPlotLayout);
    vlayout->addItem(options_layout);

    setLayout(vlayout);

}

//---------------------------------------------------------------------------

void ScatterPlotWidget::updateMarkerSize(int val)
{
    for (auto& itr : _plot_view_list)
    {
        itr->updateMarkerSize(val);
    }
}

//---------------------------------------------------------------------------

void ScatterPlotWidget::setAnalysisType(QString name)
{
    for (auto& itr : _plot_view_list)
    {
        itr->setAnalysisType(name);
    }
}


//---------------------------------------------------------------------------

void ScatterPlotWidget::setModel(MapsH5Model* model)
{
    if (model != nullptr)
    {
        for (auto& itr : _plot_view_list)
        {
            itr->setModel(model);
        }
    }
}

//---------------------------------------------------------------------------

void ScatterPlotWidget::set_log10(bool val)
{

    Preferences::inst()->setValue(STR_PRF_ScatterPlot_Log10, _ck_display_log10->isChecked());
    for (auto& itr : _plot_view_list)
    {
        itr->setLog10(val);
    }
    
}

//---------------------------------------------------------------------------
/*
QPixmap ScatterPlotWidget::getPngofChart()
{

    return _chartView->grab();

}
*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/