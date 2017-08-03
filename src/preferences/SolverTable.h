/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SOLVERTABLE_H_
#define SOLVERTABLE_H_

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QMessageBox>
#include <QList>
#include <preferences/SolverTable.h>
#include <preferences/Attribute.h>
#include <preferences/AttributeTableModel.h>

class AttributeTableModel;
class QItemSelection;
class QPushButton;
class QSortFilterProxyModel;
class QTableView;

/*---------------------------------------------------------------------------*/

/**
 * @brief SolverTable the table having all the parameters of the solver
 */
class SolverTable
: public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor
    *
    * @param parent - the Qt parent widget
    */
   SolverTable(QWidget* parent = 0);

   /**
    * Destructor
    */
   ~SolverTable();

   /**
    * @brief Process changes when parent dialog calls its accept
    */
   void acceptChanges();

   /**
    * @brief Internal slot to add an item. Called by the add item button.
    * @param
    */
   void addItem(Attribute attr);

   /**
    * @brief clearAll
    */
   void removeItems();

   /**
    * @brief Set the model's state as a string list
    * @param list - a string list where iach string is one comma separated pair
    */
   void fromStringList(const QStringList& list);

   /**
    * @brief fromAttributeList
    * @param list
    */
   void fromAttributeList(const QList<Attribute>& list);

   /**
    * @brief Get the model's state as a list of pairs
    *
    * @return list of pairs
    */
   QList<Attribute>  getSolverAttrs();

   /**
    * @brief setAddButtionVisible
    * @param visible
    */
   void setAddButtionVisible(bool visible);

   /**
    * @brief setDeleteButtionVisible
    * @param visible
    */
   void setDeleteButtionVisible(bool visible);

   /**
    * @brief Generate and return the table as a comma separated string list.
    *
    * @return a string list where each string is one comma separated pair
    */
   QStringList toStringList();

   /**
    * @brief setNMModel
    */
   void setNMModel();

signals:

   /**
    * @brief addItem
    */
   void itemChanged();

public slots:

   /**
    * @brief Set pairs for the model
    *
    * @param pairs - a list of pairs that represent the model
    */
   void setSolverAttrs(const QList<Attribute>& solverAttrs);

private:

   /**
    * @brief Layouts GUI components
    */
   void createComponents();

private slots:

   /**
    * @brief addItem
    */
   void addItem();

   /**
    * @brief removeItem
    */
   void removeItem();

   /**
    * @brief tableDataChanged
    * @param
    * @param
    */
   void tableDataChanged(const QModelIndex&, const QModelIndex&);

private:

   /**
    * @brief Add button
    */
   QPushButton* m_btnAdd;

   /**
    * @brief Remove button
    */
   QPushButton* m_btnRemove;

   /**
    * @brief Table model
    */
   AttributeTableModel* m_solverModel;

   /**
    * @brief Proxy model for sorting
    */
   QSortFilterProxyModel* m_sortModel;

   /**
    * @brief Table view for displaying model
    */
   QTableView* m_solverTable;

};

/*---------------------------------------------------------------------------*/
#endif /* PYTHONSOLVERTABLE_H_ */
