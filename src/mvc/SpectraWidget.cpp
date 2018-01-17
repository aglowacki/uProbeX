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
    _axisYLog10->setRange(1.0, 10000.0);
    _axisYLog10->setBase(10.0);


    _axisX = new QtCharts::QValueAxis();
    _axisX->setTitleText("Energy");
    _axisX->setLabelFormat("%i");
    //_axisX->setTickCount(series->count());
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


    _line_series.append(-1.0, 0.001);
    _line_series.append(-1.0, 100000.0);
    _chart->addSeries(&_line_series);

    _chartView = new QtCharts::QChartView(_chart);
    _chartView->setRubberBand(QtCharts::QChartView::RectangleRubberBand);
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


    if (spectra->size() == 0)
        return;

    QtCharts::QLineSeries *series = new QtCharts::QLineSeries();

    float new_max = spectra->maxCoeff();
    _max_log_range = std::max(_max_log_range, new_max);

    series->setName(name);
    for(unsigned int i =0; i < spectra->size(); i++)
    {
        float val = (*spectra)[i];
        if(std::isnan(val) || std::isinf(val) || val <= 0.0)
        {
            val = 0.0001f;
        }
        series->append(i, val);

    }
    _chart->addSeries(series);
    series->attachAxis(_axisX);
    _axisYLog10->setRange(1.0, _max_log_range);

    if(_display_log10)
        series->attachAxis(_axisYLog10);
    else
        series->attachAxis(_axisY);


    emit trigger_connect_markers();
}

/*---------------------------------------------------------------------------*/

void SpectraWidget::set_vertical_line(int center, QString label)
{
    _chart->removeSeries(&_line_series);
    _line_series.detachAxis(_axisYLog10);
    _line_series.detachAxis(_axisY);

    _line_series.clear();
    _line_series.append(center, 0.001);
    _line_series.append(center, 100000.0);
    _line_series.setName(label);

    _chart->addSeries(&_line_series);

    if(_display_log10)
        _line_series.attachAxis(_axisYLog10);
    else
        _line_series.attachAxis(_axisY);

    emit trigger_connect_markers();
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

    if(_display_log10)
    {
        _chart->removeAxis(_axisYLog10);
        _chart->addAxis(_axisY, Qt::AlignLeft);
        QList<QtCharts::QAbstractSeries*> series = _chart->series();
        for(QtCharts::QAbstractSeries* ser : series)
        {
            ser->detachAxis(_axisYLog10);
            ser->attachAxis(_axisY);
        }
    }
    else
    {
        _chart->removeAxis(_axisY);
        _chart->addAxis(_axisYLog10, Qt::AlignLeft);
        QList<QtCharts::QAbstractSeries*> series = _chart->series();
        for(QtCharts::QAbstractSeries* ser : series)
        {
            ser->detachAxis(_axisY);
            ser->attachAxis(_axisYLog10);
        }

    }


    /*
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
*/
    _display_log10 = !_display_log10;
    _action_check_log10->setChecked(_display_log10);
 //   _update_series();

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
        _chart->resetMatrix();
        _chart->resetTransform();
        //_chart->zoomReset();

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
