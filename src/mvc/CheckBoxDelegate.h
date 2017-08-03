/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef CHECK_BOX_DELEGATE_H
#define CHECK_BOX_DELEGATE_H

/*---------------------------------------------------------------------------*/

#include <QCheckBox>
#include <QStyledItemDelegate>

/*---------------------------------------------------------------------------*/

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

   /**
    * Enable or disable the control
    *
    * @param enabled - true or false 
    */
   void setEnabled(bool enabled);

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

   /**
    * Bool for enabling and disabling the control
    */
   bool m_enabled;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
