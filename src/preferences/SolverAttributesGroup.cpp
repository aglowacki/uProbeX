/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/SolverAttributesGroup.h>


/*---------------------------------------------------------------------------*/

SolverAttributesGroup::SolverAttributesGroup()
{

   Attribute solver = Attribute();
   for (int i = 0 ; i < CoordinateTransformGlobals::COUNT ; i++)
   {
      // Add a generic Solver to initialize the list.
      m_solverAttrs.append(solver);
   }

   // If you change or add a new mapping, make sure to add it to
   //  m_internalSolverIndex as well.
   m_solverAttrs.replace(CoordinateTransformGlobals::m2xfm_x,
                         Attribute("m2xfm_x", "0.0", "m2xfm_x"));
   m_solverAttrs.replace(CoordinateTransformGlobals::m2xfm_y,
                         Attribute("m2xfm_y", "0.0", "m2xfm_y"));
   m_solverAttrs.replace(CoordinateTransformGlobals::angle_alpha,
                         Attribute("angle_alpha", "0.0", "angle_alpha"));
   m_solverAttrs.replace(CoordinateTransformGlobals::offset_a,
                         Attribute("offset_a", "0.0", "offset_a"));
   m_solverAttrs.replace(CoordinateTransformGlobals::offset_b,
                         Attribute("offset_b", "0.0", "offset_b"));
   m_solverAttrs.replace(CoordinateTransformGlobals::offset_c,
                         Attribute("offset_c", "0.0", "offset_c"));
   m_solverAttrs.replace(CoordinateTransformGlobals::offset_d,
                         Attribute("offset_d", "0.0", "offset_d"));
   m_solverAttrs.replace(CoordinateTransformGlobals::omega,
                         Attribute("omega", "0.0", "omega"));
   m_solverAttrs.replace(CoordinateTransformGlobals::omega_prime,
                         Attribute("omega_prime", "0.0", "omega_prime"));
   m_solverAttrs.replace(CoordinateTransformGlobals::scaling_XFM_X,
                         Attribute("scaling_XFM_X", "0.0", "scaling_XFM_X"));
   m_solverAttrs.replace(CoordinateTransformGlobals::scaling_XFM_Y,
                         Attribute("scaling_XFM_Y", "0.0", "scaling_XFM_Y"));
   m_solverAttrs.replace(CoordinateTransformGlobals::z_offset,
                         Attribute("z_offset", "0.0", "z_offset"));
   m_solverAttrs.replace(CoordinateTransformGlobals::z_lin_x,
                         Attribute("z_lin_x", "0.0", "z_lin_x"));
   m_solverAttrs.replace(CoordinateTransformGlobals::z_lin_y,
                         Attribute("z_lin_y", "0.0", "z_lin_y"));

   m_internalSolverIndex.insert("m2xfm_x", CoordinateTransformGlobals::m2xfm_x);
   m_internalSolverIndex.insert("m2xfm_y", CoordinateTransformGlobals::m2xfm_y);
   m_internalSolverIndex.insert("angle_alpha",
                                CoordinateTransformGlobals::angle_alpha);
   m_internalSolverIndex.insert("offset_a",
                                CoordinateTransformGlobals::offset_a);
   m_internalSolverIndex.insert("offset_b",
                                CoordinateTransformGlobals::offset_b);
   m_internalSolverIndex.insert("offset_c",
                                CoordinateTransformGlobals::offset_c);
   m_internalSolverIndex.insert("offset_d",
                                CoordinateTransformGlobals::offset_d);
   m_internalSolverIndex.insert("omega", CoordinateTransformGlobals::omega);
   m_internalSolverIndex.insert("omega_prime",
                                CoordinateTransformGlobals::omega_prime);
   m_internalSolverIndex.insert("scaling_XFM_X",
                                CoordinateTransformGlobals::scaling_XFM_X);
   m_internalSolverIndex.insert("scaling_XFM_Y",
                                CoordinateTransformGlobals::scaling_XFM_Y);
   m_internalSolverIndex.insert("z_offset",
                                CoordinateTransformGlobals::z_offset);
   m_internalSolverIndex.insert("z_lin_x", CoordinateTransformGlobals::z_lin_x);
   m_internalSolverIndex.insert("z_lin_y", CoordinateTransformGlobals::z_lin_y);

}

/*---------------------------------------------------------------------------*/

SolverAttributesGroup::~SolverAttributesGroup()
{

}

/*---------------------------------------------------------------------------*/

void SolverAttributesGroup::addSolverAttribute(int type, Attribute attr)
{

   if (type > -1 && type < m_solverAttrs.size())
   {
      m_solverAttrs.replace(type, attr);
   }

}

/*---------------------------------------------------------------------------*/

Attribute SolverAttributesGroup::getAttribute(
      CoordinateTransformGlobals::ValueKey type) const
{

   if (type > -1 && type < m_solverAttrs.size())
   {
      return m_solverAttrs.at(type);
   }

   return Attribute();

}

/*---------------------------------------------------------------------------*/

Attribute SolverAttributesGroup::getAttribute(int index) const
{

   return getAttribute((CoordinateTransformGlobals::ValueKey)index);

}


/*---------------------------------------------------------------------------*/

int SolverAttributesGroup::getSize() const
{

   return m_solverAttrs.size();

}

/*---------------------------------------------------------------------------*/

void SolverAttributesGroup::fromStringList(QStringList args)
{

   for (int i = 0 ; i < args.length() ; i++)
   {
      QString attr = args.at(i);
      QStringList l = attr.split(",");
      if (l.size() != 4) continue;

      QString key = l.at(0).trimmed();
      if (m_internalSolverIndex.contains(key) != false)
      {
         int solverIndex = m_internalSolverIndex.value(key);
         if (solverIndex < 0 || solverIndex > m_solverAttrs.size())
         {
            // Invalid index, move to next argument
            continue;
         }

         Attribute solverAttr = m_solverAttrs.at(solverIndex);

         // TODO: validate that it is a valid solver name.
         QString val = l.at(1).trimmed();
         bool ok = false;
         double dval = val.toDouble(&ok);
         if(ok)
         {
            solverAttr.setValue(QString::number(dval));
         }
         else
         {
            solverAttr.setValue("0.0");
         }


         if (l.at(3).compare("1") == 0)
         {
            solverAttr.setIsEnable(true);
         }
         else
         {
            solverAttr.setIsEnable(false);
         }

         // Replace the value at the index of current item.
         m_solverAttrs.replace(solverIndex, solverAttr);
      }
   }

}

/*---------------------------------------------------------------------------*/

void SolverAttributesGroup::setAttribute(
      CoordinateTransformGlobals::ValueKey type,
      Attribute attr)
{

   m_solverAttrs.replace(type, attr);

}

/*---------------------------------------------------------------------------*/

QStringList SolverAttributesGroup::toStringList()
{

   QStringList str;
   for (int i = 0 ; i < m_solverAttrs.size() ; i++)
   {
      Attribute solvera = m_solverAttrs.at(i);
      str.append(solvera.toString());
   }

   return str;

}

/*---------------------------------------------------------------------------*/
