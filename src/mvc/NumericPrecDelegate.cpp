/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/NumericPrecDelegate.h>

/*---------------------------------------------------------------------------*/

NumericPrecDelegate::NumericPrecDelegate(QObject* parent) :	QStyledItemDelegate(parent)
{
}

QString NumericPrecDelegate::displayText(const QVariant& value, const QLocale& locale) const
{
	QString formattedNum = locale.toString(value.toDouble(), 'f', 14);
	return formattedNum;

};