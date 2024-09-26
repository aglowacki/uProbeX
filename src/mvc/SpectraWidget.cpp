/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/SpectraWidget.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtCharts/QLegendMarker>
#include <QSpacerItem>
#include <QLabel>
#include <QIntValidator>
#include "preferences/Preferences.h"
#include <math.h>


//---------------------------------------------------------------------------

SpectraWidget::SpectraWidget(QWidget* parent) : QWidget(parent)
{

    _max_log_range = 1.0;
    m_isTouching = false;
    _display_log10 = Preferences::inst()->getValue(STR_PFR_LOG_10).toBool();
    _currentYAxis = nullptr;
    _int_spec_max_x = 1;
    _int_spec_max_y = 1;
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
    connect(this, SIGNAL(trigger_connect_markers()),
            this, SLOT(connectMarkers()));

}

//---------------------------------------------------------------------------

SpectraWidget::~SpectraWidget()
{

}

//---------------------------------------------------------------------------

void SpectraWidget::createLayout()
{
    _axisYLog10 = new QLogValueAxis();
    _axisYLog10->setTitleText("Counts Log10");
    _axisYLog10->setLabelFormat("%.1e");
    //_axisYLog10->setRange(1.0, 10000.0);
    _axisYLog10->setBase(10.0);


    _axisX = new QValueAxis();
    _axisX->setTitleText("Energy (keV)");
    _axisX->setLabelFormat("%.1f");
    _axisX->setTruncateLabels(false);
    _axisX->setTickAnchor(0.0);
    _axisX->setTickInterval(0.5);
    _axisX->setTickType(QValueAxis::TicksDynamic);
    _axisX->setTickCount(20);

    _top_axis_elements = new QCategoryAxis();
    //_top_axis_elements->setTickAnchor(0.0);
    //_top_axis_elements->setTickInterval(0.5);
    //_top_axis_elements->setTickType(QValueAxis::TicksDynamic);
    //_top_axis_elements->setTickCount(20);
    _top_axis_elements->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    _top_axis_elements->setGridLineVisible(false);

    _axisY = new QValueAxis();
    _axisY->setTitleText("Counts");
    _axisY->setLabelFormat("%i");
    //_axisY->setTickCount(series->count());


    _chart = new QChart();
    _chart->addAxis(_axisX, Qt::AlignBottom);
    _chart->addAxis(_top_axis_elements, Qt::AlignTop);

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

    _chart->addAxis(_currentYAxis, Qt::AlignLeft);

    _line_series = nullptr;

    _chartView = new ChartView(_chart);
    connect(_chartView, &ChartView::view_zoomed, this, &SpectraWidget::onUpdateChartLineEdits);

    // Toolbar zoom out action
    _display_eneergy_min = new QLineEdit(QString::number(_axisX->min(), 'f', 2));
    _display_eneergy_min->setMinimumWidth(50);
    connect(_display_eneergy_min, SIGNAL(textEdited(const QString &)), this, SLOT(onSpectraDisplayChanged(const QString &)));


    _display_eneergy_max = new QLineEdit(QString::number(_axisX->max(), 'f', 2));
    _display_eneergy_max->setMinimumWidth(50);
    connect(_display_eneergy_max, SIGNAL(textEdited(const QString &)), this, SLOT(onSpectraDisplayChanged(const QString &)));


    //_display_height_min = new QLineEdit(QString::number(ymin));
    //_display_height_min->setMinimumWidth(100);
    //connect(_display_height_min, SIGNAL(textEdited(const QString&)), this, SLOT(onSpectraDisplayHeightChanged(const QString&)));

    //_display_height_max = new QLineEdit(QString::number(ymax, 'g', 0));
    //_display_height_max->setMinimumWidth(100);
    //connect(_display_height_max, SIGNAL(textEdited(const QString&)), this, SLOT(onSpectraDisplayHeightChanged(const QString&)));

    _btn_reset_chart_view = new QPushButton("Reset");
    connect(_btn_reset_chart_view, &QPushButton::released, this, &SpectraWidget::onResetChartView);

    _btnSsettings = new QPushButton(QIcon(":/images/gear.png"), "", this);
    _btnSsettings->setMaximumWidth(64);
    connect(_btnSsettings, &QPushButton::released, this, &SpectraWidget::onSettingsDialog);

    QHBoxLayout* spectra_layout = new QHBoxLayout();
    spectra_layout->addWidget(_chartView);

    QHBoxLayout* options_layout = new QHBoxLayout();
    options_layout->addWidget(new QLabel("Display Energy Min:"));
    options_layout->addWidget(_display_eneergy_min);
    options_layout->addWidget(new QLabel("keV  ,  "));

    options_layout->addWidget(new QLabel("Display Energy Max:"));
    options_layout->addWidget(_display_eneergy_max);
    options_layout->addWidget(new QLabel("keV"));

    /*
    options_layout->addWidget(new QLabel("   |   Display Height Min:"));
    options_layout->addWidget(_display_height_min);
    options_layout->addWidget(new QLabel("Counts  ,  "));

    options_layout->addWidget(new QLabel("Display Height Max:"));
    options_layout->addWidget(_display_height_max);
    options_layout->addWidget(new QLabel("Counts | "));
    */
    options_layout->addWidget(_btn_reset_chart_view);
    options_layout->addWidget(_btnSsettings);
    options_layout->addItem(new QSpacerItem(9999, 40, QSizePolicy::Maximum));

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->addItem(spectra_layout);
    vlayout->addItem(options_layout);

    if (Preferences::inst()->getValue(STR_PFR_SPECTRA_BLACK_BG).toBool())
    {
        _chart->setBackgroundBrush(QBrush(QColor("black")));
    }

    vlayout->setSpacing(0);
	vlayout->setContentsMargins(0, 0, 0, 0);

    setLayout(vlayout);
}

//---------------------------------------------------------------------------

void SpectraWidget::setBackgroundBlack(bool val)
{
    if (val)
    {
        _chart->setBackgroundBrush(QBrush(QColor("black")));
    }
    else
    {
        _chart->setBackgroundBrush(QBrush(QColor("white")));
    }
}

void SpectraWidget::setDisplayRange(QString wmin, QString wmax, QString hmin, QString hmax)
{
    _display_eneergy_min->setText(wmin);
    _display_eneergy_max->setText(wmax);
    //_display_height_min->setText(hmin);
    //_display_height_max->setText(hmax);
    onSpectraDisplayChanged(wmin);
    onSpectraDisplayHeightChanged(hmin);
}

//---------------------------------------------------------------------------

void SpectraWidget::onSpectraDisplayChanged(const QString &)
{
    qreal maxRange = _display_eneergy_max->text().toDouble();
    qreal minRange = _display_eneergy_min->text().toDouble();
    _axisX->setRange(minRange, maxRange);
    _top_axis_elements->setRange(minRange, maxRange);
}

//---------------------------------------------------------------------------

void SpectraWidget::onSpectraDisplayHeightChanged(const QString&)
{
    //qreal maxRange = _display_height_max->text().toDouble();
    //qreal minRange = _display_height_min->text().toDouble();
    //_currentYAxis->setRange(minRange, maxRange);
}

//---------------------------------------------------------------------------

void SpectraWidget::onResetChartViewOnlyY()
{
    //_display_height_min->setText(QString::number(1));
    //_display_height_max->setText(QString::number(_int_spec_max_y, 'g', 0));
    _currentYAxis->setRange(1, _int_spec_max_y);
}

//---------------------------------------------------------------------------

void SpectraWidget::onResetChartView()
{
    _display_eneergy_min->setText(QString::number(0, 'f', 2));
    _display_eneergy_max->setText(QString::number(_int_spec_max_x, 'f', 2));

    //_display_height_min->setText(QString::number(1));
    //_display_height_max->setText(QString::number(_int_spec_max_y, 'g', 0));

    _currentYAxis->setRange(1, _int_spec_max_y);
    _axisX->setRange(0, _int_spec_max_x);
    _top_axis_elements->setRange(0, _int_spec_max_x);


}

//---------------------------------------------------------------------------

void SpectraWidget::onUpdateChartLineEdits()
{
    _display_eneergy_min->setText(QString::number(_axisX->min(), 'f', 2));
    _display_eneergy_max->setText(QString::number(_axisX->max(), 'f', 2));
    /*
    if (_display_log10)
    {
        _display_height_min->setText(QString::number(_axisYLog10->min()));
        _display_height_max->setText(QString::number(_axisYLog10->max(), 'g', 0));
    }
    else
    {
        _display_height_min->setText(QString::number(_axisY->min()));
        _display_height_max->setText(QString::number(_axisY->max()));
    }
    */
}

//---------------------------------------------------------------------------

void SpectraWidget::append_spectra(QString name, const data_struct::ArrayTr<double>* spectra, const data_struct::ArrayTr<double>*energy, QColor* color)
{
    if (spectra == nullptr)
        return;


    if (spectra->size() == 0)
        return;


    QLineSeries *series = nullptr;

    for( auto& itr : _chart->series())
    {
        if(itr->name() == name)
        {
            series = (QLineSeries*)itr;
			break;
        }
    }

    if (name == DEF_STR_INT_SPECTRA)
    {
        _int_spec_max_x = energy->maxCoeff();
        _int_spec_max_y = spectra->maxCoeff();
    }

    if(series == nullptr)
    {
        series = new QLineSeries();

        //float new_max = spectra->maxCoeff();
        //_max_log_range = std::max(_max_log_range, new_max);

        series->setName(name);
		if (energy->size() >= spectra->size())
		{
			for (unsigned int i = 0; i < spectra->size(); i++)
			{

				float val = (*spectra)[i];
				bool isFine = std::isfinite(val);
				if (false == isFine || val == 0.0f)
				{
					val = 1.0;
				}
				if (energy != nullptr)
				{
					series->append((*energy)[i], val);
				}
				else
				{
					series->append(i, val);
				}
			}
		}
        _chart->addSeries(series);
        series->attachAxis(_axisX);
        series->attachAxis(_top_axis_elements);
        _display_eneergy_min->setText(QString::number(_axisX->min(), 'f', 2));
        _display_eneergy_max->setText(QString::number(_axisX->max(), 'f', 2));

        series->attachAxis(_currentYAxis);
        QPen pen = series->pen();
        if (color != nullptr)
        {
            pen.setColor(*color);
        }
        else
        {
            int r, g, b, a;
            if (name == DEF_STR_INT_SPECTRA)
            {
                pen.setColor(QColor::fromRgb(INT_SPEC_R, INT_SPEC_G, INT_SPEC_B));
            }
            else if (name == DEF_STR_FIT_INT_SPECTRA)
            {
                pen.setColor(QColor::fromRgb(FIT_SPEC_R, FIT_SPEC_G, FIT_SPEC_B));
            }
            if (name == DEF_STR_BACK_SPECTRA)
            {
                pen.setColor(QColor::fromRgb(BAK_SPEC_R, BAK_SPEC_G, BAK_SPEC_B));
            }
            if (name == DEF_STR_MODEL_SPECTRA)
            {
                pen.setColor(QColor::fromRgb(MOD_SPEC_R, MOD_SPEC_G, MOD_SPEC_B));
            }
        }
        series->setPen(pen);
    }
    else
    {
		if (series->count() < spectra->size())
		{
			for (int i = series->count(); i < spectra->size(); i++)
			{
				series->append(i, 0.1);
			}
		}

        QList<QPointF> points = series->points();
        auto itr = points.begin();
		if (energy->size() >= spectra->size())
		{
			for (unsigned int i = 0; i < spectra->size(); i++)
			{
				float val = (*spectra)[i];
				bool isFine = std::isfinite(val);
				if (false == isFine || val == 0.0f)
				{
					val = 1.0;
				}
				itr->setX((*energy)[i]);
				itr->setY(val);
				itr++;
			}
		}
        series->replace(points);
        QPen pen = series->pen();
        if (color != nullptr)
        {
            pen.setColor(*color);
        }
    }

    emit trigger_connect_markers();
}

//---------------------------------------------------------------------------

void SpectraWidget::clearAllSpectra()
{
    for (auto& itr : _chart->series())
    {
        _chart->removeSeries(itr);
    }
}

//---------------------------------------------------------------------------

void SpectraWidget::set_vertical_line(qreal center, QString label)
{
    if(_line_series == nullptr)
    {
        _line_series = new QLineSeries();
        _line_series->append(center, 1.0f);
        _line_series->append(center, 100000.0f);
        _line_series->setName(label);
        _chart->addSeries(_line_series);

        _line_series->attachAxis(_axisX);
        _line_series->attachAxis(_currentYAxis);


        emit trigger_connect_markers();
    }
    else
    {
        QList<QPointF> points = _line_series->points();
        for(QPointF& point : points)
        {
            point.setX(center);
        }
        _line_series->replace(points);
        _line_series->setName(label);
    }
}

//---------------------------------------------------------------------------

void SpectraWidget::set_element_lines(data_struct::Fit_Element_Map<double>* element)
{
	//clear old one
	for (QLineSeries* itr : _element_lines)
	{
		itr->detachAxis(_axisX);
        itr->detachAxis(_currentYAxis);
		_chart->removeSeries(itr);
		delete itr;
	}
	_element_lines.clear();

    float line_min = 0.1;
    float line_max = 9999;

    line_min = std::max(0.1, ((QValueAxis*)_currentYAxis)->min());
    line_max = ((QValueAxis*)_currentYAxis)->max();

    if(element != nullptr)
    {
        const std::vector<data_struct::Element_Energy_Ratio<double>>& energy_ratios = element->energy_ratios();
        for(auto& itr : energy_ratios)
        {
            QLineSeries* line = new QLineSeries();
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

//---------------------------------------------------------------------------

void SpectraWidget::remove_spectra(QString name)
{

    QList<QAbstractSeries*> series = _chart->series();
    for(QAbstractSeries* spec : series)
    {
        if(spec->name() == name)
        {
            _chart->removeSeries(spec);
            delete spec;
            break;
        }
    }

}

//---------------------------------------------------------------------------

void SpectraWidget::ShowContextMenu(const QPoint &pos)
{

    _contextMenu->exec(mapToGlobal(pos));

}

//---------------------------------------------------------------------------

void SpectraWidget::set_log10(bool val)
{

    if(_display_log10 != val)
    {
        QList<QAbstractSeries*> series = _chart->series();

        for(QAbstractSeries* ser : _element_lines)
        {
            series.removeOne(ser);
        }

        for(QAbstractSeries* ser : series)
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

        for(QAbstractSeries* ser : series)
        {
            ser->attachAxis(_currentYAxis);
        }

        emit(y_axis_changed(_display_log10));
    }
}

//---------------------------------------------------------------------------

QPixmap SpectraWidget::getPngofChart()
{

    return _chartView->grab();

}

//---------------------------------------------------------------------------
/*
void SpectraWidget::_update_series()
{
    _chart->removeAllSeries();
    for(auto& itr : _spectra_map)
    {
        QLineSeries *series = new QLineSeries();

        series->setName(itr.first);
        for(unsigned int i =0; i < itr.second.size(); i++)
        {
            if(std::isnan(itr.second[i]) || std::isinf(itr.second[i]) || itr.second[i] <= 0.0)
            {
                itr.second[i] = 0.00000001f;
            }
            series->append(i, itr.second[i]);

        }
        _chart->addSeries(series);
        series->attachAxis(_axisX);
        series->attachAxis(_currentYAxis);
    }
    //_chart->createDefaultAxes();
}
*/
//---------------------------------------------------------------------------

void SpectraWidget::connectMarkers()
{
    // Connect all markers to handler
    const auto markers = _chart->legend()->markers();
    for (QLegendMarker *marker : markers)
    {
        // Disconnect possible existing connection to avoid multiple connections
        QObject::disconnect(marker, &QLegendMarker::clicked, this, &SpectraWidget::handleMarkerClicked);
        QObject::connect(marker, &QLegendMarker::clicked, this, &SpectraWidget::handleMarkerClicked);
    }
}

//---------------------------------------------------------------------------

void SpectraWidget::disconnectMarkers()
{
    const auto markers = _chart->legend()->markers();
    for (QLegendMarker *marker : markers)
    {
        QObject::disconnect(marker, &QLegendMarker::clicked, this, &SpectraWidget::handleMarkerClicked);
    }
}

//---------------------------------------------------------------------------

void SpectraWidget::handleMarkerClicked()
{
    QLegendMarker* marker = qobject_cast<QLegendMarker*> (sender());
    Q_ASSERT(marker);

    switch (marker->type())
    {
        case QLegendMarker::LegendMarkerTypeXY:
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

//---------------------------------------------------------------------------

void SpectraWidget::clear_top_axis()
{
    QStringList labels = _top_axis_elements->categoriesLabels();
    for (QString str : labels)
    {
        _top_axis_elements->remove(str);
    }
}

//---------------------------------------------------------------------------

void SpectraWidget::set_top_axis(std::map < float, std::string> elements)
{  
    for (const auto& itr : elements)
    {
        _top_axis_elements->append(QString::fromStdString(itr.second), itr.first);
    } 
}

//---------------------------------------------------------------------------