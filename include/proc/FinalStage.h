/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef FINAL_STAGE_H
#define FINAL_STAGE_H

/*---------------------------------------------------------------------------*/

#include <proc/Stage.h>

#include <Array.h>

/*---------------------------------------------------------------------------*/

/**
 * @brief Final stage for processing the image.
 */
class FinalStage
: public Stage
{

public:

   /**
    * Constructor.
    */
   FinalStage(Stage* parent);

   /**
    * Destructor.
    */
   ~FinalStage();

   /**
    * Set the input buffer.
    */
   virtual void setInputBuffer(gstar::Array* buffer) = 0;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
