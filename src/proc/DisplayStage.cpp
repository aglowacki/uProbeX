/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <proc/DisplayStage.h>

#include <Array.h>

#include <QDebug>
#include <QDateTime>

#include <stdio.h>

/*---------------------------------------------------------------------------*/

DisplayStage::DisplayStage(Stage* parent)
: QObject(0), FinalStage(parent)
{

   m_inputBuffer = NULL;

}

/*---------------------------------------------------------------------------*/

DisplayStage::~DisplayStage()
{

}

/*---------------------------------------------------------------------------*/

void DisplayStage::execute()
{

   if (m_inputBuffer == NULL) return;

   // Emit new frame available
   emit newFrame(const_cast<gstar::Array*>(m_inputBuffer));

}

/*---------------------------------------------------------------------------*/

void DisplayStage::setInputBuffer(gstar::Array* buffer)
{

   m_inputBuffer = buffer;

}

/*---------------------------------------------------------------------------*/

void DisplayStage::update()
{

   if (getParentStage() != NULL) 
   {
      getParentStage()->update();
   }

}

/*---------------------------------------------------------------------------*/
