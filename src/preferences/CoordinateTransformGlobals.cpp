/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/CoordinateTransformGlobals.h>

/*---------------------------------------------------------------------------*/

bool CoordinateTransformGlobals::m_isMapInit = false;
QMap<CoordinateTransformGlobals::ValueKey, QString>
CoordinateTransformGlobals::m_map;

CoordinateTransformGlobals::CoordinateTransformGlobals()
{

}

void CoordinateTransformGlobals::initMap()
{

   m_map.insert(m2xfm_x, "m2xfm_x");
   m_map.insert(m2xfm_y, "m2xfm_y");

   m_map.insert(angle_alpha, "angle_alpha");
   m_map.insert(offset_a, "offset_a");
   m_map.insert(offset_b, "offset_b");
   m_map.insert(offset_c, "offset_c");
   m_map.insert(offset_d, "offset_d");
   m_map.insert(omega, "omega");
   m_map.insert(omega_prime, "omega_prime");
   m_map.insert(scaling_XFM_X, "scaling_XFM_X");
   m_map.insert(scaling_XFM_Y, "scaling_XFM_Y");
   m_map.insert(z_offset, "z_offset");
   m_map.insert(z_lin_x, "z_lin_x");
   m_map.insert(z_lin_y, "z_lin_y");

   m_isMapInit = true;

}

/*---------------------------------------------------------------------------*/

QString CoordinateTransformGlobals::keyToString(
      CoordinateTransformGlobals::ValueKey key)
{

   if(m_isMapInit == false)
      initMap();

   return m_map[key];

}

/*---------------------------------------------------------------------------*/

QMap<QString, double> CoordinateTransformGlobals::generateMap()
{

   if(m_isMapInit == false)
      initMap();

   QMap<QString, double> map;

   for(ValueKey i = m2xfm_x; i<COUNT; i=next(i))
   {
      map[m_map[i]] = 0.0;
   }

   /*
   map.insert(m_map[m2xfm_x], 0.0);
   map.insert(m_map[m2xfm_y], 0.0);
   map.insert(m_map[angle_alpha], 0.0);
   map.insert(m_map[offset_a], 0.0);
   map.insert(m_map[offset_b], 0.0);
   map.insert(m_map[offset_c], 0.0);
   map.insert(m_map[offset_d], 0.0);
   map.insert(m_map[omega], 0.0);
   map.insert(m_map[omega_prime], 0.0);
   map.insert(m_map[scaling_XFM_X], 0.0);
   map.insert(m_map[scaling_XFM_Y], 0.0);
   map.insert(m_map[z_offset], 0.0);
   map.insert(m_map[z_lin_x], 0.0);
   map.insert(m_map[z_lin_y], 0.0);
   */

   return map;

}

/*---------------------------------------------------------------------------*/

CoordinateTransformGlobals::ValueKey CoordinateTransformGlobals::stringToKey(
      QString key)
{

   for(ValueKey i = m2xfm_x; i<COUNT; i=next(i))
   {
      if(key == m_map[i])
         return i;
   }
   return COUNT;

}

/*---------------------------------------------------------------------------*/

CoordinateTransformGlobals::ValueKey CoordinateTransformGlobals::next(
      ValueKey key)
{

   switch(key)
   {
      case m2xfm_x:
         return m2xfm_y;

      case m2xfm_y:
         return angle_alpha;

      case angle_alpha:
         return offset_a;

      case offset_a:
         return offset_b;

      case offset_b:
         return offset_c;

      case offset_c:
         return offset_d;

      case offset_d:
         return omega;

      case omega:
         return omega_prime;

      case omega_prime:
         return scaling_XFM_X;

      case scaling_XFM_X:
         return scaling_XFM_Y;

      case scaling_XFM_Y:
         return z_offset;

      case z_offset:
         return z_lin_x;

      case z_lin_x:
         return z_lin_y;

      case z_lin_y:
         return COUNT;

      default:
         return COUNT;
   };

}

/*---------------------------------------------------------------------------*/
