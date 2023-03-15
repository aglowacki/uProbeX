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

ScatterPlotView::ScatterPlotView(bool display_log10, bool black_background, QWidget* parent) : QWidget(parent)
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
    QString marker_shape = Preferences::inst()->getValue(STR_PFR_MARKER_SHAPE).toString();
    if (marker_shape == "Circle")
    {
        _scatter_series->setMarkerShape(QtCharts::QScatterSeries::MarkerShapeCircle);
    }
    else
    {
        _scatter_series->setMarkerShape(QtCharts::QScatterSeries::MarkerShapeRectangle);
    }
    _scatter_series->setMarkerSize(1.0);
    _scatter_series->setUseOpenGL(true);
    _chart->addSeries(_scatter_series);
    _display_log10 = display_log10;
    if (_display_log10)
    {
        _chart->addAxis(_axisXLog10, Qt::AlignBottom);
        _chart->addAxis(_axisYLog10, Qt::AlignLeft);
        _scatter_series->attachAxis(_axisXLog10);
        _scatter_series->attachAxis(_axisYLog10);
    }
    else
    {
        _chart->addAxis(_axisX, Qt::AlignBottom);
        _chart->addAxis(_axisY, Qt::AlignLeft);
        _scatter_series->attachAxis(_axisX);
        _scatter_series->attachAxis(_axisY);
    }

    if (black_background)
    {
        _chart->setBackgroundBrush(QBrush(QColor("black")));
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

void ScatterPlotView::setShape(QtCharts::QScatterSeries::MarkerShape shape)
{
    _scatter_series->setMarkerShape(shape);
    _updatePlot();
}

//---------------------------------------------------------------------------

void ScatterPlotView::setBlackBackground(int val)
{
    if (val == 0)
    {
        _chart->setBackgroundBrush(QBrush(QColor("white")));
    }
    else
    {
        _chart->setBackgroundBrush(QBrush(QColor("black")));
    }
}

//---------------------------------------------------------------------------

void ScatterPlotView::setLog10(int val)
{
    
    if (val == 0)
    {
        _display_log10 = false;
        _scatter_series->detachAxis(_axisXLog10);
        _scatter_series->detachAxis(_axisYLog10);

        _chart->removeAxis(_axisXLog10);
        _chart->removeAxis(_axisYLog10);

        _chart->addAxis(_axisX, Qt::AlignBottom);
        _chart->addAxis(_axisY, Qt::AlignLeft);

        _scatter_series->attachAxis(_axisX);
        _scatter_series->attachAxis(_axisY);
    }
    else
    {
        _display_log10 = true;
        _scatter_series->detachAxis(_axisX);
        _scatter_series->detachAxis(_axisY);
        _chart->removeAxis(_axisX);
        _chart->removeAxis(_axisY);

        _chart->addAxis(_axisXLog10, Qt::AlignBottom);
        _chart->addAxis(_axisYLog10, Qt::AlignLeft);
        _scatter_series->attachAxis(_axisXLog10);
        _scatter_series->attachAxis(_axisYLog10);
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
        QString xSavedName = _cb_x_axis_element->currentText();
        QString ySavedName = _cb_y_axis_element->currentText();
        _cb_x_axis_element->clear();
        _cb_y_axis_element->clear();

        _cb_x_axis_element->addItem(" ");
        _cb_y_axis_element->addItem(" ");
        for (auto& itr : map_names)
        {
            _cb_x_axis_element->addItem(QString(itr.c_str()));
            _cb_y_axis_element->addItem(QString(itr.c_str()));
        }

        int xIdx = _cb_x_axis_element->findText(xSavedName);
        int yIdx = _cb_y_axis_element->findText(ySavedName);
        if (xIdx > -1)
        {
            _cb_x_axis_element->setCurrentIndex(xIdx);
        }
        if (yIdx > -1)
        {
            _cb_y_axis_element->setCurrentIndex(yIdx);
        }
    }
    _updatePlot();
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

            if (_display_log10)
            {
                x_map = x_map.unaryExpr([](float v) { return (v <= 0.0f) ? 0.0000001f : v; });
                y_map = y_map.unaryExpr([](float v) { return (v <= 0.0f) ? 0.0000001f : v; });
            }

            float xMinVal = x_map.minCoeff();
            float yMinVal = y_map.minCoeff();

            float xMaxVal = x_map.maxCoeff();
            float yMaxVal = y_map.maxCoeff();

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
            
            if (_display_log10)
            {
                _axisXLog10->setRange(xMinVal, xMaxVal);
                _axisYLog10->setRange(yMinVal, yMaxVal);
            }
            else
            {
                _axisX->setRange(xMinVal, xMaxVal);
                _axisY->setRange(yMinVal, yMaxVal);
            }

            _scatter_series->setName(sname);
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

    bool dark_theme = Preferences::inst()->getValue(STR_PFR_USE_DARK_THEME).toBool();
    

    _ck_display_log10 = new QCheckBox("Display log10");
    _ck_display_log10->setChecked(_display_log10);
    connect(_ck_display_log10, &QCheckBox::stateChanged, this, &ScatterPlotWidget::set_log10);

    _ck_black_background = new QCheckBox("Black Background");
    _ck_black_background->setChecked(dark_theme);
    connect(_ck_black_background, &QCheckBox::stateChanged, this, &ScatterPlotWidget::setBlackBackground);

    _cb_shape = new QComboBox();
    _cb_shape->addItem("Circle");
    _cb_shape->addItem("Rectangle");
    connect(_cb_shape, qOverload<const QString&>(&QComboBox::currentIndexChanged), this, &ScatterPlotWidget::onShapeChange);

    if (num_wins < 1)
    {
        num_wins = 1;
    }

    for (int i = 0; i < num_wins; i++)
    {
        _plot_view_list.push_back(new ScatterPlotView(_display_log10, dark_theme, nullptr));
        _subPlotLayout->addWidget(_plot_view_list[i]);
    }

    _sp_maker_size = new QSpinBox();
    _sp_maker_size->setRange(1, 100);
    _sp_maker_size->setSingleStep(1.0);
    _sp_maker_size->setValue(1);
    connect(_sp_maker_size, qOverload<int>(&QSpinBox::valueChanged), this, &ScatterPlotWidget::updateMarkerSize);


    QHBoxLayout* options_layout = new QHBoxLayout();
    options_layout->addWidget(_ck_display_log10);
    options_layout->addWidget(_ck_black_background);
    options_layout->addWidget(_sp_maker_size);
    options_layout->addWidget(new QLabel("Marker Shape:"));
    options_layout->addWidget(_cb_shape);

 //   options_layout->addItem(new QSpacerItem(9999, 40, QSizePolicy::Maximum));

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->addItem(_subPlotLayout);
    vlayout->addItem(options_layout);

    setLayout(vlayout);

}

//---------------------------------------------------------------------------

void ScatterPlotWidget::onShapeChange(QString val)
{
    if (val == "Circle")
    {
        for (auto& itr : _plot_view_list)
        {
            itr->setShape(QtCharts::QScatterSeries::MarkerShapeCircle);
        }
        Preferences::inst()->setValue(STR_PFR_MARKER_SHAPE, "Circle");
    }
    else
    {
        for (auto& itr : _plot_view_list)
        {
            itr->setShape(QtCharts::QScatterSeries::MarkerShapeRectangle);
        }
        Preferences::inst()->setValue(STR_PFR_MARKER_SHAPE, "Rectangle");
    }

}

//---------------------------------------------------------------------------

void ScatterPlotWidget::setBlackBackground(int val)
{
    for (auto& itr : _plot_view_list)
    {
        itr->setBlackBackground(val);
    }
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

void ScatterPlotWidget::set_log10(int val)
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