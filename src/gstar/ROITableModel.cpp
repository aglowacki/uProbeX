/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/ROITableModel.h>

using namespace gstar;

/*---------------------------------------------------------------------------*/

ROITableModel::ROITableModel(QObject* parent)
: QAbstractTableModel(parent)
{

   // Initialize header data
   m_headers[Name] = tr("Name");
   m_headers[Color] = tr("Color");
   m_headers[X] = tr("X");
   m_headers[Y] = tr("Y");
   m_headers[Width] = tr("W");
   m_headers[Height] = tr("H");

}

/*---------------------------------------------------------------------------*/

int ROITableModel::addROI(QString name, QString color,
                          double x, double y,
                          double w, double h)
{

   // Indicate beginning of insert
   beginInsertRows(QModelIndex(), m_rois.size(), m_rois.size());

   // Create and fill ROI
   ROI roi;
   roi.setX(x);
   roi.setY(y);
   roi.setWidth(w);
   roi.setHeight(h);
   roi.setColor(color);
   roi.setText(name);

   // Append ROI to end of list
   m_rois.append(roi);

   // Indicate end of insert
   endInsertRows();

   // Return index of newly added snap shot
   return m_rois.size() - 1;

}

/*---------------------------------------------------------------------------*/

void ROITableModel::clearAll()
{

   // Remove all rows
   while (rowCount(QModelIndex()) > 0) {
      removeRows(0, 1);
   }

}

/*---------------------------------------------------------------------------*/

int ROITableModel::columnCount(const QModelIndex &parent) const
{

   // Mark unused
   Q_UNUSED(parent);

   // Return 6 columns
   return 6;

}

/*---------------------------------------------------------------------------*/

QVariant ROITableModel::data(const QModelIndex &index, int role) const
{

   // Mark unused
   Q_UNUSED(role);

   // Check valid index
   if (!index.isValid()) {
      return QVariant();
   }

   // Check valid index
   if (index.row() >= m_rois.size() || index.row() < 0) {
      return QVariant();
   }

   // Return values for display and edit roles
   if (role == Qt::DisplayRole || role == Qt::EditRole) {

      // Get ROI
      ROI roi = m_rois.at(index.row());

      // Insert data
      if (index.column() == Name) {
         return roi.getText();
      }
      else if (index.column() == Color) {
         return roi.getColor();
      }
      else if (index.column() == X) {
         return QString::number(roi.getX());
      }
      else if (index.column() == Y) {
         return QString::number(roi.getY());
      }  
      else if (index.column() == Width) {
         return QString::number(roi.getWidth());
      }
      else if (index.column() == Height) {
         return QString::number(roi.getHeight());
      }

   }

   // Return empty data
   return QVariant();

}

/*---------------------------------------------------------------------------*/

Qt::ItemFlags ROITableModel::flags(const QModelIndex &index) const
{

   // Check valid index
   if (!index.isValid()) {
      return Qt::ItemIsEnabled;
   }

   // Make field editable
   return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;

}

/*---------------------------------------------------------------------------*/

QList<ROI> ROITableModel::getROIs()
{

   // Return a copy of the ROI list
   return m_rois;

}

/*---------------------------------------------------------------------------*/

QVariant ROITableModel::headerData(int section,
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
      case Name:
         return m_headers[section];
      case Color:
         return m_headers[section];
      case X:
         return m_headers[section];
      case Y:
         return m_headers[section];
      case Width:
         return m_headers[section];
      case Height:
         return m_headers[section];
      default:
         return QVariant();
      }
   }

   // Return empty data
   return QVariant();

}

/*---------------------------------------------------------------------------*/

bool ROITableModel::insertRows(int row, int count, const QModelIndex& parent)
{

   // Mark unused
   Q_UNUSED(row)
   Q_UNUSED(count)
   Q_UNUSED(parent)

   // Indicate beginning of insert
   beginInsertRows(QModelIndex(), m_rois.size(), m_rois.size());

   // Create and fill ROI
   ROI roi;
   roi.setX(-1.0);
   roi.setY(-1.0);
   roi.setWidth(-1.0);
   roi.setHeight(-1.0);
   roi.setColor(tr("blue"));
   roi.setText(tr("ROI"));

   // Append ROI to end of list
   m_rois.append(roi);

   // Indicate end of insert
   endInsertRows();

   // Return true
   return true;

}

/*---------------------------------------------------------------------------*/

bool ROITableModel::removeRows(int row, int count, const QModelIndex& parent)
{

   // Mark unused
   Q_UNUSED(parent);

   // Check bounds
   if ((row < 0) || ((row + count - 1) >= m_rois.size())) {
      return false;
   }

   // Indicate beginning of removal
   beginRemoveRows(QModelIndex(), row, row + count - 1);

   // Remove count rows
   for (int r = row + count - 1 ; r >= row ; r--) {

      // Remove row
      m_rois.removeAt(r);

   }

   // Indicate end of removal
   endRemoveRows();

   // Return true
   return true;

}

/*---------------------------------------------------------------------------*/

int ROITableModel::rowCount(const QModelIndex &parent) const
{

   // Mark unused
   Q_UNUSED(parent);

   // Return number of rows
   return m_rois.size();

}

/*---------------------------------------------------------------------------*/

bool ROITableModel::setData(const QModelIndex &index,
                            const QVariant &value,
                            int role)
{

   // Mark unused
   Q_UNUSED(role);


   // Check for valid index
   if (!index.isValid()) {
      return false;
   }

   // Get column and row
   int column = index.column();
   int row = index.row();

   // Set data
   if (column == Name) {
      m_rois[row].setText(value.value<QString>());
   }
   else if (column == Color) {
      m_rois[row].setColor(value.value<QString>());
   }
   else if (column == X) {
      m_rois[row].setX(value.toDouble());
   }
   else if (column == Y) {
      m_rois[row].setY(value.toDouble());
   }
   else if (column == Width) {
      m_rois[row].setWidth(value.toDouble());
   }
   else if (column == Height) {
      m_rois[row].setHeight(value.toDouble());
   }
   else {
      return false;
   }

   // Emit dataChanged signal
   emit(dataChanged(index, index));

   // Return true
   return true;

}

/*---------------------------------------------------------------------------*/

bool ROITableModel::setHeaderData(int section,
                                  Qt::Orientation orientation,
                                  const QVariant &value,
                                  int role)
{

   // Mark unused
   Q_UNUSED(role);


   // Check for horizontal orientation
   if (orientation != Qt::Horizontal) {
      return false;
   }

   // Assign headers
   if (section == Name || section == Color) {
      m_headers[section] = value.toString();
   }
   else {
      return false;
   }

   // Return true
   return true;

}

/*---------------------------------------------------------------------------*/
