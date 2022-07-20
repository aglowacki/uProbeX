/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SPECTRA_WIDGET_H
#define SPECTRA_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <QtCharts/QLineSeries>
#include <QtCharts/QCategoryAxis>
#include <QAction>
#include <QMenu>
#include <QWidget>
#include <QLogValueAxis>
#include <QValueAxis>
#include <QLineEdit>
#include <QPushButton>
#include "data_struct/spectra.h"
#include "data_struct/fit_element_map.h"
#include "mvc/ChartView.h"

#define DEF_STR_FIT_INT_SPECTRA "Fit Spectra"
#define DEF_STR_NEW_FIT_INT_SPECTRA "New Fit Spectra"
#define DEF_STR_INT_SPECTRA "Integrated Spectra"
#define DEF_STR_BACK_SPECTRA "Background"
#define DEF_STR_MODEL_SPECTRA "Model Spectra"

#define INT_SPEC_R 32
#define INT_SPEC_G 159
#define INT_SPEC_B 212

#define BAK_SPEC_R 153
#define BAK_SPEC_G 202
#define BAK_SPEC_B 83

#define MOD_SPEC_R 246
#define MOD_SPEC_G 166
#define MOD_SPEC_B 37

#define FIT_SPEC_R 246
#define FIT_SPEC_G 37
#define FIT_SPEC_B 37


/*---------------------------------------------------------------------------*/

/**
 * @brief When open the acquisition window, the widget is showing for capturing
 * the image from the area detector writer, the window will also be updated to
 * show the image.
 */
class SpectraWidget : public QWidget
{

    Q_OBJECT

public:

    /**
    * Constructor.
    */
    SpectraWidget(QWidget* parent = nullptr);

    /**
    * Destructor.
    */
    ~SpectraWidget();

    void append_spectra(QString name, const data_struct::ArrayTr<double>* spectra, const data_struct::ArrayTr<double>*energy=nullptr);

    void setXLabel(QString val) { _axisX->setTitleText(val); }

    void clearAllSpectra();

    QString getDisplayEnergyMin() { return _display_eneergy_min->text(); }

    QString getDisplayEnergyMax() { return _display_eneergy_max->text(); }

    QString getDisplayHeightMin() { return _display_height_min->text(); }
    
    QString getDisplayHeightMax() { return _display_height_max->text(); }

    void setDisplayRange(QString wmin, QString wmax, QString hmin, QString hmax);

    QPixmap getPngofChart();

signals:

    void trigger_connect_markers();

    void y_axis_changed(bool is_log10);

public slots:

    void ShowContextMenu(const QPoint &);

    void remove_spectra(QString name);

    void connectMarkers();

    void disconnectMarkers();

    void handleMarkerClicked();

    void set_vertical_line(qreal x_center, QString label="Element");

    void set_element_lines(data_struct::Fit_Element_Map<double>* element);

    void clear_top_axis();

    void set_top_axis(std::map < float, std::string> elements);

    void onResetChartView();

    void onResetChartViewOnlyY();

    void set_log10(bool val);

protected:

    /**
    * @brief Create layout
    */
    void createLayout();

    bool _display_log10;

    QtCharts::QChart *_chart;

    ChartView *_chartView;

    QtCharts::QLineSeries *_line_series;

private slots:

    void onSpectraDisplayChanged(const QString &);

    void onSpectraDisplayHeightChanged(const QString&);

    void onUpdateChartLineEdits();

    //void _update_series();

private:

    QLineEdit *_display_eneergy_max;

    QLineEdit *_display_eneergy_min;

    QLineEdit* _display_height_max;

    QLineEdit* _display_height_min;

    QPushButton* _btn_reset_chart_view;

    float _max_log_range;

    QtCharts::QLogValueAxis *_axisYLog10;

    QtCharts::QValueAxis * _axisX;

    QtCharts::QValueAxis * _axisY;

    QtCharts::QAbstractAxis * _currentYAxis;

    bool m_isTouching;

    QMenu *_contextMenu;

    std::vector<QtCharts::QLineSeries*> _element_lines;

    QtCharts::QCategoryAxis* _top_axis_elements;

    float _int_spec_max_x;

    float _int_spec_max_y;
};


/*---------------------------------------------------------------------------*/

#endif /* SpectraWidget_H_ */

/*---------------------------------------------------------------------------*/

