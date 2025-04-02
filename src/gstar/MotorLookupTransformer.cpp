/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/MotorLookupTransformer.h>
#include <math.h>

using namespace gstar;

//---------------------------------------------------------------------------

#define STR_MOTOR_X "X"
#define STR_MOTOR_Y "Y"

//---------------------------------------------------------------------------

MotorLookupTransformer::MotorLookupTransformer() : ITransformer()
{

   _motor_x = 0;
   _motor_y = 0;
   _rows = 0;
   _cols = 0;

}

//---------------------------------------------------------------------------

MotorLookupTransformer::~MotorLookupTransformer()
{

}

//---------------------------------------------------------------------------

bool MotorLookupTransformer::Init(QMap<QString, double> globalVars)
{ 

   return true; 

} 

//---------------------------------------------------------------------------

QMap<QString, double> MotorLookupTransformer::getAllCoef()
{

   QMap<QString, double> coefs;

   coefs.insert(STR_MOTOR_X, _motor_x);
   coefs.insert(STR_MOTOR_Y, _motor_y);

   return coefs;

}

//---------------------------------------------------------------------------

void MotorLookupTransformer::setMotors(const std::vector<float>& motor_x, const std::vector<float>& motor_y)
{
    _motor_x_arr = motor_x;
    _motor_y_arr = motor_y;
    _rows = motor_y.size();
    _cols = motor_x.size();
}

//---------------------------------------------------------------------------

bool MotorLookupTransformer::getVariable(QString name, double *val)
{

   if(name == STR_MOTOR_X)
   {
      *val = _motor_x;
      return true;
   }
   else if(name == STR_MOTOR_Y)
   {
      *val = _motor_y;
      return true;
   }
   
   return false;

}

//---------------------------------------------------------------------------

bool MotorLookupTransformer::setVariable(QString name, double val)
{

   if(name == STR_MOTOR_X)
   {
      _motor_x = val;
      return true;
   }
   else if(name == STR_MOTOR_Y)
   {
      _motor_y = val;
      return true;
   }
   
   return false;

}

//---------------------------------------------------------------------------

void MotorLookupTransformer::transformCommand(double inX,
                                         double inY,
                                         double inZ,
                                         double *outX,
                                         double *outY,
                                         double *outZ)
{
    int col = (int)inX;
    int row = (int)inY;
    
    if(row < 0 )
    {
      row = 0;
    }
    else if( row >= _rows)
    {
      row = _rows - 1;
    }
    if(col < 0)
    {
      col = 0;
    }
    else if( col >= _cols)
    {
      col = _cols - 1;
    }
   
   *outX = _motor_x_arr[col];
   *outY = _motor_y_arr[row]; 
   *outZ = 0;
    
}

//---------------------------------------------------------------------------
