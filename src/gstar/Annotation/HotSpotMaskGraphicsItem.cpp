/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/Annotation/HotSpotMaskGraphicsItem.h"

#include "gstar/AnnotationProperty.h"
#include "gstar/ImageViewScene.h"

#include <math.h>
#include <QLineF>

#include <QDebug>

using namespace gstar;

/*---------------------------------------------------------------------------*/

HotSpotMaskGraphicsItem::HotSpotMaskGraphicsItem(int width, int height, AbstractGraphicsItem* parent)
   : AbstractGraphicsItem(parent)
{

    setFlags( ItemIsSelectable );

    _mask = new QImage(width, height, QImage::Format_ARGB32);
    for(int w=0; w<width; w++)
    {
        for(int h=0; h<height; h++)
        {
            _mask->setPixelColor(w,h, QColor(0,0,0,100));
        }
    }

   _enable_mask = new AnnotationProperty();
   _enable_mask->setName("Enable");
   _enable_mask->setValue(true);

   _display_mask = new AnnotationProperty();
   _display_mask->setName("Visible");
   _display_mask->setValue(true);

   QStringList slist;
   slist.append("View");
   slist.append("Draw");
   slist.append("Erase");

   _draw_mask = new AnnotationProperty();
   _draw_mask->setName("Draw Mode");
   _draw_mask->setValue(slist);

   _alpha_value = new AnnotationProperty();
   _alpha_value->setName("Alpha");
   _alpha_value->setValue(100);

   m_data.push_back(_enable_mask);
   m_data.push_back(_display_mask);
   m_data.push_back(_draw_mask);
   m_data.push_back(_alpha_value);

}

/*---------------------------------------------------------------------------*/

void HotSpotMaskGraphicsItem::calculate()
{
/*
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
*/
}


/*---------------------------------------------------------------------------*/

void HotSpotMaskGraphicsItem::updateModel()
{
    update();
}

/*---------------------------------------------------------------------------*/

void HotSpotMaskGraphicsItem::updateView()
{
    int val = _alpha_value->getValue().toInt();

    for(int w=0; w<_mask->width(); w++)
    {
        for(int h=0; h<_mask->height(); h++)
        {
            QColor c = _mask->pixelColor(w,h);
            c.setAlpha(val);
            _mask->setPixelColor(w,h, c);
        }
    }
    update();
}

/*---------------------------------------------------------------------------*/

QRectF HotSpotMaskGraphicsItem::boundingRect() const
{

   return _mask->rect();

}

/*---------------------------------------------------------------------------*/

QRectF HotSpotMaskGraphicsItem::boundingRectMarker() const
{

   return _mask->rect();

}

/*---------------------------------------------------------------------------*/

const QString HotSpotMaskGraphicsItem::displayName() const
{

   const QString name = QString("Hotspot Mask");
   return name;

}

/*---------------------------------------------------------------------------*/

AbstractGraphicsItem* HotSpotMaskGraphicsItem::duplicate()
{

   HotSpotMaskGraphicsItem* item = new HotSpotMaskGraphicsItem(_mask->width(), _mask->height());

   item->setPos(pos());
   //item->setLine(m_line->line());
   //item->setLinkColor(m_lineColor);
   item->calculate();
   return item;

}
/*---------------------------------------------------------------------------*/

void HotSpotMaskGraphicsItem::paint(QPainter* painter,
                                 const QStyleOptionGraphicsItem* option,
                                 QWidget* widget )
{

    Q_UNUSED(option);
    Q_UNUSED(widget);
    setPos(0,0);
    //int hw = (_pixmap->width() / 2) * -1;
    //int hh = (_pixmap->height() / 2) * -1;
    if(_display_mask->getValue().toBool())
    {
        //painter->drawPixmap(hw, hh, *_pixmap);
        painter->drawPixmap(0, 0, QPixmap::fromImage(*_mask));
    }


}

/*---------------------------------------------------------------------------*/

void HotSpotMaskGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
   // Mouse click position (in item coordinates)
    int alpha_val = _alpha_value->getValue().toInt();
   QPointF pt = event -> pos();
    QPoint p(0,0);
    p.setX(static_cast<int>(pt.x()));
    p.setY(static_cast<int>(pt.y()));
    _mask->setPixelColor(p, QColor(0,255,0,alpha_val));
   // Queue an update
   update();

   // Pass mouse event
   QGraphicsItem::mousePressEvent(event);

}

/*---------------------------------------------------------------------------*/

void HotSpotMaskGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{

   // Current mouse position
   QPointF pt = event -> pos();

   // Last mouse position
   QPointF lpt = event -> lastPos();

   // Queue an update
   update();

}

/*---------------------------------------------------------------------------*/

void HotSpotMaskGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{

   // Queue an update
   update();

   // Pass mouse event
   QGraphicsItem::mouseReleaseEvent(event);

}

/*---------------------------------------------------------------------------*/
