/*-----------------------------------------------------------------------------
 * Copyright (c) 2020, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/SpectraWidgetSettingsDialog.h>
#include <QApplication>
#include <QGroupBox>
#include <preferences/Preferences.h>

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
    bool log10 = Preferences::inst()->getValue(STR_PFR_LOG_10).toBool();
    bool detailed = Preferences::inst()->getValue(STR_PFR_DETAILED_FIT_SPEC).toBool();
    
    _chkLog10 = new QCheckBox("Display Y axis in Log10");
    _chkLog10->setChecked(log10);
    _chkDetailedFit = new QCheckBox("Show Details Fit Spectra (Ka, Kb, L, M lines)");
    _chkDetailedFit->setChecked(detailed);

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
    Preferences::inst()->setValue(STR_PFR_LOG_10, _chkLog10->isChecked());
    Preferences::inst()->setValue(STR_PFR_DETAILED_FIT_SPEC, _chkDetailedFit->isChecked());
    Preferences::inst()->save();
	close();
}

/*---------------------------------------------------------------------------*/

void SpectraWidgetSettingsDialog::onCancel()
{
    close();
}

/*---------------------------------------------------------------------------*/