/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/Annotation/HotSpotMaskGraphicsItem.h"

#include "gstar/AnnotationProperty.h"
#include "gstar/ImageViewScene.h"

#include <math.h>
#include <QLineF>

using namespace gstar;

//---------------------------------------------------------------------------

HotSpotMaskGraphicsItem::HotSpotMaskGraphicsItem(int width, int height, AbstractGraphicsItem* parent)
   : AbstractGraphicsItem(parent)
{

    setFlags( ItemIsSelectable );

    _mouse_down = false;

    _mask = new QImage(width, height, QImage::Format_ARGB32);
    for(int w=0; w<width; w++)
    {
        for(int h=0; h<height; h++)
        {
            _mask->setPixelColor(w,h, QColor(0,0,0,0));
        }
    }

   _enable_mask = new AnnotationProperty();
   _enable_mask->setName("Enable");
   _enable_mask->setValue(true);

   _display_mask = new AnnotationProperty();
   _display_mask->setName("Visible");
   _display_mask->setValue(true);

   _draw_mask = new AnnotationProperty();
   _draw_mask->setName("Draw");
   _draw_mask->setValue(true);

   _erase_mask = new AnnotationProperty();
   _erase_mask->setName("Erase");
   _erase_mask->setValue(false);

   _alpha_value = new AnnotationProperty();
   _alpha_value->setName("Alpha %");
   _alpha_value->setValue(70);

   connect(_draw_mask, SIGNAL(valueChanged()), this ,SLOT(drawmask_changed()));
   connect(_erase_mask, SIGNAL(valueChanged()), this ,SLOT(erasemask_changed()));

   m_data.push_back(_enable_mask);
   m_data.push_back(_display_mask);
   m_data.push_back(_draw_mask);
   m_data.push_back(_erase_mask);
   m_data.push_back(_alpha_value);

}

//---------------------------------------------------------------------------

HotSpotMaskGraphicsItem::~HotSpotMaskGraphicsItem()
{
    emit (mask_updated(this, false));
}

//---------------------------------------------------------------------------

QString HotSpotMaskGraphicsItem::getName()
{

    for (const auto& itr : m_data)
    {
        if (itr->getName() == DEF_STR_DISPLAY_NAME)
            return itr->getValue().toString();
    }
    return "";

}

//---------------------------------------------------------------------------

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


//---------------------------------------------------------------------------

void HotSpotMaskGraphicsItem::updateModel()
{
    update();
}

//---------------------------------------------------------------------------

void HotSpotMaskGraphicsItem::updateView()
{
    int val = _alpha_value->getValue().toInt();

    val = std::min(val, 100);
    val = std::max(val, 0);

    _alpha_value->setValue(val);

    val = (int)(((float)val / 100.0) * 255.0);

    for(int w=0; w<_mask->width(); w++)
    {
        for(int h=0; h<_mask->height(); h++)
        {
            QColor c = _mask->pixelColor(w,h);
            if(c.green() > 0)
            {
                c.setAlpha(val);
                _mask->setPixelColor(w,h, c);
            }
        }
    }
    update();
    emit(mask_updated(this, false));
}

//---------------------------------------------------------------------------

void HotSpotMaskGraphicsItem::drawmask_changed()
{
    bool val = _draw_mask->getValue().toBool();
    if(val == true)
    {
        _erase_mask->setValue(false);
        updateModel();
    }
}

//---------------------------------------------------------------------------

std::vector<QPoint> HotSpotMaskGraphicsItem::get_mask_list()
{
    std::vector<QPoint> roi_list;
    for (int w = 0; w < _mask->width(); w++)
    {
        for (int h = 0; h < _mask->height(); h++)
        {
            QColor c = _mask->pixelColor(w, h);
            if (c.green() > 0)
            {
                roi_list.push_back(QPoint(w,h));
            }
        }
    }
    return roi_list;
}

//---------------------------------------------------------------------------

void HotSpotMaskGraphicsItem::erasemask_changed()
{
    bool val = _erase_mask->getValue().toBool();
    if(val == true)
    {
        _draw_mask->setValue(false);
        updateModel();
    }
}

//---------------------------------------------------------------------------

QRectF HotSpotMaskGraphicsItem::boundingRect() const
{

   return _mask->rect();

}

//---------------------------------------------------------------------------

QRectF HotSpotMaskGraphicsItem::boundingRectMarker() const
{

   return _mask->rect();

}

//---------------------------------------------------------------------------

const QString HotSpotMaskGraphicsItem::displayName() const
{

   const QString name = QString("ROI Spectra");
   return name;

}

//---------------------------------------------------------------------------

AbstractGraphicsItem* HotSpotMaskGraphicsItem::duplicate()
{

   HotSpotMaskGraphicsItem* item = new HotSpotMaskGraphicsItem(_mask->width(), _mask->height());

   item->setPos(pos());
   //item->setLine(m_line->line());
   //item->setLinkColor(m_lineColor);
   item->calculate();
   return item;

}
//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------

void HotSpotMaskGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{

    _mouse_down = true;
   // Mouse click position (in item coordinates)

    int alpha_val = (int)((_alpha_value->getValue().toFloat() / 100.0) * 255.0);
    QPointF pt = event -> pos();
    QPoint p(0,0);
    p.setX(static_cast<int>(pt.x()));
    p.setY(static_cast<int>(pt.y()));
    if(_draw_mask->getValue().toBool() == true)
    {
        _mask->setPixelColor(p, QColor(0,255,0,alpha_val));
    }
    if(_erase_mask->getValue().toBool() == true)
    {
        _mask->setPixelColor(p, QColor(0,0,0,0));
    }
   // Queue an update
   update();

   // Pass mouse event
   QGraphicsItem::mousePressEvent(event);

}

//---------------------------------------------------------------------------

void HotSpotMaskGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{

   if(_mouse_down == true)
   {
       int alpha_val = (int)((_alpha_value->getValue().toFloat() / 100.0) * 255.0);
       QPointF pt = event -> pos();
       QPoint p(0,0);
       p.setX(static_cast<int>(pt.x()));
       p.setY(static_cast<int>(pt.y()));
       if(_draw_mask->getValue().toBool() == true)
       {
           _mask->setPixelColor(p, QColor(0,255,0,alpha_val));
       }
       if(_erase_mask->getValue().toBool() == true)
       {
           _mask->setPixelColor(p, QColor(0,0,0,0));
       }
       // Queue an update
       update();
   }

}

//---------------------------------------------------------------------------

void HotSpotMaskGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{

    _mouse_down = false;

   // Queue an update
   //update();
    emit(mask_updated(this, true));

   // Pass mouse event
   QGraphicsItem::mouseReleaseEvent(event);

}

//---------------------------------------------------------------------------
