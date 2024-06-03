/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SCATTER_PLOT_VIEW_H
#define SCATTER_PLOT_VIEW_H

/*---------------------------------------------------------------------------*/

#include <QtCharts/QScatterSeries>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QChartView>
#include <QHBoxLayout>
#include <QWidget>
#include <QLogValueAxis>
#include <QValueAxis>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include "mvc/MapsH5Model.h"
#include "mvc/ScanCorrCoefDialog.h"


//---------------------------------------------------------------------------

class ScatterPlotView : public QWidget
{

    Q_OBJECT

public:

    ScatterPlotView(bool display_log10, bool black_background, QWidget* parent = nullptr);

    ~ScatterPlotView();

    void setModel(MapsH5Model* model);

    void setAnalysisType(QString curAnalysis);

    void setQuantType(QString curAnalysis);

    void updateMarkerSize(qreal val);

    void setLog10(int val);

    void setBlackBackground(int  val);

    void setShape(QScatterSeries::MarkerShape shape);

    void setGridLinesVisible(int val);

    void exportPngCsv();

    MapsH5Model* getModel() { return _model; }

    QString getAnalysisType() { return _curAnalysis; }

    void setXYAxis(QString roi_name, QString x_axis_name, QString y_axis_name);

public slots:

    void onNameChange(QString);

private:
    bool _getXY_Maps(data_struct::ArrayXXr<float>& x_map, data_struct::ArrayXXr<float>& y_map);

    void _updateNames();

    void _updatePlot();

    void _exportScatterPlotCSV(QString filePath);

    QLogValueAxis* _axisXLog10;

    QLogValueAxis* _axisYLog10;

    QValueAxis* _axisX;

    QValueAxis* _axisY;

    QLabel* _lb_roi;

    QComboBox* _cb_roi;

    QComboBox* _cb_x_axis_element;

    QComboBox* _cb_y_axis_element;

    QLabel* _lb_corr_coef;

    QChart* _chart;

    QChartView* _chartView;

    QScatterSeries* _scatter_series;

    QString _curAnalysis;

    QString _curQuant;
    
    bool _display_log10;

    MapsH5Model* _model;
};

//---------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/

#endif /* ScatterPlotView_H_ */

/*---------------------------------------------------------------------------*/

