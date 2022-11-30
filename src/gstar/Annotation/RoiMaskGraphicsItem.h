/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ROIMASKGI_H
#define ROIMASKGI_H

/*---------------------------------------------------------------------------*/

#include "LineGraphicsItem.h"
#include <opencv2/opencv.hpp>

/*---------------------------------------------------------------------------*/

namespace gstar
{

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
   RoiMaskGraphicsItem(cv::Mat& mat, int idx, QColor col, AbstractGraphicsItem* parent = 0);

   RoiMaskGraphicsItem(QImage mask, QColor color, AbstractGraphicsItem* parent = 0);

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

   /**
    * @brief boundingRectMarker
    * @return
    */
   virtual QRectF boundingRectMarker() const;

   void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );

   bool isEnabled() { return _color_ano->getValue().toBool(); }

   int alphaValue() { return _alpha_value->getValue().toInt(); }

   QImage* image_mask() { return _mask; }

   std::vector<QPoint> get_mask_list();

   QString getName();

signals:

   void mask_updated(RoiMaskGraphicsItem* ano, bool reload);

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

   void drawmask_changed();

   void erasemask_changed();

protected:

   /**
    * Reimplemented from QGraphicsItem. See Qt documentation.
    */
   void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

   /**
    * Reimplemented from QGraphicsItem. See Qt documentation.
    */
   void mousePressEvent(QGraphicsSceneMouseEvent* event);

   /**
    * Reimplemented from QGraphicsItem. See Qt documentation.
    */
   void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

   AnnotationProperty* _color_ano;

   AnnotationProperty* _alpha_value;

   QImage* _mask;

   bool _mouse_down;
};

}

/*---------------------------------------------------------------------------*/

#endif // RULERGRAPHICSITEM_H

/*---------------------------------------------------------------------------*/
