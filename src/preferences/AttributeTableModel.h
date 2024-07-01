/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ATTRIBUTETABLEMODEL_H_
#define ATTRIBUTETABLEMODEL_H_

#include <QAbstractTableModel>
#include <QList>
#include <QModelIndex>
#include <QVariant>
#include <preferences/Attribute.h>

//---------------------------------------------------------------------------

/**
 * @brief model to control the solver table
 */
class AttributeTableModel
: public QAbstractTableModel
{

   Q_OBJECT

public:

   /**
    * Properties corresponding to columns in table.
    */
   enum Properties {
      NAME,   /*!< Name */
      VALUE,      /*!< Value */
      DESC,   /*!< DESCRIPTION */
      ENABLED
   };

   /**
    * Constructor
    *
    * @param parent - parent Qt widget
    */
   AttributeTableModel(QObject* parent = 0);

   /**
      * @brief Make a call to underlying model for adding an item to the table.
      * @param name
      * @param energy - energy
      * @param flux - flux
      */
   int addSolverAttr(QString name,
                     QString value,
                     QString desc,
                     bool enabled);

   /**
    * @brief Clear all
    */
   void clearAll();

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   int columnCount(const QModelIndex &parent) const;

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   QVariant data(const QModelIndex &index,
                 int role = Qt::DisplayRole) const;

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   Qt::ItemFlags flags(const QModelIndex &index) const;

   /**
    * @brief Returns a copy of a list of all pairs
    * @return a copy of a list of all pairs
    */
   QList<Attribute> getSolverAttrs();

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   QVariant headerData(int section,
                       Qt::Orientation orientation,
                       int role) const;

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   /*bool insertRows(int row, int count,
                   const QModelIndex& parent = QModelIndex());*/

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   bool removeRows(int row,
                   int count,
                   const QModelIndex& parent = QModelIndex());

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   int rowCount(const QModelIndex &parent) const;

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   bool setData(const QModelIndex &index,
                const QVariant &value,
                int role = Qt::EditRole);

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   bool setHeaderData(int section,
                      Qt::Orientation orientation,
                      const QVariant &value,
                      int role = Qt::EditRole);

   /**
    * @brief setNMModel
    *
    * @param nmModel
    */
   void setNMModel(bool nmModel);

   /**
    * @brief Set pairs
    *
    * @param pairs - list of pairs
    */
   void setSolverAttrs(QList<Attribute> solverAttrs);

private:

   /**
    * @brief Header names
    */
   QString m_headers[4];

   /**
    * @brief List of double pairs
    */
   QList<Attribute> m_solverAttrs;

   /**
    * @brief Header names
    */
   bool m_NMModel;

};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
