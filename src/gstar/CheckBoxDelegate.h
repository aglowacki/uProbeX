/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_CHECK_BOX_DELEGATE_H
#define GSTAR_CHECK_BOX_DELEGATE_H

//---------------------------------------------------------------------------

#include <QCheckBox>
#include <QStyledItemDelegate>

//---------------------------------------------------------------------------
namespace gstar
{

class CheckBoxDelegate
: public QStyledItemDelegate
{

   Q_OBJECT

public:

   /**
    * Constructor
    *
    * @param parent - the Qt parent widget
    */
   CheckBoxDelegate(QObject* parent = 0);

   /**
    * Reimplemented paint() method. See Qt's QStyledItemDelegate documentation.
    */
   void paint(QPainter* painter,
              const QStyleOptionViewItem& option,
              const QModelIndex& index) const;

protected:

   /**
    * Reimplemented editorEvent() method. See Qt's QStyledItemDelegate
    * documentation. Mostly the same as QStyledItemEditor's.
    */
   bool editorEvent(QEvent* event,
                    QAbstractItemModel* model,
                    const QStyleOptionViewItem& option,
                    const QModelIndex& index);

private:

   /**
    * Determine clickable area for check box based on style
    *
    * @param itemStyleOptions - QStyleOptionViewItem for current style
    */
   static QRect checkBoxRect(const QStyleOptionViewItem& itemStyleOptions);

};

}


//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
