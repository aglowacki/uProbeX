/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SOURCE_STAGE_H
#define SOURCE_STAGE_H

/*---------------------------------------------------------------------------*/

#include <proc/Stage.h>

#include <Array.h>

/*---------------------------------------------------------------------------*/

/**
 * @brief The source stage of the pipeline ,inherit by other class, such as
 * AreaDetectorSourceStage.
 */
class SourceStage
: public Stage
{

public:

   /**
    * Constructor.
    */
   SourceStage(Stage* parent = 0);

   /**
    * Destructor.
    */
   ~SourceStage();

protected:

   /**
    * The source stage generates the output buffer and makes it available
    * for the next stage in the pipeline.
    */
   virtual gstar::Array* getOutputBuffer() = 0;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
