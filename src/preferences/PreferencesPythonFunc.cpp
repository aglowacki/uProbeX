/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/PreferencesPythonFunc.h>

#include <gstar/CheckBoxDelegate.h>
#include <mvc/DeselectableTreeView.h>
#include <preferences/Attribute.h>
#include <preferences/AttributeGroup.h>
#include <preferences/AttributeGroupModel.h>
#include <QFileDialog>
#include <QLabel>
#include <core/PythonLoader.h>
//#include <preferences/MotorLinksDialog.h>

#include <QItemSelectionModel>
#include <QMenu>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QMessageBox>
#include "core/defines.h"

using gstar::CheckBoxDelegate;
/*---------------------------------------------------------------------------*/

PreferencesPythonFunc::PreferencesPythonFunc(QWidget *parent)
: QWidget(parent)
{

   m_model = nullptr;
   m_foundPython = false;

   createComponents();

}

/*---------------------------------------------------------------------------*/

PreferencesPythonFunc::~PreferencesPythonFunc()
{

   if (m_model != nullptr) delete m_model;

}

/*---------------------------------------------------------------------------*/

void PreferencesPythonFunc::acceptChanges()
{


}

/*---------------------------------------------------------------------------*/

void PreferencesPythonFunc::addGroupItem()
{

   QString fileName = QFileDialog::getOpenFileName(this,
      "Open exec file", ".",
         "Python (*.py);; Shell (*.sh);; Batch (*.bat)");

   // Dialog returns a nullptr string if user press cancel.
   if (fileName.isNull() || fileName.isEmpty()) return;

   QString filePath = QFileInfo(fileName).path();//.canonicalFilePath();
   QString suffix = QFileInfo(fileName).suffix();
   QString moduleName = QFileInfo(fileName).baseName();
   AttributeGroup* mGrp = new AttributeGroup(fileName);
   if(m_foundPython)
   {

      QStringList functionList;
      try
      {
         if(false == PythonLoader::inst()->isLoaded())
            PythonLoader::inst()->init();

         functionList = PythonLoader::inst()->getFunctionList(filePath,
                                                              moduleName);
      }
      catch(PythonLoader::pyException ex)
      {
         logW<<"Error PreferencesPythonFunc::addGroupItem "<<ex.what();
      }

      if(functionList.count() > 0)
      {
         for(QString funcName : functionList)
         {
            Attribute* attr = new Attribute(funcName, funcName, "", false);
            if(suffix == "py")
               attr->setDescription("python");
            else
               attr->setDescription("shell");
            mGrp->append(attr);
         }
      }
      else
      {
         Attribute* attr = new Attribute("Function1", "", "", false);
         if(suffix == "py")
            attr->setDescription("python");
         else
         {
            attr->setDescription("shell");
            attr->setValue("N/A");
         }
         mGrp->append(attr);
      }
   }
   else
   {
      Attribute* attr = new Attribute("Function1", "", "", false);
      if(suffix == "py")
         attr->setDescription("python");
      else
      {
         attr->setDescription("shell");
         attr->setValue("N/A");
      }
      mGrp->append(attr);
   }
   m_model->appendGroup(mGrp);

   m_tree->resizeColumnToContents(0);

   if(m_model->rowCount(QModelIndex()) > 0)
      m_btnRemove->setEnabled(true);
   else
      m_btnRemove->setEnabled(false);

}

/*---------------------------------------------------------------------------*/

void PreferencesPythonFunc::addItem()
{

   QModelIndexList mList = m_tree->selectionModel()->selectedIndexes();
   if(mList.size() > 0)
   {
      QModelIndex idx = mList[0];

      if(idx.isValid() == false)
         return;

      AttributeGroup* mGrp = nullptr;
      if(idx.parent().isValid() == false) //it is a group
      {
         mGrp = static_cast<AttributeGroup*>(idx.internalPointer());
      }
      else //it is a pv attr
      {
         Attribute* attr = static_cast<Attribute*>(idx.internalPointer());
         if(attr != nullptr)
         {
            mGrp = attr->getParent();
         }
      }
      if(mGrp != nullptr)
      {
         QString attrName =
               QString("Function%1").arg(mGrp->count()+1);
         Attribute* attr = new Attribute(attrName, "", "", false);
         mGrp->append(attr);
      }
   }
   else
   {
      addGroupItem();
   }

   m_tree->resizeColumnToContents(0);


   if(m_model->rowCount(QModelIndex()) > 0)
      m_btnRemove->setEnabled(true);
   else
      m_btnRemove->setEnabled(false);

   if(m_model->rowCount(QModelIndex()) < 1)
      addGroupItem();

}

/*---------------------------------------------------------------------------*/

void PreferencesPythonFunc::contextMenuRequest(const QPoint &pos)
{

   QMenu menu;
   //if(m_foundPython)
   //{
      menu.addAction(m_addGroupAction);
   //}
   if(m_model->rowCount(QModelIndex()) > 0)
   {
      QModelIndexList mList = m_tree->selectionModel()->selectedIndexes();

      if(mList.size() > 0)
      {
         menu.addAction(m_addItemAction);
         menu.addSeparator();
         menu.addAction(m_moveUpAction);
         menu.addAction(m_moveDownAction);

         QModelIndex idx = mList[0];

         if(idx.isValid()) //check if we have pv attr
         {
            if(idx.parent().isValid())
            {
               menu.addSeparator();
               //menu.addAction(m_linkAction);
            }
         }
         menu.addSeparator();
         menu.addAction(m_removeAction);
      }
   }

   menu.exec(m_tree->mapToGlobal(pos));

}

/*---------------------------------------------------------------------------*/

void PreferencesPythonFunc::createComponents()
{

   // Background color
   //QPalette pal = this->palette();
   //pal.setColor(this->backgroundRole(), Qt::white);
   //this->setPalette(pal);

   // Autofill background
//   setAutoFillBackground(true);

   m_tree = new DeselectableTreeView();
   m_model = new AttributeGroupModel("Dialog Name",
                                     "Function Name",
                                     "Enabled");

   m_tree->setModel(m_model);

   // Setup table

   m_tree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
   m_tree->header()->setSectionResizeMode(2, QHeaderView::Fixed);
   m_tree->header()->setSectionsMovable(false);

   //m_tree->header()->setResizeMode(0, QHeaderView::Stretch);

   m_tree->setSelectionBehavior(QAbstractItemView::SelectItems);
   m_tree->setSelectionMode(QAbstractItemView::SingleSelection);
   m_tree->setAlternatingRowColors(true);

   m_tree->header()->setStretchLastSection(false);
   //m_tree->setColumnWidth(2, 45);

   m_tree->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(m_tree,
          SIGNAL(customContextMenuRequested(const QPoint &)),
          this,
          SLOT(contextMenuRequest(const QPoint &)));

   QLabel *foundPyLabel = new QLabel();
   if(PythonLoader::inst()->isLoaded())
   {
      foundPyLabel->setText("Found Python library on system.");
      m_foundPython = true;
   }
   else
   {
      try
      {
         bool checkPy = PythonLoader::inst()->safeCheck();

         if(checkPy && PythonLoader::inst()->init())
         {
            foundPyLabel->setText("Found Python library on system.");
            m_foundPython = true;
         }
         else
         {
            foundPyLabel->setText("Could not find Python library on system. Current supported versions are 2.6 or 2.7 for Linux environment.");
            m_foundPython = false;
         }
      }
      catch(PythonLoader::pyException ex)
      {
         m_foundPython = false;
		 foundPyLabel->setText("Could not find Python library on system. Current supported versions are 2.6 or 2.7 for Linux environment.");
         logW<<"Error PreferencesPythonFunc::createComponents "<<ex.what();
      }
   }

   // Check box delegate
   CheckBoxDelegate* m_delegate = new CheckBoxDelegate();
   m_tree->setItemDelegateForColumn(2, m_delegate);

   m_tree->resizeColumnToContents(2);
   connect(m_tree,
           SIGNAL(expanded(QModelIndex)),
           this,
           SLOT(treeExpanded(QModelIndex)));


   // Add ROI button
   QPushButton* m_btnAdd = new QPushButton;
   m_btnAdd -> setIcon(QIcon(":images/list-add.png"));
   m_btnAdd -> setFlat(true);
   m_btnAdd -> setFixedSize(32, 32);
//   m_btnAdd->setEnabled(m_foundPython);
   connect(m_btnAdd, SIGNAL(clicked()),
           this, SLOT(addItem()));

   // Remove ROI button
   m_btnRemove = new QPushButton;
   m_btnRemove -> setIcon(QIcon(":images/list-remove.png"));
   m_btnRemove -> setFlat(true);
   m_btnRemove -> setEnabled(false);
   m_btnRemove -> setFixedSize(32, 32);
   connect(m_btnRemove, SIGNAL(clicked()),
           this, SLOT(removeItem()));

   QPushButton* btnUp = new QPushButton;
   btnUp -> setIcon(QIcon(":images/up.png"));
   btnUp -> setFlat(true);
   btnUp -> setFixedSize(32, 32);
   connect(btnUp, SIGNAL(clicked()),
           this, SLOT(moveUp()));

   QPushButton* btnDown = new QPushButton;
   btnDown -> setIcon(QIcon(":images/down2.png"));
   btnDown -> setFlat(true);
   btnDown -> setFixedSize(32, 32);
   connect(btnDown, SIGNAL(clicked()),
           this, SLOT(moveDown()));

   // Layout for buttons
   QHBoxLayout* buttonLayout = new QHBoxLayout;
   buttonLayout -> addWidget(m_btnAdd);
   buttonLayout -> addWidget(m_btnRemove);
   buttonLayout -> addWidget(btnUp);
   buttonLayout -> addWidget(btnDown);
   buttonLayout -> addStretch();
   buttonLayout -> setSpacing(0);
   buttonLayout -> setContentsMargins(0, 0, 0, 0);

   //create actions
   m_addGroupAction = new QAction("Add File", this);
   m_addItemAction = new QAction("Add Function", this);
   m_moveDownAction = new QAction("Move Down", this);
   m_moveUpAction = new QAction("Move Up", this);
   m_removeAction = new QAction("Delete", this);

   connect(m_addGroupAction,
           SIGNAL(triggered()),
           this,
           SLOT(addGroupItem()));
   connect(m_addItemAction,
           SIGNAL(triggered()),
           this,
           SLOT(addItem()));
   connect(m_moveDownAction,
           SIGNAL(triggered()),
           this,
           SLOT(moveDown()));
   connect(m_moveUpAction,
           SIGNAL(triggered()),
           this,
           SLOT(moveUp()));
   connect(m_removeAction,
           SIGNAL(triggered()),
           this,
           SLOT(removeItem()));

   QVBoxLayout* mainLayout = new QVBoxLayout;
   mainLayout->setContentsMargins(0, 0, 0, 0);
   mainLayout->addWidget(foundPyLabel);
   mainLayout->addWidget(m_tree);
   mainLayout->addLayout(buttonLayout);
   setLayout(mainLayout);

}

/*---------------------------------------------------------------------------*/

Attribute PreferencesPythonFunc::getAttribute(QString grpName,
                                                    QString attrName)
{

   Attribute attr;

   for(AttributeGroup* grp : m_model->getGroups())
   {
      if(grp->getGroupName() == grpName)
      {
         for(int i=0; i<grp->count(); i++)
         {
            Attribute* pAttr = grp->getAttrAt(i);
            if(pAttr->getName() == attrName)
            {
               attr.setName(pAttr->getName());
               attr.setValue(pAttr->getValue());
               attr.setIsEnable(pAttr->getIsEnable());
            }
         }
      }
   }

   return attr;

}

/*---------------------------------------------------------------------------*/

QStringList PreferencesPythonFunc::getGroupStringList()
{

   QStringList sList;
   QList<AttributeGroup*> gList = m_model->getGroups();

   for (AttributeGroup* grp : gList)
   {
      sList.append(grp->toString());
   }

   return sList;

}

/*---------------------------------------------------------------------------*/

QString PreferencesPythonFunc::getPVString(QString grpName, QString attrName)
{

   QString pvStr = "";

   for (AttributeGroup* grp : m_model->getGroups())
   {
      if(grp->getGroupName() == grpName)
      {
         for(int i=0; i<grp->count(); i++)
         {
            Attribute* pAttr = grp->getAttrAt(i);
            if(pAttr->getName() == attrName)
            {
               pvStr = pAttr->getValue();
            }
         }
      }
   }

   return pvStr;
}

/*---------------------------------------------------------------------------*/

void PreferencesPythonFunc::linkItem()
{

   QModelIndexList mList = m_tree->selectionModel()->selectedIndexes();
   if(mList.size() > 0)
   {
      QModelIndex idx = mList[0];

      if(idx.isValid() == false)
         return;

      if(idx.parent().isValid() == false)
         return;

      Attribute* attr = static_cast<Attribute*>(idx.internalPointer());
      if(attr == nullptr)
         return;

      AttributeGroup* grp = attr->getParent();
      if(grp == nullptr)
         return;

/*
      MotorLinksDialog* dialog = new MotorLinksDialog(1,this);
      dialog->setPVAttrGroup(m_motorLinks);
      if (dialog->exec() == QDialog::Accepted)
      {
         int typeId = dialog->getLastSelectedRow();
         m_motorLinks.setLinkGroupAndAttr(typeId,
                                          grp->getGroupName(),
                                          attr->getName());
      }
      */
   }

}

/*---------------------------------------------------------------------------*/

void PreferencesPythonFunc::mousePressEvent(QMouseEvent *event)
{

   m_tree->selectionModel()->clearSelection();
   QWidget::mousePressEvent(event);

}

/*---------------------------------------------------------------------------*/

void PreferencesPythonFunc::moveDown()
{

   QModelIndexList mList = m_tree->selectionModel()->selectedIndexes();
   if(mList.size() > 0)
   {
      QModelIndex idx = mList[0];

      if(idx.isValid() == false)
         return;

      AttributeGroup* mGrp = nullptr;
      if(idx.parent().isValid() == false) //it is a group
      {
         if(m_model != nullptr)
         {
            if(m_model->moveDown(idx))
            {
               m_tree->selectionModel()->select(
                    m_model->index(idx.row()+1, idx.column(), idx.parent()),
                        QItemSelectionModel::SelectCurrent);
            }
         }
      }
      else //it is a pv attr
      {
         Attribute* attr = static_cast<Attribute*>(idx.internalPointer());
         if(attr != nullptr)
         {
            mGrp = attr->getParent();
            if(mGrp->moveAttrDown(attr))
            {
               m_tree->selectionModel()->select(
                 m_model->index(idx.row()+1, idx.column(), idx.parent()),
                     QItemSelectionModel::SelectCurrent);
            }
         }
      }
   }

}

/*---------------------------------------------------------------------------*/

void PreferencesPythonFunc::moveUp()
{

   QModelIndexList mList = m_tree->selectionModel()->selectedIndexes();
   if(mList.size() > 0)
   {
      QModelIndex idx = mList[0];

      if(idx.isValid() == false)
         return;

      AttributeGroup* mGrp = nullptr;
      if(idx.parent().isValid() == false) //it is a group
      {
         if(m_model != nullptr)
         {
            if(m_model->moveUp(idx))
            {
               m_tree->selectionModel()->select(
                    m_model->index(idx.row()-1, idx.column(), idx.parent()),
                        QItemSelectionModel::SelectCurrent);
            }
         }
      }
      else //it is a pv attr
      {
         Attribute* attr = static_cast<Attribute*>(idx.internalPointer());
         if(attr != nullptr)
         {
            mGrp = attr->getParent();
            if(mGrp->moveAttrUp(attr))
            {
               m_tree->selectionModel()->select(
                 m_model->index(idx.row()-1, idx.column(), idx.parent()),
                     QItemSelectionModel::SelectCurrent);
            }
         }
      }
   }

}

/*---------------------------------------------------------------------------*/

void PreferencesPythonFunc::parseGroupStringList(QStringList attrStrList)
{

   if (m_model != nullptr)
   {
      for(const QString &str : attrStrList)
      {
         AttributeGroup* grp = new AttributeGroup();
         if(grp->fromString(str))
         {
            m_model->appendGroup(grp);
            m_tree->resizeColumnToContents(0);
         }
         else
         {
            delete grp;
         }
      }
   }

   if(m_model->rowCount(QModelIndex()) > 0)
      m_btnRemove->setEnabled(true);

}

/*---------------------------------------------------------------------------*/

void PreferencesPythonFunc::removeItem()
{

   QModelIndexList mList = m_tree->selectionModel()->selectedIndexes();
   if(mList.size() > 0)
   {
      QMessageBox msgBox;
      msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
      msgBox.setDefaultButton(QMessageBox::No);
      msgBox.setText("Are you sure you want to delete the selected items?");
      int ret = msgBox.exec();

      if (ret == QMessageBox::No)
      {
         return;
      }

      QModelIndex idx = mList[0];

      if(idx.isValid() == false)
         return;

      AttributeGroup* mGrp = nullptr;
      if(idx.parent().isValid() == false) //it is a group
      {
         mGrp = static_cast<AttributeGroup*>(idx.internalPointer());
         m_model->removeGroup(mGrp);
         delete mGrp;
      }
      else //it is a pv attr
      {
         Attribute* attr = static_cast<Attribute*>(idx.internalPointer());
         if(attr != nullptr)
         {
            mGrp = attr->getParent();
            if(mGrp != nullptr)
            {
               mGrp->remove(attr);
               delete attr;
            }
         }
      }
   }

   if(m_model->rowCount(QModelIndex()) > 0)
      m_btnRemove->setEnabled(true);
   else
   {
      m_btnRemove->setEnabled(false);
   }
}

/*---------------------------------------------------------------------------*/

void PreferencesPythonFunc::tomoLinkPressed()
{
/*
   MotorLinksDialog* dialog = new MotorLinksDialog(2,this);
   dialog->setPVAttrGroup(m_motorLinks);
   if(dialog->exec() == QDialog::Accepted)
   {
      m_motorLinks = dialog->getPVAttrGroup();
   }
*/
}

/*---------------------------------------------------------------------------*/

void PreferencesPythonFunc::treeExpanded(QModelIndex idx)
{

   Q_UNUSED(idx);

   m_tree->resizeColumnToContents(0);

}

/*---------------------------------------------------------------------------*/
