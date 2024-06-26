/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_RECT_ITEM_H
#define GSTAR_RECT_ITEM_H

//---------------------------------------------------------------------------

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QImage>
#include <QList>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionGraphicsItem>

//---------------------------------------------------------------------------

namespace gstar
{

class RectItem
: public QObject, public QGraphicsItem
{

   Q_OBJECT
   Q_INTERFACES(QGraphicsItem)

public:

   /**
    * Constructor.
    *
    * @param x - x position
    * @param y - y position
    * @param w - width
    * @param h - height
    * @param parent - parent Qt widget
    */
   RectItem(qreal x = 0.0, qreal y = 0.0, qreal w = 1.0, qreal h = 1.0,
            QGraphicsItem* parent = 0);

   /**
    * Get item's boudning rectangle
    *
    * @return bounding rectangle
    */
   QRectF boundingRect() const;

   /**
    * Get a mask for the ROI.
    *
    * The mask image is the same size as the bounding item. The mask is one
    * byte per pixel with values of 255 where the ROI overlaps. The mask is
    * in row major order. The called MUST delete the memory using delete [].
    *
    * @return ROI mask
    */
   char* getMask();


   /**
    * Draw the item. Reimplemented from QGraphicsItem. See Qt documentation.
    */
   void paint(QPainter*painter,
              const QStyleOptionGraphicsItem* option,
              QWidget* widget);

   /**
    * Set item's color
    *
    * @param color - item color
    */
   void setColor(const QColor& color);

   /**
    * Set item's height
    *
    * @param height - height
    */
   void setHeight(double height);

   /**
    * Set item's descriptive text
    *
    * @param text - descriptive text
    */
   void setText(const QString& text);

   /**
    * Set item's width
    *
    * @param width - width
    */
   void setWidth(double width);

   /**
    * Set item's x position
    *
    * @param x - x position
    */
   void setX(double x);

   /**
    * Set item's y position
    *
    * @param y - y position
    */
   void setY(double y);

signals:

   /**
    * Signal to indicate that this item's geometry or position has changed.
    *
    * @param pointer to this item
    */
   void itemChanged(RectItem*);

   /**
    * Signal to indicate that this item has been selected
    *
    * @param pointer to this item
    */
   void itemSelected(RectItem*);

protected:

   /**
    * Possible selected grips.
    */
   enum Grip {
      None,          /*!< No grip selected. */
      BottomRight    /*!< Bottom right grip selected. */
   };

   /**
    * Reimplemented from QGraphicsItem. See Qt documentation.
    */
   void hoverEnterEvent(QGraphicsSceneHoverEvent *event);

   /**
    * Reimplemented from QGraphicsItem. See Qt documentation.
    */
   void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

   /**
    * Reimplemented from QGraphicsItem. See Qt documentation.
    */
   void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

   /**
    * Reimplemented from QGraphicsItem. See Qt documentation.
    */
   QVariant itemChange(GraphicsItemChange change, const QVariant& value);

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

private:

   /**
    * Get scale factor from QGraphicsView for the scene this item is in
    *
    * @return view's scale factor
    */
   double getSceneScale();

private:

   /**
    * The boudning graphics item for this item
    */
   QGraphicsItem* m_bound;

   /**
    * Item color
    */
   QColor m_color;

   /**
    * Item grip
    */
   Grip m_grip;

   /**
    * Item's size rectangle
    */
   QRectF m_rect;

   /**
    * Item text
    */
   QString m_text;

};

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
