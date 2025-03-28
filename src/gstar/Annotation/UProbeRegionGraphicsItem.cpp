/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/Annotation/UProbeRegionGraphicsItem.h"
#include "gstar/GStarResource.h"
#include <gstar/ImageViewScene.h>
#include <QSize>
#include <iostream>
#include "mvc/ScanRegionLinkDialog.h"

#include <QApplication>

using namespace gstar;

//---------------------------------------------------------------------------

UProbeRegionGraphicsItem::UProbeRegionGraphicsItem(AbstractGraphicsItem* parent)
   : AbstractGraphicsItem(parent)
{

   m_mouseOverPixelCoordModel = nullptr;
   m_lightToMicroCoordModel = nullptr;

   m_outlineColor = QColor(255, 0, 127);
   m_rect = QRectF(-300, -300, 600, 600);

   // The grip size of the grip rect
   setGripSize();
   m_outlineColorProp = new AnnotationProperty(UPROBE_COLOR, m_outlineColor);

   m_predictXProp = new AnnotationProperty(UPROBE_CENTER_POS_X, 0.0);
   m_predictYProp = new AnnotationProperty(UPROBE_CENTER_POS_Y, 0.0);

   m_widthProp = new AnnotationProperty(UPROBE_WIDTH, 0.0);
   m_heightProp = new AnnotationProperty(UPROBE_HEIGHT, 0.0);

   m_measuredXProp = new AnnotationProperty(UPROBE_MICRO_POS_X, "0.0");
   m_measuredYProp = new AnnotationProperty(UPROBE_MICRO_POS_Y, "0.0");

   m_sizeProp = new AnnotationProperty(UPROBE_SIZE, 20.0);   

   m_data.push_back(m_outlineColorProp);

   m_data.push_back(m_predictXProp);
   m_data.push_back(m_predictYProp);

   m_data.push_back(m_widthProp);
   m_data.push_back(m_heightProp);

   setSize(m_sizeProp->getValue().toDouble());

   connectAllProperties();

   // Initialize rectangle size
   setFlag(QGraphicsItem::ItemSendsGeometryChanges);
   // Accept hover events
   setAcceptHoverEvents(true);

   initialScale();
   updateStringSize();

}

//---------------------------------------------------------------------------

UProbeRegionGraphicsItem::UProbeRegionGraphicsItem(QMap<QString, QString>& marker,
                                                   AbstractGraphicsItem* parent)
                                                   : AbstractGraphicsItem(parent)
{

   m_mouseOverPixelCoordModel = nullptr;
   m_lightToMicroCoordModel = nullptr;


   m_outlineColor = QColor(marker[UPROBE_COLOR]);
   m_rect = QRectF(marker["TopLeftX"].toDouble(),
                   marker["TopLeftY"].toDouble(),
                   marker["RectWidth"].toDouble(),
                   marker["RectHeight"].toDouble());
   setGripSize();
   m_outlineColorProp = new AnnotationProperty(UPROBE_COLOR, m_outlineColor);
   m_predictXProp = new AnnotationProperty(UPROBE_CENTER_POS_X, marker[UPROBE_PRED_POS_X]);
   m_predictYProp = new AnnotationProperty(UPROBE_CENTER_POS_Y, marker[UPROBE_PRED_POS_Y]);
   m_widthProp = new AnnotationProperty(UPROBE_WIDTH, 0.0);
   m_heightProp = new AnnotationProperty(UPROBE_HEIGHT, 0.0);
   m_sizeProp = new AnnotationProperty(UPROBE_SIZE, 20.0);    

   m_data.push_back(m_outlineColorProp);
   m_data.push_back(m_predictXProp);
   m_data.push_back(m_predictYProp);
   m_data.push_back(m_widthProp);
   m_data.push_back(m_heightProp);


   setSize(m_sizeProp->getValue().toDouble());

   connectAllProperties();

   setFlag(QGraphicsItem::ItemSendsGeometryChanges);
   // Accept hover events
   setAcceptHoverEvents(true);

   initialScale();
   updateStringSize();

}

//---------------------------------------------------------------------------

UProbeRegionGraphicsItem::~UProbeRegionGraphicsItem()
{

}

//---------------------------------------------------------------------------

UProbeRegionGraphicsItem* UProbeRegionGraphicsItem::cloneRegion()
{
   UProbeRegionGraphicsItem* newRegion = new UProbeRegionGraphicsItem();

   newRegion->m_outlineColor = m_outlineColor;
   newRegion->m_rect = m_rect;

   return newRegion;
}

//---------------------------------------------------------------------------

QString UProbeRegionGraphicsItem::getUProbeName()
{
   QVariant value = this->propertyValue(UPROBE_NAME);
   if (value.typeId()  == QMetaType::QString) {
      return value.toString();
   }
   return nullptr;
}

//---------------------------------------------------------------------------

QRectF UProbeRegionGraphicsItem::boundingRect() const
{

   QFontMetrics fm(m_font);
   int currentStringWidth=fm.horizontalAdvance(this->propertyValue(UPROBE_NAME).toString());

   int width;
   if(currentStringWidth < m_lastStringWidth)
   {
      width = m_lastStringWidth;
   }
   else
   {
      width = currentStringWidth;
   }

   // Make the region width 4 pixel more for gap between string and region
   qreal regionWidth = width + m_rect.width()+4;
   qreal regionHeight = m_rect.height();
   QSizeF regionSize = QSizeF(regionWidth, regionHeight);

   // Verify that item is in a scene
   if (scene() != 0) {
      QRectF scenceRect = scene()->sceneRect();
      if(mapToScene(m_rect.center()).x() < scenceRect.center().x())
      {
         return QRectF(m_rect.topLeft(),
                       regionSize);
      }
      else
      {
         qreal textLeftPosition = m_rect.left()-1-width;
         return QRectF(QPointF(textLeftPosition, m_rect.top()),
                       regionSize);
      }
   }
   return QRectF();
}

//---------------------------------------------------------------------------

QRectF UProbeRegionGraphicsItem::boundingRectMarker() const
{

   return m_rect;

}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::calculate()
{

   //nothing to calculate

}

//---------------------------------------------------------------------------

const QString UProbeRegionGraphicsItem::displayName() const
{

   const QString name = QString("Micro Probe Region");
   return name;

}

//---------------------------------------------------------------------------

AbstractGraphicsItem* UProbeRegionGraphicsItem::duplicate()
{

   UProbeRegionGraphicsItem* item = new UProbeRegionGraphicsItem();

   QColor color(m_outlineColorProp->getValue().toString());
   item->setColor(color);
   item->setPos(pos());
   item->setSameRect(m_rect);

   item->setMouseOverPixelCoordModel(m_mouseOverPixelCoordModel);
   item->setLightToMicroCoordModel(m_lightToMicroCoordModel);
   return item;

}

//---------------------------------------------------------------------------

double UProbeRegionGraphicsItem::getFactorX()
{

   //TODO: add annotation property
   return 1.0;

}

//---------------------------------------------------------------------------

double UProbeRegionGraphicsItem::getFactorY()
{

   //TODO: add annotation property
   return 1.0;

}

//---------------------------------------------------------------------------

double UProbeRegionGraphicsItem::getHeight()
{

   return m_heightProp->getValue().toDouble();

}

//---------------------------------------------------------------------------

double UProbeRegionGraphicsItem::getWidth()
{

   return m_widthProp->getValue().toDouble();

}

//---------------------------------------------------------------------------

double UProbeRegionGraphicsItem::getX()
{

   return m_predictXProp->getValue().toDouble();

}

//---------------------------------------------------------------------------

double UProbeRegionGraphicsItem::getY()
{

   return m_predictYProp->getValue().toDouble();

}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::initialScale()
{

   QTransform trans = getFirstViewTransform();
      if (trans.isScaling())
      {
         bool isInvertable = false;
         QTransform invTrans = trans.inverted(&isInvertable);
         if (isInvertable)
         {
            double xScale = invTrans.m11();
            double s = scale();
            if (s != xScale)
            {
               setScale(100*xScale);
            }
         }
      }

}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{

   // Current mouse position
   QPointF pt = event -> pos();

   // Last mouse position
   QPointF lpt = event -> lastPos();

   // Get boundary item rectangle
   QRectF boundRect = ((ImageViewScene*) scene()) -> pixRect();

   // User interacted with a resize grip
   if (m_grip != None) {
      prepareGeometryChange();
      double last_y = m_rect.y();
      double last_x = m_rect.x();

      // Perform resize based on the edge the user is moving & limit within parent.

      // Update Height
      if (m_grip == TopLeft || m_grip == TopRight) {
         if (mapToScene(pt).y() <= boundRect.top()) {
            m_rect.setTop(mapFromScene(boundRect.topLeft()).y());
         } else {
            m_rect.setY(pt.y());
         }
      }
      if (m_grip == BottomLeft || m_grip == BottomRight) {
         if (mapToScene(pt).y() >= boundRect.bottom()) {
            m_rect.setBottom(mapFromScene(boundRect.bottomLeft()).y());
         } else {
            m_rect.setHeight(pt.y() - m_rect.top());
         }
      }

      // Update Width
      if (m_grip == BottomLeft || m_grip == TopLeft) {
         if (mapToScene(pt).x() <= boundRect.left()) {
            m_rect.setLeft(mapFromScene(boundRect.topLeft()).x());
         } else {
            m_rect.setX(pt.x());
         }
      }
      if (m_grip == BottomRight || m_grip == TopRight) {
         if (mapToScene(pt).x() >= boundRect.right()) {
            m_rect.setRight(mapFromScene(boundRect.topRight()).x());
         }  else {
            m_rect.setWidth(pt.x() - m_rect.left());
         }
      }

      // Enforce a minimum size
      if (m_rect.width() < 5) {
         m_rect.setX(last_x);
         m_rect.setWidth(5);
      }
      if (m_rect.height() < 5) {
         m_rect.setY(last_y);
         m_rect.setHeight(5);
      }
      viewChanged();
   }
   else 
   {
      update();
      // Pass mouse position
      QGraphicsItem::mouseMoveEvent(event);
      setPos(pos().x(), pos().y());
      // Check bounds
      /*      setPos(qBound(boundRect.left(), pos().x(),
                    boundRect.right() - m_rect.width()),
             qBound(boundRect.top(), pos().y(),
                    boundRect.bottom() - m_rect.height()));*/
      // Emit change
      //emit itemChanged(this);
   }
   setGripSize();
}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::zoomToRegion()
{
   // Estimated minimum text size. Should be predicted.
   QGraphicsView* v = scene()->views().first();
   QSize size = v->size();

   double rectWidth = m_rect.width();
   double rectHeight = m_rect.height();

   qreal predictedScale = 0;
   if (rectHeight == rectWidth || rectHeight < rectWidth) {
      predictedScale = rectWidth / size.width();

   } else if (rectWidth < rectHeight) {
      predictedScale = rectHeight / size.height();
   }

   int predictedPixelSize = predictFontPixelSizeByScale(predictedScale);
   // Apply predicted pixel size to factor in when zoom to region is performed.
   m_font.setPixelSize(predictedPixelSize);
   m_lastStringWidth = 0;

   ImageViewScene* imageViewScene = ((ImageViewScene*) scene());
   imageViewScene->updateZoom(this);
}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{

   // Mouse click position (in item coordinates)
   QPointF pt = event -> pos();

   // Check for bottom right grip
   if (pt.x() <= m_rect.right() &&
       pt.y() <= m_rect.bottom() &&
       pt.x() >= m_rect.right() - m_gripSize &&
       pt.y() >= m_rect.bottom() - m_gripSize)
   {
      m_grip = BottomRight;
   }// bottom left
   else if (pt.x() >= m_rect.left() &&
       pt.y() >= m_rect.bottom() - m_gripSize &&
       pt.x() <= m_rect.left() + m_gripSize &&
       pt.y() <= m_rect.bottom())
   {
      m_grip = BottomLeft;
   }//top left
   else if (pt.x() <= m_rect.left() + m_gripSize &&
       pt.y() <= m_rect.top() + m_gripSize &&
       pt.x() >= m_rect.left() &&
       pt.y() >= m_rect.top())
   {
      m_grip = TopLeft;
   }//top right
   else if (pt.x() >= m_rect.right() - m_gripSize &&
       pt.y() >= m_rect.top()&&
       pt.x() <= m_rect.right() &&
       pt.y() <= m_rect.top() + m_gripSize )
   {
      m_grip = TopRight;
   }
   // Reset cursor
   else
   {
      m_grip = None;
   }

   // Queue an update
   update();

   // Pass mouse event
   QGraphicsItem::mousePressEvent(event);

}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{

   // No grip selected
   m_grip = None;

   // Queue an update
   update();

   // Pass mouse event
   QGraphicsItem::mouseReleaseEvent(event);

}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::paint(QPainter* painter,
                     const QStyleOptionGraphicsItem* option,
                     QWidget* widget)
{

   Q_UNUSED(widget);
   // Draw the center of the rect.

   // Draw grip box
   QRectF gripBottomLeft(m_rect.left(),
                   m_rect.bottom() - m_gripSize,
                   m_gripSize, m_gripSize);

   QRectF gripBottomRight(m_rect.right() - m_gripSize,
                    m_rect.bottom() - m_gripSize,
                    m_gripSize, m_gripSize);

   QRectF gripTopLeft(m_rect.left(),
                      m_rect.top(),
                      m_gripSize, m_gripSize);

   QRectF gripTopRight(m_rect.right() - m_gripSize,
                       m_rect.top(),
                       m_gripSize, m_gripSize);


   QPen pen(m_outlineColor);
   if (option->state & QStyle::State_Selected)
   {
      pen.setStyle(Qt::DotLine);
      pen.setWidth(2);

   }
   pen.setCosmetic(true);

   painter->setPen(pen);
   painter->drawRect(m_rect);

   //if(QApplication::queryKeyboardModifiers().testFlag(Qt::ShiftModifier))
   if(isSelected())
   {

      if(m_rect.width()<10&&m_rect.height()<10)
      {
         painter->drawPoint(m_rect.center());
      }
      else
      {
         // Draw the crosshair in the center of the region box.
         double midX = m_rect.left() + ( ( m_rect.right() - m_rect.left() ) * 0.5);
         double midY = m_rect.bottom() + ( ( m_rect.top() - m_rect.bottom() ) * 0.5);
         double thirdX = ( m_rect.right() - m_rect.left() ) * 0.1;
         double thirdY = ( m_rect.top() - m_rect.bottom() ) * 0.1;
         double cX = m_rect.center().x();
         double cY = m_rect.center().y();
         painter->drawLine(midX, cY - thirdY, midX, cY + thirdY);
         painter->drawLine(cX - thirdX, midY, cX + thirdX, midY);
      }
      
      painter->drawRect(gripBottomLeft);
      painter->drawRect(gripBottomRight);
      painter->drawRect(gripTopLeft);
      painter->drawRect(gripTopRight);
   }
   updateStringSize();
   painter->setFont(m_font);

   QString str = this->propertyValue(UPROBE_NAME).toString();

   // Make 1 pixel wider for gap between string and region box
   if (scene() != 0)
   {
       QRectF scenceRect = scene()->sceneRect();

       if( mapToScene(m_rect.center()).x() < scenceRect.center().x())
       {
          double txtRightPosition = m_rect.right()+1;
          painter->drawText( QPointF(txtRightPosition, m_rect.center().y() ), str);
       }
       else
       {
          double textLeftPosition = m_rect.left()-1-m_lastStringWidth;
          painter->drawText( QPointF(textLeftPosition, m_rect.center().y() ), str);
       }
   }

}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::setColor(const QColor& color)
{

   // Set color
   m_outlineColor = color;

}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::setGripSize()
{

   if (m_rect.width() < m_rect.height())
   {
      m_gripSize = 0.1 * m_rect.width();
   }
   else
   {
      m_gripSize = 0.1 * m_rect.height();
   }

}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::setHeight(double height)
{

   // Prepare for change
   prepareGeometryChange();

   // Set width
   m_rect.setHeight(height);

}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::setMouseOverPixelCoordModel(CoordinateModel* model)
{

   if(m_mouseOverPixelCoordModel != nullptr)
   {
      disconnect(m_mouseOverPixelCoordModel,&CoordinateModel::modelUpdated,this,&UProbeRegionGraphicsItem::updateModel);
   }

   m_mouseOverPixelCoordModel = model;

   if(m_mouseOverPixelCoordModel != nullptr)
   {
      connect(m_mouseOverPixelCoordModel,&CoordinateModel::modelUpdated,this,&UProbeRegionGraphicsItem::updateModel);
   }

}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::setLightToMicroCoordModel(CoordinateModel* model)
{

   if(m_lightToMicroCoordModel != nullptr)
   {
      disconnect(m_lightToMicroCoordModel,&CoordinateModel::modelUpdated,this,&UProbeRegionGraphicsItem::updateModel);
   }

   m_lightToMicroCoordModel = model;

   if(m_lightToMicroCoordModel != nullptr)
   {
      connect(m_lightToMicroCoordModel,&CoordinateModel::modelUpdated,this,&UProbeRegionGraphicsItem::updateModel);
   }

}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::setSameRect(QRectF& rect)
{

   // Prepare for change
   prepareGeometryChange();

   // Set y
   m_rect = rect;

}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::setSize(double size)
{

   m_size = size;

   update();

}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::setWidth(double width)
{

   // Prepare for change
   prepareGeometryChange();

   // Set width
   m_rect.setWidth(width);

}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::setX(double x)
{

   // Prepare for change
   prepareGeometryChange();

   // Set x
   setPos(x, pos().y());

}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::setY(double y)
{

   // Prepare for change
   prepareGeometryChange();

   // Set y
   setPos(pos().x(), y);

}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::updateModel()
{
   // Center needs to be calculated uProbe position
   QPointF position = mapToScene(m_rect.center());

   QPointF topLeft = mapToScene(m_rect.topLeft());
   QPointF topRight = mapToScene(m_rect.topRight());
   QPointF bottomRight = mapToScene(m_rect.bottomRight());

   if(m_mouseOverPixelCoordModel != nullptr)
   {
      double x,y,z;
      m_mouseOverPixelCoordModel->runTransformer((double)position.x(),
                                                 (double)position.y(),
                                                 0.0,
                                                 &x,
                                                 &y,
                                                 &z);

      double topLeftX, topLeftY, topLeftZ;
      double topRightX, topRightY, topRightZ;
      double bottomRightX, bottomRightY, bottomRightZ;
      m_mouseOverPixelCoordModel->runTransformer((double)topLeft.x(),
                                                 (double)topLeft.y(),
                                                 0.0,
                                                 &topLeftX,
                                                 &topLeftY,
                                                 &topLeftZ);
      m_mouseOverPixelCoordModel->runTransformer((double)topRight.x(),
                                                 (double)topRight.y(),
                                                 0.0,
                                                 &topRightX,
                                                 &topRightY,
                                                 &topRightZ);
      m_mouseOverPixelCoordModel->runTransformer((double)bottomRight.x(),
                                                 (double)bottomRight.y(),
                                                 0.0,
                                                 &bottomRightX,
                                                 &bottomRightY,
                                                 &bottomRightZ);

      if(m_lightToMicroCoordModel != nullptr)
      {
         double x1,y1,z1;
         m_lightToMicroCoordModel->runTransformer(x,
                                                  y,
                                                  0.0,
                                                  &x1,
                                                  &y1,
                                                  &z1);
         m_predictXProp->setValue(x1);
         m_predictYProp->setValue(y1);

         double topLeftX1, topLeftY1, topLeftZ1;
         double topRightX1, topRightY1, topRightZ1;
         double bottomRightX1, bottomRightY1, bottomRightZ1;
         m_lightToMicroCoordModel->runTransformer(topLeftX,
                                                  topLeftY,
                                                  0.0,
                                                  &topLeftX1,
                                                  &topLeftY1,
                                                  &topLeftZ1);
         m_lightToMicroCoordModel->runTransformer(topRightX,
                                                  topRightY,
                                                  0.0,
                                                  &topRightX1,
                                                  &topRightY1,
                                                  &topRightZ1);
         m_lightToMicroCoordModel->runTransformer(bottomRightX,
                                                  bottomRightY,
                                                  0.0,
                                                  &bottomRightX1,
                                                  &bottomRightY1,
                                                  &bottomRightZ1);

         m_widthProp->setValue(topRightX1-topLeftX1);
         m_heightProp->setValue(bottomRightY1-topRightY1);

      }

   }
   else
   {
      m_predictXProp->setValue(0.0);
      m_predictYProp->setValue(0.0);
      m_widthProp->setValue(topRight.x()-topLeft.x());
      m_heightProp->setValue(bottomRight.y()-topRight.y());
   }
}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::updateStringSize()
{

   double pixelSize;

   QTransform trans = getFirstViewTransform();
   if (trans.isScaling())
   {
      bool isInvertable = false;
      QTransform invTrans = trans.inverted(&isInvertable);
      if (isInvertable)
      {
         qreal horizontalScalingFactor = invTrans.m11();

         pixelSize = predictFontPixelSizeByScale(horizontalScalingFactor);
      }
   }
   else
   {
      pixelSize = 10;
   }

/*
   if(m_rect.width() > 11)
   {
      pixelSize = m_rect.width()/10;
   }
   else
   {
      pixelSize = 1;
   }
*/

   m_font.setPixelSize(pixelSize);

   QString str = this->propertyValue(UPROBE_NAME).toString();

   QFontMetrics fm(m_font);
   int currentStringWidth=fm.horizontalAdvance(str);
   m_lastStringWidth = currentStringWidth;

}

int UProbeRegionGraphicsItem::predictFontPixelSizeByScale(qreal scale) {
   double pixelSizeScalingFactor;

   if (scale < 0.20) {
      pixelSizeScalingFactor = 21.0;
   }
   else if (scale < 0.35) {
      pixelSizeScalingFactor = 18.0;
   }
   else if (scale < 0.50) {
      pixelSizeScalingFactor = 15.0;
   }
   else if (scale < 0.65) {
      pixelSizeScalingFactor = 13.0;
   }else {
      pixelSizeScalingFactor = 12.0;
   }


   int pixelSize = scale * pixelSizeScalingFactor;
   return std::max((double)pixelSize, 3.0);

}

//---------------------------------------------------------------------------

void UProbeRegionGraphicsItem::updateView()
{
   //double x = m_positionXProp->getValue().toDouble();
   //double y = m_positionYProp->getValue().toDouble();
   
   setSize(m_sizeProp->getValue().toDouble());
   //setX(x);
   //setY(y);
   update(this->m_rect);

   m_outlineColor = QColor(m_outlineColorProp->getValue().toString());

}

//---------------------------------------------------------------------------

QString UProbeRegionGraphicsItem::getValueAsString(QString prop)
{
   if(prop == STR_Region_Box_Top_Y)
   {
      double y = m_predictYProp->getValue().toDouble();
      double h = m_heightProp->getValue().toDouble();
      double v = y - (h / 2.0);
      return QString::number(v);
   }
   else if(prop == STR_Region_Box_Left_X)
   {
      double x = m_predictXProp->getValue().toDouble();
      double w = m_widthProp->getValue().toDouble();
      double v = x - (w / 2.0);
      return QString::number(v);
   }
   else if(prop == STR_Region_Box_Right_X)
   {
      double x = m_predictXProp->getValue().toDouble();
      double w = m_widthProp->getValue().toDouble();
      double v = x + (w / 2.0);
      return QString::number(v);
   }
   else if(prop == STR_Region_Box_Bottom_Y)
   {
      double y = m_predictYProp->getValue().toDouble();
      double h = m_heightProp->getValue().toDouble();
      double v = y + (h / 2.0);
      return QString::number(v);
   }
   else if(prop == STR_Region_Box_Center_X)
   {
      return m_predictXProp->getValue().toString();
   }
   else if(prop == STR_Region_Box_Center_Y)
   {
      return m_predictYProp->getValue().toString();
   }
   else if(prop == STR_Region_Box_Width)
   {
      return m_widthProp->getValue().toString();
   }
   else if(prop == STR_Region_Box_Height)
   {
      return m_heightProp->getValue().toString();
   }
   return "";
}

//---------------------------------------------------------------------------

               