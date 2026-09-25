/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

// core/PythonLoader.h (pybind11/Python.h) must be included before any Qt header in
// this translation unit -- see the comment in PythonSolver.h for why.
#include <core/PythonLoader.h>
#include <solver/PythonSolver.h>
#include <QMessageBox>
#include "core/defines.h"

using gstar::ITransformer;

//---------------------------------------------------------------------------

PythonSolver::PythonSolver() : AbstractSolver()
{



}

//---------------------------------------------------------------------------

PythonSolver::~PythonSolver()
{



}

//---------------------------------------------------------------------------

QMap<QString, double> PythonSolver::getAllCoef()
{

   return m_dict_transform_coef;

}

//---------------------------------------------------------------------------

QMap<QString, double> PythonSolver::getMinCoef()
{

   return m_dict_min_coef;

}

//---------------------------------------------------------------------------

QMap<QString, double> PythonSolver::getOptions()
{

   return m_dict_options;

}

//---------------------------------------------------------------------------

gstar::ITransformer* PythonSolver::getTransformer()
{

   return nullptr;

}

//---------------------------------------------------------------------------

bool PythonSolver::initialPythonSolver(QString path,
                                       QString module,
                                       QString functionnName)
{

   try
   {
      if(false == PythonLoader::inst()->isLoaded())
      {
         PythonLoader::inst()->init();
      }

      m_module = module;
      m_funcName = functionnName;

      m_func = std::make_unique<pybind11::function>(
               PythonLoader::inst()->loadFunction(path, module, functionnName));
   }
   catch(const PythonLoader::pyException& px)
   {
      QString er = QString(px.what());
      er += QString("\r\n Path: "+path
                    +"\r\nModule: "+module+
                    "\r\nFunction Name: "+functionnName);
      QMessageBox::critical(nullptr, "PythonSolver Error", er);
      return false;
   }

   return true;
}

//---------------------------------------------------------------------------

bool PythonSolver::run()
{

   try
   {
      pybind11::dict result = (*m_func)(
               PythonLoader::toPyDict(m_dict_min_coef),
               PythonLoader::toPyDict(m_dict_options),
               PythonLoader::toPyDict(m_dict_transform_coef),
               PythonLoader::toPyTupleOfTuples(m_list_coord_points)).cast<pybind11::dict>();
      m_dict_min_coef = PythonLoader::toQMapDouble(result);
   }
   catch(const pybind11::error_already_set& ex)
   {
      this->m_lastErrorMsg = ex.what();
      logE<<ex.what();
      return false;
   }

   this->m_lastErrorMsg = "Finished";

   return true;

}

//---------------------------------------------------------------------------

void PythonSolver::setAllCoef(QMap<QString, double> vars)
{

   m_dict_transform_coef = vars;

}

//---------------------------------------------------------------------------

void PythonSolver::setCoordPoints(QList < QMap<QString,double> > vars)
{

   m_list_coord_points.clear();
   for(auto &point : vars)
   {
      QList<double> cPoint;
      cPoint.append(point["Lx"]);
      cPoint.append(point["Ly"]);
      cPoint.append(point["Lz"]);
      cPoint.append(point["Mx"]);
      cPoint.append(point["My"]);

      m_list_coord_points.append(cPoint);
   }

}

//---------------------------------------------------------------------------

void PythonSolver::setMinCoef(QMap<QString, double> vars)
{

   m_dict_min_coef = vars;

}

//---------------------------------------------------------------------------

void PythonSolver::setOptions(QMap<QString, double> vars)
{

   m_dict_options = vars;

}

//---------------------------------------------------------------------------

void PythonSolver::setTransformer(gstar::ITransformer* transformer)
{

   Q_UNUSED(transformer);

}
