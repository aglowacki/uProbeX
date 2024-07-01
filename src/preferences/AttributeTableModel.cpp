/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/AttributeTableModel.h>

//---------------------------------------------------------------------------

AttributeTableModel::AttributeTableModel(
   QObject* parent)
: QAbstractTableModel(parent)
{

   // Initialize header data
   m_headers[NAME] = tr("Name");
   m_headers[VALUE] = tr("Value");
   m_headers[DESC] = tr("Description");
   m_headers[ENABLED] = tr("Min Enable");

   m_NMModel = false;

}

//---------------------------------------------------------------------------

int AttributeTableModel::addSolverAttr(QString name,
                                       QString value,
                                       QString desc,
                                       bool enabled)
{

   // Indicate beginning of insert
   beginInsertRows(QModelIndex(),
                  m_solverAttrs.size(),
                  m_solverAttrs.size());

   Attribute attr(name, value, desc, enabled);
   m_solverAttrs.append(attr);

   // Indicate end of insert
   endInsertRows();

   // Return index of newly added pair
   return m_solverAttrs.size() - 1;

}

//---------------------------------------------------------------------------

void AttributeTableModel::clearAll()
{

   // Remove all rows
   while (rowCount(QModelIndex()) > 0)
   {
      removeRows(0, 1);
   }

}

//---------------------------------------------------------------------------

int AttributeTableModel::columnCount(
   const QModelIndex &parent) const
{

   // Mark unused
   Q_UNUSED(parent);

   return 4;

}

//---------------------------------------------------------------------------

QVariant AttributeTableModel::data(
   const QModelIndex &index, int role) const
{

   // Mark unused
   Q_UNUSED(role);

   // Check valid index
   if (!index.isValid())
   {
      return QVariant();
   }

   // Check valid index
   if (index.row() >= m_solverAttrs.size() || index.row() < 0)
   {
      return QVariant();
   }

   // Return values for display and edit roles
   if (role == Qt::DisplayRole || role == Qt::EditRole)
   {

      Attribute attr = m_solverAttrs.at(index.row());
      // Insert data
      if (index.column() == NAME) return attr.getName();
      else if (index.column() == VALUE) return attr.getValue();
      else if (index.column() == DESC) return attr.getDescription();
      else if (index.column() == ENABLED)
      {
         return attr.getIsEnable();
      }
   }


   // Return empty data
   return QVariant();

}

//---------------------------------------------------------------------------

Qt::ItemFlags AttributeTableModel::flags(
   const QModelIndex &index) const
{

   // Check valid index
   if (!index.isValid())
   {
      return Qt::ItemIsSelectable;
   }

   if(m_NMModel == true)
   {
      if (index.column() == 0)
      {
         return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
      }
   }
   return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
   // Make all column edit-able except first column
   // Only enable column 2 if there is a solver name
//   if (index.column() == 0)
//   {
//      return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
//   }
//   else if (index.column() == 1)
//   {
//      return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
//   }
//   else if (index.column() == 2)
//   {

//     SolverAttribute attr = m_solverAttributes.getAttribute(index.row());
//      if (attr.getSolverName().length() > 0)
//      {
//         return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
//      }*/

//      return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
//   }

}

//---------------------------------------------------------------------------

QList<Attribute> AttributeTableModel::getSolverAttrs()
{

   // Return a copy of the list
   return m_solverAttrs;

}

//---------------------------------------------------------------------------

QVariant AttributeTableModel::headerData(
   int section,
   Qt::Orientation orientation,
   int role) const
{

   // Check this is DisplayRole
   if (role != Qt::DisplayRole) return QVariant();

   // Horizontal headers
   if (orientation == Qt::Horizontal)
   {
      switch (section)
      {
         case NAME:
            return m_headers[section];
         case VALUE:
            return m_headers[section];
         case DESC:
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

bool AttributeTableModel::removeRows(int row,
                                  int count,
                                  const QModelIndex& parent)
{

   // Mark unused
   Q_UNUSED(parent);

   // Check bounds
   if ((row < 0) || ((row + count - 1) >= m_solverAttrs.size())) return false;

   // Indicate beginning of removal
   beginRemoveRows(QModelIndex(), row, row + count - 1);

   // Remove count rows
   for (int r = row + count - 1 ; r >= row ; r--)
   {
      // Remove row
      m_solverAttrs.removeAt(r);
   }

   // Indicate end of removal
   endRemoveRows();

   // Return true
   return true;

}

//---------------------------------------------------------------------------

int AttributeTableModel::rowCount(const QModelIndex &parent) const
{

   // Mark unused
   Q_UNUSED(parent);

   // Return number of rows
   return m_solverAttrs.size();

}

//---------------------------------------------------------------------------

bool AttributeTableModel::setData(const QModelIndex &index,
                                              const QVariant &value,
                                              int role)
{

   // Mark unused
   Q_UNUSED(role);


   // Check for valid index
   if (index.isValid() == false)
   {
      return false;
   }

   // Get column and row
   int column = index.column();
   int row = index.row();

   // Set data
   if (column == NAME)
   {
      m_solverAttrs[row].setName(value.toString());
   }
   else if (column == VALUE)
   {
      //Attribute attr = m_solverAttrs[row];
      bool ok = false;
      double dval = value.toDouble(&ok);
      if(ok)
      {
         m_solverAttrs[row].setValue(QString::number(dval));
      }
      else
      {
         m_solverAttrs[row].setValue("0.0");
      }

   }
   else if (column == DESC)
   {
      m_solverAttrs[row].setDescription(value.toString());
   }
   else if (column == ENABLED)
   {
      m_solverAttrs[row].setIsEnable(value.toBool());
   }
   else
   {
      return false;
   }

   // Emit dataChanged signal
   emit(dataChanged(index, index));

   // Return true
   return true;

}

//---------------------------------------------------------------------------

bool AttributeTableModel::setHeaderData(
   int section,
   Qt::Orientation orientation,
   const QVariant &value,
   int role)
{

   // Mark unused
   Q_UNUSED(role);

   // Check for horizontal orientation
   if (orientation != Qt::Horizontal)
   {
      return false;
   }

   // Assign headers
   if (section == NAME || section == VALUE || section == DESC)
   {
      m_headers[section] = value.toString();
   }
   else
   {
      return false;
   }

   // Return true
   return true;

}

//---------------------------------------------------------------------------

void AttributeTableModel::setNMModel(bool nmModel)
{

   m_NMModel = nmModel;

}

//---------------------------------------------------------------------------

void AttributeTableModel::setSolverAttrs(
   QList<Attribute> solverAttrs)
{

   // Begin reset
   beginResetModel();

   // Set new conditionals
   m_solverAttrs = solverAttrs;

   // End reset
   endResetModel();

}

//---------------------------------------------------------------------------





