/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <solver/LinearCoordTransformer.h>
#include <math.h>

//---------------------------------------------------------------------------

LinearCoordTransformer::LinearCoordTransformer() : ITransformer()
{
   _coefs[QSTR_SlopeX] =  1.0;
   _coefs[QSTR_SlopeY] =  1.0;
   _coefs[QSTR_InterceptX] =  0.0;
   _coefs[QSTR_InterceptY] =  0.0;
   _coefs[QSTR_SlopeXY] =  0.0;
   _coefs[QSTR_InterceptXY] =  0.0;
   _coefs[QSTR_SlopeYX] =  0.0;
   _coefs[QSTR_InterceptYX] =  0.0;
}

//---------------------------------------------------------------------------

LinearCoordTransformer::~LinearCoordTransformer()
{

}

//---------------------------------------------------------------------------

QMap<QString, double> LinearCoordTransformer::getAllCoef()
{

   return _coefs;

}

//---------------------------------------------------------------------------

bool LinearCoordTransformer::Init(QMap<QString, double> globalVars)
{
    for(auto &itr : _coefs.keys())
    {
      if(globalVars.count(itr) > 0)
      {
         _coefs[itr] = globalVars.value(itr);
      } 
    }
    return true;

}

//---------------------------------------------------------------------------

bool LinearCoordTransformer::getVariable(QString name, double *val)
{

   if(_coefs.count(name) > 0)
   {
      *val = _coefs.value(name);
      return true;
   }
  
   return false;

}

//---------------------------------------------------------------------------

bool LinearCoordTransformer::setVariable(QString name, double val)
{

   if(_coefs.count(name) > 0)
   {
      _coefs[name] = val;
      return true;
   }
 
   return false;

}

//---------------------------------------------------------------------------

void LinearCoordTransformer::transformCommand(double inX,
                                             double inY,
                                             double inZ,
                                             double *outX,
                                             double *outY,
                                             double *outZ)
{

    double xPred = inX * _coefs.value(QSTR_SlopeX) + _coefs.value(QSTR_InterceptX);
    double yPred = inY * _coefs.value(QSTR_SlopeY) + _coefs.value(QSTR_InterceptY);
    *outX = xPred + (yPred * _coefs.value(QSTR_SlopeYX) + _coefs.value(QSTR_InterceptYX));
    *outY = yPred + (xPred * _coefs.value(QSTR_SlopeXY) + _coefs.value(QSTR_InterceptXY));
    
    *outZ = 0.0;

}

//---------------------------------------------------------------------------

