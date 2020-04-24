/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <core/ShellRegionCaller.h>

#include <QFile>
#include <QStringList>
/*---------------------------------------------------------------------------*/


ShellRegionCaller::ShellRegionCaller()
{

   m_exists = false;

}

/*---------------------------------------------------------------------------*/

ShellRegionCaller::~ShellRegionCaller()
{

}

/*---------------------------------------------------------------------------*/

bool ShellRegionCaller::init(QString fullpath,
                             QString module,
                             QString functionname)
{

   Q_UNUSED(module);
   Q_UNUSED(functionname);

   m_fullPath = fullpath;

   m_exists = QFile(fullpath).exists();

   return m_exists;

}

/*---------------------------------------------------------------------------*/

bool ShellRegionCaller::CallFunc( QString name,
                                  double cX,
                                  double cY,
                                  double width,
                                  double height,
                                  double factorX,
                                  double factorY)
{
   bool ret = false;

   if(m_exists)
   {
      QStringList arguments;
      arguments << name
                << QString::number(cX)
                << QString::number(cY)
                << QString::number(width)
                << QString::number(height)
                << QString::number(factorX)
                << QString::number(factorY);

      QProcess myProcess; 
      myProcess.start(m_fullPath, arguments);

      ret = myProcess.waitForFinished();    
   }

   return ret;
}

/*---------------------------------------------------------------------------*/
