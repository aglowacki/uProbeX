/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef LINEAR_TRANSFORMER_H
#define LINEAR_TRANSFORMER_H

//---------------------------------------------------------------------------

#include "gstar/ITransformer.h"
#include <QString>
#include <QMap>

//---------------------------------------------------------------------------

namespace gstar
{

/**
 * @brief
 *
 */
class LinearTransformer : public ITransformer
{

public:

   /**
    * Constructor.
    */
   LinearTransformer();

   /**
    * Destructor.
    */
   virtual ~LinearTransformer();

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
    * Get scale X
    */
   double getScaleX();

   /**
    * Get scale Y
    */
   double getScaleY();

   /**
    * Get scale Z
    */
   double getScaleZ();

   /**
    * Get the top left X
    */
   double getTopLeftX();

   /**
    * Get the top left Y
    */
   double getTopLeftY();

   /**
    * @brief getVariable
    * @param name
    * @param val
    * @return
    */
   virtual bool getVariable(QString name, double *val);

   /**
    * @brief setDecimalPrecision
    * @param number
    */
   //void setDecimalPrecision(int number);

   /**
    * @brief setVariable
    * @param name
    * @param val
    * @return
    */
   virtual bool setVariable(QString name, double val);

   /**
    * @brief setDivider
    * @param dx
    * @param dy
    * @param dz
    */
   void setDivider(double dx, double dy, double dz);

   /**
    * @brief setScale
    * @param x
    * @param y
    * @param z
    */
   void setScale(double x, double y, double z);

   /**
    * Set scale X
    */
   void setScaleX(double x);

   /**
    * Set scale Y
    */
   void setScaleY(double y);

   /**
    * Set scale Z
    */
   void setScaleZ(double z);

   /**
    * Set the top left X and Y
    */
   void setTopLeft(double x, double y);

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

   /**
    * @brief m_decimalPreci
    */
   //int m_decimalPreci;

   /**
    * @brief m_divider_x
    */
   double m_divider_x;

   /**
    * @brief m_divider_y
    */
   double m_divider_y;

   /**
    * @brief m_divider_z
    */
   double m_divider_z;

   /**
   * Top left x position
   */
   double m_topLeftX;

   /**
   * Top left y position
   */
   double m_topLeftY;

   /**
    * @brief m_topLeftZ
    */
   double m_topLeftZ;

   /**
    * m_scaleX
    */
   double m_scaleX;

   /**
    * m_scaleY
    */
   double m_scaleY;

   /**
    * m_scaleZ
    */
   double m_scaleZ;

};

}
//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
