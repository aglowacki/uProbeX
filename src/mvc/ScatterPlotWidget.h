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
#include <QWidget>
#include <QLogValueAxis>
#include <QValueAxis>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include "mvc/MapsH5Model.h"
#include "mvc/ScatterPlotView.h"
#include "mvc/ScanCorrCoefDialog.h"

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

    void setQuantType(QString name);

signals:
    void updateProgressBar(int, int);

public slots:

    void set_log10(int val);
    
    void setBlackBackground(int val);

    void updateMarkerSize(int val);

    void onShapeChange(QString val);

    void setGridLinesVisible(int val);

    void onAdd();

    void onDel();

    void onSavePng();

    void onScan();

protected:

    void _createLayout();

private:

    QHBoxLayout* _subPlotLayout;

    QSpinBox* _sp_maker_size;

    QCheckBox* _ck_display_log10;

    QCheckBox* _ck_display_grid_lines;

    QCheckBox* _ck_black_background;

    //QComboBox* _cb_shape;

    QPushButton* _btn_add;

    QPushButton* _btn_del;

    QPushButton* _btn_save_png;

    QPushButton* _btn_scan_corr_coef;

    QHBoxLayout* _options_layout;

    QVBoxLayout* _mainlayout;

    std::vector< ScatterPlotView*> _plot_view_list;

    ScanCorrCoefDialog _scan_corr_coef_dialog;

};


/*---------------------------------------------------------------------------*/

#endif /* ScatterPlotWidget_H_ */

/*---------------------------------------------------------------------------*/

