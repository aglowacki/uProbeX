/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/HistogramPlot.h>

#include <QWidget>
#include <QApplication>

using namespace gstar;

/*---------------------------------------------------------------------------*/

HistogramPlot::HistogramPlot(QWidget* parent)
{

   m_widget = parent;
   m_widget->installEventFilter(this);

}

/*---------------------------------------------------------------------------*/

HistogramPlot::~HistogramPlot()
{


}

/*---------------------------------------------------------------------------*/

bool HistogramPlot::eventFilter(QObject* source, QEvent* event)
{

   if (source == m_widget)
   {
      if (!(m_widget->isEnabled()))
      {
         if ((event->type()) != QEvent::Paint)
         {
            return true;
         }
      }

      switch (event->type())
      {
         case QEvent::Paint:
         {
            QWidget* tmp = m_widget;
            m_widget = 0;
            QApplication::sendEvent(source, event);
            m_widget = tmp;
            drawHistogram();
            return true;
            break;
         }
         default:
            break;
      }
   }

   return false;

}

/*---------------------------------------------------------------------------*/

void HistogramPlot::drawHistogram()
{

   QPainter painter(m_widget);
   painter.setPen( QPen(QColor(158, 157, 162, 200), 1));
   painter.setBrush(QBrush(QColor(158, 157, 162, 90)));
   painter.setRenderHint(QPainter::Antialiasing, true);
   painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

   if (m_values.size() <= 0) return;

   float y = 0.0;

   QPolygonF polylinePoints;
   int i=0, index = 0;
   polylinePoints.insert(0, QPointF(0, ymap(0)));
   for (; i<m_widget->width(); i++)
   {
      index = (int)(i * m_values.size() / m_widget->width());
      y = ymap(m_values.at(index));
      polylinePoints.insert(i+1, QPointF(i, y));
   }
   polylinePoints.insert(i+1, QPointF(i, ymap(0)));

   painter.drawPolygon(polylinePoints, Qt::OddEvenFill);

}

/*---------------------------------------------------------------------------*/

qreal HistogramPlot::ymap(const qreal& value)
{

   // Leaving a small gap from the top
   qreal ymax = 0.0 + 0.03 * m_widget->height();
   qreal ymin = m_widget->height(); // the max-value of Y is at the bottom of
                                    // the widget, for display purposes that is
                                    //  the minimum Y.
   qreal newRange = ymin - ymax;
   qreal res = m_widget->height() - ( ( value  / m_max ) * newRange );

   // If data point is too big, just make it point to the top.
   if (res < 0) res = ymax;

   return res;

}

/*---------------------------------------------------------------------------*/

void HistogramPlot::updatePoints(QList<int> pts)
{

   QList<int> tmp = m_values;
   m_values = pts;

   m_max = 0;
   int max2 = 0;

   bool shouldUpdate = false;

   for (int i=0; i<m_values.size(); i++)
   {
      if (tmp.size() == m_values.size())
      {
         if (m_values.at(i) != tmp.at(i))
         {
            shouldUpdate = true;
         }
      }
      else
      {
         shouldUpdate = true;
      }

      if (m_values.at(i) > m_max)
      {
         max2 = m_max;
         m_max = m_values.at(i);
      }
      else if (m_values.at(i) > max2)
      {
         max2 = m_values.at(i);
      }

   }

   if ( (m_max > (2 * max2)) && max2 != 0.0 )
   {
      m_max = (int) (max2 * 1.5);
   }

   if (shouldUpdate)
   {
      m_widget->update();
   }

}

/*---------------------------------------------------------------------------*/
