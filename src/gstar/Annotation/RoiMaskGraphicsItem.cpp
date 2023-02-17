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
    _draw_action = DRAW_ACTION_MODES::OFF;

    _mask = new QImage(mat.cols, mat.rows, QImage::Format_ARGB32);

    _polygon.clear();
    _polygon.push_back(QPoint(0, 0));
    _polygon.push_back(QPoint(0, _mask->height()));
    _polygon.push_back(QPoint(_mask->width(), _mask->height()));
    _polygon.push_back(QPoint(_mask->width(), 0));

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

    _brush_size = QSize(10, 10);

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
    *_mask = mask.copy();
    _mouse_down = false;
    _draw_action = DRAW_ACTION_MODES::OFF;

    _polygon.clear();
    _polygon.push_back(QPoint(0, 0));
    _polygon.push_back(QPoint(0, _mask->height()));
    _polygon.push_back(QPoint(_mask->width(), _mask->height()));
    _polygon.push_back(QPoint(_mask->width(), 0));

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
    if (_mask != nullptr)
    {
        delete _mask;
        _mask = nullptr;
    }
}

/*---------------------------------------------------------------------------*/

void RoiMaskGraphicsItem::setMaskSize(QRectF size)
{
    _polygon.clear();
    _polygon.push_back(QPoint(0, 0));
    _polygon.push_back(QPoint(0, size.height()));
    _polygon.push_back(QPoint(size.width(), size.height()));
    _polygon.push_back(QPoint(size.width(), 0));

    _mask = new QImage(size.width(), size.height(), QImage::Format_ARGB32);
    for (int w = 0; w < size.width(); w++)
    {
        for (int h = 0; h < size.height(); h++)
        {
            _mask->setPixelColor(w, h, QColor(0, 0, 0, 0));
        }
    }
    update();
}

/*---------------------------------------------------------------------------*/

void RoiMaskGraphicsItem::add_to_roi(QPointF pos)
{
    int x = pos.x();
    int y = pos.y();
    if (x > -1 && x < _mask->width())
    {
        if (y > -1 && y < _mask->height())
        {
            int xsize = x + _brush_size.width();
            if (xsize > _mask->width() - 1)
                xsize = _mask->width() - 1;
            int ysize = y + _brush_size.height();
            if (ysize > _mask->height() - 1)
                ysize = _mask->height() - 1;
            QVariant variant = _color_ano->getValue();
            QColor col = variant.value<QColor>();
            col.setAlpha(_alpha_value->getValue().toInt());
            for (int i = x; i < xsize; i++)
            {
                for (int j = y; j < ysize; j++)
                {
                    _mask->setPixelColor(i, j, col);
                }
            }
            updateModel();
        }
    }
}

/*---------------------------------------------------------------------------*/

void RoiMaskGraphicsItem::erase_from_roi(QPointF pos)
{

    int x = pos.x();
    int y = pos.y();
    if (x > -1 && x < _mask->width())
    {
        if (y > -1 && y < _mask->height())
        {
            int xsize = x + _brush_size.width();
            if (xsize > _mask->width() - 1)
                xsize = _mask->width() - 1;
            int ysize = y + _brush_size.height();
            if (ysize > _mask->height() - 1)
                ysize = _mask->height() - 1;
            for (int i = x; i < xsize; i++)
            {
                for (int j = y; j < ysize; j++)
                {
                    _mask->setPixelColor(i, j, QColor(0, 0, 0, 0));
                }
            }
            updateModel();
        }
    }
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

QPainterPath RoiMaskGraphicsItem::shape() const
{

    QPainterPath path;
    path.addPolygon(_polygon);
    return path;

}

/*---------------------------------------------------------------------------*/

void RoiMaskGraphicsItem::updateModel()
{
    //calculate();
}

/*---------------------------------------------------------------------------*/

void RoiMaskGraphicsItem::updateView()
{
    int val = _alpha_value->getValue().toInt();

    val = std::min(val, 100);
    val = std::max(val, 0);

    _alpha_value->setValue(val);

    calculate();
    QRectF f = boundingRect();
    update(boundingRect());
}

/*---------------------------------------------------------------------------*/

QColor RoiMaskGraphicsItem::getColor() const
{
    QVariant variant = _color_ano->getValue();
    QColor color = variant.value<QColor>();
    return color;
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

void RoiMaskGraphicsItem::onMousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mouse_down = true;
        add_to_roi(event->scenePos());
        // Queue an update
        update();

        // Pass mouse event
        QGraphicsItem::mousePressEvent(event);
    }
}

/*---------------------------------------------------------------------------*/

void RoiMaskGraphicsItem::onMouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
   if(_mouse_down == true)
   {
       if (_draw_action == DRAW_ACTION_MODES::ADD)
       {
           add_to_roi(event->scenePos());
       }
       else if (_draw_action == DRAW_ACTION_MODES::ERASE)
       {
           erase_from_roi(event->screenPos());
       }

       QGraphicsItem::mouseMoveEvent(event);
   }
}

/*---------------------------------------------------------------------------*/

void RoiMaskGraphicsItem::onMouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mouse_down = false;

        // Queue an update
        //update();
        //emit(mask_updated(this, true));

        // Pass mouse event
        QGraphicsItem::mouseReleaseEvent(event);
    }
}

/*---------------------------------------------------------------------------*/
