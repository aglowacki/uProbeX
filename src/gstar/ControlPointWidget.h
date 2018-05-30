/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_CONTROL_POINT_WIDGET_H
#define GSTAR_CONTROL_POINT_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <QHBoxLayout>
#include <QIntValidator>
#include <QLineEdit>
#include <QPolygonF>
#include <QVBoxLayout>
#include <QWidget>

#include "gstar/ControlPointPicker.h"
#include "gstar/ControlPointSlider.h"

/*---------------------------------------------------------------------------*/

namespace gstar
{

class HistogramPlot;

class ControlPointWidget
: public QWidget
{

   Q_OBJECT

public:
   /**
    * Constructor
    */
   ControlPointWidget(QWidget* widget);

   /**
    * Get points
    */
   QPolygonF getPoints() const;

   /**
    * Get total range
    */
   void getTotalRange(double* min, double* max);

   /**
    * Set the picker range
    */
   void setPickerRange(double min, double max);

public slots:
   /**
    * Set color
    */
   void setColor(int color);

   /**
    * Set points
    */
   void setPoints(const QPolygonF& points, bool bRestretch);

   /**
    * Set sub range
    */
   void setSubRange(double min, double max);

   /**
    * Set total range
    */
   void setTotalRange(double min, double max);

   /**
    * Update histogram
    */
   void updateHistogram(QList<int> points);

signals:

   /**
    * Change controled points
    */
   void controlPointsChanged(const QPolygonF& points);

   /**
    * Change sub range
    */
   void subRangeChanged(double min, double max);

private slots:

   /**
    * Change points
    */
   void pointsChanged(const QPolygonF& points);

   /**
    * Update sub range
    */
   void subRangeUpdated(double min, double max);

   /**
    * Change text max sub range
    */
   void textMaxSubRangeChanged();

   /**
    * Change text min sub range
    */
   void textMinSubRangeChanged();

private:
   /**
    * Control point picker
    */
   ControlPointPicker* m_ctrlPointPicker;

   /**
    * Control point slider
    */
   ControlPointSlider* m_ctrlPointSlider;

   /**
    * Min edit
    */
   QLineEdit* m_editMin;

   /**
    * Max edit
    */
   QLineEdit* m_editMax;

   /**
    * Min validator
    */
   QIntValidator* m_validatorMin;

   /**
    * Max validator
    */
   QIntValidator* m_validatorMax;

   /**
    * Histogram
    */
   HistogramPlot* m_histogram;

};

}

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
