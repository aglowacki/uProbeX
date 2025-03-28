/*-----------------------------------------------------------------------------
 * Copyright (c) 2025, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPPED_COORDINATE_TRANSFORMER_H
#define MAPPED_COORDINATE_TRANSFORMER_H

//---------------------------------------------------------------------------

#include <gstar/ITransformer.h>
#include <QString>
#include <QMap>
#include "preferences/Attribute.h"
#include <unordered_map>


//---------------------------------------------------------------------------
/**
 * @brief
 *
 */
class MappedCoordTransformer : public gstar::ITransformer
{

public:

   /**
    * Constructor.
    */
   MappedCoordTransformer();

   /**
    * Destructor.
    */
   virtual ~MappedCoordTransformer();

   /**
    * @brief Init
    * @param globalVars
    * @return
    */
   bool Init(QMap<QString, double> globalVars);

   bool Init2(const std::vector<float>& x_axis, const std::vector<float>& y_axis);

   /**
    * @brief getAllCoef
    * @return
    */
   virtual QMap<QString, double> getAllCoef();

   /**
    * @brief getVariable
    * @param name
    * @param val
    * @return
    */
   virtual bool getVariable(QString name, double* val);

   /**
    * @brief setVariable
    * @param name
    * @param val
    * @return
    */
   virtual bool setVariable(QString name, double val);

   /**
    * @brief transformCommand
    * @param inX
    * @param inY
    * @param inZ
    * @param outX
    * @param outY
    */
   virtual void transformCommand(double inX,
                                 double inY,
                                 double inZ,
                                 double *outX,
                                 double *outY,
                                 double *outZ);

protected:

   std::vector<float> _x_axis;
   std::vector<float> _y_axis;
   //std::vector<float> _z_axis;

};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
