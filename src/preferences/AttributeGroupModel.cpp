/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/AttributeGroupModel.h>

#include <preferences/AttributeGroup.h>

//---------------------------------------------------------------------------

AttributeGroupModel::AttributeGroupModel(QObject* parent)
: QAbstractTableModel(parent)
{

   // Initialize header data
   m_headers[NAME] = tr("Component");
   m_headers[VALUE] = tr("PV Name");
   m_headers[ENABLED] = tr("Enable");

}

//---------------------------------------------------------------------------

AttributeGroupModel::AttributeGroupModel(QString nameTitle,
                                         QString valueTitle,
                                         QString enabledTitle,
                                         QObject* parent)
: QAbstractTableModel(parent)
{

   // Initialize header data
   m_headers[NAME] = nameTitle;
   m_headers[VALUE] = valueTitle;
   m_headers[ENABLED] = enabledTitle;

}

//---------------------------------------------------------------------------

void AttributeGroupModel::appendGroup(AttributeGroup *mGroup)
{

   int s = m_groups.size();
   beginInsertRows(QModelIndex(), s, s+1);
   m_groups.append(mGroup);
   endInsertRows();

   connect(mGroup,
           SIGNAL(Inserted(AttributeGroup*, int)),
           this,
           SLOT(Inserted(AttributeGroup*, int)));

   connect(mGroup,
           SIGNAL(ReOrdered(AttributeGroup*, int, int)),
           this,
           SLOT(ReOrdered(AttributeGroup*, int, int)));

   connect(mGroup,
           SIGNAL(Removed(AttributeGroup*, int)),
           this,
           SLOT(Removed(AttributeGroup*, int)));

}

//---------------------------------------------------------------------------

void AttributeGroupModel::clearAll()
{

   // Remove all rows
   while (rowCount(QModelIndex()) > 0)
   {
      removeRows(0, 1);
   }
}

//---------------------------------------------------------------------------

int AttributeGroupModel::columnCount(const QModelIndex &parent) const
{

   // Mark unused
   Q_UNUSED(parent);

   // Return 4 columns
   return 3;

}

//---------------------------------------------------------------------------

QVariant AttributeGroupModel::data(const QModelIndex &index, int role) const
{

   // Check valid index
   if (!index.isValid())
   {
      return QVariant();
   }

   QModelIndex parent = index.parent();
   if(!parent.isValid())
   {
      // Check valid index
      if (index.row() >= m_groups.size() || index.row() < 0)
      {
         return QVariant();
      }

      AttributeGroup* mGroup = m_groups.at(index.row());

      if (role == Qt::DisplayRole || role == Qt::EditRole)
      {
         if (index.column() == NAME)
            return mGroup->getGroupName();
         else if (index.column() == VALUE)
            //return mGroup->getGroupName();
            return QVariant();
         else if (index.column() == ENABLED)
            return mGroup->isEnabled();
         else
            return QVariant();
      }
      return QVariant();
   }
   else
   {
      AttributeGroup* mGroup = m_groups.at(parent.row());
      Attribute* attr = mGroup->getAttrAt(index.row());

      if (attr == nullptr)
         return QVariant();

      // Return values for display and edit roles
      if (role == Qt::DisplayRole || role == Qt::EditRole)
      {

         // Insert data
         if (index.column() == NAME) return attr->getName();
         else if (index.column() == VALUE) return attr->getValue();
         else if (index.column() == ENABLED)
         {
            if (attr->getValue().length() > 0) return attr->getIsEnable();
            else return false;
         }

      }

      else if (role == Qt::ToolTipRole)
      {
         return attr->getDescription();
      }
   }
   // Return empty data
   return QVariant();

}

//---------------------------------------------------------------------------

Qt::ItemFlags AttributeGroupModel::flags(const QModelIndex &index) const
{

   // Check valid index
   if (!index.isValid())
   {
      return Qt::ItemIsSelectable;
   }

   // Make all column edit-able except first column
   // Only enable column 2 if there is a pv name
   if (index.column() == NAME)
   {
      return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
   }
   else if (index.column() == VALUE)
   {
      if(index.parent().isValid() == false)
      {
         return Qt::ItemIsSelectable;
      }
      return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
   }
   else if (index.column() == ENABLED)
   {
      return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
/*
      if(index.parent().isValid() == false)
      {
         return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
      }

      // Get attribute
      Attribute* attr = static_cast<Attribute*>(index.internalPointer());
      if (attr == nullptr)
         return Qt::ItemIsSelectable;

      // If no pv name is set, disable the cell
      if (attr->getValue().length() > 0)
      {
         return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
      }
      */
   }

   return Qt::ItemIsSelectable;

}

//---------------------------------------------------------------------------

QList<AttributeGroup*> AttributeGroupModel::getGroups()
{

   // Return a copy of the list
   return m_groups;

}

//---------------------------------------------------------------------------

QModelIndex AttributeGroupModel::index(int row,
                                 int column,
                                 const QModelIndex& parent) const
{

   AttributeGroup* item = nullptr;

   if (m_groups.size() < 1)
   {
      return QModelIndex();
   }

   if(!parent.isValid())
   {
      if (row < m_groups.size())
      {
         item = m_groups.at(row);
         return createIndex(row, column, item);
      }
      return QModelIndex();
   }

   item = static_cast<AttributeGroup*>(parent.internalPointer());
   if(item == nullptr)
   {
      return QModelIndex();
   }
   else
   {
      Attribute* childItem = item->getAttrAt(row);

      if (childItem != nullptr)
      {
         return createIndex(row, column, childItem);
      }
   }
   return QModelIndex();

}

//---------------------------------------------------------------------------

bool AttributeGroupModel::moveDown(QModelIndex idx)
{

   if(idx.isValid())
   {
      AttributeGroup* grp = static_cast<AttributeGroup*>(idx.internalPointer());
      int pos = m_groups.indexOf(grp);
      if(pos > -1 && pos < m_groups.count() - 1)
      {
         beginResetModel();
         m_groups.removeAt(pos);
         m_groups.insert(pos+1, grp);
         endResetModel();
         /*
         emit dataChanged(index(idx.row(), 0, idx.parent()),
                          index(idx.row()+1, 2, idx.parent()));
         */
         return true;
      }
   }
   return false;

}

//---------------------------------------------------------------------------

bool AttributeGroupModel::moveUp(QModelIndex idx)
{

   if(idx.isValid())
   {
      AttributeGroup* grp = static_cast<AttributeGroup*>(idx.internalPointer());
      int pos = m_groups.indexOf(grp);
      if(pos > 0 && pos < m_groups.count())
      {
         beginResetModel();
         m_groups.removeAt(pos);
         m_groups.insert(pos-1, grp);
         endResetModel();
         /*
         emit dataChanged(index(idx.row()-1, 0, idx.parent()),
                          index(idx.row(), 2, idx.parent()));
         */
         return true;
      }
   }
   return false;

}

//---------------------------------------------------------------------------

void AttributeGroupModel::Inserted(AttributeGroup* grp, int idx)
{

   int rowIdx = m_groups.indexOf(grp);
   if (rowIdx > -1)
   {
      QModelIndex pIdx = createIndex(rowIdx, 0, grp);
      beginInsertRows(pIdx, idx, idx+1);
      endInsertRows();
   }

}

//---------------------------------------------------------------------------

void AttributeGroupModel::ReOrdered(AttributeGroup* grp, int start, int end)
{

   int rowIdx = m_groups.indexOf(grp);
   if (rowIdx > -1)
   {
      QModelIndex pIdx = createIndex(rowIdx, 0, grp);
      emit dataChanged(index(start, 0, pIdx),
                       index(end, 0, pIdx));
   }

}

//---------------------------------------------------------------------------

void AttributeGroupModel::Removed(AttributeGroup* grp, int idx)
{

   int rowIdx = m_groups.indexOf(grp);
   if (rowIdx > -1)
   {
      QModelIndex pIdx = createIndex(rowIdx, 0, grp);
      beginRemoveRows(pIdx, idx, idx);
      endRemoveRows();
   }

}

//---------------------------------------------------------------------------

QVariant AttributeGroupModel::headerData(int section,
                                           Qt::Orientation orientation,
                                           int role) const
{

   // Check this is DisplayRole
   if (role != Qt::DisplayRole) {
      return QVariant();
   }

   // Horizontal headers
   if (orientation == Qt::Horizontal) {
      switch (section) {
      case NAME:
         return m_headers[section];
      case VALUE:
         return m_headers[section];
      case ENABLED:
         return m_headers[section];
      default:
         return QVariant();
      }
   }

   // Return empty data
   return QVariant();

}

//---------------------------------------------------------------------------

QModelIndex AttributeGroupModel::parent(const QModelIndex& index)const
{

   if (!index.isValid())
      return QModelIndex();

   AttributeGroup* mGroup =
         static_cast<AttributeGroup*>(index.internalPointer());

   if(m_groups.contains(mGroup))
   {
      return QModelIndex();
   }
   else
   {
      Attribute* attr =  static_cast<Attribute*>(index.internalPointer());
      mGroup = attr->getParent();
      return createIndex(m_groups.indexOf(mGroup), 0, mGroup);
   }
   return QModelIndex();

}

//---------------------------------------------------------------------------

void AttributeGroupModel::removeGroup(AttributeGroup *grp)
{

   disconnect(grp,
           SIGNAL(Inserted(AttributeGroup*, int)),
           this,
           SLOT(Inserted(AttributeGroup*, int)));

   disconnect(grp,
           SIGNAL(Removed(AttributeGroup*, int)),
           this,
           SLOT(Removed(AttributeGroup*, int)));

   int idx = m_groups.indexOf(grp);
   if(idx > -1)
   {
      beginRemoveRows(QModelIndex(), idx, idx);
      m_groups.removeOne(grp);
      endRemoveRows();
   }

}

//---------------------------------------------------------------------------

int AttributeGroupModel::rowCount(const QModelIndex &parent) const
{

   if(!parent.isValid())
   {
      return m_groups.size();
   }

   AttributeGroup* mGroup =
         static_cast<AttributeGroup*>(parent.internalPointer());

   if(m_groups.contains(mGroup))
   {
      return mGroup->count();
   }
   return 0;

}

//---------------------------------------------------------------------------
/*
void AttributeGroupModel::setMotorGroups(AttributesGroup attrs)
{

   // Begin reset
   beginResetModel();

   // Set new conditionals
   m_groups = attrs;

   // End reset
   endResetModel();

}
*/
//---------------------------------------------------------------------------

bool AttributeGroupModel::setData(const QModelIndex &index,
                                    const QVariant &value,
                                    int role)
{

   // Mark unused
   Q_UNUSED(role);

   // Check for valid index
   if (!index.isValid()) return false;

   // Get column and row
   int column = index.column();
   int row = index.row();

   QModelIndex parent = index.parent();
   if(!parent.isValid())
   {
      // Check valid index
      if (row >= m_groups.size() || row < 0)
      {
         return false;
      }
      AttributeGroup* mGroup = m_groups.at(index.row());

      if (column == NAME)
      {
         mGroup->setGroupName(value.toString().trimmed());
      }
      else if (column == VALUE)
      {
         return false;
      }
      else if (column == ENABLED)
      {
         //beginResetModel();
         mGroup->setEnabled(value.toBool());
         //endResetModel();
      }
   }
   else
   {
      AttributeGroup* mGroup = m_groups.at(parent.row());
      Attribute* attr = mGroup->getAttrAt(index.row());
      // Set data
      if (column == NAME)
      {
         attr->setName(value.toString().trimmed());
      }
      else if (column == VALUE)
      {
         attr->setValue(value.toString().trimmed());
      }
      else if (column == ENABLED)
      {
         // If no pv name is set, edit is disallowed.
         if (attr->getValue().length() < 1)
         {
            return false;
         }
         attr->setIsEnable(value.toBool());
      }
      else
      {
         return false;
      }
   }
   // Emit dataChanged signal
   emit(dataChanged(index, index));

   // Return true
   return true;

}

//---------------------------------------------------------------------------
