/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef COMBO_BOX_BOOL_DELEGATE_H
#define COMBO_BOX_BOOL_DELEGATE_H

//---------------------------------------------------------------------------

#include <string>
#include <vector>

#include <QItemDelegate>
#include <QModelIndex>
#include <QWidget>
#include <QVariant>

class ComboBoxBoolDelegate : public QItemDelegate
{
Q_OBJECT
public:
  ComboBoxBoolDelegate(QObject *parent = 0);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

  void setCustomCol(int col) {_custom_col = col;}
  
private:
  std::vector<std::string> Items;
  int _custom_col;
};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
