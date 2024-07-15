/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef LINEAR_COORDINATE_TRANSFORMER_H
#define LINEAR_COORDINATE_TRANSFORMER_H

//---------------------------------------------------------------------------

#include <gstar/ITransformer.h>
#include <QString>
#include <QMap>
#include "preferences/Attribute.h"
#include <unordered_map>

const QString QSTR_SlopeX = "SlopeX";
const QString QSTR_SlopeY = "SlopeY";
const QString QSTR_InterceptX = "InterceptX";
const QString QSTR_InterceptY = "InterceptY";
const QString QSTR_SlopeXY = "SlopeXY";
const QString QSTR_InterceptXY = "InterceptXY";
const QString QSTR_SlopeYX = "SlopeYX";
const QString QSTR_InterceptYX = "InterceptYX";



//---------------------------------------------------------------------------
/**
 * @brief
 *
 */
class LinearCoordTransformer : public gstar::ITransformer
{

public:

   /**
    * Constructor.
    */
   LinearCoordTransformer();

   /**
    * Destructor.
    */
   virtual ~LinearCoordTransformer();

   /**
    * @brief Init
    * @param globalVars
    * @return
    */
   bool Init(QMap<QString, double> globalVars);

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

   QMap<QString, double> _coefs;

};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
