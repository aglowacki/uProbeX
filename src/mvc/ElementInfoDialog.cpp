/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ElementInfoDialog.h>

#include <QLabel>
#include <QGridLayout>
#include "data_struct/element_info.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

ElementInfoDialog::ElementInfoDialog() : QDialog()
{

    createLayout();

}

//---------------------------------------------------------------------------

ElementInfoDialog::~ElementInfoDialog()
{

}

//---------------------------------------------------------------------------

void ElementInfoDialog::createLayout()
{
	QLabel *lbl_element = new QLabel("Element");
	//QLabel *lbl_shell = new QLabel("Shell");
	_cb_element_selector = new QComboBox();
	
	for(const std::string& e : data_struct::Element_Symbols)
    {
        _cb_element_selector->addItem(QString::fromStdString(e));
    }

	_btn_update = new QPushButton("Update");
	_btn_cancel = new QPushButton("Cancel");
	connect(_btn_update, &QPushButton::pressed, this, &ElementInfoDialog::onUpdate);
	connect(_btn_cancel, &QPushButton::pressed, this, &ElementInfoDialog::close);

	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->setSizeConstraint(QLayout::SetFixedSize);
	mainLayout->addWidget(lbl_element, 0, 0);
	mainLayout->addWidget(_cb_element_selector, 0, 1);
	//mainLayout->addWidget(lbl_shell, 1, 0);
	//mainLayout->addWidget(, 1, 1);
	mainLayout->addWidget(_btn_update, 2, 0);
	mainLayout->addWidget(_btn_cancel, 2, 1);
	mainLayout->setRowStretch(3, 1);

	setLayout(mainLayout);

	setWindowTitle("Element Info");
}

//---------------------------------------------------------------------------

void ElementInfoDialog::onUpdate()
{
	//emit onNewGridLayout(sbRow->value(), sbCol->value());
	close();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
