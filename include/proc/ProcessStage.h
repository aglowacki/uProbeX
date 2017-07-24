/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PROCESS_STAGE_H
#define PROCESS_STAGE_H

/*---------------------------------------------------------------------------*/

#include <proc/Stage.h>

#include <Array.h>

/*---------------------------------------------------------------------------*/

/**
 * @brief Stage for processing image.
 */
class ProcessStage
: public Stage
{

public:

   /**
    * Constructor.
    */
   ProcessStage(Stage* parent = 0);

   /**
    * Destructor.
    */
   ~ProcessStage();

   /**
    * Set the input buffer.
    */
   virtual void setInputBuffer(gstar::Array* buffer) = 0;

   /**
    * Get the output buffer
    */
   virtual gstar::Array* getOutputBuffer() = 0;

   /**
    * Get the output buffer
    */
   virtual void setRGBSequence(int first, int second, int third) = 0;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
