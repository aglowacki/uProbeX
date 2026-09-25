/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

// core/PythonLoader.h (pybind11/Python.h) must be included before any Qt header in
// this translation unit -- see the comment in PythonSolver.h for why.
#include <core/PythonLoader.h>
#include <solver/PythonTransformer.h>
#include <QMessageBox>

#include "core/defines.h"
//---------------------------------------------------------------------------

PythonTransformer::PythonTransformer(QString path,
                                     QString filename,
                                     QString functionnName) : ITransformer()
{

   try
   {
      if(false == PythonLoader::inst()->isLoaded())
      {
         PythonLoader::inst()->init();
      }

      m_foundFuncs = false;
      m_module = filename;
      m_funcName = functionnName;

      m_func = std::make_unique<pybind11::function>(
               PythonLoader::inst()->loadFunction(path, filename, functionnName));
   }
   catch(const PythonLoader::pyException& px)
   {
      logE<<px.what();
      QString er = QString(px.what());
      er += QString("\r\n Path: "+path
                    +"\r\nModule: "+filename+
                    "\r\nFunction Name: "+functionnName);
      QMessageBox::critical(nullptr, "PythonLoader Error", er);
      return;
   }

   m_foundFuncs = true;

}

//---------------------------------------------------------------------------

PythonTransformer::~PythonTransformer()
{

}

//---------------------------------------------------------------------------

QMap<QString, double> PythonTransformer::getAllCoef()
{

   return m_globalVars;

}

//---------------------------------------------------------------------------

bool PythonTransformer::getVariable(QString name, double *val)
{

    if(m_globalVars.contains(name))
    {
        *val = m_globalVars[name];
        return true;
    }
    else
    {
        return false;
    }

}

//---------------------------------------------------------------------------

bool PythonTransformer::foundFunctions()
{

    return m_foundFuncs;

}

//---------------------------------------------------------------------------

bool PythonTransformer::Init(QMap<QString, double> globalVars)
{

    if(!m_globalVars.empty())
        m_globalVars.clear();

    QMap<QString, double>::const_iterator i = globalVars.constBegin();

    while (i != globalVars.constEnd())
    {
        m_globalVars.insert(i.key(), i.value());
        i++;
    }

   return true;

}

//---------------------------------------------------------------------------

bool PythonTransformer::setVariable(QString name, double val)
{

    if(m_globalVars.contains(name))
    {
        m_globalVars[name] = val;
        return true;
    }
    else
    {
        return false;
    }

}

//---------------------------------------------------------------------------

void PythonTransformer::transformCommand(double inX,
                                         double inY,
                                         double inZ,
                                         double *outX,
                                         double *outY,
                                         double *outZ)
{

   *outX = 0.0;
   *outY = 0.0;
   *outZ = 0.0;

   try
   {
      pybind11::list result = (*m_func)(PythonLoader::toPyDict(m_globalVars), inX, inY, inZ).cast<pybind11::list>();
      if (result.size() != 3)
      {
         throw PythonLoader::pyException("my_transform must return a 3 element list [x, y, z]");
      }
      *outX = result[0].cast<double>();
      *outY = result[1].cast<double>();
      *outZ = result[2].cast<double>();
   }
   catch(const pybind11::error_already_set& ex)
   {
      logE<<ex.what();
   }
   catch(const PythonLoader::pyException& ex)
   {
      logE<<ex.what();
   }

}

//---------------------------------------------------------------------------
