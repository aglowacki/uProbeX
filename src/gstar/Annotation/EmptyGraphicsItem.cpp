/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "EmptyGraphicsItem.h"

#include "gstar/AnnotationProperty.h"

using namespace gstar;

/*---------------------------------------------------------------------------*/

EmptyGraphicsItem::EmptyGraphicsItem(AbstractGraphicsItem* parent) :
   AbstractGraphicsItem(parent)
{

}

/*---------------------------------------------------------------------------*/

QRectF EmptyGraphicsItem::boundingRect() const
{

   return QRectF();

}

/*---------------------------------------------------------------------------*/

QRectF EmptyGraphicsItem::boundingRectMarker() const
{

   return QRectF();

}

/*---------------------------------------------------------------------------*/

void EmptyGraphicsItem::calculate()
{

   //nothing to calculate

}

/*---------------------------------------------------------------------------*/

const QString EmptyGraphicsItem::displayName() const
{

   const QString name = QString("Empty");
   return name;

}

/*---------------------------------------------------------------------------*/

AbstractGraphicsItem* EmptyGraphicsItem::duplicate()
{

   AbstractGraphicsItem* item = new EmptyGraphicsItem();

   return item;

}

/*---------------------------------------------------------------------------*/

void EmptyGraphicsItem::paint(QPainter* painter,
                                 const QStyleOptionGraphicsItem* option,
                                 QWidget* widget )
{

    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
      //for derived classes

}

/*---------------------------------------------------------------------------*/

void EmptyGraphicsItem::updateModel()
{

}

/*---------------------------------------------------------------------------*/

void EmptyGraphicsItem::updateView()
{

}

/*---------------------------------------------------------------------------*/
