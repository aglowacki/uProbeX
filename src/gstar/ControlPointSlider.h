/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_CONTROL_POINT_SLIDER_H
#define GSTAR_CONTROL_POINT_SLIDER_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QPainter>

#include "gstar/ControlPoints.h"

/*---------------------------------------------------------------------------*/

namespace gstar
{

class ControlPointSlider
: public QWidget
{

   Q_OBJECT

public:
   /**
    * Constructor
    */
   ControlPointSlider();

   /**
    * Get min max
    */
   void getMinMax(double* outMin, double* outMax);

public slots:
   /**
    * Set max
    */
   void setMax(double max);

   /**
    * Set min
    */
   void setMin(double min);

   /**
    * Set min max
    */
   void setMinMax(double min, double max);

   /**
    * Set range
    */
   void setRange(double min, double max);

signals:
   /**
    * Max change
    */
   void maxChanged(double max);

   /**
    * Min change
    */
   void minChanged(double min);

   /**
    * Min max change
    */
   void minMaxChanged(double min, double max);

private slots:
   /**
    * Update min max slider
    */
   void sliderMinMaxUpdated(const QPolygonF& points);

private:
   /**
    * Control points
    */
   ControlPoints* m_controlPoints;

};

}

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
