/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ATTRIBUTE_GROUP_MODEL
#define ATTRIBUTE_GROUP_MODEL

/*---------------------------------------------------------------------------*/

#include <preferences/Attribute.h>
#include <preferences/AttributeGroup.h>

#include <QString>
#include <QAbstractTableModel>
#include <QList>

/*---------------------------------------------------------------------------*/

/**
 * @brief Model to manipulate the attributes, such as how to set attributes
 * and how to get list of attributes. The model's properties could be set in
 * other class.
 */
class AttributeGroupModel
: public QAbstractTableModel
{
   Q_OBJECT

public:

   /**
    * Properties held by the model
    */
   enum Properties {
      NAME,
      VALUE,
      ENABLED,
   };

   /**
    * @brief AttributeGroupModel
    * @param parent
    */
   AttributeGroupModel(QObject* parent = 0);

   /**
    * @brief AttributeGroupModel
    * @param nameTitle
    * @param valueTitle
    * @param enabledTitle
    * @param parent
    */
   AttributeGroupModel(QString nameTitle,
                       QString valueTitle,
                       QString enabledTitle,
                       QObject* parent = 0);

   /**
    * @brief appendGroup
    * @param mGroup
    */
   void appendGroup(AttributeGroup* mGroup);

   /**
    * Clear all values from the mode.
    */
   void clearAll();

   /**
    * Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   int columnCount(const QModelIndex &parent) const;

   /**
    * Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   QVariant data(const QModelIndex &index,
                 int role = Qt::DisplayRole) const;

   /**
    * Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   Qt::ItemFlags flags(const QModelIndex &index) const;

   /**
    * List of attributes held by the model
    *
    * @return a copy of a list of all conditionals
    */
   QList<AttributeGroup*> getGroups();

   QModelIndex index(int row,
                     int column,
                     const QModelIndex& parent) const;

   /**
    * Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   QVariant headerData(int section,
                       Qt::Orientation orientation,
                       int role) const;

   QModelIndex parent(const QModelIndex& index)const;

   /**
    * @brief removeGroup
    * @param grp
    */
   void removeGroup(AttributeGroup* grp);

   /**
    * Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   int rowCount(const QModelIndex &parent) const;

   /**
    * Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   bool setData(const QModelIndex &index,
                const QVariant &value,
                int role = Qt::EditRole);

   /**
    * @brief moveDown
    * @param idx
    */
   bool moveDown(QModelIndex idx);

   /**
    * @brief moveUp
    * @param idx
    * @return
    */
   bool moveUp(QModelIndex idx);

protected slots:

   /**
    * @brief groupUpdated
    * @param grp
    */
   void Inserted(AttributeGroup* grp, int rowIdx);

   /**
    * @brief ReOrdered
    * @param grp
    * @param rowIdx
    */
   void ReOrdered(AttributeGroup* grp, int start, int end);

   /**
    * @brief groupRemoved
    * @param grp
    */
   void Removed(AttributeGroup* grp, int rowIdxs);

private:

   /**
    * Header names
    */
   QString m_headers[3];

   /**
    * @brief m_groups
    */
   QList<AttributeGroup*> m_groups;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
