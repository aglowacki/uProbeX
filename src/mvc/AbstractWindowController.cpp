/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/AbstractWindowController.h>

using gstar::ImageViewWidget;
using gstar::Array;

/*---------------------------------------------------------------------------*/

AbstractWindowController::AbstractWindowController()
{

   m_widget = nullptr;
   m_preferences = nullptr;

}

/*---------------------------------------------------------------------------*/

AbstractWindowController::~AbstractWindowController()
{


}

/*---------------------------------------------------------------------------*/

