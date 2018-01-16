/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef FitElementsTableModel_H_
#define FitElementsTableModel_H_

#include <algorithm>
#include <QAbstractTableModel>
#include <QList>
#include <QModelIndex>
#include <QVariant>
#include "data_struct/xrf/fit_element_map.h"

/*---------------------------------------------------------------------------*/

/**
 * @brief model to control the solver table
 */
class FitElementsTableModel
: public QAbstractTableModel
{

   Q_OBJECT

public:

   /**
    * Properties corresponding to columns in table.
    */
   enum HEADERS {
      Z,
      NAME,
      VALUE,
      NUM_PROPS
   };

   /**
    * Constructor
    *
    * @param parent - parent Qt widget
    */
   FitElementsTableModel(QObject* parent = 0);

   void setDisplayHeaderMinMax(bool val);

   data_struct::xrf::Fit_Parameters getAsFitParams();

   void updateFitElements(data_struct::xrf::Fit_Element_Map_Dict * elements_to_fit);

   void updateElementValues(data_struct::xrf::Fit_Parameters *fit_params);

   data_struct::xrf::Fit_Element_Map *getElementByIndex(QModelIndex index) const;

   /**
    * @brief Clear all
    */
   void clearAll();

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   int columnCount(const QModelIndex &parent) const override;

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

   QModelIndex index(int row, int column,
                         const QModelIndex &parent = QModelIndex()) const override;

   QModelIndex parent(const QModelIndex &index) const override;

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


private:

   /**
    * @brief Header names
    */
   QString m_headers[HEADERS::NUM_PROPS];

   //data_struct::xrf::Fit_Element_Map_Dict _elements_to_fit;
   std::map<int, data_struct::xrf::Fit_Element_Map*> _elements;

   std::vector<int> _row_indicies;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
