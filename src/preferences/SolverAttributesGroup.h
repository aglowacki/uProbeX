/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SOLVER_ATTRIBUTES_GROUP_H
#define SOLVER_ATTRIBUTES_GROUP_H

/*---------------------------------------------------------------------------*/
#include <QStringList>
#include <preferences/Attribute.h>
#include <QList>

/*---------------------------------------------------------------------------*/

/**
 * @brief Solver group of Solver attributes which is defined in the
 * preference dialog.
 */
class SolverAttributesGroup
{
public:

   /**
    * Default constructor
    */
   SolverAttributesGroup();

   /**
    * Destructor
    */
   ~SolverAttributesGroup();

   /**
    * Update a Solver name for component of given type.
    *
    * @param type - One of the SolverKeys enum types
    * @param attr - SolverAttribute object
    */
   void addSolverAttribute(int type, Attribute attr);

   /**
    * Initialize solver attributes from a string list
    */
   void fromStringList(QStringList args);

   /**
    * Return a SolverAttribute object with given type.
    *
    * @param type - One of the SolverKeys enum types.
    */
   Attribute getAttribute(QString name) const;

   Attribute getAttribute(unsigned int idx) const;
   /**
    * Total number of solvers
    */
   int getSize() const;

   /**
    * @brief setAttribute
    * @param type
    * @param attr
    */
   void setAttribute(unsigned int idx, Attribute attr);

   /**
    * Returns all solver attributes as a list of strings.
    */
   QStringList toStringList();

private:

   /**
    * Store key-value pairs of Solver-key and attribute
    */
   QList<Attribute> m_solverAttrs;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
