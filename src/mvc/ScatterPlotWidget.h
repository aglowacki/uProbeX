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

    ScatterPlotView(bool display_log10, bool black_background, QWidget* parent = nullptr);

    ~ScatterPlotView();

    void setModel(MapsH5Model* model);

    void setAnalysisType(QString curAnalysis);

    void updateMarkerSize(qreal val);

    void setLog10(int val);

    void setBlackBackground(int  val);

    void setShape(QtCharts::QScatterSeries::MarkerShape shape);

    void setGridLinesVisible(int val);

    //QPixmap getPngofChart();

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

    bool _display_log10;

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

    void set_log10(int val);
    
    void setBlackBackground(int val);

    void updateMarkerSize(int val);

    void onShapeChange(QString val);

    void setGridLinesVisible(int val);

protected:

    void _createLayout();

private:

    QHBoxLayout* _subPlotLayout;

    QSpinBox* _sp_maker_size;

    QCheckBox* _ck_display_log10;

    QCheckBox* _ck_display_grid_lines;

    QCheckBox* _ck_black_background;

    QComboBox* _cb_shape;

    std::vector< ScatterPlotView*> _plot_view_list;

};


/*---------------------------------------------------------------------------*/

#endif /* ScatterPlotWidget_H_ */

/*---------------------------------------------------------------------------*/

