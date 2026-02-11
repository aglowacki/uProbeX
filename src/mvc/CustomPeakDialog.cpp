/*-----------------------------------------------------------------------------
 * Copyright (c) 2026, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "mvc/CustomPeakDialog.h"
#include <QLabel>

//---------------------------------------------------------------------------

CustomPeakDialog::CustomPeakDialog(QWidget* parent) : QDialog(parent)
{

	QVBoxLayout* layout = new QVBoxLayout();

	_le_name = new QLineEdit();
	_le_name->setText("Custom1");

	_le_peak_energy_center = new QLineEdit();
	_le_peak_energy_center->setText("0.0");

	QHBoxLayout* hbox = new QHBoxLayout();
	_btn_accept = new QPushButton("Accept");
	connect(_btn_accept, &QPushButton::pressed, this, &CustomPeakDialog::on_accepted);
	_btn_cancel = new QPushButton("Cancel");
	connect(_btn_cancel, &QPushButton::pressed, this, &CustomPeakDialog::on_rejected);
	hbox->addWidget(_btn_accept);
	hbox->addWidget(_btn_cancel);

	QHBoxLayout* hbox_name = new QHBoxLayout();
	QHBoxLayout* hbox_energy = new QHBoxLayout();

	hbox_name->addWidget(new QLabel("Name:"));
	hbox_name->addWidget(_le_name);
	hbox_energy->addWidget(new QLabel("Peak Energy Center:"));
	hbox_energy->addWidget(_le_peak_energy_center);

	layout->addItem(hbox_name);
	layout->addItem(hbox_energy);
	layout->addItem(hbox);
	
	setLayout(layout);

	_btn_accept->setAutoDefault(false);
	_btn_cancel->setAutoDefault(false);

}

//---------------------------------------------------------------------------

CustomPeakDialog::~CustomPeakDialog()
{

}

//---------------------------------------------------------------------------

void CustomPeakDialog::on_accepted()
{

	emit accepted();
	accept();
}

//---------------------------------------------------------------------------

void CustomPeakDialog::on_rejected()
{
	close();
}

//---------------------------------------------------------------------------