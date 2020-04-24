/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <solver/PythonTransformer.h>
//#include <QLibrary>

#include <core/PythonLoader.h>

#include "core/defines.h"
/*---------------------------------------------------------------------------*/

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

      if(false == PythonLoader::inst()->loadFunction(path, filename, functionnName))
      {
         logW<<"Failed to load function: "<<filename.toStdString()<<" "<<functionnName.toStdString() << "\n";
         return;
      }

      if(false == PythonLoader::inst()->setNumArgs(m_module, m_funcName, 4))
      {
         logW<<"Failed to set argument count";
         return;
      }

      if(false == PythonLoader::inst()->setRetCnt(m_module, m_funcName, 3))
      {
         logW<<"Failed to set argument count";
         return;
      }
   }
   catch(PythonLoader::pyException px)
   {
      logW<<px.what();
      QString er = QString(px.what());
      er += QString("\r\n Path: "+path
                    +"\r\nModule: "+filename+
                    "\r\nFunction Name: "+functionnName);
      QMessageBox::critical(nullptr, "PythonLoader Error", er);
      return;
   }

   m_foundFuncs = true;

}

/*---------------------------------------------------------------------------*/

PythonTransformer::~PythonTransformer()
{

}

/*---------------------------------------------------------------------------*/

QMap<QString, double> PythonTransformer::getAllCoef()
{

   return m_globalVars;

}

/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/

bool PythonTransformer::foundFunctions()
{

    return m_foundFuncs;

}

/*---------------------------------------------------------------------------*/

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

   return PythonLoader::inst()->setArgDict(m_module, m_funcName, 0, globalVars);

}

/*---------------------------------------------------------------------------*/

bool PythonTransformer::setVariable(QString name, double val)
{

    if(m_globalVars.contains(name))
    {
        m_globalVars[name] = val;

        if(false == PythonLoader::inst()->setArgDict(m_module,
                                                     m_funcName,
                                                     0,
                                                     m_globalVars))
        {
           QMessageBox::critical(0, "Python Transformer",
                                "Argument failed to set.");
           return false;
        }
        return true;
    }
    else
    {
        return false;
    }

}

/*---------------------------------------------------------------------------*/

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
      PythonLoader::inst()->setArgDouble(m_module, m_funcName, 1, inX);
      PythonLoader::inst()->setArgDouble(m_module, m_funcName, 2, inY);
      PythonLoader::inst()->setArgDouble(m_module, m_funcName, 3, inZ);


      PythonLoader::inst()->callFunc(m_module,
                                     m_funcName,
                                     PythonLoader::RET_LIST);
      PythonLoader::inst()->getRetDouble(m_module, m_funcName, 0, outX);
      PythonLoader::inst()->getRetDouble(m_module, m_funcName, 1, outY);
      PythonLoader::inst()->getRetDouble(m_module, m_funcName, 2, outZ);
   }
   catch(PythonLoader::pyException ex)
   {
      logW<<ex.what();
   }

}

/*---------------------------------------------------------------------------*/
