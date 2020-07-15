/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef FitParamsTableModel_H_
#define FitParamsTableModel_H_

#include <algorithm>
#include <QAbstractTableModel>
#include <QList>
#include <QModelIndex>
#include <QVariant>
#include "data_struct/fit_parameters.h"


/*---------------------------------------------------------------------------*/

/**
 * @brief model to control the solver table
 */
class FitParamsTableModel
: public QAbstractTableModel
{

   Q_OBJECT

public:

   /**
    * Properties corresponding to columns in table.
    */
   enum Properties {
      NAME,
      VALUE,
      BOUND_TYPE,
      MIN_VAL,
      MAX_VAL,
      STEP_SIZE,
      NUM_PROPS
   };

   /**
    * Constructor
    *
    * @param parent - parent Qt widget
    */
   FitParamsTableModel(QObject* parent = 0);

   void setOptimizerSupportsMinMax(bool val);

   void setFitParams(data_struct::Fit_Parameters fit_params);

   void updateFitParams(data_struct::Fit_Parameters* fit_params);

   void only_keep_these_keys(data_struct::Fit_Parameters fit_params);

   data_struct::Fit_Parameters getFitParams() { return _fit_parameters; }

   float getFitParamValue(const std::string name) { return _fit_parameters.at(name).value; }


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

   bool setDataFitBounds(const QModelIndex &index,
                         const data_struct::E_Bound_Type &value);

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   bool setHeaderData(int section,
                      Qt::Orientation orientation,
                      const QVariant &value,
                      int role = Qt::EditRole);


   void setEditable(bool val) { _editable = val; }

signals:
    void onEnergyChange();

private:

   /**
    * @brief Header names
    */
   QString m_headers[NUM_PROPS];

   data_struct::Fit_Parameters _fit_parameters;

   std::vector<std::string> _row_indicies;

   bool _optimizer_supports_min_max;

   bool _editable;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
