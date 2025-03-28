/*-----------------------------------------------------------------------------
 * Copyright (c) 2025, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <solver/MappedCoordTransformer.h>

//---------------------------------------------------------------------------

MappedCoordTransformer::MappedCoordTransformer() : ITransformer()
{
   
}

//---------------------------------------------------------------------------

MappedCoordTransformer::~MappedCoordTransformer()
{

}

//---------------------------------------------------------------------------

QMap<QString, double> MappedCoordTransformer::getAllCoef()
{

   QMap<QString, double> coefs;
   return coefs;

}

//---------------------------------------------------------------------------

bool MappedCoordTransformer::Init(QMap<QString, double> globalVars)
{
    
    return true;

}

//---------------------------------------------------------------------------

bool MappedCoordTransformer::Init2(const std::vector<float>& x_axis, const std::vector<float>& y_axis)
{

    _x_axis = x_axis;
    _y_axis = y_axis;
    return true;

}

//---------------------------------------------------------------------------

bool MappedCoordTransformer::getVariable(QString name, double *val)
{
  
   return false;

}

//---------------------------------------------------------------------------

bool MappedCoordTransformer::setVariable(QString name, double val)
{
 
   return false;

}

//---------------------------------------------------------------------------

void MappedCoordTransformer::transformCommand(double inX,
                                             double inY,
                                             double inZ,
                                             double *outX,
                                             double *outY,
                                             double *outZ)
{

    int xidx = static_cast<int>(inX);
    int yidx = static_cast<int>(inY);
    if(xidx >= 0 && xidx < _x_axis.size() )
    {
         *outX = _x_axis[xidx];
    }
    else
    {
         *outX = 0.0;
    }
    if(yidx >= 0 && yidx < _y_axis.size() )
    {
         *outY = _y_axis[yidx];
    }
    else
    {
         *outY = 0.0;
    }
    *outZ = 0.0;

}

//---------------------------------------------------------------------------

