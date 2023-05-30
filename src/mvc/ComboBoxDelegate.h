/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef COMBO_BOX_DELEGATE_H
#define COMBO_BOX_DELEGATE_H

/*---------------------------------------------------------------------------*/

#include <string>
#include <vector>

#include <QItemDelegate>
#include <QModelIndex>
#include <QWidget>
#include <QVariant>

class ComboBoxDelegate : public QItemDelegate
{
Q_OBJECT
public:
  ComboBoxDelegate(std::vector<std::string> cbItems, QObject *parent = 0);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
  std::vector<std::string> Items;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
