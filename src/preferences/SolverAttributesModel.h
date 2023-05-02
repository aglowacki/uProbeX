/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SOLVER_ATTRIBUTES_MODEL
#define SOLVER_ATTRIBUTES_MODEL

/*---------------------------------------------------------------------------*/

#include <preferences/Attribute.h>
#include <preferences/SolverAttributesGroup.h>

#include <QString>
#include <QAbstractTableModel>
#include <QList>

/*---------------------------------------------------------------------------*/

/**
 * @brief Model to manipulate the Solver attributes, such as how to set
 * Solver attributes and how to get list of Solver attributes. The model's
 *  properties could be set in other class.
 */
class SolverAttributesModel : public QAbstractTableModel
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

   SolverAttributesModel(QObject* parent = 0);

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
    * List of Solver attributes held by the model
    *
    * @return a copy of a list of all conditionals
    */
   SolverAttributesGroup getSolverAttributes();

   /**
    * Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   QVariant headerData(int section,
                       Qt::Orientation orientation,
                       int role) const;

   /**
    * Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   int rowCount(const QModelIndex &parent) const;

   /**
    * Set conditionals
    *
    * @param conds - set conditionals
    */
   void setSolverAttributes(SolverAttributesGroup attrs);

   /**
    * Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   bool setData(const QModelIndex &index,
                const QVariant &value,
                int role = Qt::EditRole);

private:

   /**
    * Header names
    */
   QString m_headers[3];

   /**
    * List of ROIs. This is the model
    */
   SolverAttributesGroup m_solverAttributes;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
