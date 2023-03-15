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
#include <QHBoxLayout>
//#include <QAction>
//#include <QMenu>
#include <QWidget>
#include <QLogValueAxis>
#include <QValueAxis>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
//#include <QPushButton>
#include <mvc/MapsH5Model.h>


//---------------------------------------------------------------------------

class ScatterPlotView : public QWidget
{

    Q_OBJECT

public:

    ScatterPlotView(bool display_log10, QWidget* parent = nullptr);

    ~ScatterPlotView();

    void setModel(MapsH5Model* model);

    void setAnalysisType(QString curAnalysis);

    void updateMarkerSize(qreal val);

    void setLog10(int val);

public slots:

    void onNameChange(QString);

private:

    void _updateNames();

    void _updatePlot();

    QtCharts::QLogValueAxis* _axisXLog10;

    QtCharts::QLogValueAxis* _axisYLog10;

    QtCharts::QValueAxis* _axisX;

    QtCharts::QValueAxis* _axisY;

    QComboBox* _cb_x_axis_element;

    QComboBox* _cb_y_axis_element;

    QtCharts::QChart* _chart;

    QtCharts::QChartView* _chartView;

    QtCharts::QScatterSeries* _scatter_series;

    QString _curAnalysis;

    MapsH5Model* _model;
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

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

    void setModel(MapsH5Model* model);

    void setAnalysisType(QString name);

public slots:

    void set_log10(bool val);
    
    void updateMarkerSize(int val);

protected:

    void _createLayout();

private:

    QHBoxLayout* _subPlotLayout;

    QSpinBox* _sp_maker_size;

    QCheckBox* _ck_display_log10;

    std::vector< ScatterPlotView*> _plot_view_list;

};


/*---------------------------------------------------------------------------*/

#endif /* ScatterPlotWidget_H_ */

/*---------------------------------------------------------------------------*/

