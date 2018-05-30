/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_COORDINATE_MODEL_H
#define GSTAR_COORDINATE_MODEL_H

/*---------------------------------------------------------------------------*/

#include <QtGui>

/*---------------------------------------------------------------------------*/

namespace gstar
{
   class ITransformer;
}

/*---------------------------------------------------------------------------*/

namespace gstar
{

class CoordinateModel : public QObject
{

Q_OBJECT

public:
   /**
    * Constructor
    */
   CoordinateModel();

   /**
    * @brief CoordinateModel
    * @param transformer
    */
   CoordinateModel(ITransformer* transformer);

   /**
    * Destructor
    */
   ~CoordinateModel();

   /**
    * @brief getTransformer
    * @return
    */
   ITransformer* getTransformer();

   /**
    * @brief getTransformerVariable
    * @param name
    * @param val
    * @return
    */
   bool getTransformerVariable(QString name, double *val);

   /**
    * @brief SetTransformerVariable
    * @param name
    * @param val
    * @return
    */
   bool setTransformerVariable(QString name, double val);

   /**
    * @brief setTransformer
    * @param transformer
    */
   void setTransformer(ITransformer* transformer);

   /**
    * @brief setTransformerPrecision
    * @param number
    */
   void setTransformerPrecision(int number);

   /**
    * @brief runTransformer
    */
   void runTransformer(double inX,
                       double inY,
                       double inZ,
                       double *outX,
                       double *outY,
                       double *outZ);

signals:

   /**
    * @brief modelUpdated
    */
   void modelUpdated();

   /**
    * @brief transformOutput
    * @param outX
    * @param outY
    * @param outZ
    */
   void transformOutput(double outX, double outY, double outZ);

protected:

   /**
    * @brief transformer
    */
   ITransformer* m_transformer;

};

}

#endif
