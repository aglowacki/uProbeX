/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PYTHON_REGION_CALLER_H
#define PYTHON_REGION_CALLER_H

/*---------------------------------------------------------------------------*/

#include <core/RegionCaller.h>
#include <core/PythonLoader.h>
#include <QString>

/*---------------------------------------------------------------------------*/

/**
 * @brief Class to call python functions that take 6 parameters from region box
 *
 */
class PythonRegionCaller : public RegionCaller
{

public:

   /**
    * Constructor.
    */
   PythonRegionCaller();

   /**
    * Destructor.
    */
   ~PythonRegionCaller();

   /**
    * @brief init
    * @param path
    * @param filename
    * @param functionnName
    * @return
    */
   virtual bool init(QString path, QString module, QString functionnName);

   /**
    * @brief CallFunc
    * @param cX - center x
    * @param cY - center y
    * @param width
    * @param height
    * @param factorX - direction , 1.0 right , -1.0 left
    * @param factorY - direction, 1.0 down, -1.0 up
    */
   virtual bool CallFunc(QString name,
                         double cX,
                         double cY,
                         double width,
                         double height,
                         double factorX,
                         double factorY);

private:

   QString m_module;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
