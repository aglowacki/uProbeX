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
   ScanRegionGraphicsItem(AbstractGraphicsItem* parent = 0);

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

   const QString displayName() const;

   void setQueueId(QString id);

private:

   QString _scan_id;

};

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
