/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/AnnotationToolBarWidget.h"

#include <QAction>
#include <QCheckBox>
#include <QLabel>
#include <QToolBar>

using namespace gstar;

//---------------------------------------------------------------------------

AnnotationToolBarWidget::AnnotationToolBarWidget(QWidget* parent) :
   QWidget(parent)
{

   // Background color
   //QPalette pal = this->palette();
//   pal.setColor(this->backgroundRole(), Qt::white);
//   this->setPalette(pal);
//   setAutoFillBackground(true);

   m_toolbar = new QToolBar();
   //m_toolbar->setPalette(pal);

   m_rulerAction = std::make_unique<QAction>(QIcon(":images/ruler.png"),
                               "Ruler",
                               nullptr);

   m_intensityLineAction = std::make_unique<QAction>(QIcon(":images/intensity.png"),
                               "Line Out Intensity",
                               nullptr);

   m_intensityPieAction = std::make_unique<QAction>(QIcon(":images/arc.png"),
                               "Arc Intensity",
                               nullptr);

   m_markerAction = std::make_unique<QAction>(QIcon(":images/marker.png"),
                               "Marker",
                               nullptr);

   m_crossHairAction = std::make_unique<QAction>(QIcon(":images/crosshair.png"),
                                  "Cross Hair",
                                  nullptr);


   connect(m_rulerAction.get(), &QAction::triggered, this, &AnnotationToolBarWidget::clickRuler);

   connect(m_intensityLineAction.get(), &QAction::triggered, this, &AnnotationToolBarWidget::clickIntensityLine);

   connect(m_intensityPieAction.get(), &QAction::triggered, this, &AnnotationToolBarWidget::clickIntensityPie);

   connect(m_markerAction.get(), &QAction::triggered, this, &AnnotationToolBarWidget::clickMarker);

   connect(m_crossHairAction.get(), &QAction::triggered, this, &AnnotationToolBarWidget::clickCrossHair);

   QLabel* enableLable = new QLabel("Visible:");
   m_chkSetVisible = new QCheckBox();
   m_chkSetVisible->setChecked(true);
   connect(m_chkSetVisible, &QCheckBox::stateChanged, this, &AnnotationToolBarWidget::setActionsEnabled);

   m_toolbar->addWidget(enableLable);
   m_toolbar->addWidget(m_chkSetVisible);
   m_toolbar->addAction(m_rulerAction.get());
//   m_toolbar->addAction(m_intensityLineAction.get());
//   m_toolbar->addAction(m_intensityPieAction.get());
   m_toolbar->addAction(m_markerAction.get());
//   m_toolbar->addAction(m_crossHairAction.get());

   m_toolbar->setContentsMargins(QMargins(0, 0, 0, 0));

}

AnnotationToolBarWidget::~AnnotationToolBarWidget()
{

}

//---------------------------------------------------------------------------

QWidget* AnnotationToolBarWidget::getToolBar()
{

   return m_toolbar;

}

//---------------------------------------------------------------------------

void AnnotationToolBarWidget::setActionsEnabled(int state)
{

   bool enabled = false;
   if (state == Qt::Checked)
   {
      enabled = true;
   }

   m_rulerAction->setEnabled(enabled);
   m_markerAction->setEnabled(enabled);

   emit enabledStateChanged(enabled);

}

//---------------------------------------------------------------------------
