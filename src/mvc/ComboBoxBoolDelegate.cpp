/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ComboBoxBoolDelegate.h>
#include <QComboBox>
#include <QWidget>
#include <QModelIndex>
#include <QApplication>
#include <QString>

#include <iostream>

//---------------------------------------------------------------------------

ComboBoxBoolDelegate::ComboBoxBoolDelegate(QObject *parent) : QItemDelegate(parent)
{
    Items = {"False", "True"};
    _custom_col = -1;
}

//---------------------------------------------------------------------------

QWidget *ComboBoxBoolDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    QComboBox* editor = new QComboBox(parent);
    for(unsigned int i = 0; i < Items.size(); ++i)
    {
        editor->addItem(Items[i].c_str());
    }
    return editor;
}

//---------------------------------------------------------------------------

void ComboBoxBoolDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);  
    QString value = index.model()->data(index, Qt::EditRole).toString();
    if(value == "False")
    {
        comboBox->setCurrentIndex(0);
    }
    else
    {
        comboBox->setCurrentIndex(1);
    }
}

//---------------------------------------------------------------------------

void ComboBoxBoolDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentIndex(), Qt::EditRole);
}

//---------------------------------------------------------------------------

void ComboBoxBoolDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

//---------------------------------------------------------------------------

void ComboBoxBoolDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem myOption = option;
    myOption.text = index.data().toString();
    if(_custom_col > -1 )
    {
        if(_custom_col == index.column())
        {
            QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);
        }
        else
        {
            QItemDelegate::paint(painter, option, index);
        }
    }
    else
    {
        QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);
    }
}

//---------------------------------------------------------------------------
