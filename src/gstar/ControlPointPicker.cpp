/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/ControlPointPicker.h>

#include <QDebug>

using namespace gstar;

/*---------------------------------------------------------------------------*/

ControlPointPicker::ControlPointPicker()
{

   m_controlPoints = new ControlPoints(this);
   m_controlPoints -> setRange(0.0, 255.0);

   connect(m_controlPoints, SIGNAL(pointsChanged(const QPolygonF&)),
           this, SLOT(pointsChanged(const QPolygonF&)));

   m_type = ControlPointPicker::Blue;

}

/*---------------------------------------------------------------------------*/

int ControlPointPicker::getColor()
{

   return m_type;

}

/*---------------------------------------------------------------------------*/

void ControlPointPicker::getMinMax(double* outMin, double* outMax)
{

   m_controlPoints->getMinMax(outMin, outMax);

}

/*---------------------------------------------------------------------------*/

QPolygonF ControlPointPicker::getPoints() const
{
   
   return m_controlPoints -> getPoints();

}

/*---------------------------------------------------------------------------*/

void ControlPointPicker::getTotalRange(double* min, double* max)
{

   m_controlPoints -> getTotalRange(min, max);

}

/*---------------------------------------------------------------------------*/

void ControlPointPicker::paintEvent(QPaintEvent* event)
{

   Q_UNUSED(event)

   QPainter p(this);

   QImage sim(size(), QImage::Format_RGB32);
   QLinearGradient sg(0, 0, 0, height());
   sg.setColorAt(1, Qt::black);
   QColor green(5, 155, 123);

   if (!isEnabled())
      sg.setColorAt(0, Qt::gray);
   else if (m_type == Red)
      sg.setColorAt(0, Qt::red);
   else if (m_type == Green)
      sg.setColorAt(0, green);
   else if (m_type == Blue)
      sg.setColorAt(0, Qt::blue);
   else
      sg.setColorAt(0, Qt::white);

   QPainter pim(&sim);
   pim.fillRect(rect(), sg);

   p.drawImage(0, 0, sim);

   p.setPen(QColor(128, 128, 128));
   p.drawRect(0, 0, width() - 1, height() - 1);

}

/*---------------------------------------------------------------------------*/

void ControlPointPicker::pointsChanged(const QPolygonF& points)
{

   emit controlPointsChanged(points);

}

/*---------------------------------------------------------------------------*/

void ControlPointPicker::setColor(int type)
{

   m_type = type;

   update();

}

/*---------------------------------------------------------------------------*/

void ControlPointPicker::setPoints(const QPolygonF& points, bool bRestretch)
{

   m_controlPoints -> setPoints(points, bRestretch, false);

}

/*---------------------------------------------------------------------------*/

void ControlPointPicker::setSubRange(double min, double max)
{

   //m_controlPoints -> SetMin(min);
   //m_controlPoints -> SetMax(max);
   m_controlPoints -> setMaxMin(max, min);

   update();

}

/*---------------------------------------------------------------------------*/

void ControlPointPicker::setTotalRange(double min, double max)
{

   m_controlPoints -> setRange(min, max);

   update();

}


/*---------------------------------------------------------------------------*/

void ControlPointPicker::setPickerRange(double min, double max)
{

   m_controlPoints -> setPickerPointRange(min, max);

   update();

}

/*---------------------------------------------------------------------------*/
