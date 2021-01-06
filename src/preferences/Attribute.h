/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

/*---------------------------------------------------------------------------*/

#include <QString>

#define STR_m2xfm_x "m2xfm_x"
#define STR_m2xfm_y "2xfm_y"
#define STR_angle_alpha "angle_alpha"
#define STR_offset_a "offset_a"
#define STR_offset_b "offset_b"
#define STR_offset_c "offset_c"
#define STR_offset_d "offset_d"
#define STR_omega "omega"
#define STR_omega_prime "omega_prime"
#define STR_scaling_XFM_X "scaling_XFM_X"
#define STR_scaling_XFM_Y "scaling_XFM_Y"
#define STR_z_offset "z_offset"
#define STR_z_lin_x "z_lin_x"
#define STR_z_lin_y "z_lin_y"

/*---------------------------------------------------------------------------*/

class AttributeGroup;

/*---------------------------------------------------------------------------*/

/**
 * @brief Attributes definition class, each Solver attribute can have its own
 * type, name, description.
 */
class Attribute
{

public:

   /**
    * Constructor
    */
   Attribute();

   /**
    * Constructor
    */
   Attribute(QString name,
               QString value,
               QString description,
               bool enabled=false);

   /**
    * Destructor
    */
   ~Attribute();

   /**
    * Check weather the enable flag is turned on.
    *
    * @return boolean flag value
    */
   bool getIsEnable();

   /**
    * Description of the Solver e.g. A
    *
    * @return Type of the Solver.
    */
   QString getDescription();

   /**
    * value of the Solver e.g perch:m1
    *
    * @return Type of the Solver.
    */
   QString getValue();

   /**
    * Type of the Solver e.g. motor record
    *
    * @return Type of the Solver.
    */
   const QString getName();

   /**
    * @brief getParent
    * @return
    */
   AttributeGroup* getParent();

   /**
    * Set the enable flag
    *
    * @param boolean flag
    */
   void setIsEnable(bool enable);

   /**
    * Set Solver description
    */
   void setDescription(QString value);

   /**
    * Set the name of the Solver
    */
   void setValue(QString value);

   /**
    * @brief setParent
    * @param parent
    */
   void setParent(AttributeGroup* parent);

   /**
    * Set the type of the Solver
    */
   void setName(QString value);

   /**
    * Get as a QVariant string type
    */
   QString toString();

   void fromString(QString val);

private:

   /**
    * @brief m_parent
    */
   AttributeGroup* m_parent;

   /**
    * Type of solver e.g. motor record
    */
   QString m_name;

   /**
    * Value
    */
   QString m_value;

   /**
    * A brief description of the Solver.
    */
   QString m_desc;

   /**
    * Indicate weather this Solver is enabled or not.
    */
   bool m_enabled;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
