/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SHELL_REGION_CALLER_H
#define SHELL_REGION_CALLER_H

//---------------------------------------------------------------------------

#include <core/RegionCaller.h>
#include <QProcess>
#include <QString>

//---------------------------------------------------------------------------

/**
 * @brief Class for calling shell/batch scripts with region box parameters
 *
 */
class ShellRegionCaller : public RegionCaller
{

public:

   /**
    * Constructor.
    */
   ShellRegionCaller();

   /**
    * Destructor.
    */
   virtual ~ShellRegionCaller();

   /**
    * @brief init
    * @param path
    * @param filename
    * @param functionnName
    * @return
    */
   virtual bool init(QString fullpath, QString module, QString functionnName);

   /**
    * @brief CallFunc
    * @param cX
    * @param cY
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
                         double factorY);


private:

   QString m_fullPath;

   bool m_exists;

};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
