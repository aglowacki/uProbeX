/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/SolverAttributesGroup.h>

//---------------------------------------------------------------------------

SolverAttributesGroup::SolverAttributesGroup()
{

   m_solverAttrs.append(Attribute("m2xfm_x", "0.0", "m2xfm_x"));
   m_solverAttrs.append(Attribute("m2xfm_y", "0.0", "m2xfm_y"));
   m_solverAttrs.append(Attribute("angle_alpha", "0.0", "angle_alpha"));
   m_solverAttrs.append(Attribute("offset_a", "0.0", "offset_a"));
   m_solverAttrs.append(Attribute("offset_b", "0.0", "offset_b"));
   m_solverAttrs.append(Attribute("offset_c", "0.0", "offset_c"));
   m_solverAttrs.append(Attribute("offset_d", "0.0", "offset_d"));
   m_solverAttrs.append(Attribute("omega", "0.0", "omega"));
   m_solverAttrs.append(Attribute("omega_prime", "0.0", "omega_prime"));
   m_solverAttrs.append(Attribute("scaling_XFM_X", "0.0", "scaling_XFM_X"));
   m_solverAttrs.append(Attribute("scaling_XFM_Y", "0.0", "scaling_XFM_Y"));
   m_solverAttrs.append(Attribute("z_offset", "0.0", "z_offset"));
   m_solverAttrs.append(Attribute("z_lin_x", "0.0", "z_lin_x"));
   m_solverAttrs.append(Attribute("z_lin_y", "0.0", "z_lin_y"));

}

//---------------------------------------------------------------------------

SolverAttributesGroup::~SolverAttributesGroup()
{

}

//---------------------------------------------------------------------------

void SolverAttributesGroup::addSolverAttribute(int type, Attribute attr)
{
    throw "error";
}

//---------------------------------------------------------------------------

Attribute SolverAttributesGroup::getAttribute(QString name) const
{
    
    for (Attribute itr : m_solverAttrs)
    {
        if (itr.getName() == name)
            return itr;
    }

    return Attribute();
}

//---------------------------------------------------------------------------

Attribute SolverAttributesGroup::getAttribute(unsigned int idx) const
{

    if (m_solverAttrs.size() < idx)
    {
       return m_solverAttrs.at(idx);
    }

    return Attribute();
}

//---------------------------------------------------------------------------

int SolverAttributesGroup::getSize() const
{

   return m_solverAttrs.size();

}

//---------------------------------------------------------------------------

void SolverAttributesGroup::fromStringList(QStringList args)
{
    m_solverAttrs.clear();
    for (int i = 0; i < args.length(); i++)
    {
        QString attr = args.at(i);
        QStringList l = attr.split(";");
        for (QString s : l)
        {
            Attribute att;
            att.fromString(s);
            m_solverAttrs.append(att);
        }
    }
}

//---------------------------------------------------------------------------

void SolverAttributesGroup::setAttribute(unsigned int idx, Attribute attr)
{

    if (m_solverAttrs.size() < idx)
    {
        m_solverAttrs.replace(idx, attr);
    }

}

//---------------------------------------------------------------------------

QStringList SolverAttributesGroup::toStringList()
{

   QStringList str;
   for (auto& itr: m_solverAttrs)
   {
      str.append(itr.toString());
      str.append(";");
   }

   return str;

}

//---------------------------------------------------------------------------
