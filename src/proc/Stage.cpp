/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <proc/Stage.h>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

/*---------------------------------------------------------------------------*/

Stage::Stage(Stage* parent)
: m_stageNext(NULL), m_stageParent(parent)
{

   m_stageType = STAGE_UNDEFINED;

}

/*---------------------------------------------------------------------------*/

Stage::~Stage()
{

}

/*---------------------------------------------------------------------------*/

Stage* Stage::getNextStage()
{

   return m_stageNext;

}

/*---------------------------------------------------------------------------*/

Stage* Stage::getParentStage()
{

   return m_stageParent;

}

/*---------------------------------------------------------------------------*/

int Stage::getStageType()
{

   return m_stageType;

}

/*---------------------------------------------------------------------------*/

void Stage::nextStage()
{


}
/*---------------------------------------------------------------------------*/

void Stage::setNextStage(Stage* astage)
{

   m_stageNext = astage;
   astage->setParentStage(this);

}

/*---------------------------------------------------------------------------*/

void Stage::setParentStage(Stage* astage)
{

   m_stageParent = astage;

}

/*---------------------------------------------------------------------------*/

void Stage::setStageType(int type)
{

   m_stageType = type;

}
/*---------------------------------------------------------------------------*/
