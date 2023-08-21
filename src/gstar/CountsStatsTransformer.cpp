/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/CountsStatsTransformer.h>
#include <math.h>

using namespace gstar;

/*---------------------------------------------------------------------------*/

#define STR_AVG "Avg"
#define STR_MEADIAN "Median"
#define STR_STDEV "StDev"

/*---------------------------------------------------------------------------*/

CountsStatsTransformer::CountsStatsTransformer() : ITransformer()
{

   _avg = 0.0;
   _median = 0.0;
   _stdev = 0.0;

}

/*---------------------------------------------------------------------------*/

CountsStatsTransformer::~CountsStatsTransformer()
{

}

/*---------------------------------------------------------------------------*/

bool CountsStatsTransformer::Init(QMap<QString, double> globalVars)
{ 

   return true; 

} 

/*---------------------------------------------------------------------------*/

QMap<QString, double> CountsStatsTransformer::getAllCoef()
{

   QMap<QString, double> coefs;

   coefs.insert(STR_AVG, _avg);
   coefs.insert(STR_MEADIAN, _median);
   coefs.insert(STR_STDEV, _stdev);

   return coefs;

}

/*---------------------------------------------------------------------------*/

void CountsStatsTransformer::setCounts(const data_struct::ArrayXXr<float>& counts)
{
    _avg = counts.mean();
    _stdev = 0.0;
    Eigen::ArrayXf cnts(counts.size());
    int i = 0;
    for (int y = 0; y < counts.rows(); y++)
    {
        for (int x = 0; x < counts.cols(); x++)
        {
            cnts[i] = counts(y,x);
            i++;
            _stdev += pow((counts(y,x) - _avg), 2);
        }
    }
    _stdev = sqrtf(_stdev / cnts.size());

    std::sort(cnts.begin(), cnts.end(), [](float const& t1, float const& t2) { return t1 < t2; });
    int idx = cnts.size() / 2;
    _median = cnts(idx);
}

/*---------------------------------------------------------------------------*/

bool CountsStatsTransformer::getVariable(QString name, double *val)
{

   if(name == STR_AVG)
   {
      *val = _avg;
      return true;
   }
   else if(name == STR_MEADIAN)
   {
      *val = _median;
      return true;
   }
   else if (name == STR_STDEV)
   {
       *val = _stdev;
       return true;
   }

   return false;

}

/*---------------------------------------------------------------------------*/

bool CountsStatsTransformer::setVariable(QString name, double val)
{

   if(name == STR_AVG)
   {
       _avg = val;
      return true;
   }
   else if(name == STR_MEADIAN)
   {
       _median = val;
      return true;
   }
   else if (name == STR_STDEV)
   {
       _stdev = val;
       return true;
   }
   
   return false;

}

/*---------------------------------------------------------------------------*/

void CountsStatsTransformer::transformCommand(double inX,
                                         double inY,
                                         double inZ,
                                         double *outX,
                                         double *outY,
                                         double *outZ)
{
    *outX = _avg;
    *outY = _median;
    *outZ = _stdev;
    
}

/*---------------------------------------------------------------------------*/
