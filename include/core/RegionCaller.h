/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef REGION_CALLER_H
#define REGION_CALLER_H

/*---------------------------------------------------------------------------*/

#include <QString>

/*---------------------------------------------------------------------------*/

/**
 * @brief Abstract class for region box actions.
 *
 */
class RegionCaller
{

public:

   /**
    * Constructor.
    */
   RegionCaller();

   /**
    * Destructor.
    */
   ~RegionCaller();

   virtual bool init(QString path, QString module, QString functionnName) = 0;

   /**
    * @brief CallFunc
    * @param cX - center x
    * @param cY - center y
    * @param width
    * @param height
    * @param factorX
    * @param factorY
    */
   virtual bool CallFunc(QString name,
                         double cX,
                         double cY,
                         double width,
                         double height,
                         double factorX,
                         double factorY) = 0;

   /**
    * @brief getFuncName
    * @return
    */
   virtual QString getFuncName();

protected:
      QString m_funcName;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
