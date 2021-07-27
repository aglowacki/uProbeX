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
   _n_bins = 128;
   _bins.resize(_n_bins);
   for (int i = 0; i < _n_bins; i++)
   {
       _bins[i] = 0;
   }

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
    painter.setPen( QPen(QColor(188, 187, 192, 200), 1));
    painter.setBrush(QBrush(QColor(188, 187, 192, 90)));
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    float y = 0.0;

    QPolygonF polylinePoints;
    int i=0, index = 0;
    polylinePoints.insert(0, QPointF(0, ymap(0)));

    qreal bins = (qreal)_n_bins;
    for (; i< m_widget->width(); i++)
    {
        int bin = std::floor(( (qreal)(i) / (qreal)(m_widget->width()) ) * bins);
        y = ymap(_bins[bin]);
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
   qreal res = m_widget->height() - ( ( value  / _max_bin_val) * newRange );

   // If data point is too big, just make it point to the top.
   if (res < 0) res = ymax;

   return res;

}

/*---------------------------------------------------------------------------*/

void HistogramPlot::updatePoints(const data_struct::ArrayXr& pts)
{

    real_t minCoef = pts.minCoeff();
    real_t maxCoef = pts.maxCoeff();

    real_t bin_width = (maxCoef - minCoef) / (real_t)(_n_bins-1);
    for (int i = 0; i < pts.size(); i++)
    {
        int bin = std::floor( (pts(i) - minCoef) / bin_width);
        _bins[bin] = _bins[bin] + 1;
    }

    _max_bin_val = 0;
    for (int i = 0; i < _n_bins; i++)
    {
        _max_bin_val  = std::max(_bins[i], _max_bin_val);
    }

    m_widget->update();
   
}

/*---------------------------------------------------------------------------*/
