/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ROIMASKGI_H
#define ROIMASKGI_H

/*---------------------------------------------------------------------------*/

#include "gstar/Annotation/AbstractGraphicsItem.h"
#include "gstar/RectItem.h"
#include <opencv2/opencv.hpp>

/*---------------------------------------------------------------------------*/

namespace gstar
{

    
    enum class DRAW_ACTION_MODES { OFF, ADD, ERASE };

/**
 * @brief The RulerGraphicsItem class
 */
class RoiMaskGraphicsItem : public AbstractGraphicsItem
{
    Q_OBJECT

public:

    /**
    * @brief RulerGraphicsItem
    * @param parent
    */
    // blank constructor for classid
   RoiMaskGraphicsItem(cv::Mat& mat, int idx, QColor col, AbstractGraphicsItem* parent = 0);

   RoiMaskGraphicsItem(int rows, int cols, QColor col, AbstractGraphicsItem* parent = 0);

   RoiMaskGraphicsItem(QImage mask, QColor color, int alpha, AbstractGraphicsItem* parent = 0);

   ~RoiMaskGraphicsItem();

   /**
    * @brief className
    * @return
    */
   const QString displayName() const;

   /**
    * @brief duplicate
    * @return
    */
   AbstractGraphicsItem* duplicate();

   /**
    * @brief boundingRect
    * @return
    */
   QRectF boundingRect() const;

   QColor getColor() const;

   /**
    * @brief boundingRectMarker
    * @return
    */
   virtual QRectF boundingRectMarker() const;

   void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );

   //bool isEnabled() { return _color_ano->getValue().toBool(); }

   int alphaValue() { return _alpha_value->getValue().toInt(); }

   QImage* image_mask() { return _mask; }

   QString getName();

   void to_roi_vec(std::vector<std::pair<unsigned int, unsigned int>>& rois);
 
   void add_to_roi(QPointF pos);

   void erase_from_roi(QPointF pos);

   void setMaskSize(QRectF size);

   void setBrushSize(QSize brushSize);

   void setDrawAction(DRAW_ACTION_MODES action_mode) { _draw_action = action_mode; }

   QPainterPath shape() const;

   virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);

   virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

public slots:
   /**
    * @brief updateModel
    */
   void updateModel();

   /**
    * @brief updateView
    */
   void updateView();

   /**
    * @brief calculate
    */
   void calculate();

   /**
    * Reimplemented from QGraphicsItem. See Qt documentation.
    */
   void onMouseMoveEvent(QGraphicsSceneMouseEvent* event);

   /**
    * Reimplemented from QGraphicsItem. See Qt documentation.
    */
   void onMousePressEvent(QGraphicsSceneMouseEvent* event);

   /**
    * Reimplemented from QGraphicsItem. See Qt documentation.
    */
   void onMouseReleaseEvent(QGraphicsSceneMouseEvent* event);

protected:

    void _init(QColor color, int alpha);

    AnnotationProperty* _color_ano;

    AnnotationProperty* _alpha_value;

    QImage* _mask;

    QSize _brush_size;

    QPolygon _polygon;

    DRAW_ACTION_MODES _draw_action;

    bool _mouse_down;

    RectItem* _cursor;
};

}

/*---------------------------------------------------------------------------*/

#endif // RULERGRAPHICSITEM_H

/*---------------------------------------------------------------------------*/
