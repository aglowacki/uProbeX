/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <solver/LinearSolver.h>

//---------------------------------------------------------------------------

LinearSolver::LinearSolver()  :  AbstractSolver()
{

}

//---------------------------------------------------------------------------

LinearSolver::~LinearSolver()
{

}

//--------------------------------------------------------------------------

QMap<QString, double> LinearSolver::getAllCoef()
{
   return _dict_min_coef;
}

//--------------------------------------------------------------------------

QString LinearSolver::getLastErrorMessage()
{
   return "N/A";
}

//--------------------------------------------------------------------------

QMap<QString, double> LinearSolver::getMinCoef()
{
   return _dict_min_coef;
}

//--------------------------------------------------------------------------

QMap<QString, double> LinearSolver::getOptions()
{
   return _dict_options;
}

//--------------------------------------------------------------------------

gstar::ITransformer* LinearSolver::getTransformer()
{
   return _transformer;
}

//--------------------------------------------------------------------------

void LinearSolver::setAllCoef(QMap<QString, double> vars)
{

}

//--------------------------------------------------------------------------

void LinearSolver::setCoordPoints(QList < QMap<QString,double> > vars)
{

}

//--------------------------------------------------------------------------

void LinearSolver::setMinCoef(QMap<QString, double> vars)
{

}

//--------------------------------------------------------------------------

void LinearSolver::setOptions(QMap<QString, double> vars)
{

}

//--------------------------------------------------------------------------

void LinearSolver::setTransformer(gstar::ITransformer* transformer)
{

}

//--------------------------------------------------------------------------

bool LinearSolver::run()
{
   return false;
}

//--------------------------------------------------------------------------
