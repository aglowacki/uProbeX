/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ScanRegionDialog.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QGridLayout>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

ScanRegionDialog::ScanRegionDialog() : QDialog()
{

    _createLayout();

}

//---------------------------------------------------------------------------

ScanRegionDialog::~ScanRegionDialog()
{

}

//---------------------------------------------------------------------------

void ScanRegionDialog::_createLayout()
{
	rowLabel = new QLabel("Rows");
	colLabel = new QLabel("Cols");
	sbRow = new QSpinBox();
	sbCol = new QSpinBox();

	sbRow->setMinimum(1);
	sbRow->setMaximum(6);
	sbRow->setSingleStep(1);
	sbRow->setValue(1);

	sbCol->setMinimum(1);
	sbCol->setMaximum(6);
	sbCol->setSingleStep(1);
	sbCol->setValue(1);

	updateBtn = new QPushButton("Update");
	cancelBtn = new QPushButton("Cancel");
	connect(updateBtn, SIGNAL(pressed()), this, SLOT(onUpdate()));
	connect(cancelBtn, SIGNAL(pressed()), this, SLOT(close()));

	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->setSizeConstraint(QLayout::SetFixedSize);
	mainLayout->addWidget(rowLabel, 0, 0);
	mainLayout->addWidget(colLabel, 0, 1);
	mainLayout->addWidget(sbRow, 1, 0);
	mainLayout->addWidget(sbCol, 1, 1);
	mainLayout->addWidget(updateBtn, 2, 0);
	mainLayout->addWidget(cancelBtn, 2, 1);
	mainLayout->setRowStretch(3, 1);

	setLayout(mainLayout);

	setWindowTitle("Scan Region");
}

//---------------------------------------------------------------------------

void ScanRegionDialog::onUpdate()
{
	emit NewScan();
	close();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
