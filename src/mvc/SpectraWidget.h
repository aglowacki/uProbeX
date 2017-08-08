/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SPECTRA_WIDGET_H
#define SPECTRA_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

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

   /**
    * @brief Accept the window change state
    * @param oldState
    * @param newState
    */
   void windowChanged(Qt::WindowStates oldState, Qt::WindowStates newState);

protected:

   /**
    * @brief Create layout
    */
   void createLayout();

   QtCharts::QChart *_chart;

   QtCharts::QChartView *_chartView;
};


/*---------------------------------------------------------------------------*/

#endif /* SpectraWidget_H_ */

/*---------------------------------------------------------------------------*/
