/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/SpectraWidget.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtCharts/QLegendMarker>

#include <QDebug>

#include <math.h>


/*---------------------------------------------------------------------------*/

SpectraWidget::SpectraWidget(QWidget* parent) : QWidget(parent)
{

    _max_log_range = 1.0;
    m_isTouching = false;
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
    _axisX->setLabelFormat("%.3f");
    //_axisX->setTickCount(series->count());
    //_axisX->setRange(0, 2048);
    _axisX->setTickCount(20);

    _axisY = new QtCharts::QValueAxis();
    _axisY->setTitleText("Counts");
    _axisY->setLabelFormat("%i");
    //_axisY->setTickCount(series->count());


    _chart = new QtCharts::QChart();
    _chart->addAxis(_axisX, Qt::AlignBottom);

    if(_display_log10)
    {
        _chart->addAxis(_axisYLog10, Qt::AlignLeft);
    }
    else
    {
        _chart->addAxis(_axisY, Qt::AlignLeft);
    }

    _line_series = nullptr;

    _chartView = new QtCharts::QChartView(_chart);
    _chartView->setRubberBand(QtCharts::QChartView::RectangleRubberBand);
    //_chartView->setRenderHint(QPainter::Antialiasing);

    QLayout* layout = new QHBoxLayout();
    layout->addWidget(_chartView);
    setLayout(layout);
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::append_spectra(QString name, data_struct::Spectra* spectra, data_struct::Spectra *energy)
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
        }
    }

    if(series == nullptr)
    {
        series = new QtCharts::QLineSeries();

        //float new_max = spectra->maxCoeff();
        //_max_log_range = std::max(_max_log_range, new_max);

        series->setName(name);
        for(unsigned int i =0; i < spectra->size(); i++)
        {

            float val = (*spectra)[i];
            bool isFine = std::isfinite(val);
            if (false == isFine || val <= 0.0f)
            {
                val = 1.0;
            }
            if(energy !=nullptr)
            {
                series->append((*energy)[i], val);
            }
            else
            {
                series->append(i, val);
            }
        }
        _chart->addSeries(series);
        series->attachAxis(_axisX);
        //_axisYLog10->setRange(1.0, _max_log_range);
        //_axisY->setRange(0.0, new_max);

        if(_display_log10)
            series->attachAxis(_axisYLog10);
        else
            series->attachAxis(_axisY);
    }
    else
    {
        QList<QPointF> points = series->points();
        auto itr = points.begin();
        for(unsigned int i =0; i < spectra->size(); i++)
        {
            float val = (*spectra)[i];
            bool isFine = std::isfinite(val);
            if (false == isFine || val <= 0.0f)
            {
                val = 1.0;
            }
            itr->setY(val);
            itr++;
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

        //if(_display_log10)
        //    _line_series->attachAxis(_axisYLog10);
        //else
            _line_series->attachAxis(_axisY);

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
    if(element != nullptr)
    {
        for(QtCharts::QLineSeries* itr : _element_lines)
        {
           itr->detachAxis(_axisX);
           itr->detachAxis(_axisY);
           _chart->removeSeries(itr);
           delete itr;
        }
        _element_lines.clear();

        const std::vector<data_struct::Element_Energy_Ratio>& energy_ratios = element->energy_ratios();

        int ka_cnt = 0;
        int kb_cnt = 0;
        int l_cnt = 0;
        int m_cnt = 0;

        for(auto& itr : energy_ratios)
        {
            QtCharts::QLineSeries* line = new QtCharts::QLineSeries();
            line->append(itr.energy, 1.0f);
            line->append(itr.energy, 10000.0f);
            QString eName = QString(element->full_name().c_str());
            switch(itr.ptype)
            {
                case data_struct::Element_Param_Type::Ka_Line:
                ka_cnt ++;
                eName += " Ka"+QString::number(ka_cnt);
                break;

                case data_struct::Element_Param_Type::Kb_Line:
                kb_cnt ++;
                eName += " Kb"+QString::number(kb_cnt);
                break;

                case data_struct::Element_Param_Type::L_Line:
                l_cnt ++;
                eName += " L"+QString::number(l_cnt);
                break;

                case data_struct::Element_Param_Type::M_Line:
                m_cnt ++;
                eName += " L"+QString::number(m_cnt);
                break;
            }
            line->setName(eName);
            _chart->addSeries(line);
            line->attachAxis(_axisX);
            line->attachAxis(_axisY);
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

    QtCharts::QAbstractAxis* remove_axis;
    QtCharts::QAbstractAxis* add_axis;

    if(_display_log10)
    {
        remove_axis = _axisYLog10;
        add_axis = _axisY;
    }
    else
    {
        remove_axis = _axisY;
        add_axis = _axisYLog10;
    }

    for(QtCharts::QAbstractSeries* ser : series)
    {
        ser->detachAxis(remove_axis);
    }
    _chart->removeAxis(remove_axis);
    _chart->addAxis(add_axis, Qt::AlignLeft);

    for(QtCharts::QAbstractSeries* ser : series)
    {
        ser->attachAxis(add_axis);
    }


    _display_log10 = !_display_log10;
    _action_check_log10->setChecked(_display_log10);

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
        series->attachAxis(_axisYLog10);
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
        _chart->zoomReset();

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
        qDebug() << "Unknown marker type";
        break;
        }
    }
}
