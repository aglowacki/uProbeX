/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef LINEGRAPHICSITEM_H
#define LINEGRAPHICSITEM_H

/*---------------------------------------------------------------------------*/

#include "AbstractGraphicsItem.h"

/*---------------------------------------------------------------------------*/

class QGraphicsRectItem;

namespace gstar
{

/**
 * @brief The LineGraphicsItem class
 */
class LineGraphicsItem : public AbstractGraphicsItem
{

public:

   /**
    * @brief LineGraphicsItem
    * @param parent
    */
   LineGraphicsItem(AbstractGraphicsItem* parent = 0);

   /**
    *
    */
   ~LineGraphicsItem();

   /**
    * @brief className
    * @return
    */
   const QString displayName() const;

   /**
    * @brief endNodePos
    * @return
    */
   QPointF endNodePos();

   /**
    * @brief boundingRect
    * @return
    */
   QRectF boundingRect() const;
   /**
    * @brief boundingRectMarker
    * @return
    */
   QRectF boundingRectMarker() const;

   /**
    * @brief paint
    * @param painter
    * @param option
    * @param widget
    */
   void paint(QPainter* painter,
              const QStyleOptionGraphicsItem* option,
              QWidget* widget);

   /**
    * @brief setLinkColor
    * @param color
    */
   void setLinkColor(QColor color);

   /**
    * @brief shape
    * @return
    */
   QPainterPath shape() const;

   /**
    * @brief setEndPos
    * @param pos
    */
   void setEndPos(QPointF pos);

   /**
    * @brief setLine
    * @param line
    */
   void setLine(QLineF line);

   /**
    * @brief setStartPos
    * @param pos
    */
   void setStartPos(QPointF pos);

   /**
    * @brief startNodePos
    * @return
    */
   QPointF startNodePos();

public slots:

   /**
    * @brief updateModel
    */
   void updateModel();

   /**
    * @brief updateView
    */
   void updateView();

protected:

   /**
    * @brief checkPos
    * @param newPos
    * @return
    */
   bool checkBoundPos(QPointF newPos, QPointF offset);

   /**
    * @brief connectAll
    */
   void connectAll();

   /**
    * @brief disconnectAll
    */
   void disconnectAll();

   /**
    * @brief Stop the GraphicsItems from leaving the scene
    * @param change
    * @param value
    * @return
    */
   QVariant itemChange(GraphicsItemChange change, const QVariant& value);

   /**
    * @brief mouseMoveEvent
    * @param event
    */
   void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

   /**
    * @brief mousePressEvent
    * @param event
    */
   void mousePressEvent(QGraphicsSceneMouseEvent *event);

   /**
    * @brief mouseReleaseEvent
    * @param event
    */
   void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

   /**
    * @brief textBoundingBox
    * @return
    */
   QRectF textBoundingRect() const;

protected:

   enum MoveType {LINE, P1, P2};

   /**
    * @brief m_endXProp
    */
   AnnotationProperty* m_endXProp;

   /**
    * @brief m_endYProp
    */
   AnnotationProperty* m_endYProp;

   /**
    * @brief m_isMovable
    */
   bool m_isMovable;

   /**
    * @brief m_isSelected
    */
   bool m_isSelected;

   /**
    * @brief m_line
    */
   QGraphicsLineItem* m_line;

   /**
    * @brief m_lineColor
    */
   QColor m_lineColor;

   /**
    * @brief m_lineColorProp
    */
   AnnotationProperty* m_lineColorProp;

   /**
    * @brief m_moveType
    */
   MoveType m_moveType;

   /**
    * @brief m_nodeSize
    */
   int m_nodeSize;

   /**
    * @brief m_startXProp
    */
   AnnotationProperty* m_startXProp;

   /**
    * @brief m_startYProp
    */
   AnnotationProperty* m_startYProp;

   /**
    * @brief m_text
    */
   QString m_text;

};

}

/*---------------------------------------------------------------------------*/

#endif // RULERGI_H

/*---------------------------------------------------------------------------*/
