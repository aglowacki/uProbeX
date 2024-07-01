/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/Annotation/IntensityLineGraphicsItem.h"

#include "gstar/AnnotationProperty.h"

using namespace gstar;

//---------------------------------------------------------------------------

IntensityLineGraphicsItem::IntensityLineGraphicsItem() : LineGraphicsItem()
{

   m_intensity = new AnnotationProperty("Intensity", 0.0);

   //appendChild(new AbstractGraphicsItem(m_intensity, this));

//   m_data->setName("Intensity Line");

   m_data.push_back(m_intensity);


}

//---------------------------------------------------------------------------

void IntensityLineGraphicsItem::calculate()
{

    double intensity = 0.0;
    m_text = QString("%1").arg(intensity);

   //grab the intensity of the pixels

}

//---------------------------------------------------------------------------

const QString IntensityLineGraphicsItem::displayName() const
{

   const QString name = QString("Intensity Line");
   return name;

}

//---------------------------------------------------------------------------

AbstractGraphicsItem* IntensityLineGraphicsItem::duplicate()
{

   IntensityLineGraphicsItem* item = new IntensityLineGraphicsItem();

   item->setPos(pos());
   item->setStartPos(m_line->line().p1());
   item->setEndPos(m_line->line().p2());
   item->setLinkColor(m_lineColor);
   item->calculate();
   return item;

}

//---------------------------------------------------------------------------
