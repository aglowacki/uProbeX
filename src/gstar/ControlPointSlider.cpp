/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/ControlPointSlider.h>

using namespace gstar;

/*---------------------------------------------------------------------------*/

ControlPointSlider::ControlPointSlider()
{

   setMaximumHeight(12);

   m_controlPoints = new ControlPoints(this);
   m_controlPoints -> setMode(ControlPoints::MinMaxSlider);

   connect(m_controlPoints, SIGNAL(pointsChanged(const QPolygonF&)),
           this, SLOT(sliderMinMaxUpdated(const QPolygonF&)));

}

/*---------------------------------------------------------------------------*/

void ControlPointSlider::getMinMax(double* outMin, double* outMax)
{

   m_controlPoints->getMinMax(outMin, outMax);

}

/*---------------------------------------------------------------------------*/

void ControlPointSlider::setMax(double max)
{

   Q_UNUSED(max)

}

/*---------------------------------------------------------------------------*/

void ControlPointSlider::setMin(double min)
{

   Q_UNUSED(min)

}

/*---------------------------------------------------------------------------*/

void ControlPointSlider::setMinMax(double min, double max)
{

   QPolygonF points;
   points.push_back(QPointF(min, 0.0));
   points.push_back(QPointF(max, 0.0));
   m_controlPoints -> setPoints(points, false);

}

/*---------------------------------------------------------------------------*/

void ControlPointSlider::setRange(double min, double max)
{

   m_controlPoints -> setRange(min, max);
   //m_controlPoints -> SetMin(min);
   //m_controlPoints -> SetMax(max);
   m_controlPoints -> setMaxMin(max, min);

}

/*---------------------------------------------------------------------------*/

void ControlPointSlider::sliderMinMaxUpdated(const QPolygonF& points)
{

   emit minChanged(points[0].x());
   emit maxChanged(points[points.size() - 1].x());
   emit minMaxChanged(points[0].x(), points[points.size() - 1].x());

}

/*---------------------------------------------------------------------------*/
