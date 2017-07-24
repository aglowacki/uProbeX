/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GENERIC_TRANSFORM_GLOBALS_H
#define GENERIC_TRANSFORM_GLOBALS_H

/*---------------------------------------------------------------------------*/

#include <QString>
#include <QMap>

/*---------------------------------------------------------------------------*/

/**
 * @brief
 */
class CoordinateTransformGlobals
{

public:

   enum ValueKey {
        m2xfm_x
      , m2xfm_y
      , angle_alpha
      , offset_a
      , offset_b
      , offset_c
      , offset_d
      , omega
      , omega_prime
      , scaling_XFM_X
      , scaling_XFM_Y
      , z_offset
      , z_lin_x
      , z_lin_y

      ,COUNT
   };

   /**
    * @brief keyToString
    * @param key
    * @return
    */
   static QString keyToString(ValueKey key);

   /**
    * @brief generateMap
    * @return
    */
   static QMap<QString, double> generateMap();

   /**
    * @brief stringToKey
    * @param key
    * @return
    */
   static ValueKey stringToKey(QString key);

   /**
    * @brief next
    * @return
    */
   static ValueKey next(ValueKey key);

private:

   /**
    * Constructor
    */
   CoordinateTransformGlobals();

   /**
    * @brief initMap
    */
   static void initMap();

   /**
    * @brief m_map
    */
   static QMap<CoordinateTransformGlobals::ValueKey, QString> m_map;

   /**
    * @brief m_isMapInit
    */
   static bool m_isMapInit;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
