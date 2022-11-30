/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/Annotation/RoiMaskGraphicsItem.h"

#include "gstar/AnnotationProperty.h"
#include "gstar/ImageViewScene.h"

#include <math.h>
#include <QLineF>

using namespace gstar;

/*---------------------------------------------------------------------------*/

static const QString CONST_STATIC_ROI_NAME = QString("ROI");

RoiMaskGraphicsItem::RoiMaskGraphicsItem(cv::Mat& mat, int idx, QColor col, AbstractGraphicsItem* parent)
   : AbstractGraphicsItem(parent)
{

    setFlags( ItemIsSelectable );

    _mouse_down = false;

    _mask = new QImage(mat.cols, mat.rows, QImage::Format_ARGB32);
    for(int w=0; w< mat.cols; w++)
    {
        for(int h=0; h< mat.rows; h++)
        {
            int color_idx = mat.at<int>(h, w);
            if (color_idx == idx)
            {
                col.setAlpha(178);
                _mask->setPixelColor(w, h, col);
            }
            else
            {
                _mask->setPixelColor(w, h, QColor(0, 0, 0, 0));
            }
        }
    }

   _color_ano = new AnnotationProperty();
   _color_ano->setName("Color");
   _color_ano->setValue(col);

   _alpha_value = new AnnotationProperty();
   _alpha_value->setName("Alpha %");
   _alpha_value->setValue(70);

   m_data.push_back(_color_ano);
   m_data.push_back(_alpha_value);

}

/*---------------------------------------------------------------------------*/

RoiMaskGraphicsItem::RoiMaskGraphicsItem(QImage mask, QColor color, int alpha, AbstractGraphicsItem* parent) : AbstractGraphicsItem(parent)
{
    _mask = new QImage();
    *_mask = mask;

    _color_ano = new AnnotationProperty();
    _color_ano->setName("Color");
    _color_ano->setValue(color);

    _alpha_value = new AnnotationProperty();
    _alpha_value->setName("Alpha %");
    _alpha_value->setValue(alpha);

    m_data.push_back(_color_ano);
    m_data.push_back(_alpha_value);
}

/*---------------------------------------------------------------------------*/

RoiMaskGraphicsItem::~RoiMaskGraphicsItem()
{
    emit (mask_updated(this, false));
}

/*---------------------------------------------------------------------------*/

QString RoiMaskGraphicsItem::getName()
{

    for (const auto& itr : m_data)
    {
        if (itr->getName() == DEF_STR_DISPLAY_NAME)
            return itr->getValue().toString();
    }
    return "";

}

/*---------------------------------------------------------------------------*/

void RoiMaskGraphicsItem::calculate()
{
    QVariant variant = _color_ano->getValue();
    QColor color = variant.value<QColor>();
    int alpha = _alpha_value->getValue().toInt();
    alpha = (int)(((float)alpha / 100.0) * 255.0);
    color.setAlpha(alpha);
    

    for (int w = 0; w < _mask->width(); w++)
    {
        for (int h = 0; h < _mask->height(); h++)
        {
            if(_mask->pixel(w,h) != 0)
            {
                _mask->setPixelColor(w, h, color);
            }
        }
    }
}


/*---------------------------------------------------------------------------*/

void RoiMaskGraphicsItem::updateModel()
{
    update();
}

/*---------------------------------------------------------------------------*/

void RoiMaskGraphicsItem::updateView()
{
    int val = _alpha_value->getValue().toInt();

    val = std::min(val, 100);
    val = std::max(val, 0);

    _alpha_value->setValue(val);

    calculate();
    update();
    emit(mask_updated(this, false));
}

/*---------------------------------------------------------------------------*/

QColor RoiMaskGraphicsItem::getColor() const
{
    QVariant variant = _color_ano->getValue();
    QColor color = variant.value<QColor>();
    return color;
}

/*---------------------------------------------------------------------------*/

void RoiMaskGraphicsItem::drawmask_changed()
{
    /*
    bool val = _draw_mask->getValue().toBool();
    if(val == true)
    {
        _erase_mask->setValue(false);
        updateModel();
    }
    */
}

/*---------------------------------------------------------------------------*/

void RoiMaskGraphicsItem::to_roi_vec(std::vector<std::pair<unsigned int, unsigned int>>& rois)
{
    for (int w = 0; w < _mask->width(); w++)
    {
        for (int h = 0; h < _mask->height(); h++)
        {
            if( _mask->pixel(w, h) != 0)
            {
                rois.push_back({ w, h });
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

void RoiMaskGraphicsItem::erasemask_changed()
{
    /*
    bool val = _erase_mask->getValue().toBool();
    if(val == true)
    {
        _draw_mask->setValue(false);
        updateModel();
    }
    */
}

/*---------------------------------------------------------------------------*/

QRectF RoiMaskGraphicsItem::boundingRect() const
{

   return _mask->rect();

}

/*---------------------------------------------------------------------------*/

QRectF RoiMaskGraphicsItem::boundingRectMarker() const
{

   return _mask->rect();

}

/*---------------------------------------------------------------------------*/

const QString RoiMaskGraphicsItem::displayName() const
{
   return CONST_STATIC_ROI_NAME;
}

/*---------------------------------------------------------------------------*/

AbstractGraphicsItem* RoiMaskGraphicsItem::duplicate()
{
    QColor col = QColor(_color_ano->getValue().toString());
    RoiMaskGraphicsItem* item = new RoiMaskGraphicsItem(*_mask, col, _alpha_value->getValue().toInt());
    item->prependProperty(new AnnotationProperty(DEF_STR_DISPLAY_NAME, getName()));
    return item;

}
/*---------------------------------------------------------------------------*/

void RoiMaskGraphicsItem::paint(QPainter* painter,
                                 const QStyleOptionGraphicsItem* option,
                                 QWidget* widget )
{

    Q_UNUSED(option);
    Q_UNUSED(widget);
    setPos(0, 0);
    painter->drawPixmap(0, 0, QPixmap::fromImage(*_mask));
}

/*---------------------------------------------------------------------------*/

void RoiMaskGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    /*
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
   */
}

/*---------------------------------------------------------------------------*/

void RoiMaskGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    /*
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
   */
}

/*---------------------------------------------------------------------------*/

void RoiMaskGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    /*
    _mouse_down = false;

   // Queue an update
   //update();
    emit(mask_updated(this, true));

   // Pass mouse event
   QGraphicsItem::mouseReleaseEvent(event);
   */
}

/*---------------------------------------------------------------------------*/
