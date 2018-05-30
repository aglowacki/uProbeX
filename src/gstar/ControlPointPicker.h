/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_CONTROL_POINT_PICKER_H
#define GSTAR_CONTROL_POINT_PICKER_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QPainter>
#include <QPolygonF>

#include "gstar/ControlPoints.h"

/*---------------------------------------------------------------------------*/

namespace gstar
{

class ControlPointPicker
: public QWidget
{

   Q_OBJECT

public:

   enum Type {
      Red,
      Green,
      Blue,
      White
   };
   /**
    * Constructor
    */
   ControlPointPicker();

   /**
    * Get color
    */
   int getColor();

   /**
    * Get min max
    */
   void getMinMax(double* outMin, double* outMax);

   /**
    * Get points
    */
   QPolygonF getPoints() const;

   /**
    * Get total range
    */
   void getTotalRange(double* min, double* max);

   /**
    * Paint event
    */
   void paintEvent(QPaintEvent* event);

   /**
    * Set color
    */
   void setColor(int type);

   void setPickerRange(double min, double max);

public slots:
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

signals:
   /**
    * Color points changed
    */
   void controlPointsChanged(const QPolygonF& points);

private slots:
   /**
    * Points changed
    */
   void pointsChanged(const QPolygonF& points);

private:
   /**
    * type
    */
   int m_type;

   /**
    * Control points
    */
   ControlPoints* m_controlPoints;

};

}

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
