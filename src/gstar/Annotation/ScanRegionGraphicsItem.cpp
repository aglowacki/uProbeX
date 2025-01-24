/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/Annotation/ScanRegionGraphicsItem.h"
#include "gstar/GStarResource.h"
#include <gstar/ImageViewScene.h>
#include <QSize>
#include <iostream>

#include <QApplication>

using namespace gstar;

//---------------------------------------------------------------------------

ScanRegionGraphicsItem::ScanRegionGraphicsItem(std::map<QString, BlueskyPlan> * avail_scans, AbstractGraphicsItem* parent)
   : UProbeRegionGraphicsItem(parent)
{
   _avail_scans = avail_scans;
   _scan_dialog.setAvailScans(avail_scans);
   connect(&_scan_dialog, &ScanRegionDialog::ScanUpdated, this, &ScanRegionGraphicsItem::onScanUpdated);

   //prependProperty(new AnnotationProperty("Edit", QIcon(":/images/editing.png")));

}

//---------------------------------------------------------------------------

ScanRegionGraphicsItem::ScanRegionGraphicsItem(QMap<QString, QString>& marker,
                                                   AbstractGraphicsItem* parent)
                                                   : UProbeRegionGraphicsItem(parent)
{
/*
   m_mouseOverPixelCoordModel = nullptr;
   m_lightToMicroCoordModel = nullptr;


   m_outlineColor = QColor(marker[UPROBE_COLOR]);
   m_rect = QRectF(marker["TopLeftX"].toDouble(),
                   marker["TopLeftY"].toDouble(),
                   marker["RectWidth"].toDouble(),
                   marker["RectHeight"].toDouble());
   setGripSize();
   m_outlineColorProp = new AnnotationProperty(UPROBE_COLOR, m_outlineColor);
   m_predictXProp = new AnnotationProperty(UPROBE_PRED_POS_X, marker[UPROBE_PRED_POS_X]);
   m_predictYProp = new AnnotationProperty(UPROBE_PRED_POS_Y, marker[UPROBE_PRED_POS_Y]);
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
*/
}

//---------------------------------------------------------------------------

const QString ScanRegionGraphicsItem::displayName() const
{

   return QString("ScanRegion");

}

//---------------------------------------------------------------------------

ScanRegionGraphicsItem* ScanRegionGraphicsItem::cloneRegion()
{
   ScanRegionGraphicsItem* newRegion = new ScanRegionGraphicsItem(_avail_scans);

   //newRegion->m_outlineColor = m_outlineColor;
   newRegion->m_rect = m_rect;

   return newRegion;
}

//---------------------------------------------------------------------------

QDialog* ScanRegionGraphicsItem::get_custom_dialog() 
{
   _scan_dialog.updateProps(m_data);
   return &_scan_dialog; 
}

//---------------------------------------------------------------------------

void ScanRegionGraphicsItem::onScanUpdated(const BlueskyPlan& plan)
{

   setPropertyValue(DEF_STR_DISPLAY_NAME, _scan_dialog.getScanName());
   BlueskyPlan nplan = plan;
   nplan.name = _scan_dialog.getScanName();
   emit scanUpdated(nplan);

}

//---------------------------------------------------------------------------

