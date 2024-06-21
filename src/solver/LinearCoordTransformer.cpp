/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <solver/LinearCoordTransformer.h>
#include <math.h>

/*---------------------------------------------------------------------------*/

LinearCoordTransformer::LinearCoordTransformer() : ITransformer()
{


}

/*---------------------------------------------------------------------------*/

LinearCoordTransformer::~LinearCoordTransformer()
{

}

/*---------------------------------------------------------------------------*/

QMap<QString, double> LinearCoordTransformer::getAllCoef()
{

    QMap<QString, double> m_coefs {
       {STR_m2xfm_x, 0.0 },
       {STR_m2xfm_y, 0.0 }
    };
   return m_coefs;

}

/*---------------------------------------------------------------------------*/

bool LinearCoordTransformer::Init(QMap<QString, double> globalVars)
{

    
    if(globalVars.count(STR_m2xfm_x) > 0)
    {
        m_2xfm_x = globalVars[STR_m2xfm_x];
    }
    if(globalVars.count(STR_m2xfm_y) > 0)
    {
        m_2xfm_y = globalVars[STR_m2xfm_y];
    }
    
    return true;

}

/*---------------------------------------------------------------------------*/

bool LinearCoordTransformer::getVariable(QString name, double *val)
{

   if(name == STR_m2xfm_x)
   {
      *val = m_2xfm_x;
      return true;
   }
   else if(name == STR_m2xfm_y)
   {
      *val = m_2xfm_y;
      return true;
   }
  
   return false;

}

/*---------------------------------------------------------------------------*/

bool LinearCoordTransformer::setVariable(QString name, double val)
{

   if(name == STR_m2xfm_x)
   {
      m_2xfm_x= val;
      return true;
   }
   else if(name == STR_m2xfm_y)
   {
      m_2xfm_y = val;
      return true;
   }
 
   return false;

}

/*---------------------------------------------------------------------------*/

void LinearCoordTransformer::transformCommand(double inX,
                                             double inY,
                                             double inZ,
                                             double *outX,
                                             double *outY,
                                             double *outZ)
{
/*
    *outX = -(
            ( inX - m_offset_c ) *
            sin( (m_angle_alpha + m_omega_prime) / 180.0 * M_PI)
            / sin( (m_omega_prime) / 180.0 * M_PI ) - (inY - m_offset_d)
            * sin( ( m_omega - m_omega_prime - m_angle_alpha ) / 180.0 * M_PI )
            / sin( m_omega_prime / 180.0 * M_PI )
        )  * m_scaling_XFM_X + m_offset_a
        + (inZ - m_z_offset) * m_z_lin_x - m_2xfm_x;


    *outY = -(
            -(inX - m_offset_c) * sin( m_angle_alpha / 180.0 * M_PI)
            / sin( m_omega_prime / 180.0 * M_PI) + (inY - m_offset_d)
            * sin( ( m_omega_prime + m_angle_alpha ) / 180.0 * M_PI)
            / sin( m_omega_prime / 180.0 * M_PI)
        ) * m_scaling_XFM_Y + m_offset_b
        + (inZ - m_z_offset) * m_z_lin_y - m_2xfm_y;

    *outZ = 0.0;
*/
}

/*---------------------------------------------------------------------------*/

