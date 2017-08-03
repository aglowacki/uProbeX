/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ATTRIBUTE_GROUP_H
#define ATTRIBUTE_GROUP_H

/*---------------------------------------------------------------------------*/

#include <preferences/Attribute.h>

#include <QStringList>
#include <QMap>
#include <QObject>

/*---------------------------------------------------------------------------*/

/**
 * @brief group of attributes which is defined in the preference dialog.
 */
class AttributeGroup : public QObject
{

   Q_OBJECT

public:

   /**
    * Default constructor
    */
   AttributeGroup(QObject* parent = 0);


   /**
    * Constructor
    */
   AttributeGroup(QString name, QObject* parent = 0);

   /**
    * Destructor
    */
   ~AttributeGroup();

   /**
    * @brief append
    * @param motorPV
    */
   void append(Attribute* motorPV);

   /**
    * @brief return number of attributes
    * @return
    */
   int count();

   /**
    * Initialize pv attributes from a string list
    */
   bool fromString(QString str);

   /**
    * @brief getAttrAt
    * @param index
    * @return
    */
   Attribute* getAttrAt(int index);

   /**
    * @brief getGroupName
    * @return
    */
   QString getGroupName();

   /**
    * @brief isEnabled
    * @return
    */
   bool isEnabled();

   /**
    * @brief moveAttrDown
    * @param attr
    * @return
    */
   bool moveAttrDown(Attribute* attr);

   /**
    * @brief moveAttrUp
    * @param attr
    * @return
    */
   bool moveAttrUp(Attribute* attr);

   /**
    * @brief remove
    * @param motorPV
    */
   void remove(Attribute* motorPV);

   /**
    * @brief setEnabled
    */
   void setEnabled(bool state);

   /**
    * @brief setGroupName
    */
   void setGroupName(QString name);

   /**
    * @brief toString
    * @return
    */
   QString toString();

signals:

   /**
    * @brief pvInserted
    */
   void Inserted(AttributeGroup*, int);

   /**
    * @brief pvReOrdered
    */
   void ReOrdered(AttributeGroup*, int, int);

   /**
    * @brief pvRemoved
    */
   void Removed(AttributeGroup*, int);

protected:

   /**
    * @brief clearAndDeleteAttributes
    */
   void clearAndDeleteAttributes();

private:

   /**
    * @brief m_enabled
    */
   bool m_enabled;

   /**
    * @brief m_groupName
    */
   QString m_groupName;

   /**
    * @brief m_attrs
    */
   QList<Attribute*> m_attrs;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
