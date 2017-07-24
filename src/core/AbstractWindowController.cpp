/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <core/AbstractWindowController.h>
#include <proc/ColorLevelsProcessStage.h>
#include <proc/DisplayStage.h>
#include <proc/Histogram.h>

using gstar::ImageViewWidget;
using gstar::Array;

/*---------------------------------------------------------------------------*/

AbstractWindowController::AbstractWindowController()
{

   m_widget = NULL;
   m_preferences = NULL;
   m_histogram = NULL;
   m_displayStage = NULL;
   m_processStage = NULL;

}

/*---------------------------------------------------------------------------*/

AbstractWindowController::~AbstractWindowController()
{


}

/*---------------------------------------------------------------------------*/

Histogram* AbstractWindowController::getHistogram()
{

   return m_histogram;

}

/*---------------------------------------------------------------------------*/
