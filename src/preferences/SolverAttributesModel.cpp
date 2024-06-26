/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/SolverAttributesModel.h>

#include <preferences/SolverAttributesGroup.h>

//---------------------------------------------------------------------------

SolverAttributesModel::SolverAttributesModel(QObject* parent)
: QAbstractTableModel(parent)
{

   // Initialize header data
   m_headers[NAME] = tr("Name");
   m_headers[VALUE] = tr("Value");
   m_headers[ENABLED] = tr("Min Enable");

}

//---------------------------------------------------------------------------

void SolverAttributesModel::clearAll()
{

   // Remove all rows
   while (rowCount(QModelIndex()) > 0)
   {
      removeRows(0, 1);
   }

}

//---------------------------------------------------------------------------

int SolverAttributesModel::columnCount(const QModelIndex &parent) const
{

   // Mark unused
   Q_UNUSED(parent);

   // Return 3 columns
   return 3;

}

//---------------------------------------------------------------------------

QVariant SolverAttributesModel::data(const QModelIndex &index, int role) const
{

   // Mark unused
   Q_UNUSED(role);

   // Check valid index
   if (!index.isValid())
   {
      return QVariant();
   }

   // Check valid index
   if (index.row() >= m_solverAttributes.getSize() || index.row() < 0)
   {
      return QVariant();
   }

   // Get attribute
   Attribute attr = m_solverAttributes.getAttribute(index.row());

   // Return values for display and edit roles
   if (role == Qt::DisplayRole || role == Qt::EditRole)
   {

      // Insert data
      if (index.column() == NAME) return attr.getName();
      else if (index.column() == VALUE) return attr.getValue();
      else if (index.column() == ENABLED)
      {
         return attr.getIsEnable();
      }

   }

   else if (role == Qt::ToolTipRole)
   {
      return attr.getDescription();
   }

   // Return empty data
   return QVariant();

}

//---------------------------------------------------------------------------

Qt::ItemFlags SolverAttributesModel::flags(const QModelIndex &index) const
{

   // Check valid index
   if (!index.isValid())
   {
      return Qt::ItemIsSelectable;
   }

   // Make all column edit-able except first column
   // Only enable column 2 if there is a solver name
   if (index.column() == 0)
   {
      return Qt::ItemIsEnabled;
   }
   else if (index.column() == 1)
   {
      return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
   }
   else if (index.column() == 2)
   {
      /*
      SolverAttribute attr = m_solverAttributes.getAttribute(index.row());
      if (attr.getSolverName().length() > 0)
      {
         return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
      }
      */
      return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
   }

   return Qt::ItemIsSelectable;

}

//---------------------------------------------------------------------------

SolverAttributesGroup SolverAttributesModel::getSolverAttributes()
{

   // Return a copy of the list
   return m_solverAttributes;

}

//---------------------------------------------------------------------------

QVariant SolverAttributesModel::headerData(int section,
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

int SolverAttributesModel::rowCount(const QModelIndex &parent) const
{

   // Mark unused
   Q_UNUSED(parent);

   // Return number of rows
   return m_solverAttributes.getSize();

}

//---------------------------------------------------------------------------

void SolverAttributesModel::setSolverAttributes(SolverAttributesGroup attrs)
{

   // Begin reset
   beginResetModel();

   // Set new conditionals
   m_solverAttributes = attrs;

   // End reset
   endResetModel();

}

//---------------------------------------------------------------------------

bool SolverAttributesModel::setData(const QModelIndex &index,
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

   // Set data
   if (column == VALUE)
   {
      Attribute attr = m_solverAttributes.getAttribute(row);
      bool ok = false;
      double dval = value.toDouble(&ok);
      if(ok)
      {
         attr.setValue(QString::number(dval));
      }
      else
      {
         attr.setValue("0.0");
      }
      m_solverAttributes.addSolverAttribute(row, attr);
   }
   else if (column == ENABLED)
   {
      Attribute attr = m_solverAttributes.getAttribute(row);
      attr.setIsEnable(value.toBool());
      m_solverAttributes.addSolverAttribute(row, attr);
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
