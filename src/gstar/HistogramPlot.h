/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_HISTOGRAM_PLOT_H
#define GSTAR_HISTOGRAM_PLOT_H

/*---------------------------------------------------------------------------*/

#include <QtGui>
#include "data_struct/spectra.h"

/*---------------------------------------------------------------------------*/

namespace gstar
{

class HistogramPlot
: public QObject
{

   Q_OBJECT

public:
   /**
    * Constructor
    */
   HistogramPlot(QWidget* parent);

   /**
    * Destructor
    */
   ~HistogramPlot();

   /**
    * Event filter
    */
   bool eventFilter(QObject* object, QEvent* event);

   void set_min_max_lines(qreal minCoef, qreal maxCoef);

public slots:
   /**
    * Update points
    */
   void updatePoints(const data_struct::ArrayTr<float>& pts);

private:
   /**
    * Draw histogram
    */
   void drawHistogram();

   /**
    * Ymap
    */
   qreal ymap(const qreal& value);

private:

    QWidget* m_widget;

   std::vector<int> _bins;

   int _n_bins;

   int  _max_bin_val;

   qreal _min_line;

   qreal _max_line;

   QPen _default_pen;

   QPen _min_pen;

   QPen _max_pen;

   float _minCoef;

   float _maxCoef;

   float _bin_width;
};

}

#endif
