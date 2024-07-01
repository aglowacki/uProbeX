/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <solver/SV_CoordTransformer.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265359
#endif


//---------------------------------------------------------------------------

SV_CoordTransformer::SV_CoordTransformer() : ITransformer()
{


}

//---------------------------------------------------------------------------

SV_CoordTransformer::~SV_CoordTransformer()
{

}

//---------------------------------------------------------------------------

QMap<QString, double> SV_CoordTransformer::getAllCoef()
{

    QMap<QString, double> m_coefs {
       {STR_m2xfm_x, -64.1418 },
       {STR_m2xfm_y, -41.8941 },
       {STR_angle_alpha, 1.19},
       {STR_offset_a, 10.27},
       {STR_offset_b, 2.13},
       {STR_offset_c, 0.0},
       {STR_offset_d, 0.0},
       {STR_omega, 90.0},
       {STR_omega_prime, 90.04},
       {STR_scaling_XFM_X, 1.03},
       {STR_scaling_XFM_Y, 1.0},
       {STR_z_offset, 0.0},
       {STR_z_lin_x, -0.18},
       {STR_z_lin_y, 0.02},
    };
   return m_coefs;

}

//---------------------------------------------------------------------------

bool SV_CoordTransformer::Init(QMap<QString, double> globalVars)
{

    
    if(globalVars.count(STR_m2xfm_x) > 0)
    {
        m_2xfm_x = globalVars[STR_m2xfm_x];
    }
    if(globalVars.count(STR_m2xfm_y) > 0)
    {
        m_2xfm_y = globalVars[STR_m2xfm_y];
    }
    if(globalVars.count(STR_angle_alpha) > 0)
    {
        m_angle_alpha = globalVars[STR_angle_alpha];
    }
    if(globalVars.count(STR_offset_a) > 0)
    {
        m_offset_a = globalVars[STR_offset_a];
    }
    if(globalVars.count(STR_offset_b) > 0)
    {
        m_offset_b = globalVars[STR_offset_b];
    }
    if(globalVars.count(STR_offset_c) > 0)
    {
        m_offset_c = globalVars[STR_offset_c];
    }
    if(globalVars.count(STR_offset_d) > 0)
    {
        m_offset_d = globalVars[STR_offset_d];
    }
    if(globalVars.count(STR_omega) > 0)
    {
        m_omega = globalVars[STR_omega];
    }
    if(globalVars.count(STR_omega_prime) > 0)
    {
        m_omega_prime = globalVars[STR_omega_prime];
    }
    if(globalVars.count(STR_scaling_XFM_X) > 0)
    {
        m_scaling_XFM_X = globalVars[STR_scaling_XFM_X];
    }
    if(globalVars.count(STR_scaling_XFM_Y) > 0)
    {
        m_scaling_XFM_Y = globalVars[STR_scaling_XFM_Y];
    }
    if(globalVars.count(STR_z_offset) > 0)
    {
        m_z_offset = globalVars[STR_z_offset];
    }
    if(globalVars.count(STR_z_lin_x) > 0)
    {
        m_z_lin_x = globalVars[STR_z_lin_x];
    }
    if(globalVars.count(STR_z_lin_y) > 0)
    {
        m_z_lin_y = globalVars[STR_z_lin_y];
    }

    return true;

}

//---------------------------------------------------------------------------

bool SV_CoordTransformer::getVariable(QString name, double *val)
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
   else if(name == STR_angle_alpha)
   {
      *val = m_angle_alpha;
      return true;
   }
   else if(name == STR_offset_a)
   {
      *val = m_offset_a;
      return true;
   }
   else if(name == STR_offset_b)
   {
      *val = m_offset_b;
      return true;
   }
   else if(name == STR_offset_c)
   {
      *val = m_offset_c;
      return true;
   }
   else if(name == STR_offset_d)
   {
      *val = m_offset_d;
      return true;
   }
   else if(name == STR_omega)
   {
      *val = m_omega;
      return true;
   }
   else if(name == STR_omega_prime)
   {
      *val = m_omega_prime;
      return true;
   }
   else if(name == STR_scaling_XFM_X)
   {
      *val = m_scaling_XFM_X;
      return true;
   }
   else if(name == STR_scaling_XFM_Y)
   {
      *val = m_scaling_XFM_Y;
      return true;
   }
   else if(name == STR_z_offset)
   {
      *val = m_z_offset;
      return true;
   }
   else if(name == STR_z_lin_x)
   {
      *val = m_z_lin_x;
      return true;
   }
   else if(name == STR_z_lin_y)
   {
      *val = m_z_lin_y;
      return true;
   }

   return false;

}

//---------------------------------------------------------------------------

bool SV_CoordTransformer::setVariable(QString name, double val)
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
   else if(name == STR_angle_alpha)
   {
      m_angle_alpha = val;
      return true;
   }
   else if(name == STR_offset_a)
   {
      m_offset_a = val;
      return true;
   }
   else if(name == STR_offset_b)
   {
      m_offset_b = val;
      return true;
   }
   else if(name == STR_offset_c)
   {
      m_offset_c = val;
      return true;
   }
   else if(name == STR_offset_d)
   {
      m_offset_d = val;
      return true;
   }
   else if(name == STR_omega)
   {
      m_omega = val;
      return true;
   }
   else if(name == STR_omega_prime)
   {
      m_omega_prime = val;
      return true;
   }
   else if(name == STR_scaling_XFM_X)
   {
      m_scaling_XFM_X = val;
      return true;
   }
   else if(name == STR_scaling_XFM_Y)
   {
      m_scaling_XFM_Y = val;
      return true;
   }
   else if(name == STR_z_offset)
   {
      m_z_offset = val;
      return true;
   }
   else if(name == STR_z_lin_x)
   {
      m_z_lin_x = val;
      return true;
   }
   else if(name == STR_z_lin_y)
   {
      m_z_lin_y = val;
      return true;
   }

   return false;

}

//---------------------------------------------------------------------------

void SV_CoordTransformer::transformCommand(double inX,
                                             double inY,
                                             double inZ,
                                             double *outX,
                                             double *outY,
                                             double *outZ)
{

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

}

//---------------------------------------------------------------------------

