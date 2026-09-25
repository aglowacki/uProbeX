/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

// core/PythonLoader.h (pybind11/Python.h) must be included before any Qt header in
// this translation unit -- see the comment in PythonSolver.h for why.
#include <core/PythonLoader.h>
#include <core/PythonRegionCaller.h>
#include <QMessageBox>

#include "core/defines.h"
//---------------------------------------------------------------------------

PythonRegionCaller::PythonRegionCaller()
{


}

//---------------------------------------------------------------------------

PythonRegionCaller::~PythonRegionCaller()
{

}

//---------------------------------------------------------------------------

bool PythonRegionCaller::init(QString path,
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
      QMessageBox::critical(nullptr, "PythonRegionCaller Error", px.what());
      return false;
   }

   return true;

}

//---------------------------------------------------------------------------

bool PythonRegionCaller::CallFunc(QString name,
                                  double cX,
                                  double cY,
                                  double width,
                                  double height,
                                  double factorX,
                                  double factorY)
{

   try
   {
      (*m_func)(name.toStdString(), cX, cY, width, height, factorX, factorY);
   }
   catch(const pybind11::error_already_set& ex)
   {
      logE<<ex.what();
      return false;
   }

   return true;
}

//---------------------------------------------------------------------------


