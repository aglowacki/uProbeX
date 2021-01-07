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
   _counts_ptr = nullptr;
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

void CountsLookupTransformer::setAnalyaisElement(std::string analysis_type, std::string element)
{
    _analysis_type = analysis_type;
    _element = element;
    if(_model != nullptr && _analysis_type.length() > 0 && _element.length() > 0)
    {
        data_struct::Fit_Count_Dict* fit_counts = _model->getAnalyzedCounts(_analysis_type);
        if (fit_counts != nullptr)
        {
            if(fit_counts->count(_element) > 0)
            {
                if (_normalizer != nullptr)
                {
                    _normalized_counts = fit_counts->at(_element) / (*_normalizer);
                    float min_coef = _normalized_counts.minCoeff();
                    if (std::isfinite(min_coef) == false)
                    {
                        min_coef = 0.0f;
                    }
                    _normalized_counts = _normalized_counts.unaryExpr([min_coef](float v) { return std::isfinite(v) ? v : min_coef; });

                    _min_counts = _normalized_counts.minCoeff();
                    _max_counts = _normalized_counts.maxCoeff();
                    _rows = _normalized_counts.rows();
                    _cols = _normalized_counts.cols();
                    _counts_ptr = &_normalized_counts;

                }
                else
                {
                    _min_counts = fit_counts->at(_element).minCoeff();
                    _max_counts = fit_counts->at(_element).maxCoeff();
                    _rows = fit_counts->at(_element).rows();
                    _cols = fit_counts->at(_element).cols();
                    _counts_ptr = &fit_counts->at(_element);
                }
            }
            else
            {
                _min_counts = 0.0;
                _max_counts = 0.0;
                _counts_ptr = nullptr;
            }
        }
        else
        {
            _min_counts = 0.0;
            _max_counts = 0.0;
            _counts_ptr = nullptr;
        }
    }
    else
    {
        _min_counts = 0.0;
        _max_counts = 0.0;
        _counts_ptr = nullptr;
    }

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
    if(_counts_ptr!= nullptr)
    {
        if(row > -1 && row < _rows && col > -1 && col < _cols)
        {
            *outX = (*_counts_ptr)(row,col);
        }
        *outY = _min_counts;
        *outZ = _max_counts;
    }
    else
    {

        *outX = 0.0;
        *outY = 0.0;
        *outZ = 0.0;
    }
}

/*---------------------------------------------------------------------------*/
