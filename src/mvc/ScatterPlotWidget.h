/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SCATTER_PLOT_WIDGET_H
#define SCATTER_PLOT_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <QtCharts/QScatterSeries>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QChartView>
//#include <QAction>
//#include <QMenu>
#include <QWidget>
#include <QLogValueAxis>
#include <QValueAxis>
#include <QComboBox>
//#include <QPushButton>
#include "data_struct/fit_element_map.h"


/*---------------------------------------------------------------------------*/

/**
 * @brief When open the acquisition window, the widget is showing for capturing
 * the image from the area detector writer, the window will also be updated to
 * show the image.
 */
class ScatterPlotWidget : public QWidget
{

    Q_OBJECT

public:

    /**
    * Constructor.
    */
    ScatterPlotWidget(QWidget* parent = nullptr);

    /**
    * Destructor.
    */
    ~ScatterPlotWidget();

signals:

    //void y_axis_changed(bool is_log10);

public slots:

    //void ShowContextMenu(const QPoint &);

    void set_log10(bool val);

protected:

    /**
    * @brief Create layout
    */
    void createLayout();

    bool _display_log10;

    QtCharts::QChart *_chart;

    QtCharts::QScatterSeries *_scatter_series;

private slots:

    //void onSpectraDisplayChanged(const QString &);

    //void onSpectraDisplayHeightChanged(const QString&);

    //void onUpdateChartLineEdits();

    //void _update_series();

private:

    QComboBox *_x_axis_element;

    QComboBox *_y_axis_element;

    QtCharts::QLogValueAxis *_axisYLog10;

    QtCharts::QValueAxis * _axisX;

    QtCharts::QValueAxis * _axisY;

    QtCharts::QAbstractAxis * _currentYAxis;

};


/*---------------------------------------------------------------------------*/

#endif /* ScatterPlotWidget_H_ */

/*---------------------------------------------------------------------------*/

