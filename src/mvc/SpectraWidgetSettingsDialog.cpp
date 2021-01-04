/*-----------------------------------------------------------------------------
 * Copyright (c) 2020, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/SpectraWidgetSettingsDialog.h>
#include <QApplication>
#include <QGroupBox>

/*---------------------------------------------------------------------------*/

SpectraWidgetSettingsDialog::SpectraWidgetSettingsDialog(QWidget *parent) : QDialog(parent)
{
    _accepted = false;
    this->setWindowTitle("Spectra Widget Settings");
    createLayout();
}

/*---------------------------------------------------------------------------*/

SpectraWidgetSettingsDialog::~SpectraWidgetSettingsDialog()
{
  
}

/*---------------------------------------------------------------------------*/

void SpectraWidgetSettingsDialog::createLayout()
{

    _chkLog10 = new QCheckBox("Display Y axis in Log10");
    _chkLog10->setChecked(true);
    _chkDetailedFit = new QCheckBox("Show Details Fit Spectra (Ka, Kb, L, M lines)");

    QVBoxLayout* chklayout = new QVBoxLayout();
    chklayout->addWidget(_chkLog10);
    chklayout->addWidget(_chkDetailedFit);


    _btn_accept = new QPushButton("Accept");
    connect(_btn_accept, &QPushButton::released, this, &SpectraWidgetSettingsDialog::onAccepted);

    _btn_cancel = new QPushButton("Cancel");
    connect(_btn_cancel, &QPushButton::released, this, &SpectraWidgetSettingsDialog::onCancel);

    QHBoxLayout* buttonlayout = new QHBoxLayout();
    buttonlayout->addWidget(_btn_accept);
    buttonlayout->addWidget(_btn_cancel);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addItem(chklayout);
    layout->addItem(buttonlayout);
 
    setLayout(layout);

}

/*---------------------------------------------------------------------------*/

void SpectraWidgetSettingsDialog::onAccepted()
{
	_accepted = true;
	close();
}

/*---------------------------------------------------------------------------*/

void SpectraWidgetSettingsDialog::onCancel()
{
    close();
}

/*---------------------------------------------------------------------------*/