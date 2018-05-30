/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_CONTROL_POINTS_H
#define GSTAR_CONTROL_POINTS_H

/*---------------------------------------------------------------------------*/

#include <QtGui>

/*---------------------------------------------------------------------------*/

namespace gstar
{

class ControlPoints
: public QObject
{

   Q_OBJECT

public:

   enum Mode {
      Points,
      MinMaxSlider
   };

   /**
    * Constructor
    */
   ControlPoints(QWidget* widget);

   /**
    * Event filter
    */
   bool eventFilter(QObject* object, QEvent* event);

   /**
    * Get points
    */
   QPolygonF getPoints() const;

   /**
    * Get total range
    */
   void getTotalRange(double* min, double* max);

   /**
    * Get min and max
    *
    * @param outMin - min values
    * @param outMax - max value
    */
   void getMinMax(double* outMin, double* outMax);

   /**
    * Paint points
    */
   void paintPoints();

   /**
    * Set max
    */
   void setMax(double max);

   /**
    * Set max min
    */
   void setMaxMin(double max, double min);

   /**
    * Set min
    */
   void setMin(double min);

   /**
    * Set mode
    */
   void setMode(int mode);

   /**
    * Set points
    */
   void setPoints(const QPolygonF& points, 
                  bool bRestretch = false, 
                  bool emitSignal = true);

   /**
    * Set range
    */
   void setRange(int min, int max);

   /**
    * Set picker range
    */
   void setPickerPointRange(int min, int max);

signals:
   /**
    * Points changed
    */
   void pointsChanged(const QPolygonF& points);

private:
   /**
    * Bound point
    */
   QPointF boundPoint(int index, const QPointF& value) const;

   /**
    * Point bound rect
    */
   QRectF pointBoundingRect(int i) const;

   /**
    * Change pixel value
    */
   QPointF pixelToNormalValue(const QPointF& pixel) const;

   /**
    * Change normal value
    */
   QPointF normalValueToPixel(const QPointF& value) const;

   /**
    * Move points
    */
   void movePoint(int i, const QPointF &newPos, bool emitChange = true);

   /**
    * Restretch points
    */
   void restretchPoints();

   /**
    * Parent widget
    */
   QWidget* m_widget;

   /**
    * Pointsize
    */
   QSizeF m_pointSize;

   /**
    * Pointpen
    */
   QPen m_pointPen;

   /**
    * Pointbrush
    */
   QBrush m_pointBrush;

   /**
    * Pen connection
    */
   QPen m_connectionPen;

   /**
    * Slider pen
    */
   QPen m_sliderPen;

   /**
    * Slider brush
    */
   QBrush m_sliderBrush;

   /**
    * Min
    */
   int m_min;

   /**
    * Max
    */
   int m_max;

   /**
    * Old min and max
    */
   int m_minOld;
   int m_maxOld;

   /**
    * Mode
    */
   int m_mode;

   /**
    * Current index
    */
   int m_currentIndex;

   /**
    * range
    */
   double m_range[2];

   /**
    * Flag to know if a point is being moved using mouse.
    */
   bool m_pointInMove;

   /**
    * Control points
    */
   QPolygonF m_points;

};

}

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
