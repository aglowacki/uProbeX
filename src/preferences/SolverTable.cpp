/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/SolverTable.h>
#include <gstar/CheckBoxDelegate.h>
#include <QHeaderView>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QVBoxLayout>


using gstar::CheckBoxDelegate;
/*---------------------------------------------------------------------------*/

SolverTable::SolverTable(QWidget* parent) : QWidget(parent)
{

   m_btnAdd = nullptr;
   m_btnRemove = nullptr;
   m_solverModel = nullptr;
   m_sortModel = nullptr;
   m_solverTable = nullptr;

   createComponents();

}

/*---------------------------------------------------------------------------*/

SolverTable::~SolverTable()
{

   if (m_sortModel != nullptr) delete m_sortModel;

}

/*---------------------------------------------------------------------------*/

void SolverTable::acceptChanges()
{

}

/*---------------------------------------------------------------------------*/

void SolverTable::addItem()
{

   // Check valid model
   if (m_solverModel == nullptr) return;

   // Get row number for new item
   int row = m_solverModel->rowCount(QModelIndex()) + 1;

   // Add item
   m_solverModel -> addSolverAttr(tr("name"), tr("0.0"), tr("desc"), true);

   // Enable remove button
   if (m_solverModel->rowCount(QModelIndex()) > 0) m_btnRemove->setEnabled(true);

   // Select newly added pair
   m_solverTable->selectionModel()->clearSelection();
   m_solverTable->selectRow(row);

   emit itemChanged();

}

/*---------------------------------------------------------------------------*/

void SolverTable::addItem(Attribute attr)
{

   // Check valid model
   if (m_solverModel == nullptr) return;

   // Get row number for new item
   int row = m_solverModel->rowCount(QModelIndex()) + 1;

   // Add item
   m_solverModel -> addSolverAttr(attr.getName(),
                                  attr.getValue(),
                                  attr.getDescription(),
                                  attr.getIsEnable());

   // Select newly added pair
   m_solverTable->selectionModel()->clearSelection();
   m_solverTable->selectRow(row - 1);

}

/*---------------------------------------------------------------------------*/

void SolverTable::removeItems()
{

   m_solverModel -> clearAll();

}

/*---------------------------------------------------------------------------*/

void SolverTable::createComponents()
{

   // Background color
//   QPalette pal = this->palette();
//   pal.setColor(this->backgroundRole(), Qt::white);
//   this->setPalette(pal);

//   // Autofill background
//   setAutoFillBackground(true);

   m_solverTable = new QTableView;
   m_solverModel = new AttributeTableModel();

   // Check box delegate
   CheckBoxDelegate* m_delegate = new CheckBoxDelegate();
   m_solverTable->setItemDelegateForColumn(3, m_delegate);

   // Setup table
   m_solverTable->setModel(m_solverModel);

   m_solverTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
   m_solverTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
   m_solverTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
   m_solverTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

   m_solverTable->hideColumn(2);

   m_solverTable->verticalHeader()->hide();
   m_solverTable->verticalHeader()->setDefaultSectionSize(
   m_solverTable->verticalHeader()->minimumSectionSize());
   m_solverTable->setSelectionBehavior(QAbstractItemView::SelectItems);
   m_solverTable->setSelectionMode(QAbstractItemView::SingleSelection);
   m_solverTable->setAlternatingRowColors(true);
   m_solverTable->resizeColumnsToContents();

   connect(m_solverModel,
           SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
           this,
           SLOT(tableDataChanged(const QModelIndex&, const QModelIndex&)));

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

void SolverTable::fromStringList(const QStringList& list)
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

void SolverTable::fromAttributeList(const QList<Attribute>& list)
{

   m_solverModel->setSolverAttrs(list);

}

/*---------------------------------------------------------------------------*/

QList<Attribute> SolverTable::getSolverAttrs()
{

   return m_solverModel->getSolverAttrs();

}

/*---------------------------------------------------------------------------*/

void SolverTable::removeItem()
{

   // Check valid model
   if (m_solverModel == nullptr) return;

   // Get current selection
   QModelIndexList list = m_solverTable->selectionModel()->selectedIndexes();

   // Check for selected indexes
   if (list.size() < 1) return;

   // Get first one
   QModelIndex index = list.at(0);

   // Get row of current selection
   int row = index.row();

   // Remove row if it's valid
   if (row >= 0 && row < m_solverModel->rowCount(QModelIndex()))
   {
      // Remove from model
      m_solverModel->removeRows(row, 1);
   }

   // Update selected row
   if (m_solverModel->rowCount(QModelIndex()) > 0)
   {
      // If row removed was last row in model
      if (row >= m_solverModel->rowCount(QModelIndex()))
      {
         row = m_solverModel->rowCount(QModelIndex()) - 1;
      }

      // Select newly added item
      m_solverTable->selectionModel()->clearSelection();
      m_solverTable->selectRow(row);
   }

   // Disable remove button
   if (m_solverModel->rowCount(QModelIndex()) <= 0)
   {
      m_btnRemove->setEnabled(false);
   }

   emit itemChanged();

}

/*---------------------------------------------------------------------------*/

void SolverTable::setAddButtionVisible(bool visible)
{

   m_btnAdd->setVisible(visible);

}

/*---------------------------------------------------------------------------*/

void SolverTable::setDeleteButtionVisible(bool visible)
{

   m_btnRemove->setVisible(visible);

}

/*---------------------------------------------------------------------------*/

void SolverTable::setNMModel()
{

   m_solverModel->setNMModel(true);

}

/*---------------------------------------------------------------------------*/

void SolverTable::setSolverAttrs(
   const QList<Attribute>& solverAttrs)
{

   m_solverModel->setSolverAttrs(solverAttrs);

}

/*---------------------------------------------------------------------------*/

void SolverTable::tableDataChanged(const QModelIndex&, const QModelIndex&)
{

   emit itemChanged();

}

/*---------------------------------------------------------------------------*/

QStringList SolverTable::toStringList()
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

QMap<QString, double> SolverTable::toMap()
{

   QMap<QString, double> lst;
   QList<Attribute> attrs = getSolverAttrs();
   for (int i = 0 ; i < attrs.size() ; i++)
   {
      Attribute attr = attrs.at(i);
      lst.insert(attr.getName(), attr.getValue().toDouble());
   }
   return lst;

}

/*---------------------------------------------------------------------------*/

QMap<QString, double> SolverTable::toSelectedMap()
{

   QMap<QString, double> lst;
   QList<Attribute> attrs = getSolverAttrs();
   for (int i = 0 ; i < attrs.size() ; i++)
   {
      Attribute attr = attrs.at(i);
      if(attr.getIsEnable())
      {
        lst.insert(attr.getName(), attr.getValue().toDouble());
      }
   }
   return lst;

}

/*---------------------------------------------------------------------------*/

