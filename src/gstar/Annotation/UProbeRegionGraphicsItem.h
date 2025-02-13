/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef UPROBEREGIONGRAPHICSITEM_H
#define UPROBEREGIONGRAPHICSITEM_H

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
#include <QPixmap>
#include "gstar/Annotation/AbstractGraphicsItem.h"
#include "gstar/AnnotationProperty.h"
#include "gstar/CoordinateModel.h"
#include "gstar/Annotation/UProbeMarkerGraphicsItem.h"
//---------------------------------------------------------------------------

namespace gstar
{

class UProbeRegionGraphicsItem : public AbstractGraphicsItem
{

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
   UProbeRegionGraphicsItem(AbstractGraphicsItem* parent = 0);

   /**
   * @brief MarkerGraphicsItem
   * @param parent
   * @param marker infomation
   */
   UProbeRegionGraphicsItem(QMap<QString, QString>& marker,
                            AbstractGraphicsItem* parent = 0);

   virtual ~UProbeRegionGraphicsItem();

   /**
    * @brief coneRegion
    * @param uProbeRegion
    * @return
    */
   virtual UProbeRegionGraphicsItem* cloneRegion();

   /**
    * Get item's boudning rectangle
    *
    * @return bounding rectangle
    */
   QRectF boundingRect() const;

   /**
    * @brief boundingRectMarker
    * @return
    */
   QRectF boundingRectMarker() const;

   /**
    * @brief getUProbeName
    * @return
    */
   QString getUProbeName();

   /**
    * @brief getFactorX
    * @return
    */
   double getFactorX();

   /**
    * @brief getFactorY
    * @return
    */
   double getFactorY();

   /**
    * @brief getHeight
    * @return
    */
   double getHeight();

   /**
    * @brief getWidth
    * @return
    */
   double getWidth();

   /**
    * @brief getX
    * @return
    */
   double getX();

   /**
    * @brief getY
    * @return
    */
   double getY();

   QString getValueAsString(QString prop);

   /**
    * @brief initialStringSize
    */
   void updateStringSize();

   /**
    * Draw the item. Reimplemented from QGraphicsItem. See Qt documentation.
    */
   void paint(QPainter*painter,
              const QStyleOptionGraphicsItem* option,
              QWidget* widget);

   /**
    * @brief setMouseOverPixelCoordModel
    * @param model
    */
   void setMouseOverPixelCoordModel(CoordinateModel* model);

   /**
    * @brief setLightToMicroCoordModel
    * @param model
    */
   void setLightToMicroCoordModel(CoordinateModel* model);

   /**
    * @brief setGripSize
    */
   void setGripSize();

   /**
    * @brief setSize
    * @param size
    */
   void setSize(double size);

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
    * Set item's rect
    *
    * @param rect - rect
    */
   void setSameRect(QRectF& rect);

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
    * @brief calculate
    */
   void calculate();

   /**
    * @brief updateModel
    */
   void updateModel();

   /**
    * @brief updateView
    */
   void updateView();

   /**
     * Perform a zoom to this item on scene.
     */
   void zoomToRegion();

signals:

   /**
    * Signal to indicate that this item's geometry or position has changed.
    *
    * @param pointer to this item
    */
   //void itemIsChanged(UProbeRegionGraphicsItem*);

   /**
    * Signal to indicate that this item has been selected
    *
    * @param pointer to this item
    */
   //void itemSelected(RectItem*);

protected:

   /**
    * Possible selected grips.
    */
   enum Grip {
      None,          // No grip selected.
      BottomRight,   // Bottom right grip selected.
      BottomLeft,     // Bottom left grip selected.
      TopLeft,
      TopRight
   };

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
    * Set scale factor from QGraphicsView for the scene this item is in
    *
    */
   void initialScale();

   /**
    * Standard function to generate the font pixel size for a given scale.
    *
    * @param scale - the decimal percentage of the scene that will be shown to the user.
    * @return
    */
   int predictFontPixelSizeByScale(qreal scale);

protected:

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
    * @brief m_mouseOverPixelCoordModel
    */
   CoordinateModel* m_mouseOverPixelCoordModel;

   /**
    * @brief m_lightToMicroCoordModel
    */
   CoordinateModel* m_lightToMicroCoordModel;

   /**
    * @brief m_outlineColor
    */
   QColor m_outlineColor;

   /**
    * @brief m_font
    */
   QFont m_font;

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
    * @brief m_positionZProp
    */
   AnnotationProperty* m_positionZProp;

   /**
    * @brief m_predictXProp
    */
   AnnotationProperty* m_predictXProp;

   /**
    * @brief m_predictXProp
    */
   AnnotationProperty* m_predictYProp;

   /**
    * @brief m_widthProp
    */
   AnnotationProperty* m_widthProp;

   /**
    * @brief m_heightProp
    */
   AnnotationProperty* m_heightProp;

   /**
    * @brief m_predictXProp
    */
   AnnotationProperty* m_measuredXProp;

   /**
    * @brief m_predictXProp
    */
   AnnotationProperty* m_measuredYProp;

   /**
    * @brief m_size
    */
   double m_size;

   /**
    * @brief m_gripSize
    */
   double m_gripSize;
   /**
    * @brief m_sizeProp
    */
   AnnotationProperty* m_sizeProp;

   /**
    * @brief m_lastStringWidth
    */
   int m_lastStringWidth;

};

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
