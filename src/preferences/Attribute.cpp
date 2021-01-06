/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <QStringList>
#include <preferences/Attribute.h>
#include <preferences/AttributeGroup.h>

/*---------------------------------------------------------------------------*/

Attribute::Attribute()
{

   // Default initialization
   m_name = "Generic";
   m_value = "";
   m_desc = "Generic place holder Solver";
   m_enabled = false;

}

/*---------------------------------------------------------------------------*/

Attribute::Attribute(QString type,
                         QString value,
                         QString description,
                         bool enabled)
{

   m_name = type;
   m_value =  value;
   m_desc = description;
   m_enabled = enabled;

}

/*---------------------------------------------------------------------------*/

Attribute::~Attribute()
{

}

/*---------------------------------------------------------------------------*/

bool Attribute::getIsEnable()
{

   return m_enabled;

}

/*---------------------------------------------------------------------------*/

QString Attribute::getDescription()
{

   return m_desc;

}

/*---------------------------------------------------------------------------*/

QString Attribute::getValue()
{

   return m_value;

}

/*---------------------------------------------------------------------------*/

const QString Attribute::getName()
{

   return m_name;

}

/*---------------------------------------------------------------------------*/

AttributeGroup* Attribute::getParent()
{

   return m_parent;

}

/*---------------------------------------------------------------------------*/

void Attribute::setIsEnable(bool enabled)
{

   m_enabled = enabled;

}

/*---------------------------------------------------------------------------*/

void Attribute::setDescription(QString value)
{

   m_desc = value;

}

/*---------------------------------------------------------------------------*/

void Attribute::setValue(QString value)
{

   m_value = value;

}

/*---------------------------------------------------------------------------*/

void Attribute::setName(QString value)
{

   m_name = value;

}

/*---------------------------------------------------------------------------*/

void Attribute::setParent(AttributeGroup *parent)
{

   m_parent = parent;

}

/*---------------------------------------------------------------------------*/

QString Attribute::toString()
{

   return QString("%1,%2,%3,%4").arg(m_name)
                             .arg(m_value)
                             .arg(m_desc)
                             .arg(m_enabled);

}

/*---------------------------------------------------------------------------*/

void Attribute::fromString(QString val)
{
    QStringList sl = val.split(",");
    if (sl.size() >= 4)
    {
        m_name = sl.at(0);
        m_value = sl.at(1);
        m_desc = sl.at(2);
        if (sl.at(3) == "true")
        {
            m_enabled = true;
        }
        else
        {
            m_enabled = false;
        }
    }

}

/*---------------------------------------------------------------------------*/