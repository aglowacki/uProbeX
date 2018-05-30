/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/Annotation/IntensityPieGraphicsItem.h"

#include "gstar/AnnotationProperty.h"

using namespace gstar;

/*---------------------------------------------------------------------------*/

IntensityPieGraphicsItem::IntensityPieGraphicsItem() : PieGraphicsItem()
{

   m_intensity = new AnnotationProperty("Intensity", 0.0);

//   appendChild(new AbstractGraphicsItem(m_intensity, this));

//   m_data->setName("Intensity Pie");
   m_data.push_back(m_intensity);

}

/*---------------------------------------------------------------------------*/

void IntensityPieGraphicsItem::calculate()
{

   //for now just set to 0
   m_intensity->setValue(0.0);

}

/*---------------------------------------------------------------------------*/

const QString IntensityPieGraphicsItem::displayName() const
{

   const QString name = QString("Intensity Pie");
   return name;

}

/*---------------------------------------------------------------------------*/

AbstractGraphicsItem* IntensityPieGraphicsItem::duplicate()
{

   AbstractGraphicsItem* item = new IntensityPieGraphicsItem();

   item->setPos(pos());
   return item;

}

/*---------------------------------------------------------------------------*/
