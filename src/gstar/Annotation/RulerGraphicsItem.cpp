/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/Annotation/RulerGraphicsItem.h"

#include "gstar/AnnotationProperty.h"
#include "gstar/ImageViewScene.h"

#include <math.h>
#include <QLineF>

#include <QDebug>

using namespace gstar;

/*---------------------------------------------------------------------------*/

RulerGraphicsItem::RulerGraphicsItem(AbstractGraphicsItem* parent)
   : LineGraphicsItem(parent)
{

   m_length = new AnnotationProperty();
   m_length->setName("Length");
   m_length->setValue(0.0);

/*
   appendChild(new AbstractGraphicsItem(m_length, this));
*/
//   m_data.push_back(me("Ruler");

   m_data.push_back(m_length);

}

/*---------------------------------------------------------------------------*/

void RulerGraphicsItem::calculate()
{

   QString unitsLabel = "";
   double unitsPerPixelX = 1.0;
   double unitsPerPixelY = 1.0;

   //get units per pixel from scene
   QGraphicsScene* bScene = scene();
   if (bScene != nullptr)
   {
      ImageViewScene* iScene = dynamic_cast<ImageViewScene*>(bScene);
      if (iScene != nullptr)
      {
         unitsLabel = iScene->getUnitsLabel();
         unitsPerPixelX = iScene->getUnitsPerPixelX();
         unitsPerPixelY = iScene->getUnitsPerPixelY();
      }
   }

   double dx = m_line->line().dx();
   double dy = m_line->line().dy();
   dx *= unitsPerPixelX;
   dy *= unitsPerPixelY;

   double len = sqrt( (dx * dx) + (dy * dy) );
   m_text = QString("%1%2").arg(len).arg(unitsLabel);
   m_length->setValue(m_text);

}

/*---------------------------------------------------------------------------*/

const QString RulerGraphicsItem::displayName() const
{

   const QString name = QString("Ruler");
   return name;

}

/*---------------------------------------------------------------------------*/

AbstractGraphicsItem* RulerGraphicsItem::duplicate()
{

   RulerGraphicsItem* item = new RulerGraphicsItem();

   item->setPos(pos());
   item->setLine(m_line->line());
   item->setLinkColor(m_lineColor);
   item->calculate();
   return item;

}

/*---------------------------------------------------------------------------*/
