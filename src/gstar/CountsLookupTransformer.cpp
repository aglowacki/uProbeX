/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/CountsLookupTransformer.h>
#include <math.h>

using namespace gstar;

/*---------------------------------------------------------------------------*/

#define STR_COUNTS "Counts"
#define STR_MIN_COUNTS "Min Counts"
#define STR_MAX_COUNTS "Max Counts"

/*---------------------------------------------------------------------------*/

CountsLookupTransformer::CountsLookupTransformer() : ITransformer()
{

   _counts = 0.0;
   _min_counts = 0.0;
   _max_counts = 0.0;
   _rows = 0;
   _cols = 0;

}

/*---------------------------------------------------------------------------*/

CountsLookupTransformer::~CountsLookupTransformer()
{

}

/*---------------------------------------------------------------------------*/

bool CountsLookupTransformer::Init(QMap<QString, double> globalVars)
{ 

   return true; 

} 

/*---------------------------------------------------------------------------*/

QMap<QString, double> CountsLookupTransformer::getAllCoef()
{

   QMap<QString, double> coefs;

   coefs.insert(STR_COUNTS, _counts);
   coefs.insert(STR_MIN_COUNTS, _min_counts);
   coefs.insert(STR_MAX_COUNTS, _max_counts);

   return coefs;

}

/*---------------------------------------------------------------------------*/

void CountsLookupTransformer::setCounts(const data_struct::ArrayXXr& counts)
{
    _counts_arr = counts;
    _min_counts = _counts_arr.minCoeff();
    _max_counts = _counts_arr.maxCoeff();
    _rows = _counts_arr.rows();
    _cols = _counts_arr.cols();
}

/*---------------------------------------------------------------------------*/

bool CountsLookupTransformer::getVariable(QString name, double *val)
{

   if(name == STR_COUNTS)
   {
      *val = _counts;
      return true;
   }
   else if(name == STR_MIN_COUNTS)
   {
      *val = _min_counts;
      return true;
   }
   else if(name == STR_MAX_COUNTS)
   {
      *val = _max_counts;
      return true;
   }

   return false;

}

/*---------------------------------------------------------------------------*/

bool CountsLookupTransformer::setVariable(QString name, double val)
{

   if(name == STR_COUNTS)
   {
      _counts = val;
      return true;
   }
   else if(name == STR_MIN_COUNTS)
   {
      _min_counts = val;
      return true;
   }
   else if(name == STR_MAX_COUNTS)
   {
      _max_counts = val;
      return true;
   }

   return false;

}

/*---------------------------------------------------------------------------*/

void CountsLookupTransformer::transformCommand(double inX,
                                         double inY,
                                         double inZ,
                                         double *outX,
                                         double *outY,
                                         double *outZ)
{
    int col = (int)inX;
    int row = (int)inY;
    
    if(row > -1 && row < _rows && col > -1 && col < _cols)
    {
        *outX = _counts_arr(row,col);
    }
    else
    {
        *outX = 0.0;
    }
    *outY = _min_counts;
    *outZ = _max_counts;
    
}

/*---------------------------------------------------------------------------*/
