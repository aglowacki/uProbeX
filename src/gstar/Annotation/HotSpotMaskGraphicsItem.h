/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef HOTSPOTMASKGI_H
#define HOTSPOTMASKGI_H

/*---------------------------------------------------------------------------*/

#include "LineGraphicsItem.h"

/*---------------------------------------------------------------------------*/

namespace gstar
{

/**
 * @brief The RulerGraphicsItem class
 */
class HotSpotMaskGraphicsItem : public AbstractGraphicsItem
{
    Q_OBJECT

public:

    /**
    * @brief RulerGraphicsItem
    * @param parent
    */
   HotSpotMaskGraphicsItem(int width, int height, AbstractGraphicsItem* parent = 0);

   ~HotSpotMaskGraphicsItem();

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

   bool isEnabled() { return _enable_mask->getValue().toBool(); }

   int alphaValue() { return _alpha_value->getValue().toInt(); }

   QImage* image_mask() { return _mask; }

   std::vector<QPoint> get_mask_list();

   QString getName();

signals:

   void mask_updated(HotSpotMaskGraphicsItem* ano, bool reload);

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

   AnnotationProperty* _enable_mask;

   AnnotationProperty* _display_mask;

   AnnotationProperty* _draw_mask;

   AnnotationProperty* _erase_mask;

   AnnotationProperty* _alpha_value;

   QImage* _mask;

   bool _mouse_down;
};

}

/*---------------------------------------------------------------------------*/

#endif // RULERGRAPHICSITEM_H

/*---------------------------------------------------------------------------*/
