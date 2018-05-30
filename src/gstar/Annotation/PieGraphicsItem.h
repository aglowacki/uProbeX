/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PIEGRAPHICSITEM_H
#define PIEGRAPHICSITEM_H

/*---------------------------------------------------------------------------*/

#include "AbstractGraphicsItem.h"

/*---------------------------------------------------------------------------*/

namespace gstar
{

class AnnotationProperty;

/**
 * @brief The PieGraphicsItem class
 */
class PieGraphicsItem : public AbstractGraphicsItem
{
public:

    /**
    * @brief PieGraphicsItem
    * @param parent
    */
   PieGraphicsItem(AbstractGraphicsItem* parent = 0);

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
    * @brief className
    * @return
    */
   const QString displayName() const;

   //void setSize(QSize size);

   /**
    * @brief getPos
    * @return
    */
   QPoint getPos();

   /**
    * @brief setRadius
    * @param radius
    */
   void setRadius(double radius);

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
    * @brief m_positionXProp
    */
   AnnotationProperty* m_positionXProp;

   /**
    * @brief m_positionYProp
    */
   AnnotationProperty* m_positionYProp;

   /**
    * @brief m_outlineColor
    */
   QColor m_outlineColor;

   /**
    * @brief m_radius
    */
   double m_radius;

   /**
    * @brief m_radiusProp
    */
   AnnotationProperty* m_radiusProp;

   /**
    * @brief m_startAngle
    */
   double m_startAngle;

   /**
    * @brief m_startAngleProp
    */
   AnnotationProperty* m_startAngleProp;

   /**
    * @brief m_span
    */
   double m_span;

   /**
    * @brief m_spanProp
    */
   AnnotationProperty* m_spanProp;

};

}

/*---------------------------------------------------------------------------*/

#endif // PIEGRAPHICSITEM_H

/*---------------------------------------------------------------------------*/
