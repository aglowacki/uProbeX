/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

/*---------------------------------------------------------------------------*/

#include <QString>

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
   QString getName();

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
