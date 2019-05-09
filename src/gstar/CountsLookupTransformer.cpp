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
   _model = nullptr;
   _analysis_type = "";
   _element = "";

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

double CountsLookupTransformer::getCounts()
{

   return _counts;

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

void CountsLookupTransformer::setCounts(double x)
{

   _counts = x;

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

    if(_model != nullptr && _analysis_type.length() > 0 && _element.length() > 0)
    {
        data_struct::Fit_Count_Dict* fit_counts = _model->getAnalyzedCounts(_analysis_type);
        if (fit_counts != nullptr)
        {
            if(fit_counts->count(_element) > 0)
            {
                int col = (int)inX;
                int row = (int)inY;
                int height = fit_counts->at(_element).rows();
                int width = fit_counts->at(_element).cols();
                if(row > -1 && row < height && col > -1 && col < width)
                {
                    *outX = fit_counts->at(_element)(row,col);
                }
                //*outY = fit_counts->at(_element).minCoeff();
                //*outZ = fit_counts->at(_element).maxCoeff();
            }
            else
            {
                *outX = 0.0;
                *outY = 0.0;
                *outZ = 0.0;
            }
        }
        else
        {
            *outX = 0.0;
            *outY = 0.0;
            *outZ = 0.0;
        }
    }
    else
    {

        *outX = 0.0;
        *outY = 0.0;
        *outZ = 0.0;
    }
}

/*---------------------------------------------------------------------------*/
