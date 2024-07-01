/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/Profile.h>


//---------------------------------------------------------------------------

Profile::Profile()
{

   // Default initialization
   m_name = "";
   m_desc = "";
   m_filePath = "";

}

//---------------------------------------------------------------------------

Profile::Profile(const Profile &p)
{
    m_name = p.m_name;
    m_desc = p.m_desc;
    m_filePath = p.m_filePath;
    m_attrsCoefficient = p.m_attrsCoefficient;
    m_attrsOption = p.m_attrsOption;

}

//---------------------------------------------------------------------------

Profile::~Profile()
{

}

//---------------------------------------------------------------------------

QString Profile::attrsCoefficienttoString()
{

   QString str;

   QString cntStr = QString("%1").arg(m_attrsCoefficient.count());
   str.append(m_name);
   str.append(";");
   str.append(cntStr);
   for (int i = 0 ; i < m_attrsCoefficient.size() ; i++)
   {
      Attribute pva = m_attrsCoefficient.at(i);
      str.append(";");
      str.append(pva.toString());
   }

   return str;

}

//---------------------------------------------------------------------------

QString Profile::attrsOptiontoString()
{

   QString str;

   QString cntStr = QString("%1").arg(m_attrsOption.count());
   str.append(m_name);
   str.append(";");
   str.append(cntStr);
   for (int i = 0 ; i < m_attrsOption.size() ; i++)
   {
      Attribute pva = m_attrsOption.at(i);
      str.append(";");
      str.append(pva.toString());
   }

   return str;

}

//---------------------------------------------------------------------------

bool Profile::coefficientfromString(QString str)
{

   if(str.length() < 1)
      return false;

   QStringList args = str.split(";");
   if (args.size() < 3)
      return false;

   m_name = args.at(0).trimmed();

   bool ok = false;
   int cnt = args.at(1).toInt(&ok);
   if(ok && cnt+2 <= args.length())
   {
      for (int i=2; i<cnt+2; i++)
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
         Attribute Attr(key, pvStr, desc, enabled);
         m_attrsCoefficient.append(Attr);
      }
   }

   return true;
}

//---------------------------------------------------------------------------

QList<Attribute> Profile::getCoefficientAttrs()
{

   return m_attrsCoefficient;

}

//---------------------------------------------------------------------------

QList<Attribute> Profile::getOptionAttrs()
{

   return m_attrsOption;

}

//---------------------------------------------------------------------------

QString Profile::getDescription()
{

   return m_desc;

}

//---------------------------------------------------------------------------

QString Profile::getFilePath()
{

   return m_filePath;

}

//---------------------------------------------------------------------------

QString Profile::getValue()
{

   return m_desc;

}

//---------------------------------------------------------------------------

QString Profile::getName()
{

   return m_name;

}

//---------------------------------------------------------------------------

void Profile::setCoefficientAttrs(QList<Attribute> attrsCoefficient)
{

   m_attrsCoefficient.clear();
   m_attrsCoefficient = attrsCoefficient;

}

//---------------------------------------------------------------------------

void Profile::setDescription(QString value)
{

   m_desc = value;

}

//---------------------------------------------------------------------------

void Profile::setFilePath(QString filePath)
{

   m_filePath = filePath;

}

//---------------------------------------------------------------------------

void Profile::setValue(QString value)
{

   m_desc = value;

}

//---------------------------------------------------------------------------

void Profile::setName(QString value)
{

   m_name = value;

}

//---------------------------------------------------------------------------

void Profile::setOptionAttrs(QList<Attribute> attrsOption)
{

   m_attrsOption.clear();
   m_attrsOption = attrsOption;

}

//---------------------------------------------------------------------------

bool Profile::optionfromString(QString str)
{

   if(str.length() < 1)
      return false;

   QStringList args = str.split(";");
   if (args.size() < 3)
      return false;

   m_name = args.at(0).trimmed();

   bool ok = false;
   int cnt = args.at(1).toInt(&ok);
   if(ok && cnt+2 <= args.length())
   {
      for (int i=2; i<cnt+2; i++)
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
         Attribute Attr(key, pvStr, desc, enabled);
         m_attrsOption.append(Attr);
      }
   }

   return true;
}

//---------------------------------------------------------------------------
