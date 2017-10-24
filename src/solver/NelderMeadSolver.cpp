/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <solver/NelderMeadSolver.h>
#include <preferences/CoordinateTransformGlobals.h>
#include <ITransformer.h>
#include <solver/asa047.hpp>
#include <exception>

using gstar::ITransformer;

/*---------------------------------------------------------------------------*/

#include <string>

const QString NelderMeadSolver::STR_ITER = "Iterations";
const QString NelderMeadSolver::STR_STEP_SIZE = "StepSize";

/*---------------------------------------------------------------------------*/

double TotalErrorFunction(double vars[], void *p)
{

   NelderMeadSolver* emin = (NelderMeadSolver*)p;
   ITransformer* transformer = emin->getTransformer();
   unsigned int numPoints = emin->getNumPoints();
   NelderMeadSolver::LightPoint* points = emin->getPoints();
   QMap<QString, int> varIndexes = emin->getVarIndexes();

   double dxSum = 0.0;
   double dySum = 0.0;
   double totalError = 0.0;
   double Px, Py, tempZ;

   for(const QString &key : varIndexes.keys())
   {
      transformer->setVariable(key, vars[varIndexes[key]]);
   }

   for(unsigned int i=0; i<numPoints; i++)
   {
      Px = 0.0;
      Py = 0.0;
      transformer->transformCommand(points[i].lx,
                                    points[i].ly,
                                    points[i].lz,
                                    &Px,
                                    &Py,
                                    &tempZ);

      double dx = (Px - points[i].mx) * 1000.0;
      dxSum += dx * dx;
      double dy = (Py - points[i].my) * 1000.0;
      dySum += dy * dy;
   }

   dxSum /= numPoints;
   dySum /= numPoints;

   totalError = dxSum + dySum;

   return totalError;

}

NelderMeadSolver::NelderMeadSolver() : AbstractSolver()
{

   m_transformer = NULL;
   m_numPoints = 0.0;
   m_mins = NULL;
   m_nvars = 0;
   m_points = NULL;
   m_dict_options.insert(STR_ITER, 100000);
   m_dict_options.insert(STR_STEP_SIZE, 0.01);

}

/*---------------------------------------------------------------------------*/

NelderMeadSolver::~NelderMeadSolver()
{

   if (m_points != NULL)
      delete [] m_points;
   m_points = NULL;

}

/*---------------------------------------------------------------------------*/

ITransformer* NelderMeadSolver::getTransformer()
{

   return m_transformer;

}

/*---------------------------------------------------------------------------*/

unsigned int NelderMeadSolver::getNumPoints()
{

   return m_numPoints;

}

/*---------------------------------------------------------------------------*/

NelderMeadSolver::LightPoint* NelderMeadSolver::getPoints()
{

   return m_points;

}

/*---------------------------------------------------------------------------*/

QMap<QString, int> NelderMeadSolver::getVarIndexes()
{

   return m_varIndexMap;

}

/*---------------------------------------------------------------------------*/

QMap<QString, double> NelderMeadSolver::getAllCoef()
{

   QMap<QString, double> dic;

   return dic;

}

/*---------------------------------------------------------------------------*/

QMap<QString, double> NelderMeadSolver::getMinCoef()
{

   return m_dict_min_coef;

}

/*---------------------------------------------------------------------------*/

QMap<QString, double> NelderMeadSolver::getOptions()
{

   return m_dict_options;

}

/*---------------------------------------------------------------------------*/

void NelderMeadSolver::setAllCoef(QMap<QString, double> vars)
{

   if(m_transformer != NULL)
   {
      if(!m_transformer->Init(vars))
      {
         //error
      }
   }

}

/*---------------------------------------------------------------------------*/

void NelderMeadSolver::setCoordPoints(QList < QMap<QString,double> > vars)
{

   if( m_points != NULL )
      delete [] m_points;

   m_numPoints = vars.count();
   //creat arrays
   m_points = new struct LightPoint[m_numPoints];
   for(unsigned int i=0; i< m_numPoints; i++)
   {
      QMap<QString,double> point = vars.at(i);
      if(point.count() >= 5)
      {
         if(point.contains("Lx") &&
            point.contains("Ly") &&
            point.contains("Lz") &&
            point.contains("Mx") &&
            point.contains("My") )
         {
            m_points[i].lx = point["Lx"];
            m_points[i].ly = point["Ly"];
            m_points[i].lz = point["Lz"];
            m_points[i].mx = point["Mx"];
            m_points[i].my = point["My"];
         }
         else
            throw std::exception();//"Missing dictionary keys");
      }
   }

}

/*---------------------------------------------------------------------------*/

void NelderMeadSolver::setMinCoef(QMap<QString, double> vars)
{

   m_dict_min_coef = vars;

}

/*---------------------------------------------------------------------------*/

void NelderMeadSolver::setOptions(QMap<QString, double> vars)
{

   if(vars.contains(STR_ITER) && vars.contains(STR_STEP_SIZE))
   {
      m_dict_options[STR_ITER] = vars[STR_ITER];
      m_dict_options[STR_STEP_SIZE] = vars[STR_STEP_SIZE];
   }

}

/*---------------------------------------------------------------------------*/

void NelderMeadSolver::setTransformer(gstar::ITransformer* transformer)
{

   m_transformer = transformer;

}

/*---------------------------------------------------------------------------*/

bool NelderMeadSolver::run()
{

   if(m_transformer == NULL)
   {
      m_lastErrorMsg = "No transformer set!";
      return false;
   }

   m_nvars = m_dict_min_coef.count();
   double* start = new double[m_nvars];
   double* xmin = new double[m_nvars];
   double ynewlo = 0.0;
   double reqmin = 0.1;
   double* step = new double[m_nvars];
   int konvge = 1;
   int kcount = m_dict_options[STR_ITER];//itrCount;
   int icount = 0;
   int numres = 0;
   int ifault = 0;

   int i = 0;
   for (const QString &key : m_dict_min_coef.keys())
   {
      m_varIndexMap.insert(key, i);
      start[i] = m_dict_min_coef[key];
      step[i] = m_dict_options[STR_STEP_SIZE];

      //start[i] = (*vars)[key].startVal;
      //step[i] = (*vars)[key].stepSize;
      i++;
   }

   nelmin(TotalErrorFunction, m_nvars, start, xmin, &ynewlo,
          reqmin, step, konvge, kcount, &icount, &numres, &ifault,
          (void*)this);

   for(const QString &key : m_dict_min_coef.keys())
   {
      m_dict_min_coef[key] = xmin[m_varIndexMap[key]];
      //(*vars)[key].minVal = xmin[m_varIndexMap[key]];
   }


   delete [] start;
   delete [] xmin;
   delete [] step;

   if(ifault == 1)
   {
      m_lastErrorMsg = "Invalid input values!";
      return false;
   }
   else if(ifault == 2)
   {
      m_lastErrorMsg = "Iterations Past Count!";
      return false;
   }
   else
      m_lastErrorMsg = "Finished";

   return true;

}

/*---------------------------------------------------------------------------*/
