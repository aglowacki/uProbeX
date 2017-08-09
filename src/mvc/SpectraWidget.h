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

   void set_spectra(data_struct::xrf::Spectra* spectra);

public slots:
   void ShowContextMenu(const QPoint &);

protected:

   /**
    * @brief Create layout
    */
   void createLayout();

   bool _display_log10;

   QtCharts::QLineSeries *_series;

   QtCharts::QChart *_chart;

   QtCharts::QChartView *_chartView;


private slots:

   void _check_log10();

   void _update_series();

private:
   std::valarray<float> _spectra;

   QAction *_action_check_log10;

   QMenu *_contextMenu;
};


/*---------------------------------------------------------------------------*/

#endif /* SpectraWidget_H_ */

/*---------------------------------------------------------------------------*/
