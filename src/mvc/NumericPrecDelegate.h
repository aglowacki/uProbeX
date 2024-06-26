/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef NUMERIC_PREC_DELEGATE_H
#define NUMERIC_PREC_DELEGATE_H

//---------------------------------------------------------------------------

#include <QStyledItemDelegate>
#include <QDoubleSpinBox>

class NumericPrecDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit NumericPrecDelegate(QObject* parent = 0);
	virtual QString displayText(const QVariant& value, const QLocale& locale) const;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
        const QModelIndex& index) const Q_DECL_OVERRIDE
    {
        auto w = QStyledItemDelegate::createEditor(
            parent, option, index);

        auto sp = qobject_cast<QDoubleSpinBox*>(w);
        if (sp)
        {
            sp->setDecimals(14);
        }
        return w;
    }

signals:

public slots:

};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
