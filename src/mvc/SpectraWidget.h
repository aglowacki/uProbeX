/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SPECTRA_WIDGET_H
#define SPECTRA_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QAction>
#include <QMenu>
#include <QWidget>
#include <QLogValueAxis>
#include <QValueAxis>
#include "data_struct/xrf/spectra.h"

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
    SpectraWidget(QWidget* parent = NULL);

    /**
    * Destructor.
    */
    ~SpectraWidget();

    void append_spectra(QString name, data_struct::xrf::Spectra* spectra);

    //void remove_spectra(QString name);

signals:

    void trigger_connect_markers();

public slots:

    void ShowContextMenu(const QPoint &);

    void remove_spectra(QString name);

    void connectMarkers();

    void disconnectMarkers();

    void handleMarkerClicked();

    void set_vertical_line(qreal x_center, QString label="Element");

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

    //void _update_series();

private:

    float _max_log_range;

    QtCharts::QLogValueAxis *_axisYLog10;

    QtCharts::QValueAxis * _axisX;

    QtCharts::QValueAxis * _axisY;

    bool m_isTouching;

    std::map<QString, data_struct::xrf::Spectra> _spectra_map;

    QAction *_action_check_log10;

    QMenu *_contextMenu;
};


/*---------------------------------------------------------------------------*/

#endif /* SpectraWidget_H_ */

/*---------------------------------------------------------------------------*/

