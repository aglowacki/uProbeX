/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PREFERENCES_PYTHON_FUNC_H
#define PREFERENCES_PYTHON_FUNC_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QModelIndex>

#include <preferences/Attribute.h>

class DeselectableTreeView;
class AttributeGroupModel;
class QItemSelectionModel;
class QPushButton;

/*---------------------------------------------------------------------------*/

/**
 * @brief Preferences dialog table using python fuction for region box calibration
 * point
 */
class PreferencesPythonFunc
: public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor
    *
    * @param parent - the Qt parent widget
    */
   PreferencesPythonFunc(QWidget* parent = 0);

   /**
    * Destructor
    */
   ~PreferencesPythonFunc();

   /**
    * Process changes when parent dialog calls its accept
    */
   void acceptChanges();

   /**
    * @brief getAttribute
    * @param grpName
    * @param attrName
    * @return
    */
   Attribute getAttribute(QString grpName, QString attrName);

   /**
    * @brief getPVString
    * @param grpName
    * @param attrName
    * @return
    */
   QString getPVString(QString grpName, QString attrName);

   /**
    * @brief returns a string list of the pv motor groups
    * @return
    */
   QStringList getGroupStringList();

   /**
    * @brief parseGroupStringList
    * @param attrStrList
    */
   void parseGroupStringList(QStringList attrStrList);

protected slots:

   /**
    * @brief
    */
   void addGroupItem();

   /**
    * @brief addItem
    */
   void addItem();

   /**
    * @brief contextMenuRequest
    * @param pos
    */
   void contextMenuRequest(const QPoint &pos);

   /**
    * @brief linkItem
    */
   void linkItem();

   /**
    * @brief moveDown
    */
   void moveDown();

   /**
    * @brief moveUp
    */
   void moveUp();

   /**
    * @brief removeItem
    */
   void removeItem();

   /**
    * @brief tomoLinkPressed
    */
   void tomoLinkPressed();

   /**
    * @brief treeExpanded
    * @param idx
    */
   void treeExpanded(QModelIndex idx);

private:

   /**
    * @brief Layouts GUI components
    */
   void createComponents();

   /**
    * @brief mousePressEvent
    * @param event
    */
   void mousePressEvent(QMouseEvent *event);

private:

   /**
    * @brief m_addRulerAction
    */
   QAction* m_addGroupAction;

   /**
    * @brief m_addPVAction
    */
   QAction* m_addItemAction;

   /**
    * @brief m_btnRemove
    */
   QPushButton* m_btnRemove;

   /**
    * @brief m_linkAction
    */
   QAction* m_linkAction;

   /**
    * @brief m_moveDownAction
    */
   QAction* m_moveDownAction;

   /**
    * @brief m_moveUpAction
    */
   QAction* m_moveUpAction;

   /**
    * QTreeView
    */
   DeselectableTreeView* m_tree;

   /**
    * Model for Attributes
    */
   AttributeGroupModel* m_model;

   /**
    * @brief m_removeAction
    */
   QAction* m_removeAction;

   /**
    * @brief m_foundPython
    */
   bool m_foundPython;
};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
