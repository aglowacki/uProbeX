/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/CheckBoxDelegate.h>

#include <QApplication>
#include <QMouseEvent>

using namespace gstar;

/*---------------------------------------------------------------------------*/

CheckBoxDelegate::CheckBoxDelegate(QObject* parent)
: QStyledItemDelegate(parent)
{

}

/*---------------------------------------------------------------------------*/

QRect CheckBoxDelegate::checkBoxRect(const QStyleOptionViewItem&
                                     itemStyleOptions)
{

   QStyleOptionButton styleOption;
   QRect rect = QApplication::style()->subElementRect(
      QStyle::SE_CheckBoxIndicator,
      &styleOption);

   QPoint point(itemStyleOptions.rect.x() +
                   itemStyleOptions.rect.width() / 2 -
                   rect.width() / 2,
                itemStyleOptions.rect.y() +
                   itemStyleOptions.rect.height() / 2 -
                   rect.height() / 2);

   return QRect(point, rect.size());

}

/*---------------------------------------------------------------------------*/

bool CheckBoxDelegate::editorEvent(QEvent* event,
                                   QAbstractItemModel* model,
                                   const QStyleOptionViewItem& option,
                                   const QModelIndex& index)
{

   if ((event->type() == QEvent::MouseButtonRelease) ||
       (event->type() == QEvent::MouseButtonDblClick))
   {

      QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

      if (mouseEvent->button() != Qt::LeftButton ||
          !checkBoxRect(option).contains(mouseEvent->pos()))
      {
         return false;
      }

      if (event->type() == QEvent::MouseButtonDblClick)
      {
         return true;
      }

   }

   else if (event->type() == QEvent::KeyPress)
   {
      if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space &&
          static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
      {
         return false;
      }
   }

   else
   {
      return false;
   }

   bool checked = index.model()->data(index, Qt::DisplayRole).toBool();

   return model->setData(index, !checked, Qt::EditRole);

}

/*---------------------------------------------------------------------------*/

void CheckBoxDelegate::paint(QPainter* painter,
                             const QStyleOptionViewItem& option,
                             const QModelIndex &index) const
{

   bool checked = index.model()->data(index, Qt::DisplayRole).toBool();
   Qt::ItemFlags flags = index.model()->flags(index);

   QStyleOptionButton styleOption;

   if (flags & Qt::ItemIsEnabled)
   {
      styleOption.state |= QStyle::State_Enabled;
   }

   if (checked)
   {
      styleOption.state |= QStyle::State_On;
   }
   else
   {
      styleOption.state |= QStyle::State_Off;
   }

   styleOption.rect = checkBoxRect(option);

   QApplication::style()->drawControl(QStyle::CE_CheckBox,
                                      &styleOption,
                                      painter);

}

/*---------------------------------------------------------------------------*/
