/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MOTOR_LOOKUP_TRANSFORMER_H
#define MOTOR_LOOKUP_TRANSFORMER_H

/*---------------------------------------------------------------------------*/

#include "gstar/ITransformer.h"
#include "mvc/MapsH5Model.h"
#include <QString>
#include <QMap>
#include <vector>

/*---------------------------------------------------------------------------*/

namespace gstar
{

/**
 * @brief
 *
 */
class MotorLookupTransformer : public ITransformer
{

public:

   /**
    * Constructor.
    */
   MotorLookupTransformer();

   /**
    * Destructor.
    */
   virtual ~MotorLookupTransformer();

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

   void setMotors(const std::vector<float>& motor_x, const std::vector<float>& motor_y);

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

   //const data_struct::ArrayXXr<float>* get_motor_array() { return &_counts_arr; }

protected:

   double _motor_x;

   double _motor_y;

   int _rows;

   int _cols;

   std::vector<float> _motor_x_arr;

   std::vector<float> _motor_y_arr;
};

}
/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
