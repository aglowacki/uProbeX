/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef EMPTYGRAPHICSITEM_H
#define EMPTYGRAPHICSITEM_H

//---------------------------------------------------------------------------

#include "AbstractGraphicsItem.h"

#include <QRectF>

//---------------------------------------------------------------------------

namespace gstar
{

/**
 * @brief The EmptyGraphicsItem class is used for creating headers rows in the
 *          GraphicsModel. Thsese items will not draw anything in the scene.
 */
class EmptyGraphicsItem : public AbstractGraphicsItem
{

public:

    /**
    * @brief EmptyGraphicsItem : Constructor.
    * @param parent
    */
   EmptyGraphicsItem(AbstractGraphicsItem* parent = 0);

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

};

}

//---------------------------------------------------------------------------

#endif // EMPTYGRAPHICSITEM_H

//---------------------------------------------------------------------------
