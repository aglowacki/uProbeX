/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <solver/Solver.h>

/*---------------------------------------------------------------------------*/

Solver::Solver()
{

   m_impl = nullptr;

}

/*---------------------------------------------------------------------------*/

Solver::~Solver()
{

}

/*---------------------------------------------------------------------------*/

QMap<QString, double> Solver::getAllCoef()
{

   QMap<QString, double> dic;
   if(m_impl != nullptr)
      dic = m_impl->getAllCoef();

   return dic;

}

/*---------------------------------------------------------------------------*/

AbstractSolver* Solver::getImpl()
{

   return m_impl;

}

/*---------------------------------------------------------------------------*/

QMap<QString, double> Solver::getMinCoef()
{

   QMap<QString, double> dic;
   if(m_impl != nullptr)
      dic = m_impl->getMinCoef();

   return dic;

}

/*---------------------------------------------------------------------------*/

QMap<QString, double> Solver::getOptions()
{

   QMap<QString, double> dic;
   if(m_impl != nullptr)
      dic = m_impl->getOptions();

   return dic;

}

/*---------------------------------------------------------------------------*/

gstar::ITransformer* Solver::getTransformer()
{

   if(m_impl != nullptr)
      return m_impl->getTransformer();

   return nullptr;

}

/*---------------------------------------------------------------------------*/

void Solver::setAllCoef(QMap<QString, double> vars)
{

   if(m_impl != nullptr)
      m_impl->setAllCoef(vars);

}
/*--------------------------------------------------------------------------*/

QString Solver::getLastErrorMessage()
{

   if(m_impl != nullptr)
      return m_impl->getLastErrorMessage();

   return "";

}

/*---------------------------------------------------------------------------*/

void Solver::setCoordPoints(QList < QMap<QString,double> > vars)
{

   if(m_impl != nullptr)
      m_impl->setCoordPoints(vars);

}

/*---------------------------------------------------------------------------*/

void Solver::setImpl(AbstractSolver* impl)
{

   m_impl = impl;

}

/*---------------------------------------------------------------------------*/

void Solver::setMinCoef(QMap<QString, double> vars)
{

   if(m_impl != nullptr)
      m_impl->setMinCoef(vars);

}

/*---------------------------------------------------------------------------*/

void Solver::setOptions(QMap<QString, double> vars)
{

   if(m_impl != nullptr)
      m_impl->setOptions(vars);

}

/*---------------------------------------------------------------------------*/

void Solver::setTransformer(gstar::ITransformer* transformer)
{

   if(m_impl != nullptr)
      m_impl->setTransformer(transformer);

}

/*---------------------------------------------------------------------------*/

bool Solver::run()
{

   if(m_impl != nullptr)
      return m_impl->run();

   return false;

}

/*---------------------------------------------------------------------------*/
