/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/ProfileTable.h>
#include <CheckBoxDelegate.h>
#include <QHeaderView>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QVBoxLayout>
#include <QDebug>

using gstar::CheckBoxDelegate;
/*---------------------------------------------------------------------------*/

ProfileTable::ProfileTable(QWidget* parent) : QWidget(parent)
{

   m_btnAdd = NULL;
   m_btnRemove = NULL;
   m_solverModel = NULL;
   m_solverTable = NULL;

   createComponents();

}

/*---------------------------------------------------------------------------*/

ProfileTable::~ProfileTable()
{



}

/*---------------------------------------------------------------------------*/

void ProfileTable::acceptChanges()
{

}

/*---------------------------------------------------------------------------*/

void ProfileTable::addItem()
{

   // Check valid model
   if (m_solverModel == NULL) return;

   // Get row number for new item
   int row = m_solverModel->rowCount(QModelIndex()) + 1;

   // Add item
   m_solverModel -> addSolverAttr(tr("name"), tr("0.0"), tr("desc"), true);

   // Enable remove button
   if (m_solverModel->rowCount(QModelIndex()) > 0) m_btnRemove->setEnabled(true);

   emit addItem(tr("name"), tr("desc"));

   // Select newly added pair
//   m_solverTable->selectionModel()->clearSelection();
//   m_solverTable->selectRow(row - 1);

}

/*---------------------------------------------------------------------------*/

void ProfileTable::addItem(Attribute attr)
{

   // Check valid model
   if (m_solverModel == NULL) return;

   // Get row number for new item
   int row = m_solverModel->rowCount(QModelIndex()) + 1;

   // Add item
   m_solverModel -> addSolverAttr(attr.getName(),
                                  attr.getValue(),
                                  attr.getDescription(),
                                  attr.getIsEnable());

   // Select newly added pair
   m_solverTable->selectionModel()->clearSelection();
   m_solverTable->selectRow(row);

}

/*---------------------------------------------------------------------------*/

void ProfileTable::createComponents()
{

   // Background color
   QPalette pal = this->palette();
   pal.setColor(this->backgroundRole(), Qt::white);
   this->setPalette(pal);

   // Autofill background
   setAutoFillBackground(true);

   m_solverTable = new QTableView;
   m_solverModel = new AttributeTableModel();

   m_selectionModel = new QItemSelectionModel(m_solverModel);

   // Check box delegate
   CheckBoxDelegate* m_delegate = new CheckBoxDelegate();
//   m_solverTable->setItemDelegateForColumn(2, m_delegate);

   // Setup table
   m_solverTable->setModel(m_solverModel);
#if QT_VERSION >= 0x050000
   m_solverTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
   m_solverTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
   m_solverTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
   m_solverTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
#else
   m_solverTable->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
   m_solverTable->horizontalHeader()->setResizeMode(1, QHeaderView::Interactive);
   m_solverTable->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
   m_solverTable->horizontalHeader()->setResizeMode(3, QHeaderView::Stretch);
#endif
   m_solverTable->hideColumn(1);
   m_solverTable->hideColumn(3);

   m_solverTable->verticalHeader()->hide();
   m_solverTable->verticalHeader()->setDefaultSectionSize(
   m_solverTable->verticalHeader()->minimumSectionSize());
//   m_solverTable->setSelectionBehavior(QAbstractItemView::SelectItems);
      m_solverTable->setSelectionBehavior(QAbstractItemView::SelectRows);
   m_solverTable->setSelectionMode(QAbstractItemView::SingleSelection);
   m_solverTable->setAlternatingRowColors(true);
   m_solverTable->resizeColumnsToContents();

   m_solverTable->setSelectionModel(m_selectionModel);
   m_solverTable->setFixedHeight(100);
   connect(m_solverModel,
           SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
           this,
           SLOT(editItem(const QModelIndex&, const QModelIndex&)));

   connect(m_selectionModel,
           SIGNAL(selectionChanged(const QItemSelection &,
                                   const QItemSelection &)),
           this,
           SIGNAL(switchItem(const QItemSelection &,
                           const QItemSelection &)));
   // Add button
   m_btnAdd = new QPushButton;
   m_btnAdd -> setIcon(QIcon(":images/list-add.png"));
   m_btnAdd -> setFlat(true);
   m_btnAdd -> setFixedSize(32, 32);
   connect(m_btnAdd, SIGNAL(clicked()), this, SLOT(addItem()));

   // Remove button
   m_btnRemove = new QPushButton;
   m_btnRemove -> setIcon(QIcon(":images/list-remove.png"));
   m_btnRemove -> setFlat(true);

   m_btnRemove->setEnabled(true);

   m_btnRemove -> setFixedSize(32, 32);
   connect(m_btnRemove, SIGNAL(clicked()), this, SLOT(removeItem()));

   // Layout for buttons
   QHBoxLayout* buttonLayout = new QHBoxLayout;
   buttonLayout -> addWidget(m_btnAdd);
   buttonLayout -> addWidget(m_btnRemove);
   buttonLayout -> addStretch();
   buttonLayout -> setSpacing(0);
   buttonLayout -> setContentsMargins(0, 0, 0, 0);

   QVBoxLayout* mainLayout = new QVBoxLayout;
   mainLayout->setContentsMargins(0, 0, 0, 0);
   mainLayout->addWidget(m_solverTable);
   mainLayout->addLayout(buttonLayout);
   setLayout(mainLayout);

}

/*---------------------------------------------------------------------------*/

void ProfileTable::editItem(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{

      if(!topLeft.isValid())
      {
         return;
      }

      QString editValue = m_solverModel->data(topLeft, Qt::DisplayRole).toString();
      int position = topLeft.column();

      emit editItem(position, editValue);
}

/*---------------------------------------------------------------------------*/

void ProfileTable::fromStringList(const QStringList& list)
{

   QList<Attribute> solverAttrs;
   for (int i = 0 ; i < list.length() ; i++)
   {

      QString attr = list.at(i);
      QStringList l = attr.split(",");
      if (l.size() != 4)  continue;

      Attribute at(l.at(0), l.at(1), l.at(2),(bool)l.at(3).toInt());
      solverAttrs.append(at);
   }

   m_solverModel->setSolverAttrs(solverAttrs);

}

/*---------------------------------------------------------------------------*/

void ProfileTable::fromAttributeList(const QList<Attribute>& list)
{

   m_solverModel->setSolverAttrs(list);

}

/*---------------------------------------------------------------------------*/

QList<Attribute> ProfileTable::getSolverAttrs()
{

   return m_solverModel->getSolverAttrs();

}

/*---------------------------------------------------------------------------*/

void ProfileTable::removeItem()
{

   // Check valid model
   if (m_solverModel == NULL) return;

   // Get current selection
   QModelIndexList list = m_solverTable->selectionModel()->selectedIndexes();

   // Check for selected indexes
   if (list.size() < 1) return;

   // Get first one
   QModelIndex index = list.at(0);

   // Get row of current selection
   int row = index.row();
   int selectRow = row;

   // Remove row if it's valid
   if (row >= 0 && row < m_solverModel->rowCount(QModelIndex()))
   {
      // Remove from model
      m_solverModel->removeRows(row, 1);
   }

   emit removeItem(selectRow);

   // Update selected row
   if (m_solverModel->rowCount(QModelIndex()) > 0)
   {
      // If row removed was last row in model
      if (row >= m_solverModel->rowCount(QModelIndex()))
      {
         row = m_solverModel->rowCount(QModelIndex()) - 1;
      }

      // Select
    // m_solverTable->selectionModel()->clearSelection();
      m_solverTable->selectRow(row);
   }

   // Disable remove button
   if (m_solverModel->rowCount(QModelIndex()) <= 0)
   {
      m_btnRemove->setEnabled(false);
   }

}

/*---------------------------------------------------------------------------*/

void ProfileTable::setRow(int index)
{

      m_solverTable->selectRow(index);

}

/*---------------------------------------------------------------------------*/

void ProfileTable::setSolverAttrs(
   const QList<Attribute>& solverAttrs)
{

   m_solverModel->setSolverAttrs(solverAttrs);

}

/*---------------------------------------------------------------------------*/

QStringList ProfileTable::toStringList()
{

   QStringList str;
   QList<Attribute> attrs = getSolverAttrs();
   for (int i = 0 ; i < attrs.size() ; i++)
   {
      Attribute attr = attrs.at(i);
      str.append(QString("%1,%2,%3,%4").arg(attr.getName())
                                       .arg(attr.getValue())
                                       .arg(attr.getDescription())
                                       .arg(attr.getIsEnable()));
   }
   return str;

}

/*---------------------------------------------------------------------------*/

