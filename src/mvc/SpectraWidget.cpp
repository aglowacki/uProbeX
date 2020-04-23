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
#include <math.h>


/*---------------------------------------------------------------------------*/

SpectraWidget::SpectraWidget(QWidget* parent) : QWidget(parent)
{

    _max_log_range = 1.0;
    m_isTouching = false;
    _display_log10 = true;
    _currentYAxis = nullptr;
    _int_spec_max_x = 1;
    _int_spec_max_y = 1;
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

    connect(this, SIGNAL(trigger_connect_markers()),
            this, SLOT(connectMarkers()));

}

/*---------------------------------------------------------------------------*/

SpectraWidget::~SpectraWidget()
{

}

/*---------------------------------------------------------------------------*/

void SpectraWidget::createLayout()
{
    _axisYLog10 = new QtCharts::QLogValueAxis();
    _axisYLog10->setTitleText("Counts Log10");
    _axisYLog10->setLabelFormat("%.1e");
    //_axisYLog10->setRange(1.0, 10000.0);
    _axisYLog10->setBase(10.0);


    _axisX = new QtCharts::QValueAxis();
    _axisX->setTitleText("Energy (keV)");
    _axisX->setLabelFormat("%1.0f");
    //_axisX->setTickCount(series->count());
    //_axisX->setRange(0, 2048);
    _axisX->setTickCount(20);

    _top_axis_elements = new QtCharts::QCategoryAxis();
    //_top_axis_elements->setTickCount(20);

    _axisY = new QtCharts::QValueAxis();
    _axisY->setTitleText("Counts");
    _axisY->setLabelFormat("%i");
    //_axisY->setTickCount(series->count());


    _chart = new QtCharts::QChart();
    _chart->addAxis(_axisX, Qt::AlignBottom);
    //_chart->addAxis(_top_axis_elements, Qt::AlignTop);

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

    _chartView = new QtCharts::QChartView(_chart);
    _chartView->setRubberBand(QtCharts::QChartView::RectangleRubberBand);
    //_chartView->setRenderHint(QPainter::Antialiasing);
   

    _display_eneergy_min = new QLineEdit(QString::number(_axisX->min()));
    _display_eneergy_min->setMinimumWidth(100);
    connect(_display_eneergy_min, SIGNAL(textEdited(const QString &)), this, SLOT(onSpectraDisplayChanged(const QString &)));


    _display_eneergy_max = new QLineEdit(QString::number(_axisX->max()));
    _display_eneergy_max->setMinimumWidth(100);
    connect(_display_eneergy_max, SIGNAL(textEdited(const QString &)), this, SLOT(onSpectraDisplayChanged(const QString &)));


    _display_height_min = new QLineEdit(QString::number(ymin));
    _display_height_min->setMinimumWidth(100);
    connect(_display_height_min, SIGNAL(textEdited(const QString&)), this, SLOT(onSpectraDisplayHeightChanged(const QString&)));

    _display_height_max = new QLineEdit(QString::number(ymax, 'g', 0));
    _display_height_max->setMinimumWidth(100);
    connect(_display_height_max, SIGNAL(textEdited(const QString&)), this, SLOT(onSpectraDisplayHeightChanged(const QString&)));

    _btn_reset_chart_view = new QPushButton("Reset");
    connect(_btn_reset_chart_view, &QPushButton::released, this, &SpectraWidget::onResetChartView);

    QHBoxLayout* spectra_layout = new QHBoxLayout();
    spectra_layout->addWidget(_chartView);

    QHBoxLayout* options_layout = new QHBoxLayout();
    options_layout->addWidget(new QLabel("Display Energy Min:"));
    options_layout->addWidget(_display_eneergy_min);
    options_layout->addWidget(new QLabel("KeV  ,  "));

    options_layout->addWidget(new QLabel("Display Energy Max:"));
    options_layout->addWidget(_display_eneergy_max);
    options_layout->addWidget(new QLabel("KeV"));

    options_layout->addWidget(new QLabel("   |   Display Height Min:"));
    options_layout->addWidget(_display_height_min);
    options_layout->addWidget(new QLabel("Counts  ,  "));

    options_layout->addWidget(new QLabel("Display Height Max:"));
    options_layout->addWidget(_display_height_max);
    options_layout->addWidget(new QLabel("Counts | "));

    options_layout->addWidget(_btn_reset_chart_view);

    options_layout->addItem(new QSpacerItem(9999, 40, QSizePolicy::Maximum));

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->addItem(spectra_layout);
    vlayout->addItem(options_layout);



    setLayout(vlayout);
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::onSpectraDisplayChanged(const QString &)
{
    qreal maxRange = _display_eneergy_max->text().toDouble();
    qreal minRange = _display_eneergy_min->text().toDouble();
    _axisX->setRange(minRange, maxRange);
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::onSpectraDisplayHeightChanged(const QString&)
{
    qreal maxRange = _display_height_max->text().toDouble();
    qreal minRange = _display_height_min->text().toDouble();
    _currentYAxis->setRange(minRange, maxRange);
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::onResetChartView()
{
    _display_eneergy_min->setText(QString::number(0));
    _display_eneergy_max->setText(QString::number(_int_spec_max_x));
    _display_height_min->setText(QString::number(1));
    _display_height_max->setText(QString::number(_int_spec_max_y, 'g', 0));
    _axisX->setRange(0, _int_spec_max_x);
    _currentYAxis->setRange(1, _int_spec_max_y);
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::onUpdateChartLineEdits()
{
    _display_eneergy_min->setText(QString::number(_axisX->min()));
    _display_eneergy_max->setText(QString::number(_axisX->max()));
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
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::append_spectra(QString name, const data_struct::ArrayXr* spectra, const data_struct::ArrayXr *energy)
{
    if (spectra == nullptr)
        return;


    if (spectra->size() == 0)
        return;


    QtCharts::QLineSeries *series = nullptr;

    for( auto& itr : _chart->series())
    {
        if(itr->name() == name)
        {
            series = (QtCharts::QLineSeries*)itr;
			break;
        }
    }

    if (name == "Integrated Spectra")
    {
        _int_spec_max_x = energy->maxCoeff();
        _int_spec_max_y = spectra->maxCoeff();
    }

    if(series == nullptr)
    {
        series = new QtCharts::QLineSeries();

        //float new_max = spectra->maxCoeff();
        //_max_log_range = std::max(_max_log_range, new_max);

        series->setName(name);
		if (energy->size() >= spectra->size())
		{
			for (unsigned int i = 0; i < spectra->size(); i++)
			{

				float val = (*spectra)[i];
				bool isFine = std::isfinite(val);
				if (false == isFine || val <= 0.0f)
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
        _display_eneergy_min->setText(QString::number(_axisX->min()));
        _display_eneergy_max->setText(QString::number(_axisX->max()));

        series->attachAxis(_currentYAxis);
        //series->attachAxis(_top_axis_elements);
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
				if (false == isFine || val <= 0.0f)
				{
					val = 1.0;
				}
				itr->setX((*energy)[i]);
				itr->setY(val);
				itr++;
			}
		}
        series->replace(points);
    }

    emit trigger_connect_markers();
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::set_vertical_line(qreal center, QString label)
{
    if(_line_series == nullptr)
    {
        _line_series = new QtCharts::QLineSeries();
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

/*---------------------------------------------------------------------------*/

void SpectraWidget::set_element_lines(data_struct::Fit_Element_Map * element)
{
	//clear old one
	for (QtCharts::QLineSeries* itr : _element_lines)
	{
		itr->detachAxis(_axisX);
        itr->detachAxis(_currentYAxis);
		_chart->removeSeries(itr);
		delete itr;
	}
	_element_lines.clear();

    float line_min = 0;
    float line_max = 9999;
    //QAbstractAxis doesn't support max and min so we have to cast
    if(_display_log10)
    {
        line_min = std::max(0.0, ((QtCharts::QLogValueAxis*)_currentYAxis)->min());
        line_max = ((QtCharts::QLogValueAxis*)_currentYAxis)->max();
    }
    else
    {
        line_min = std::max(0.0, ((QtCharts::QValueAxis*)_currentYAxis)->min());
        line_max = ((QtCharts::QValueAxis*)_currentYAxis)->max();
    }




    if(element != nullptr)
    {
        const std::vector<data_struct::Element_Energy_Ratio>& energy_ratios = element->energy_ratios();

        for(auto& itr : energy_ratios)
        {
            QtCharts::QLineSeries* line = new QtCharts::QLineSeries();
            line->append(itr.energy, line_min);
            float line_ratio = data_struct::Element_Param_Percent_Map.at(itr.ptype);
            line->append(itr.energy, (line_max * line_ratio));
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

/*---------------------------------------------------------------------------*/

void SpectraWidget::remove_spectra(QString name)
{

    QList<QtCharts::QAbstractSeries*> series = _chart->series();
    for(QtCharts::QAbstractSeries* spec : series)
    {
        if(spec->name() == name)
        {
            _chart->removeSeries(spec);
            delete spec;
            break;
        }
    }

}

/*---------------------------------------------------------------------------*/

void SpectraWidget::ShowContextMenu(const QPoint &pos)
{
    _contextMenu->exec(mapToGlobal(pos));
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::_check_log10()
{

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

    if(_display_log10) //if current one is log10, set to normal
    {
        _currentYAxis = _axisY;
    }
    else
    {
        _currentYAxis = _axisYLog10;
    }

    _chart->addAxis(_currentYAxis, Qt::AlignLeft);

    for(QtCharts::QAbstractSeries* ser : series)
    {
        ser->attachAxis(_currentYAxis);
    }


    _display_log10 = !_display_log10;
    _action_check_log10->setChecked(_display_log10);

    emit(y_axis_changed(_display_log10));

}

/*---------------------------------------------------------------------------*/
/*
void SpectraWidget::_update_series()
{
    _chart->removeAllSeries();
    for(auto& itr : _spectra_map)
    {
        QtCharts::QLineSeries *series = new QtCharts::QLineSeries();

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
/*---------------------------------------------------------------------------*/
/*
bool SpectraWidget::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::TouchBegin) {
        // By default touch events are converted to mouse events. So
        // after this event we will get a mouse event also but we want
        // to handle touch events as gestures only. So we need this safeguard
        // to block mouse events that are actually generated from touch.
        m_isTouching = true;

        // Turn off animations when handling gestures they
        // will only slow us down.
        _chart->setAnimationOptions(QtCharts::QChart::NoAnimation);
    }
    return QWidget::viewportEvent(event);
}
*/
/*---------------------------------------------------------------------------*/

void SpectraWidget::mousePressEvent(QMouseEvent *event)
{
    if (m_isTouching)
        return;
    QWidget::mousePressEvent(event);
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isTouching)
        return;
    QWidget::mouseMoveEvent(event);
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_isTouching)
        m_isTouching = false;

    // Because we disabled animations when touch event was detected
    // we must put them back on.
    _chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);

    onUpdateChartLineEdits();

    QWidget::mouseReleaseEvent(event);
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        _chart->zoomIn();
        break;
    case Qt::Key_Minus:
        _chart->zoomOut();
        break;
    case Qt::Key_Left:
        _chart->scroll(-10, 0);
        break;
    case Qt::Key_Right:
        _chart->scroll(10, 0);
        break;
    case Qt::Key_Up:
        _chart->scroll(0, 10);
        break;
    case Qt::Key_Down:
        _chart->scroll(0, -10);
        break;
    case Qt::Key_R:
        //_chart->resetMatrix();
        //_chart->resetTransform();
        //_chart->zoomReset();
        onResetChartView();
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::connectMarkers()
{
    // Connect all markers to handler
    const auto markers = _chart->legend()->markers();
    for (QtCharts::QLegendMarker *marker : markers)
    {
        // Disconnect possible existing connection to avoid multiple connections
        QObject::disconnect(marker, &QtCharts::QLegendMarker::clicked, this, &SpectraWidget::handleMarkerClicked);
        QObject::connect(marker, &QtCharts::QLegendMarker::clicked, this, &SpectraWidget::handleMarkerClicked);
    }
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::disconnectMarkers()
{
    const auto markers = _chart->legend()->markers();
    for (QtCharts::QLegendMarker *marker : markers)
    {
        QObject::disconnect(marker, &QtCharts::QLegendMarker::clicked, this, &SpectraWidget::handleMarkerClicked);
    }
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::handleMarkerClicked()
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
        //qDebug() << "Unknown marker type";
        break;
        }
    }
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::set_top_axis(std::map<std::string, float> elements)
{  
    for (const auto& itr : elements)
    {
        _top_axis_elements->append(QString::fromStdString(itr.first), itr.second);
    } 
}