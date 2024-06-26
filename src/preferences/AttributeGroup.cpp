/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/AttributeGroup.h>


//---------------------------------------------------------------------------

AttributeGroup::AttributeGroup(QObject* parent) : QObject(parent)
{

   m_enabled = true;

}

//---------------------------------------------------------------------------

AttributeGroup::AttributeGroup(QString name, QObject* parent)
   : QObject(parent)
{

   m_groupName = name;
   m_enabled = true;

}

//---------------------------------------------------------------------------

AttributeGroup::~AttributeGroup()
{

   clearAndDeleteAttributes();

}

//---------------------------------------------------------------------------

void AttributeGroup::append(Attribute *motorPV)
{

   motorPV->setParent(this);
   m_attrs.append(motorPV);
   int idx = m_attrs.indexOf(motorPV);

   emit Inserted(this, idx);

}

//---------------------------------------------------------------------------

void AttributeGroup::clearAndDeleteAttributes()
{

   for(Attribute* attr : m_attrs)
   {
      delete attr;
      attr = nullptr;
   }

   m_attrs.clear();

}

//---------------------------------------------------------------------------

int AttributeGroup::count()
{

   return m_attrs.count();

}

//---------------------------------------------------------------------------

bool AttributeGroup::fromString(QString str)
{

   if(str.length() < 1)
      return false;

   QStringList args = str.split(";");
   if (args.size() < 4)
      return false;

   clearAndDeleteAttributes();

   m_groupName = args.at(0).trimmed();
   if (args.at(1).compare("1") == 0)
   {
      m_enabled = true;
   }
   else
   {
      m_enabled = false;
   }

   bool ok = false;
   int cnt = args.at(2).toInt(&ok);
   if(ok && cnt+3 <= args.length())
   {
      for (int i=3; i<cnt+3; i++)
      {
         QString attr = args.at(i);
         QStringList l = attr.split(",");
         if (l.size() != 4) continue;

         QString key = l.at(0).trimmed();
         QString pvStr = l.at(1).trimmed();
         QString desc = l.at(2).trimmed();
         bool enabled = false;
         if (l.at(3).compare("1") == 0 && pvStr.length() > 0)
         {
            enabled = true;
         }
         Attribute* Attr = new Attribute(key, pvStr, desc, enabled);
         append(Attr);
      }
   }

   return true;
}

//---------------------------------------------------------------------------

Attribute* AttributeGroup::getAttrAt(int index)
{

   Attribute* attr = nullptr;
   if(index > - 1 && index < m_attrs.count())
   {
      attr = m_attrs.at(index);
   }
   return attr;

}

//---------------------------------------------------------------------------

QString AttributeGroup::getGroupName()
{

   return m_groupName;

}

//---------------------------------------------------------------------------

bool AttributeGroup::isEnabled()
{

   return m_enabled;

}

//---------------------------------------------------------------------------

bool AttributeGroup::moveAttrDown(Attribute *attr)
{

   int pos = m_attrs.indexOf(attr);
   if(pos > -1 && pos < m_attrs.count() - 1)
   {
      m_attrs.removeAt(pos);
      m_attrs.insert(pos+1, attr);
      emit ReOrdered(this, pos, pos+1);
      return true;
   }
   return false;

}

//---------------------------------------------------------------------------

bool AttributeGroup::moveAttrUp(Attribute *attr)
{

   int pos = m_attrs.indexOf(attr);
   if(pos > 0 && pos < m_attrs.count())
   {
      m_attrs.removeAt(pos);
      m_attrs.insert(pos-1, attr);
      emit ReOrdered(this, pos-1, pos);
      return true;
   }
   return false;

}

//---------------------------------------------------------------------------

void AttributeGroup::remove(Attribute *motorPV)
{

   int idx = m_attrs.indexOf(motorPV);
   m_attrs.removeOne(motorPV);

   emit Removed(this, idx);

}

//---------------------------------------------------------------------------

void AttributeGroup::setEnabled(bool state)
{

   m_enabled = state;
   for(Attribute* attr : m_attrs)
   {
      attr->setIsEnable(state);
   }
   emit ReOrdered(this, 0, count());

}

//---------------------------------------------------------------------------

void AttributeGroup::setGroupName(QString name)
{

   m_groupName = name;

}

//---------------------------------------------------------------------------

QString AttributeGroup::toString()
{

   QString str;
   QString enableStr = QString("%1").arg(m_enabled);
   QString cntStr = QString("%1").arg(m_attrs.count());
   str.append(m_groupName);
   str.append(";");
   str.append(enableStr);
   str.append(";");
   str.append(cntStr);
   for (int i = 0 ; i < m_attrs.size() ; i++)
   {
      Attribute* pva = m_attrs.at(i);
      str.append(";");
      str.append(pva->toString());
   }

   return str;

}

//---------------------------------------------------------------------------
