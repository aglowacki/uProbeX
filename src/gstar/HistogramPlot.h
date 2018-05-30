/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_HISTOGRAM_PLOT_H
#define GSTAR_HISTOGRAM_PLOT_H

/*---------------------------------------------------------------------------*/

#include <QtGui>

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

public slots:
   /**
    * Update points
    */
   void updatePoints(QList<int> pts);

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
   /**
    * Widget
    */
   QWidget* m_widget;

   /**
    * Values
    */
   QList<int> m_values;

   /**
    * Max
    */
   int m_max;

};

}

#endif
