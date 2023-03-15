/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ScatterPlotWidget.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QLabel>

/*---------------------------------------------------------------------------*/

ScatterPlotWidget::ScatterPlotWidget(QWidget* parent) : QWidget(parent)
{

    _display_log10 = true;
    _currentYAxis = nullptr;
    createLayout();
    /*
    _action_check_log10 = new QAction("Toggle Log10", this);
    _action_check_log10->setCheckable(true);
    _action_check_log10->setChecked(_display_log10);
    connect(_action_check_log10, SIGNAL(triggered()), this, SLOT(_check_log10()));

    _contextMenu = new QMenu(("Context menu"), this);
    _contextMenu->addAction(_action_check_log10);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));
            */
    /*
    connect(this, SIGNAL(trigger_connect_markers()),
            this, SLOT(connectMarkers()));
            */
}

/*---------------------------------------------------------------------------*/

ScatterPlotWidget::~ScatterPlotWidget()
{

}

/*---------------------------------------------------------------------------*/

void ScatterPlotWidget::createLayout()
{
    _axisYLog10 = new QtCharts::QLogValueAxis();
    _axisYLog10->setTitleText("Counts Log10");
    _axisYLog10->setLabelFormat("%.1e");
    _axisYLog10->setBase(10.0);

    _axisX = new QtCharts::QValueAxis();
    _axisX->setTitleText("Energy (keV)");
    _axisX->setLabelFormat("%.2f");
    _axisX->setTickAnchor(0.0);
    _axisX->setTickInterval(0.5);
    _axisX->setTickType(QtCharts::QValueAxis::TicksDynamic);
    _axisX->setTickCount(20);

    _axisY = new QtCharts::QValueAxis();
    _axisY->setTitleText("Counts");
    _axisY->setLabelFormat("%i");

    _chart = new QtCharts::QChart();
    _chart->addAxis(_axisX, Qt::AlignBottom);

    if (_display_log10)
    {
        _currentYAxis = _axisYLog10;
    }
    else
    {
        _currentYAxis = _axisY;
    }

    _chart->addAxis(_currentYAxis, Qt::AlignLeft);

    // Toolbar zoom out action

    QtCharts::QChartView* chartview = new QtCharts::QChartView(_chart);
    QHBoxLayout* spectra_layout = new QHBoxLayout();
    spectra_layout->addWidget(chartview);

    QHBoxLayout* options_layout = new QHBoxLayout();
    options_layout->addWidget(new QLabel("Display Energy Min:"));
//    options_layout->addWidget(_display_eneergy_min);
    options_layout->addWidget(new QLabel("keV  ,  "));

    options_layout->addWidget(new QLabel("Display Energy Max:"));
 //   options_layout->addWidget(_display_eneergy_max);
    options_layout->addWidget(new QLabel("keV"));

 //   options_layout->addWidget(_btn_reset_chart_view);

    options_layout->addItem(new QSpacerItem(9999, 40, QSizePolicy::Maximum));

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->addItem(spectra_layout);
    vlayout->addItem(options_layout);

    setLayout(vlayout);
}

/*---------------------------------------------------------------------------*/
/*
void ScatterPlotWidget::set_element_lines(data_struct::Fit_Element_Map<double>* element)
{
    QScatterSeries *series0 = new QScatterSeries();
    series0->setName("scatter1");
    series0->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    >setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    series0->setMarkerSize(15.0);

    series0->append(0, 6);
    series0->append(2, 4);
    series0->append(3, 8);
    series0->append(7, 4);
    series0->append(10, 5);


	//clear old one
	for (QtCharts::QLineSeries* itr : _element_lines)
	{
		itr->detachAxis(_axisX);
        itr->detachAxis(_currentYAxis);
		_chart->removeSeries(itr);
		delete itr;
	}
	_element_lines.clear();

    float line_min = 0.1;
    float line_max = 9999;

    line_min = std::max(0.1, ((QtCharts::QValueAxis*)_currentYAxis)->min());
    line_max = ((QtCharts::QValueAxis*)_currentYAxis)->max();

    if(element != nullptr)
    {
        const std::vector<data_struct::Element_Energy_Ratio<double>>& energy_ratios = element->energy_ratios();
        for(auto& itr : energy_ratios)
        {
            QtCharts::QLineSeries* line = new QtCharts::QLineSeries();
            QPen pen = line->pen();
            switch (itr.ptype)
            {
            case data_struct::Element_Param_Type::Kb1_Line:
            case data_struct::Element_Param_Type::Kb2_Line:
                pen.setColor(QColor::fromRgb(LINE_KB_R, LINE_KB_G, LINE_KB_B));
                break;
            case data_struct::Element_Param_Type::Ka1_Line:
            case data_struct::Element_Param_Type::Ka2_Line:
                pen.setColor(QColor::fromRgb(LINE_KA_R, LINE_KA_G, LINE_KA_B));
                break;
                //L1
            case data_struct::Element_Param_Type::Lb3_Line: // 4
            case data_struct::Element_Param_Type::Lb4_Line: // 5
            case data_struct::Element_Param_Type::Lg2_Line: // 7
            case data_struct::Element_Param_Type::Lg3_Line: // 8
            case data_struct::Element_Param_Type::Lg4_Line: // 9
                pen.setColor(QColor::fromRgb(LINE_L1_R, LINE_L1_G, LINE_L1_B));
                break;
                //L2
            case data_struct::Element_Param_Type::Lb1_Line: // 2
            case data_struct::Element_Param_Type::Lg1_Line: // 6
            case data_struct::Element_Param_Type::Ln_Line:  // 11
                pen.setColor(QColor::fromRgb(LINE_L2_R, LINE_L2_G, LINE_L2_B));
                break;
                //L3
            case data_struct::Element_Param_Type::La1_Line: // 0
            case data_struct::Element_Param_Type::La2_Line: // 1
            case data_struct::Element_Param_Type::Lb2_Line: // 3
            case data_struct::Element_Param_Type::Ll_Line:  // 10
                pen.setColor(QColor::fromRgb(LINE_L3_R, LINE_L3_G, LINE_L3_B));
                break;
            default:
                pen.setColor(QColor::fromRgb(LINE_M_R, LINE_M_G, LINE_M_B));
                break;
            }

            line->setPen(pen);
            line->append(itr.energy, line_min);
            //float line_ratio = data_struct::Element_Param_Percent_Map.at(itr.ptype);
            float line_ratio = itr.ratio;
			if (itr.ratio == 0)
			{
				line->append(itr.energy, (line_min));
			}
			else
			{
                float line_height = 1.0;
                if (_display_log10)
                {
                    line_height = pow(10.0, (log10(line_max) * line_ratio) );
                }
                else
                {
                    line_height = line_max * line_ratio;
                }
				line->append(itr.energy, line_height);
			}
			QString eName = QString(element->full_name().c_str());
            eName = QString(data_struct::Element_Param_Str_Map.at(itr.ptype).c_str());

            line->setName(eName);
            _chart->addSeries(line);
            line->attachAxis(_axisX);
            line->attachAxis(_currentYAxis);
            _element_lines.push_back(line);
        }
        emit trigger_connect_markers();
    }
}
*/
/*---------------------------------------------------------------------------*/

void ScatterPlotWidget::set_log10(bool val)
{
    /*
    QList<QtCharts::QAbstractSeries*> series = _chart->series();

    for(QtCharts::QAbstractSeries* ser : _element_lines)
    {
        series.removeOne(ser);
    }

    for(QtCharts::QAbstractSeries* ser : series)
    {
        ser->detachAxis(_currentYAxis);
    }
    _chart->removeAxis(_currentYAxis);

    _display_log10 = val;

    if(_display_log10) //if current one is log10, set to normal
    {
        _currentYAxis = _axisYLog10;
    }
    else
    {
        _currentYAxis = _axisY;
    }

    _chart->addAxis(_currentYAxis, Qt::AlignLeft);

    for(QtCharts::QAbstractSeries* ser : series)
    {
        ser->attachAxis(_currentYAxis);
    }

    emit(y_axis_changed(_display_log10));
    */
}

//---------------------------------------------------------------------------
/*
QPixmap ScatterPlotWidget::getPngofChart()
{

    return _chartView->grab();

}

//---------------------------------------------------------------------------

void ScatterPlotWidget::connectMarkers()
{
    // Connect all markers to handler
    const auto markers = _chart->legend()->markers();
    for (QtCharts::QLegendMarker *marker : markers)
    {
        // Disconnect possible existing connection to avoid multiple connections
        QObject::disconnect(marker, &QtCharts::QLegendMarker::clicked, this, &ScatterPlotWidget::handleMarkerClicked);
        QObject::connect(marker, &QtCharts::QLegendMarker::clicked, this, &ScatterPlotWidget::handleMarkerClicked);
    }
}

//---------------------------------------------------------------------------

void ScatterPlotWidget::disconnectMarkers()
{
    const auto markers = _chart->legend()->markers();
    for (QtCharts::QLegendMarker *marker : markers)
    {
        QObject::disconnect(marker, &QtCharts::QLegendMarker::clicked, this, &ScatterPlotWidget::handleMarkerClicked);
    }
}

//---------------------------------------------------------------------------

void ScatterPlotWidget::handleMarkerClicked()
{
    QtCharts::QLegendMarker* marker = qobject_cast<QtCharts::QLegendMarker*> (sender());
    Q_ASSERT(marker);

    switch (marker->type())
    {
        case QtCharts::QLegendMarker::LegendMarkerTypeXY:
        {
        // Toggle visibility of series
        marker->series()->setVisible(!marker->series()->isVisible());

        // Turn legend marker back to visible, since hiding series also hides the marker
        // and we don't want it to happen now.
        marker->setVisible(true);

        // Dim the marker, if series is not visible
        qreal alpha = 1.0;

        if (!marker->series()->isVisible())
            alpha = 0.5;

        QColor color;
        QBrush brush = marker->labelBrush();
        color = brush.color();
        color.setAlphaF(alpha);
        brush.setColor(color);
        marker->setLabelBrush(brush);

        brush = marker->brush();
        color = brush.color();
        color.setAlphaF(alpha);
        brush.setColor(color);
        marker->setBrush(brush);

        QPen pen = marker->pen();
        color = pen.color();
        color.setAlphaF(alpha);
        pen.setColor(color);
        marker->setPen(pen);

        break;
        }
    default:
        {
        
        break;
        }
    }
}
*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/