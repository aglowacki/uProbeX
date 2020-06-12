/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SPECTRA_WIDGET_H
#define SPECTRA_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <QtCharts/QChartView>
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

    void append_spectra(QString name, const data_struct::ArrayXr* spectra, const data_struct::ArrayXr *energy=nullptr);

    //void remove_spectra(QString name);

    void setXLabel(QString val) { _axisX->setTitleText(val); }

    void clearAllSpectra();

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

    void set_element_lines(data_struct::Fit_Element_Map * element);

    void set_top_axis(std::map<std::string, float> elements);

    void onResetChartView();

protected:

    /**
    * @brief Create layout
    */
    void createLayout();

    //bool viewportEvent(QEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);

    bool _display_log10;

    QtCharts::QChart *_chart;

    QtCharts::QChartView *_chartView;

    QtCharts::QLineSeries *_line_series;

private slots:

    void _check_log10();

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

    //std::map<QString, data_struct::Spectra> _spectra_map;

    QAction *_action_check_log10;

    QMenu *_contextMenu;

    std::vector<QtCharts::QLineSeries*> _element_lines;

    QtCharts::QCategoryAxis* _top_axis_elements;

    float _int_spec_max_x;

    float _int_spec_max_y;
};


/*---------------------------------------------------------------------------*/

#endif /* SpectraWidget_H_ */

/*---------------------------------------------------------------------------*/

