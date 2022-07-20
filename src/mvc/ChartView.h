/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef CHART_VIEW_H
#define CHART_VIEW_H

/*---------------------------------------------------------------------------*/

#include <QtCharts/QChartView>

/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/

class ChartView : public QtCharts::QChartView
{

   Q_OBJECT

public:

    /**
     * Constructor.
     */
    ChartView(QtCharts::QChart* chart, QWidget *parent = nullptr);

    /**
     * Destructor.
     */
    ~ChartView();

signals:

    void view_zoomed();

protected:

    void mousePressEvent(QMouseEvent* event);

    //void mouseMoveEvent(QMouseEvent* event);

    void mouseReleaseEvent(QMouseEvent* event);

    void keyPressEvent(QKeyEvent* event);
};


/*---------------------------------------------------------------------------*/

#endif 

/*---------------------------------------------------------------------------*/
