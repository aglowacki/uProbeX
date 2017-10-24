/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <solver/CoordinateTransformer.h>
#include <preferences/CoordinateTransformGlobals.h>
#include <math.h>

#include <QDebug>
#ifndef M_PI
#define M_PI 3.14159265359
#endif


/*---------------------------------------------------------------------------*/

CoordinateTransformer::CoordinateTransformer() : ITransformer()
{

   m_hasAllNeededGlobals = false;

}

/*---------------------------------------------------------------------------*/

CoordinateTransformer::~CoordinateTransformer()
{

}

/*---------------------------------------------------------------------------*/

QMap<QString, double> CoordinateTransformer::getAllCoef()
{

   QMap<QString, double> coefs = CoordinateTransformGlobals::generateMap();

   QString sKey;

   sKey = CoordinateTransformGlobals::keyToString(
            CoordinateTransformGlobals::m2xfm_x);
   coefs[sKey] = m_2xfm_x;

   sKey = CoordinateTransformGlobals::keyToString(
            CoordinateTransformGlobals::m2xfm_y);
   coefs[sKey] = m_2xfm_y;

   sKey = CoordinateTransformGlobals::keyToString(
            CoordinateTransformGlobals::angle_alpha);
   coefs[sKey] = m_angle_alpha;

   sKey = CoordinateTransformGlobals::keyToString(
            CoordinateTransformGlobals::offset_a);
   coefs[sKey] = m_offset_a;

   sKey = CoordinateTransformGlobals::keyToString(
            CoordinateTransformGlobals::offset_b);
   coefs[sKey] = m_offset_b;

   sKey = CoordinateTransformGlobals::keyToString(
            CoordinateTransformGlobals::offset_c);
   coefs[sKey] = m_offset_c;

   sKey = CoordinateTransformGlobals::keyToString(
            CoordinateTransformGlobals::offset_d);
   coefs[sKey] = m_offset_d;

   sKey = CoordinateTransformGlobals::keyToString(
            CoordinateTransformGlobals::omega);
   coefs[sKey] = m_omega;

   sKey = CoordinateTransformGlobals::keyToString(
            CoordinateTransformGlobals::omega_prime);
   coefs[sKey] = m_omega_prime;

   sKey = CoordinateTransformGlobals::keyToString(
            CoordinateTransformGlobals::scaling_XFM_X);
   coefs[sKey] = m_scaling_XFM_X;

   sKey = CoordinateTransformGlobals::keyToString(
            CoordinateTransformGlobals::scaling_XFM_Y);
   coefs[sKey] = m_scaling_XFM_Y;

   sKey = CoordinateTransformGlobals::keyToString(
            CoordinateTransformGlobals::z_offset);
   coefs[sKey] = m_z_offset;

   sKey = CoordinateTransformGlobals::keyToString(
            CoordinateTransformGlobals::z_lin_x);
   coefs[sKey] = m_z_lin_x;

   sKey = CoordinateTransformGlobals::keyToString(
            CoordinateTransformGlobals::z_lin_y);
   coefs[sKey] = m_z_lin_y;

   return coefs;

}

/*---------------------------------------------------------------------------*/

bool CoordinateTransformer::Init(QMap<QString, double> globalVars)
{

   bool b_2xfm_x = false;
   bool b_2xfm_y = false;
   bool b_angle_alpha = false;
   bool b_offset_a = false;
   bool b_offset_b = false;
   bool b_offset_c = false;
   bool b_offset_d = false;
   bool b_omega = false;
   bool b_omega_prime = false;
   bool b_scaling_XFM_X = false;
   bool b_scaling_XFM_Y = false;
   bool b_z_offset = false;
   bool b_z_lin_x = false;
   bool b_z_lin_y = false;

   m_hasAllNeededGlobals = false;

   for(const QString &name : globalVars.keys())
   {

      if(name == CoordinateTransformGlobals::keyToString(
               CoordinateTransformGlobals::m2xfm_x))
      {
         b_2xfm_x = true;
         m_2xfm_x = globalVars[name];
      }
      else if(name == CoordinateTransformGlobals::keyToString(
                 CoordinateTransformGlobals::m2xfm_y))
      {
         b_2xfm_y = true;
         m_2xfm_y = globalVars[name];
      }
      else if(name == CoordinateTransformGlobals::keyToString(
                 CoordinateTransformGlobals::angle_alpha))
      {
         b_angle_alpha = true;
         m_angle_alpha = globalVars[name];
      }
      else if(name == CoordinateTransformGlobals::keyToString(
                 CoordinateTransformGlobals::offset_a))
      {
         b_offset_a = true;
         m_offset_a = globalVars[name];
      }
      else if(name == CoordinateTransformGlobals::keyToString(
                 CoordinateTransformGlobals::offset_b))
      {
         b_offset_b = true;
         m_offset_b = globalVars[name];
      }
      else if(name == CoordinateTransformGlobals::keyToString(
                 CoordinateTransformGlobals::offset_c))
      {
         b_offset_c = true;
         m_offset_c = globalVars[name];
      }
      else if(name == CoordinateTransformGlobals::keyToString(
                 CoordinateTransformGlobals::offset_d))
      {
         b_offset_d = true;
         m_offset_d = globalVars[name];
      }
      else if(name == CoordinateTransformGlobals::keyToString(
                 CoordinateTransformGlobals::omega))
      {
         b_omega = true;
         m_omega = globalVars[name];
      }
      else if(name == CoordinateTransformGlobals::keyToString(
                 CoordinateTransformGlobals::omega_prime))
      {
         b_omega_prime = true;
         m_omega_prime = globalVars[name];
      }
      else if(name == CoordinateTransformGlobals::keyToString(
                 CoordinateTransformGlobals::scaling_XFM_X))
      {
         b_scaling_XFM_X = true;
         m_scaling_XFM_X = globalVars[name];
      }
      else if(name == CoordinateTransformGlobals::keyToString(
                 CoordinateTransformGlobals::scaling_XFM_Y))
      {
         b_scaling_XFM_Y = true;
         m_scaling_XFM_Y = globalVars[name];
      }
      else if(name == CoordinateTransformGlobals::keyToString(
                 CoordinateTransformGlobals::z_offset))
      {
         b_z_offset = true;
         m_z_offset = globalVars[name];
      }
      else if(name == CoordinateTransformGlobals::keyToString(
                 CoordinateTransformGlobals::z_lin_x))
      {
         b_z_lin_x = true;
         m_z_lin_x = globalVars[name];
      }
      else if(name == CoordinateTransformGlobals::keyToString(
                 CoordinateTransformGlobals::z_lin_y))
      {
         b_z_lin_y = true;
         m_z_lin_y = globalVars[name];
      }

   }


   if( b_2xfm_x && b_2xfm_y && b_angle_alpha && b_offset_a && b_offset_b &&
       b_offset_c && b_offset_d && b_omega && b_omega_prime && b_scaling_XFM_X
       && b_scaling_XFM_Y && b_z_offset && b_z_lin_x && b_z_lin_y)
   {
      m_hasAllNeededGlobals = true;
   }

   return m_hasAllNeededGlobals;

}

/*---------------------------------------------------------------------------*/

bool CoordinateTransformer::getVariable(QString name, double *val)
{

   if(name == CoordinateTransformGlobals::keyToString(
            CoordinateTransformGlobals::m2xfm_x))
   {
      *val = m_2xfm_x;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::m2xfm_y))
   {
      *val = m_2xfm_y;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::angle_alpha))
   {
      *val = m_angle_alpha;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::offset_a))
   {
      *val = m_offset_a;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::offset_b))
   {
      *val = m_offset_b;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::offset_c))
   {
      *val = m_offset_c;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::offset_d))
   {
      *val = m_offset_d;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::omega))
   {
      *val = m_omega;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::omega_prime))
   {
      *val = m_omega_prime;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::scaling_XFM_X))
   {
      *val = m_scaling_XFM_X;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::scaling_XFM_Y))
   {
      *val = m_scaling_XFM_Y;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::z_offset))
   {
      *val = m_z_offset;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::z_lin_x))
   {
      *val = m_z_lin_x;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::z_lin_y))
   {
      *val = m_z_lin_y;
      return true;
   }

   return false;

}

/*---------------------------------------------------------------------------*/

bool CoordinateTransformer::setVariable(QString name, double val)
{

   if(name == CoordinateTransformGlobals::keyToString(
            CoordinateTransformGlobals::m2xfm_x))
   {
      m_2xfm_x= val;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::m2xfm_y))
   {
      m_2xfm_y = val;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::angle_alpha))
   {
      m_angle_alpha = val;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::offset_a))
   {
      m_offset_a = val;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::offset_b))
   {
      m_offset_b = val;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::offset_c))
   {
      m_offset_c = val;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::offset_d))
   {
      m_offset_d = val;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::omega))
   {
      m_omega = val;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::omega_prime))
   {
      m_omega_prime = val;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::scaling_XFM_X))
   {
      m_scaling_XFM_X = val;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::scaling_XFM_Y))
   {
      m_scaling_XFM_Y = val;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::z_offset))
   {
      m_z_offset = val;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::z_lin_x))
   {
      m_z_lin_x = val;
      return true;
   }
   else if(name == CoordinateTransformGlobals::keyToString(
              CoordinateTransformGlobals::z_lin_y))
   {
      m_z_lin_y = val;
      return true;
   }

   return false;

}

/*---------------------------------------------------------------------------*/

void CoordinateTransformer::transformCommand(double inX,
                                             double inY,
                                             double inZ,
                                             double *outX,
                                             double *outY,
                                             double *outZ)
{

   if(m_hasAllNeededGlobals)
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

}

/*---------------------------------------------------------------------------*/

