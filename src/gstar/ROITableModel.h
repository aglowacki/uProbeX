/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_ROI_TABLE_MODEL_H
#define GSTAR_ROI_TABLE_MODEL_H

/*---------------------------------------------------------------------------*/

#include <QAbstractTableModel>
#include <QList>
#include <QModelIndex>
#include <QVariant>

#include "gstar/ROI.h"

/*---------------------------------------------------------------------------*/

namespace gstar
{
/**
 * @brief The ROITableModel ...
 */
class ROITableModel
: public QAbstractTableModel
{

   Q_OBJECT

public:

   /**
    * Properties corresponding to columns in table.
    */
   enum Properties {
      Name,     /*!< ROI name */
      Color,    /*!< ROI color */
      X,        /*!< ROI x position */
      Y,        /*!< ROI y position */
      Width,    /*!< ROI width */
      Height    /*!< ROI height */
   };

   /**
    * Constructor
    *
    * @param parent - parent Qt widget
    */
   ROITableModel(QObject* parent = 0);

   /**
    * Add a ROI to the model
    *
    * @param name - ROI name
    * @param color - ROI color name
    * @param x - x position
    * @param y - y position
    * @param w - width
    * @param h - height
    */
   int addROI(QString name, QString color,
              double x, double y,
              double w, double h);

   /**
    * Clear all ROIs
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
    * Returns a copy of a list of all ROIs
    *
    * @return a copy of a list of all ROIs
    */
   QList<ROI> getROIs();

   /**
    * Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   QVariant headerData(int section, 
                       Qt::Orientation orientation, 
                       int role) const;

   /**
    * Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   bool insertRows(int row, int count,
                   const QModelIndex& parent = QModelIndex());

   /**
    * Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   bool removeRows(int row, 
                   int count, 
                   const QModelIndex& parent = QModelIndex());

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
    * Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   bool setHeaderData(int section, 
                      Qt::Orientation orientation, 
                      const QVariant &value, 
                      int role = Qt::EditRole);

private:

   /**
    * Header names
    */
   QString m_headers[6];

   /**
    * List of ROIs. This is the model
    */
   QList<ROI> m_rois;

};

}
/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
