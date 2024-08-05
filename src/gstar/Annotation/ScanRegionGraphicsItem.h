/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SCAN_REGION_GRAPHICSITEM_H
#define SCAN_REGION_GRAPHICSITEM_H

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
#include "gstar/Annotation/UProbeRegionGraphicsItem.h"
#include "mvc/ScanRegionDialog.h"
#include "mvc/BlueSkyPlan.h"

//---------------------------------------------------------------------------

namespace gstar
{

class ScanRegionGraphicsItem : public UProbeRegionGraphicsItem
{

Q_OBJECT

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
   ScanRegionGraphicsItem(std::map<QString, BlueskyPlan> * avail_scans, AbstractGraphicsItem* parent = 0);

   /**
   * @brief MarkerGraphicsItem
   * @param parent
   * @param marker infomation
   */
   ScanRegionGraphicsItem(QMap<QString, QString>& marker,
                            AbstractGraphicsItem* parent = 0);

   /**
    * @brief coneRegion
    * @param uProbeRegion
    * @return
    */
   virtual ScanRegionGraphicsItem* cloneRegion();

   virtual QDialog* get_custom_dialog();

   const QString displayName() const;
/*
   QRectF boundingRect() const;

   QRectF boundingRectMarker() const;

   QString getUProbeName();

   double getFactorX();

   double getFactorY();

   double getHeight();

   double getWidth();

   double getX();

   double getY();

   void updateStringSize();

   void paint(QPainter*painter,
              const QStyleOptionGraphicsItem* option,
              QWidget* widget);

   void setMouseOverPixelCoordModel(CoordinateModel* model);

   void setLightToMicroCoordModel(CoordinateModel* model);

   void setGripSize();

   void setSize(double size);

   void setColor(const QColor& color);

   void setHeight(double height);

   void setSameRect(QRectF& rect);

   void setWidth(double width);

   void setX(double x);

   void setY(double y);

   const QString displayName() const;

   AbstractGraphicsItem* duplicate();

   void calculate();

   void updateModel();

   void updateView();

   void zoomToRegion();

signals:

protected:

   enum Grip {
      None,          // No grip selected.
      BottomRight,   // Bottom right grip selected.
      BottomLeft,     // Bottom left grip selected.
      TopLeft,
      TopRight
   };

   void hoverEnterEvent(QGraphicsSceneHoverEvent *event);

   void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

   void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

   void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

   void mousePressEvent(QGraphicsSceneMouseEvent* event);

   void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
*/

public slots:

   void onScanUpdated(); 

private:
   ScanRegionDialog _scan_dialog;

   std::map<QString, BlueskyPlan> *_avail_scans;
/*
   void initialScale();

   int predictFontPixelSizeByScale(qreal scale);

private:

   QGraphicsItem* m_bound;

   QColor m_color;

   Grip m_grip;

   QRectF m_rect;

   CoordinateModel* m_mouseOverPixelCoordModel;

   CoordinateModel* m_lightToMicroCoordModel;

   QColor m_outlineColor;

   QFont m_font;

   AnnotationProperty* m_outlineColorProp;

   QPolygon m_polygon;

   AnnotationProperty* m_positionXProp;

   AnnotationProperty* m_positionYProp;

   AnnotationProperty* m_positionZProp;

   AnnotationProperty* m_predictXProp;

   AnnotationProperty* m_predictYProp;

   AnnotationProperty* m_widthProp;

   AnnotationProperty* m_heightProp;

   AnnotationProperty* m_measuredXProp;

   AnnotationProperty* m_measuredYProp;

   double m_size;

   double m_gripSize;

   AnnotationProperty* m_sizeProp;

   int m_lastStringWidth;
*/
};

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
