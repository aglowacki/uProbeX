/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PROFILETABLE_H_
#define PROFILETABLE_H_

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QMessageBox>
#include <QList>
#include <QItemSelectionModel>

#include <preferences/Attribute.h>
#include <preferences/AttributeTableModel.h>

class AttributeTableModel;
class QItemSelection;
class QTableView;

/*---------------------------------------------------------------------------*/

/**
 * @brief SolverTable the table having all the parameters of the solver
 */
class ProfileTable
: public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor
    *
    * @param parent - the Qt parent widget
    */
   ProfileTable(QWidget* parent = 0);

   /**
    * Destructor
    */
   ~ProfileTable();

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

   void setRow(int index);

   /**
    * @brief Generate and return the table as a comma separated string list.
    *
    * @return a string list where each string is one comma separated pair
    */
   QStringList toStringList();

signals:

   /**
    * @brief addItem
    * @param name
    * @param desc
    */
   void addItem(QString name, QString desc);

   /**
    * @brief deleteItem
    * @param name
    * @param desc
    */
   void removeItem(int selectRow);

   void editItem(int, QString);

   /**
    * @brief switchItem
    * @param
    * @param
    */
   void switchItem(const QItemSelection &, const QItemSelection &);


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
    * @brief Internal slot to add an item. Called by the add item button.
    */
   void addItem();

   /**
    * @brief Internal slot to remove the selected item. Called by the remove
    * item button.
    */
   void removeItem();

   /**
    * @brief editItem
    * @param
    * @param
    */
   void editItem(const QModelIndex&, const QModelIndex&);

private:

   /**
    * @brief m_calSelectionModel
    */
   QItemSelectionModel* m_selectionModel;

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
    * @brief Table view for displaying model
    */
   QTableView* m_solverTable;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
