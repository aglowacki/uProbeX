/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef COUNTS_STATS_TRANSFORMER_H
#define COUNTS_STATS_TRANSFORMER_H

//---------------------------------------------------------------------------

#include "gstar/ITransformer.h"
#include "mvc/MapsH5Model.h"
#include <QString>
#include <QMap>

//---------------------------------------------------------------------------

namespace gstar
{

/**
 * @brief
 *
 */
class CountsStatsTransformer : public ITransformer
{

public:

   /**
    * Constructor.
    */
   CountsStatsTransformer();

   /**
    * Destructor.
    */
   virtual ~CountsStatsTransformer();

   /**
    * Init variables
    */ 
   virtual bool Init(QMap<QString, double> globalVars); 

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
   virtual bool getVariable(QString name, double *val);

   /**
    * @brief setVariable
    * @param name
    * @param val
    * @return
    */
   virtual bool setVariable(QString name, double val);

   void setCounts(const data_struct::ArrayXXr<float> &counts);

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

   float _avg;

   float _median;

   float _stdev;
};

}
//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
