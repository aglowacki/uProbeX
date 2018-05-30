/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MARKERGRAPHICSITEM_H
#define MARKERGRAPHICSITEM_H

/*---------------------------------------------------------------------------*/

#include "AbstractGraphicsItem.h"

#include <QPolygon>

/*---------------------------------------------------------------------------*/

namespace gstar
{

/**
 * @brief The MarkerGraphicsItem class
 */
class MarkerGraphicsItem : public AbstractGraphicsItem
{

public:

    /**
    * @brief MarkerGraphicsItem
    * @param parent
    */
   MarkerGraphicsItem(AbstractGraphicsItem* parent = 0);

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
    * @brief paint
    * @param painter
    * @param option
    * @param widget
    */
   void paint(QPainter* painter,
              const QStyleOptionGraphicsItem* option,
              QWidget* widget);

   /**
    * @brief setSize
    * @param size
    */
   void setSize(double size);

   /**
    * @brief setColor
    * @param color
    */
   void setColor(QColor color);

   /**
    * @brief shape
    * @return
    */
   QPainterPath shape() const;

public slots:

   /**
    * @brief updateModel
    */
   void updateModel();

   /**
    * @brief updateView
    */
   void updateView();

protected slots:

   /**
    * @brief calculate
    */
   void calculate();

private:

   /**
    * @brief m_outlineColor
    */
   QColor m_outlineColor;

   /**
    * @brief m_outlineColorProp
    */
   AnnotationProperty* m_outlineColorProp;

   /**
    * @brief m_polygon
    */
   QPolygon m_polygon;

   /**
    * @brief m_positionXProp
    */
   AnnotationProperty* m_positionXProp;

   /**
    * @brief m_positionYProp
    */
   AnnotationProperty* m_positionYProp;

   /**
    * @brief m_size
    */
   double m_size;

   /**
    * @brief m_sizeProp
    */
   AnnotationProperty* m_sizeProp;

};

}

/*---------------------------------------------------------------------------*/

#endif // MARKERGRAPHICSITEM_H

/*---------------------------------------------------------------------------*/
