/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef COORDINATE_TRANSFORMER_H
#define COORDINATE_TRANSFORMER_H

/*---------------------------------------------------------------------------*/

#include <gstar/ITransformer.h>
#include <QString>
#include <QMap>

/*---------------------------------------------------------------------------*/

/**
 * @brief
 *
 */
class CoordinateTransformer : public gstar::ITransformer
{

public:

   /**
    * Constructor.
    */
   CoordinateTransformer();

   /**
    * Destructor.
    */
   virtual ~CoordinateTransformer();

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
   virtual bool getVariable(QString name, double *val);

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

   /**
    * @brief m_2xfm_x
    */
   double m_2xfm_x;

   /**
    * @brief m_2xfm_y
    */
   double m_2xfm_y;

   /**
    * @brief m_hasAllNeededGlobals
    */
   bool m_hasAllNeededGlobals;

   /**
    * @brief m_angleAlpha
    */
   double m_angle_alpha;

   /**
    * @brief m_offset_a
    */
   double m_offset_a;

   /**
    * @brief m_offset_b
    */
   double m_offset_b;

   /**
    * @brief m_offsetC
    */
   double m_offset_c;

   /**
    * @brief m_offset_d
    */
   double m_offset_d;

   /**
    * @brief m_omega
    */
   double m_omega;

   /**
    * @brief m_omega_prime
    */
   double m_omega_prime;

   /**
    * @brief m_scaling_XFM_X
    */
   double m_scaling_XFM_X;

   /**
    * @brief m_scaling_XFM_Y
    */
   double m_scaling_XFM_Y;

   /**
    * @brief m_z_offset
    */
   double m_z_offset;

   /**
    * @brief m_z_lin_x
    */
   double m_z_lin_x;

   /**
    * @brief m_z_lin_y
    */
   double m_z_lin_y;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
