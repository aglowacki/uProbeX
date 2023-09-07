/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "mvc/ChartView.h"


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ChartView::ChartView(QChart* chart, QWidget *parent) : QChartView(chart, parent)
{
    setRubberBand(QChartView::HorizontalRubberBand);
    setMouseTracking(true);
    setInteractive(true);
    //setRenderHint(QPainter::Antialiasing);
}

/*---------------------------------------------------------------------------*/

ChartView::~ChartView()
{

}

/*---------------------------------------------------------------------------*/

void ChartView::mousePressEvent(QMouseEvent* event)
{
    
    QChartView::mousePressEvent(event);
}

/*---------------------------------------------------------------------------*/

void ChartView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        return;
    }

    emit(view_zoomed());

    QChartView::mouseReleaseEvent(event);
}

/*---------------------------------------------------------------------------*/

void ChartView::keyPressEvent(QKeyEvent* event)
{
   
    switch (event->key()) {
    case Qt::Key_Left:
        chart()->scroll(-10, 0);
        break;
    case Qt::Key_Right:
        chart()->scroll(10, 0);
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}

/*---------------------------------------------------------------------------*/