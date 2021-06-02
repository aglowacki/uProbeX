/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PROFILE_H
#define PROFILE_H

/*---------------------------------------------------------------------------*/

#include <QList>
#include <QString>
#include <QObject>
#include <QVariant>
#include <QMap>
#include <QStringList>

#include <preferences/Attribute.h>

/*---------------------------------------------------------------------------*/

//class AttributeGroup;

/*---------------------------------------------------------------------------*/

/**
 * @brief
 */
class Profile
{


public:

   /**
    * Constructor
    */
   Profile();

   Profile(const Profile &p);

   /**
    * Destructor
    */
   ~Profile();

   /**
    * @brief attrsCoefficienttoString
    * @return coefficient attrs
    */
   QString attrsCoefficienttoString();

   /**
    * @brief attrsOptiontoString
    * @return option attrs
    */
   QString attrsOptiontoString();

   /**
    * @brief fromString
    * @param str
    */
   bool fromString(QString str);

   /**
    * @brief coefficientfromString
    * @param str
    */
   bool coefficientfromString(QString str);

   /**
    * @brief optionfromString
    * @param str
    */
   bool optionfromString(QString str);

   /**
    * @brief getCoefficientAttrs
    * @return coefficient attrs
    */
   QList<Attribute> getCoefficientAttrs();

   /**
    * @brief getDescription
    * @return description
    */
   QString getDescription();

   /**
    * @brief getDescription
    * @return description
    */
   QString getFilePath();

   /**
    * @brief getOptionAttrs
    * @return option attrs
    */
   QList<Attribute> getOptionAttrs();

   /**
    * @brief getValue
    * @return value
    */
   QString getValue();

   /**
    * @brief getName
    * @return name
    */
   QString getName();

   /**
    * @brief setCoefficientAttrs
    * @param attrsCoefficient
    */
   void setCoefficientAttrs(QList<Attribute> attrsCoefficient);

   /**
    * @brief setDescription
    * @param value
    */
   void setDescription(QString value);

   /**
    * @brief setFilePath
    * @param filePath
    */
   void setFilePath(QString filePath);

   /**
    * @brief setName
    * @param value
    */
   void setName(QString value);

   /**
    * @brief setOptionAttrs
    * @param attrsOption
    */
   void setOptionAttrs(QList<Attribute> attrsOption);

   /**
    * @brief setValue
    * @param value
    */
   void setValue(QString value);

private:

   /**
    * @brief m_name
    */
   QString m_name;

   /**
    * @brief m_desc
    */
   QString m_desc;

   /**
    * @brief m_value
    */
   QString m_filePath;

   /**
    * @brief m_attrsCoefficient
    */
   QList<Attribute> m_attrsCoefficient;

   /**
    * @brief m_attrsOption
    */
   QList<Attribute> m_attrsOption;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
