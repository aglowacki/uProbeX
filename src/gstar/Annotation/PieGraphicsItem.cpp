/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/Annotation/PieGraphicsItem.h"

#include "gstar/AnnotationProperty.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

using namespace gstar;

//---------------------------------------------------------------------------

PieGraphicsItem::PieGraphicsItem(AbstractGraphicsItem* parent)
   : AbstractGraphicsItem(parent)
{

   m_radius = 10.0;
   m_startAngle = 0.0;
   m_span = 270.0;
   m_outlineColor = QColor(0, 0, 0);

   m_positionXProp = new AnnotationProperty("X", 0.0);
   m_positionYProp = new AnnotationProperty("Y", 0.0);
   m_radiusProp = new AnnotationProperty("Radius", 10.0);
   m_startAngleProp = new AnnotationProperty("Start Angle", 0.0);
   m_spanProp = new AnnotationProperty("Span", 270.0);

/*
   appendChild(new AbstractGraphicsItem(m_radiusProp, this));
   appendChild(new AbstractGraphicsItem(m_startAngleProp, this));
   appendChild(new AbstractGraphicsItem(m_spanProp, this));
   appendChild(new AbstractGraphicsItem(m_positionProp, this));
*/
   //m_data.push_back(new AnnotationProperty("Pie"));

   m_data.push_back(m_positionXProp);
   m_data.push_back(m_positionYProp);
   m_data.push_back(m_radiusProp);
   m_data.push_back(m_startAngleProp);
   m_data.push_back(m_spanProp);

   connectAllProperties();

}

//---------------------------------------------------------------------------

QRectF PieGraphicsItem::boundingRect() const
{

   const QRectF rect = QRectF(-m_radius, -m_radius, m_radius * 2, m_radius * 2);
   return rect;

}

//---------------------------------------------------------------------------

QRectF PieGraphicsItem::boundingRectMarker() const
{

   const QRectF rect = QRectF(-m_radius, -m_radius, m_radius * 2, m_radius * 2);
   return rect;

}

//---------------------------------------------------------------------------

const QString PieGraphicsItem::displayName() const
{

   const QString name = QString("Pie");
   return name;

}

//---------------------------------------------------------------------------

QPoint PieGraphicsItem::getPos()
{

   QPoint p = QPoint((int)(pos().x()), (int)(pos().y()));
   return p;

}

//---------------------------------------------------------------------------

void PieGraphicsItem::paint(QPainter* painter,
                            const QStyleOptionGraphicsItem* option,
                            QWidget* widget)
{

   Q_UNUSED(widget);

   QPen pen(m_outlineColor);
   if (option->state & QStyle::State_Selected)
   {
      pen.setStyle(Qt::DotLine);
      pen.setWidth(2);
   }
   painter->setPen(pen);
   QRect rect( -m_radius, -m_radius, m_radius * 2, m_radius * 2);
   painter->drawPie(rect, m_startAngle * 16, m_span * 16);

}

//---------------------------------------------------------------------------

void PieGraphicsItem::setRadius(double radius)
{

   disconnectAllProperties();
   disconnectAllViewItems();
   m_radius = radius;
   m_radiusProp->setValue(radius);
   connectAllProperties();
   connectAllViewItems();

}

//---------------------------------------------------------------------------

void PieGraphicsItem::updateModel()
{

   QPointF position = getPos();
   m_positionXProp->setValue(position.x());
   m_positionYProp->setValue(position.y());
   m_radiusProp->setValue(m_radius);
   m_startAngleProp->setValue(m_startAngle);
   m_spanProp->setValue(m_span);
   update();

}

//---------------------------------------------------------------------------

void PieGraphicsItem::updateView()
{

   double x = m_positionXProp->getValue().toDouble();
   double y = m_positionYProp->getValue().toDouble();
   setPos(x, y);
   m_radius = m_radiusProp->getValue().toDouble();
   m_startAngle = m_startAngleProp->getValue().toDouble();
   m_span = m_spanProp->getValue().toDouble();
   update();

}

//---------------------------------------------------------------------------
