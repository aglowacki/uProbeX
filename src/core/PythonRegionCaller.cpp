/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <core/PythonRegionCaller.h>
#include <core/PythonLoader.h>

#include <QDebug>
/*---------------------------------------------------------------------------*/

const static int num_args = 7;

PythonRegionCaller::PythonRegionCaller()
{


}

/*---------------------------------------------------------------------------*/

PythonRegionCaller::~PythonRegionCaller()
{

}

/*---------------------------------------------------------------------------*/

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

      if(false == PythonLoader::inst()->loadFunction(path, module, functionnName))
      {
         qDebug()<<"Failed to load function: "<<module<<" "<<functionnName;
         return false;
      }

      if(false == PythonLoader::inst()->setNumArgs(m_module, m_funcName, num_args))
      {
         qDebug()<<"Failed to set argument count";
         return false;
      }
   }
   catch(PythonLoader::pyException px)
   {
      QMessageBox::critical(nullptr, "PythonRegionCaller Error", px.what());
      return false;
   }

   return true;

}

/*---------------------------------------------------------------------------*/

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
      if( false == PythonLoader::inst()->setArgString(m_module, m_funcName, 0, name))
         return false;
      if( false == PythonLoader::inst()->setArgDouble(m_module, m_funcName, 1, cX))
         return false;
      if( false == PythonLoader::inst()->setArgDouble(m_module, m_funcName, 2, cY))
         return false;
      if( false == PythonLoader::inst()->setArgDouble(m_module, m_funcName, 3, width))
         return false;
      if( false == PythonLoader::inst()->setArgDouble(m_module, m_funcName, 4, height))
         return false;
      if( false == PythonLoader::inst()->setArgDouble(m_module, m_funcName, 5, factorX))
         return false;
      if( false == PythonLoader::inst()->setArgDouble(m_module, m_funcName, 6, factorY))
         return false;

      PythonLoader::inst()->callFunc(m_module, m_funcName, PythonLoader::RET_NONE);
   }
   catch(PythonLoader::pyException ex)
   {
      //QMessageBox::critical(0, "Error", ex.what());
      qDebug()<<ex.what();
      return false;
   }

   return true;
}

/*---------------------------------------------------------------------------*/


