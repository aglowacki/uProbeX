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
#include "mvc/BlueskyComm.h"

//---------------------------------------------------------------------------

namespace gstar
{

#define STR_QUEUED "Queued"
#define STR_NOT_QUEUED "Not Queued"
#define STR_NEED_UPDATE "Needs Update"

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

   
   ~ScanRegionGraphicsItem();

   /**
    * @brief coneRegion
    * @param uProbeRegion
    * @return
    */
   virtual ScanRegionGraphicsItem* cloneRegion();

   const QString displayName() const;

   void setPlan(const BlueskyPlan& plan);

   bool isQueued();

   BlueskyPlan getPlan() { return _bs_plan; }

   BlueskyPlan* getPlanPtr() { return &_bs_plan; }

signals:
   void planRemoved(BlueskyPlan);

private slots:
   void onPlanDirty();

protected:

   void _init();

   AnnotationProperty* _queue_status;

   AnnotationProperty* _plan_type;

   BlueskyPlan _bs_plan;

};

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
