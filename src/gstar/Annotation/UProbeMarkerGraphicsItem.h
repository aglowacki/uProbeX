/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef UPROBEMARKERGRAPHICSITEM_H
#define UPROBEMARKERGRAPHICSITEM_H

//---------------------------------------------------------------------------

#include "AbstractGraphicsItem.h"

#include <QPolygon>
#include <QFont>


//---------------------------------------------------------------------------

namespace gstar
{

class CoordinateModel;

/**
 * @brief The UProbeMarkerGraphicsItem class
 * Custom annotation class used for uProbeX application
 */
class UProbeMarkerGraphicsItem : public AbstractGraphicsItem
{

public:

    /**
    * @brief MarkerGraphicsItem
    * @param parent
    */
   UProbeMarkerGraphicsItem(AbstractGraphicsItem* parent = 0);

   /**
   * @brief MarkerGraphicsItem
   * @param parent
   * @param marker infomation
   */
   UProbeMarkerGraphicsItem(QMap<QString, QString>& marker,
                            AbstractGraphicsItem* parent = 0);

   /**
    * @brief boundingRect
    * @return
    */
   QRectF boundingRect() const;

   /**
    * @brief boundingRect
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
    * @brief setColorProperty
    * @param color
    */
   void setColorProperty(QVariant color);

   /**
    * @brief setMeasuredXProperty
    * @param mx
    */
   void setMeasuredXProperty(QVariant mx);

   /**
    * @brief setMeasuredYProperty
    * @param my
    */
   void setMeasuredYProperty(QVariant my);

   /**
    * @brief setPositionXProperty
    * @param lx
    */
   void setPositionXProperty(QVariant lx);

   /**
    * @brief setPositionYProperty
    * @param ly
    */
   void setPositionYProperty(QVariant ly);

   /**
    * @brief setPositionZProperty
    * @param lz
    */
   void setPositionZProperty(QVariant lz);

   /**
    * @brief setPredictXProperty
    * @param px
    */
   void setPredictXProperty(QVariant px);

   /**
    * @brief setPredictYProperty
    * @param py
    */
   void setPredictYProperty(QVariant py);

   /**
    * @brief shape
    * @return
    */
   QPainterPath shape() const;

   /**
    * @brief initialStringSize
    */
   void updateStringSize();

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
    * @brief m_lineWidth
    */
   double m_lineWidth;

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
