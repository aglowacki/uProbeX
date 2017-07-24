/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <QApplication>

#include <core/uProbeX.h>
#include <QDateTime>
#include <QString>
#include "core/PythonLoader.h"

/*---------------------------------------------------------------------------*/

int main(int argc, char** argv)
{

   //Logger::setActive(false);

   // Startup Qt
   QApplication a(argc, argv);

   if(argc > 1)
   {
      QString arg1 = QString(argv[1]);
      if(arg1 == "-testPython")
      {
         try
         {
            PythonLoader::inst()->init();
         }
         catch(PythonLoader::pyException ex)
         {
            qDebug()<<"error main "<<ex.what();
         }

         exit(0);
      }
   }

   // Startup application
   uProbeX* widget = new uProbeX(NULL);
   widget->setAttribute(Qt::WA_DeleteOnClose);
   widget->show();

   // Run
   return a.exec();

   // Clean up
   if (widget != NULL)
   {
      delete widget;
   }

}

/*---------------------------------------------------------------------------*/
