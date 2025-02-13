/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/RectItem.h>
#include <gstar/ImageViewScene.h>

using namespace gstar;

//---------------------------------------------------------------------------

RectItem::RectItem(qreal x, qreal y, qreal w, qreal h,
                   QGraphicsItem* parent)
: QGraphicsItem(parent)
{

   // Initialize rectangle size
   m_rect = QRectF(0.0, 0.0, w, h);
   setPos(x, y);

   // Set movable and selectable flags
   setFlag(QGraphicsItem::ItemIsMovable);
   setFlag(QGraphicsItem::ItemIsSelectable);
   setFlag(QGraphicsItem::ItemSendsGeometryChanges);

   // Accept hover events
   setAcceptHoverEvents(true);

}

//---------------------------------------------------------------------------

QRectF RectItem::boundingRect() const
{

   // Return bounding rectangle
   return m_rect;

}

//---------------------------------------------------------------------------

char* RectItem::getMask()
{

   // Get bounding rectangle
   QRectF boundRect = ((ImageViewScene*) scene()) -> pixRect();

   // Get image dimensions
   int w = boundRect.width();
   int h = boundRect.height();

   // Check image dimensions
   if (w <= 0 || h <= 0) {
      return nullptr;
   }

   // Create new image mask
   char* mask = new char[w * h];

   // Loop through each pixel in image
   for (int r = 0 ; r < h ; r++) {
      for (int c = 0 ; c < w ; c++) {

         // Test if pixel is within ROI
         if (contains(mapFromScene(QPointF((float) c, (float) r)))) {
            mask[r * w + c] = (char)255;
         }
         else {
            mask[r * w + c] = (char)0;
         }

      }
   }

   // Return mask
   return mask;

}

//---------------------------------------------------------------------------

double RectItem::getSceneScale()
{

   // The view's scale
   double scale = 1.0;

   // Get list of views on this scene
   QList<QGraphicsView *> l = scene() -> views();

   // There are no views, so return 1.0
   if (l.isEmpty()) {
      return scale;
   }

   // Get the first view
   QGraphicsView* v = l[0];

   // Get the scale portion of its transform
   QTransform t = v -> transform();
   scale = t.m11();

   // Return the scale
   return scale;

}

//---------------------------------------------------------------------------

void RectItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{

   // Mark unused
   Q_UNUSED(event);

   // Return if item is not selected
   if (!isSelected()) {
      return;
   }

   // Set cursor
   setCursor(Qt::SizeAllCursor);

}

//---------------------------------------------------------------------------

void RectItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
/*
   // Mark unused
   Q_UNUSED(event);

   // Reset cursor
   unsetCursor();
*/
}

//---------------------------------------------------------------------------

void RectItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{

   // Return if item is not selected
   if (!isSelected()) {
      return;
   }
/*
   // Get scale factor from QGraphicsView
   double scale = getSceneScale();

   // Mouse click position (in item coordinates)
   QPointF pt = event -> pos();

   // Check for bottom right grip
   if (pt.x() <= m_rect.width() &&
       pt.y() <= m_rect.height() &&
       pt.x() >= m_rect.width() - 8 / scale &&
       pt.y() >= m_rect.height() - 8 / scale) {
      setCursor(Qt::SizeFDiagCursor);
   }

   // Reset cursor
   else {
      setCursor(Qt::SizeAllCursor);
   }
*/
}

//---------------------------------------------------------------------------

QVariant RectItem::itemChange(GraphicsItemChange change,
                              const QVariant& value)
{

   // Check for change in position
   // Only check for one of QGraphicsItem::ItemPositionChange or
   // QGraphicsItem::ItemPositionHasChanged to avoid infinite recursion
    if (change == QGraphicsItem::ItemPositionHasChanged) {

        if (scene() != nullptr)
        {
            // Get boundary item rectangle
            QRectF boundRect = ((ImageViewScene*)scene())->pixRect();

            // Check bounds
            setPos(qBound(boundRect.left(), pos().x(),
                boundRect.right() - m_rect.width()),
                qBound(boundRect.top(), pos().y(),
                    boundRect.bottom() - m_rect.height()));

        }
    }

   // Get ItemSelectedHasChanged change
   if (change == QGraphicsItem::ItemSelectedHasChanged) {

      // If this item is the newest one selected then
      // emit itemSelected signal, which should be connected
      // to ROIView, which in turn should deselect all other
      // selected items.
      if (isSelected() == true) {
         emit itemSelected(this);
      }

   }

   return QGraphicsItem::itemChange(change, value);

}

//---------------------------------------------------------------------------

void RectItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{

   // Current mouse position
   QPointF pt = event -> pos();

   // Last mouse position
   QPointF lpt = event -> lastPos();

   // Get scale factor from QGraphicsView
   double scale = getSceneScale();

   // Get boundary item rectangle
   QRectF boundRect = ((ImageViewScene*) scene()) -> pixRect();

   // Check for bottom right grip
   if (m_grip == BottomRight) {

      // Indicate geometry change about to occur
      prepareGeometryChange();

      // Check against top edge
      if (pt.y() < 8.0 / scale) {
         m_rect.setHeight(8.0 / scale);
      }
      else if (mapToScene(pt).y() >= boundRect.bottom()) {
         m_rect.setBottom(mapFromScene(boundRect.bottomRight()).y());
      }
      else {
         m_rect.setHeight(pt.y());
      }

      // Check against left edge
      if (pt.x() < 8.0 / scale) {
         m_rect.setWidth(8.0 / scale);
      }
      else if (mapToScene(pt).x() >= boundRect.right()) {
         m_rect.setRight(mapFromScene(boundRect.bottomRight()).x());
      }
      else {
         m_rect.setWidth(pt.x());
      }

      // Emit change
      emit itemChanged(this);

   }

   // No grip selected (this is a move)
   else {

      // Queue an update
      update();

      // Pass mouse position
      QGraphicsItem::mouseMoveEvent(event);

      // Check bounds
      setPos(qBound(boundRect.left(), pos().x(),
                    boundRect.right() - m_rect.width()),
             qBound(boundRect.top(), pos().y(),
                    boundRect.bottom() - m_rect.height()));

      // Emit change
      emit itemChanged(this);

   }

}

//---------------------------------------------------------------------------

void RectItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{

   // Get scale factor from QGraphicsView
   double scale = getSceneScale();

   // Mouse click position (in item coordinates)
   QPointF pt = event -> pos();

   // Check for bottom right grip
   if (pt.x() <= m_rect.width() &&
       pt.y() <= m_rect.height() &&
       pt.x() >= m_rect.width() - 8 / scale &&
       pt.y() >- m_rect.height() - 8 / scale) {
      m_grip = BottomRight;
   }

   // No grip selected
   else {
      m_grip = None;
   }

   // Queue an update
   update();

   // Pass mouse event
   QGraphicsItem::mousePressEvent(event);

}

//---------------------------------------------------------------------------

void RectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{

   // No grip selected
   m_grip = None;

   // Queue an update
   update();

   // Pass mouse event
   QGraphicsItem::mouseReleaseEvent(event);

}

//---------------------------------------------------------------------------

void RectItem::paint(QPainter* painter,
                     const QStyleOptionGraphicsItem* option,
                     QWidget* widget)
{

   // Mark unused
   Q_UNUSED(widget);

   // Get scale factor from QGraphicsView
   double scale = getSceneScale();

   // Save initial pen
   QPen oldPen = painter -> pen();

   // New pen for drawing ROI item
   QPen pen;

   // Make pen cosmetic
   pen.setCosmetic(true);

   // Set width and color
   pen.setWidthF(2.0);
   pen.setColor(m_color);
   painter -> setPen(pen);

   // Draw ROI as rect
   painter -> drawRect(m_rect);

   // Draw text string on top of ROI
   //painter -> drawText(QPointF(x1, y2), m_text);

   // Draw outline and grips if item is selected
   if (option -> state & QStyle::State_Selected) {

      // Set style, width and color
      pen.setStyle(Qt::DashLine);
      pen.setWidthF(1.0);
      pen.setColor(Qt::cyan);
      painter -> setPen(pen);

      // Draw outline as rectangle
      painter -> drawRect(m_rect);

      // Solid line for grips
      pen.setStyle(Qt::SolidLine);
      painter -> setPen(pen);

      // Draw grip
      QRectF grip(m_rect.right() - 8 / scale, 
                  m_rect.bottom() - 8 / scale,
                  8 / scale, 
                  8 / scale);
      painter -> drawRect(grip);

   }

   // Restore old pen: not sure if this is needed
   painter -> setPen(oldPen);

}

//---------------------------------------------------------------------------

void RectItem::setColor(const QColor& color)
{

   // Set color
   m_color = color;

   // Queue update
   update();

}

//---------------------------------------------------------------------------

void RectItem::setHeight(double height)
{

   // Prepare for change
   prepareGeometryChange();

   // Set width
   m_rect.setHeight(height);

}

//---------------------------------------------------------------------------

void RectItem::setText(const QString& text)
{

   // Set text
   m_text = text;

}

//---------------------------------------------------------------------------

void RectItem::setWidth(double width)
{

   // Prepare for change
   prepareGeometryChange();

   // Set width
   m_rect.setWidth(width);

}

//---------------------------------------------------------------------------

void RectItem::setX(double x)
{

   // Prepare for change
   prepareGeometryChange();

   // Set x
   setPos(x, pos().y());

}

//---------------------------------------------------------------------------

void RectItem::setY(double y)
{

   // Prepare for change
   prepareGeometryChange();

   // Set y
   setPos(pos().x(), y);

}

//---------------------------------------------------------------------------
