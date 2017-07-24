/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef STAGE_H
#define STAGE_H

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

/**
 * @brief A stage in the processing pipeline...
 *
 * A stage is a processing step in our image pipeline. A stage can have a parent
 * stage or a next stage. Theses stages are further distinguished into source,
 * process and final (destination) stages. Currently, the stages are linear
 * without support for branches or multiple child stages.
 *
 * The concrete implementation of this class should implement execute() method,
 * which is called by the parent stage at appropriate step.
 */
class Stage
{

public:

   enum {
      STAGE_UNDEFINED,
      STAGE_SOURCE,
      STAGE_PROCESS,
      STAGE_FINAL
   };
   /**
    * Constructor.
    */
   Stage(Stage* parent = 0);

   /**
    * Destructor.
    */
   ~Stage();

   virtual void execute() = 0;

   /**
    * Get the next stage in sequence of pipeline operation.
    */
   Stage* getNextStage();

   /**
    * Set the parent stage.
    */
   Stage* getParentStage();

   /**
    * Type of the stage given the enum.
    */
   int getStageType();

   /**
    *  Run the next stage in the pipeline if it exists.  
    */
   void nextStage();

   /**
    * Set the next stage in sequence of pipeline operation.
    */
   void setNextStage(Stage* astage);

   /**
    * Set the parent stage.
    */
   void setParentStage(Stage* astage);

   /**
    * Set the stage type
    */
   void setStageType(int type);

   /**
    * A child stage will call its parent to update, and that parent
    * if has a parent wil call update on that and so on until the
    * execution reach the source stage.
    */
   virtual void update() = 0;

protected:

   /**
    * Next stage in the pipeline sequence.
    */
   Stage* m_stageNext;

   /**
    * The parent stage of the current stage. It could be NULL in case of a
    * source stage.
    */
   Stage* m_stageParent;

   /**
    * Type of this stage (source, process, or final)
    */
   int m_stageType;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
