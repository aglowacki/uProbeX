/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsModel.h>
#include <QDebug>

/*----------------src/mvc/MapsModel.cpp \-----------------------------------------------------------*/

MapsModel::MapsModel(QString filepath)
{

   try
   {


   }
   catch (std::string& s)
   {
      throw s;
   }
   catch (...)
   {
      throw std::string("Failed to open Maps Analyzed dataset!");
   }

}

/*---------------------------------------------------------------------------*/

MapsModel::~MapsModel()
{

}

/*---------------------------------------------------------------------------*/
