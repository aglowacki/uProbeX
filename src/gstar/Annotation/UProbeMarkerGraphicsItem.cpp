/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "UProbeMarkerGraphicsItem.h"
#include "gstar/GStarResource.h"
#include "gstar/AnnotationProperty.h"
#include "gstar/CoordinateModel.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include<QGraphicsScene>

using namespace gstar;

//---------------------------------------------------------------------------

UProbeMarkerGraphicsItem::UProbeMarkerGraphicsItem(AbstractGraphicsItem* parent)
   : AbstractGraphicsItem(parent)
{

   m_mouseOverPixelCoordModel = nullptr;
   m_lightToMicroCoordModel = nullptr;

   m_outlineColor = QColor(255, 0, 127);
   m_lineWidth = 2;
   m_size = 20.0;
   m_lastStringWidth = m_size * 2;

   //m_innerColorProp = new AnnotationProperty("Fill Color", m_innerColor);

   m_outlineColorProp = new AnnotationProperty(UPROBE_COLOR, m_outlineColor);
   m_positionXProp = new AnnotationProperty(UPROBE_LIGHT_POS_X, 0.0);
   m_positionYProp = new AnnotationProperty(UPROBE_LIGHT_POS_Y, 0.0);
   m_positionZProp = new AnnotationProperty(UPROBE_LIGHT_POS_Z, 0.0);

   m_predictXProp = new AnnotationProperty(UPROBE_PRED_POS_X, 0.0);
   m_predictYProp = new AnnotationProperty(UPROBE_PRED_POS_Y, 0.0);

   m_measuredXProp = new AnnotationProperty(UPROBE_MICRO_POS_X, "0.0");
   m_measuredYProp = new AnnotationProperty(UPROBE_MICRO_POS_Y, "0.0");

   m_sizeProp = new AnnotationProperty(UPROBE_SIZE, m_size);


   m_data.push_back(m_outlineColorProp);
   m_data.push_back(m_positionXProp);
   m_data.push_back(m_positionYProp);
   m_data.push_back(m_positionZProp);

   m_data.push_back(m_predictXProp);
   m_data.push_back(m_predictYProp);

   m_data.push_back(m_measuredXProp);
   m_data.push_back(m_measuredYProp);

   //m_data.push_back(m_sizeProp);

   setSize(m_sizeProp->getValue().toDouble());

   connectAllProperties();

}

//---------------------------------------------------------------------------

UProbeMarkerGraphicsItem::UProbeMarkerGraphicsItem(QMap<QString, QString>& marker, AbstractGraphicsItem* parent)
   : AbstractGraphicsItem(parent)
{

   m_mouseOverPixelCoordModel = nullptr;
   m_lightToMicroCoordModel = nullptr;

   m_outlineColor = QColor(marker[UPROBE_COLOR]);
   m_lineWidth = 2;
   m_size = 20.0;
   m_lastStringWidth = m_size * 2;

   m_outlineColorProp = new AnnotationProperty(UPROBE_COLOR, m_outlineColor);
   m_positionXProp = new AnnotationProperty(UPROBE_LIGHT_POS_X, marker["LX"]);
   m_positionYProp = new AnnotationProperty(UPROBE_LIGHT_POS_Y, marker["LY"]);
   m_positionZProp = new AnnotationProperty(UPROBE_LIGHT_POS_Z, marker["LZ"]);

   m_predictXProp = new AnnotationProperty(UPROBE_PRED_POS_X, marker["PX"]);
   m_predictYProp = new AnnotationProperty(UPROBE_PRED_POS_Y, marker["PY"]);

   m_measuredXProp = new AnnotationProperty(UPROBE_MICRO_POS_X, marker["MX"]);
   m_measuredYProp = new AnnotationProperty(UPROBE_MICRO_POS_Y, marker["MY"]);

   m_sizeProp = new AnnotationProperty(UPROBE_SIZE, m_size);


   m_data.push_back(m_outlineColorProp);
   m_data.push_back(m_positionXProp);
   m_data.push_back(m_positionYProp);
   m_data.push_back(m_positionZProp);

   m_data.push_back(m_predictXProp);
   m_data.push_back(m_predictYProp);

   m_data.push_back(m_measuredXProp);
   m_data.push_back(m_measuredYProp);

   //m_data.push_back(m_sizeProp);

   setSize(m_sizeProp->getValue().toDouble());

   connectAllProperties();

}

//---------------------------------------------------------------------------

QRectF UProbeMarkerGraphicsItem::boundingRect() const
{


   QFont myFont;
   QFontMetrics fm(myFont);
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

   qreal regionWidth = width + m_size+m_lineWidth*2;
   qreal regionHeight = m_size+m_lineWidth*2;
   QSizeF regionSize = QSizeF(regionWidth, regionHeight);

   QRectF scenceRect = scene()->sceneRect();
   if(pos().x() < scenceRect.center().x())
   {
      qreal regionHalfWidth = m_lineWidth + m_size*0.5;
      return QRectF(QPointF(-regionHalfWidth, -regionHalfWidth),
                    regionSize);
   }
   else
   {
      qreal regionHalfWidth = m_lineWidth + m_size*0.5+width;
      qreal regionHalfHeight = m_lineWidth + m_size*0.5;
      return QRectF(QPointF(-regionHalfWidth, -regionHalfHeight),
                    regionSize);
   }

}

//---------------------------------------------------------------------------

QRectF UProbeMarkerGraphicsItem::boundingRectMarker() const
{

   return m_polygon.boundingRect();

}

//---------------------------------------------------------------------------

void UProbeMarkerGraphicsItem::calculate()
{

   //nothing to calculate

}

//---------------------------------------------------------------------------

const QString UProbeMarkerGraphicsItem::displayName() const
{

   const QString name = QString("Calibration Point");
   return name;

}

//---------------------------------------------------------------------------

AbstractGraphicsItem* UProbeMarkerGraphicsItem::duplicate()
{

   UProbeMarkerGraphicsItem* item = new UProbeMarkerGraphicsItem();

   QColor color(m_outlineColorProp->getValue().toString());
   item->setColor(color);
   item->setSize(m_sizeProp->getValue().toDouble());
   item->setPos(pos());
   item->setMouseOverPixelCoordModel(m_mouseOverPixelCoordModel);
   item->setLightToMicroCoordModel(m_lightToMicroCoordModel);
   return item;

}

//---------------------------------------------------------------------------

void UProbeMarkerGraphicsItem::paint(QPainter* painter,
           const QStyleOptionGraphicsItem* option,
           QWidget* widget)
{

   Q_UNUSED(widget);

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
            setScale(xScale);
         }
      }
   }
   else
   {
      double s = scale();
      //rescale to 1 if otherwise
      if (s != 1.0)
      {
         setScale(1.0);
      }
   }

   QPen pen(m_outlineColor);
   if (option->state & QStyle::State_Selected)
   {
      pen.setStyle(Qt::DotLine);
      pen.setWidth(2);
   }
   painter->setPen(pen);
   painter->setBrush(m_outlineColor);
   painter->drawPolygon(m_polygon);

//   QFont myFont;
//   QFontMetrics fm(myFont);
//   int currentStringWidth=fm.width(str);
//   m_lastStringWidth = currentStringWidth;

   updateStringSize();
   painter->setFont(m_font);

   QString str = this->propertyValue(UPROBE_NAME).toString();

   QRectF scenceRect = scene()->sceneRect();
   if(pos().x() < scenceRect.center().x())
   {
      double txtRightPosition = m_lineWidth*2+m_size*0.5;
      painter->drawText( QPointF(txtRightPosition, 0 ), str);
   }
   else
   {
      double textLeftPosition = m_lineWidth+m_size*0.5+m_lastStringWidth;
      painter->drawText( QPointF(-textLeftPosition, 0 ), str);
   }

}

//---------------------------------------------------------------------------

void UProbeMarkerGraphicsItem::setMouseOverPixelCoordModel(CoordinateModel* model)
{

   if(m_mouseOverPixelCoordModel != nullptr)
   {
      disconnect(m_mouseOverPixelCoordModel,
                 SIGNAL(modelUpdated()),
                 this,
                 SLOT(updateModel()));
   }

   m_mouseOverPixelCoordModel = model;

   if(m_mouseOverPixelCoordModel != nullptr)
   {
      connect(m_mouseOverPixelCoordModel,
              SIGNAL(modelUpdated()),
              this,
              SLOT(updateModel()));
   }

}

//---------------------------------------------------------------------------

void UProbeMarkerGraphicsItem::setLightToMicroCoordModel(CoordinateModel* model)
{

   if(m_lightToMicroCoordModel != nullptr)
   {
      disconnect(m_lightToMicroCoordModel,
                 SIGNAL(modelUpdated()),
                 this,
                 SLOT(updateModel()));
   }

   m_lightToMicroCoordModel = model;

   if(m_lightToMicroCoordModel != nullptr)
   {
      connect(m_lightToMicroCoordModel,
              SIGNAL(modelUpdated()),
              this,
              SLOT(updateModel()));
   }

}

//---------------------------------------------------------------------------

void UProbeMarkerGraphicsItem::setColor(QColor color)
{

   m_outlineColor = color;

}
//---------------------------------------------------------------------------

void UProbeMarkerGraphicsItem::setColorProperty(QVariant color)
{

   m_outlineColorProp->setValue(color);

}

//---------------------------------------------------------------------------

void UProbeMarkerGraphicsItem::setMeasuredXProperty(QVariant mx)
{

   m_measuredXProp->setValue(mx);

}

//---------------------------------------------------------------------------

void UProbeMarkerGraphicsItem::setMeasuredYProperty(QVariant my)
{

   m_measuredYProp->setValue(my);

}

//---------------------------------------------------------------------------

void UProbeMarkerGraphicsItem::setPositionXProperty(QVariant lx)
{

   m_positionXProp->setValue(lx);

}

//---------------------------------------------------------------------------

void UProbeMarkerGraphicsItem::setPositionYProperty(QVariant ly)
{

   m_positionYProp->setValue(ly);

}

//---------------------------------------------------------------------------

void UProbeMarkerGraphicsItem::setPositionZProperty(QVariant lz)
{

   m_positionZProp->setValue(lz);

}

//---------------------------------------------------------------------------

void UProbeMarkerGraphicsItem::setPredictXProperty(QVariant px)
{

   m_predictXProp->setValue(px);

}

//---------------------------------------------------------------------------

void UProbeMarkerGraphicsItem::setPredictYProperty(QVariant py)
{

   m_predictYProp->setValue(py);

}

//---------------------------------------------------------------------------

void UProbeMarkerGraphicsItem::setSize(double size)
{

   m_size = size;
   double halfSize = size * 0.5;
   m_polygon.clear();

   double lineWidth =2;
   m_lineWidth = lineWidth;
   m_polygon.push_back(QPoint(-lineWidth, halfSize+lineWidth));
   m_polygon.push_back(QPoint(-lineWidth, lineWidth));
   m_polygon.push_back(QPoint(-(halfSize+lineWidth), lineWidth));
   m_polygon.push_back(QPoint(-(halfSize+lineWidth), -lineWidth));
   m_polygon.push_back(QPoint(-lineWidth, -lineWidth));
   m_polygon.push_back(QPoint(-lineWidth, -(halfSize+lineWidth)));

   m_polygon.push_back(QPoint(lineWidth, -(halfSize+lineWidth)));
   m_polygon.push_back(QPoint(lineWidth, -lineWidth));
   m_polygon.push_back(QPoint((halfSize+lineWidth), -lineWidth));
   m_polygon.push_back(QPoint((halfSize+lineWidth), lineWidth));
   m_polygon.push_back(QPoint(lineWidth, lineWidth));
   m_polygon.push_back(QPoint(lineWidth, halfSize+lineWidth));

   update();

}

//---------------------------------------------------------------------------

QPainterPath UProbeMarkerGraphicsItem::shape() const
{

   QPainterPath path;
   path.addPolygon(m_polygon);
   return path;

}

//---------------------------------------------------------------------------

void UProbeMarkerGraphicsItem::updateModel()
{

   QPointF position = pos();
   m_sizeProp->setValue(m_size);
   if(m_mouseOverPixelCoordModel != nullptr)
   {
      double x,y,z;
      m_mouseOverPixelCoordModel->runTransformer((double)position.x(),
                                                 (double)position.y(),
                                                 0.0,
                                                 &x,
                                                 &y,
                                                 &z);
      m_positionXProp->setValue(x);
      m_positionYProp->setValue(y);

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
      }

   }
   else
   {
      m_positionXProp->setValue(position.x());
      m_positionYProp->setValue(position.y());

      m_predictXProp->setValue(0.0);
      m_predictYProp->setValue(0.0);
   }

}

//---------------------------------------------------------------------------

void UProbeMarkerGraphicsItem::updateStringSize()
{

   int pixelSize = 12;

   m_font.setPixelSize(pixelSize);
   QString str = this->propertyValue(UPROBE_NAME).toString();

   QFontMetrics fm(m_font);
   int currentStringWidth=fm.horizontalAdvance(str);
   m_lastStringWidth = currentStringWidth;

}

//---------------------------------------------------------------------------

void UProbeMarkerGraphicsItem::updateView()
{

   //double x = m_positionXProp->getValue().toDouble();
   //double y = m_positionYProp->getValue().toDouble();

   setSize(m_sizeProp->getValue().toDouble());
   //setX(x);
   //setY(y);

   m_outlineColor = QColor(m_outlineColorProp->getValue().toString());

}

//---------------------------------------------------------------------------
