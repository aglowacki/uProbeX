/*-----------------------------------------------------------------------------
 * Copyright (c) 2020, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/SpectraWidgetSettingsDialog.h>
#include <QApplication>
#include <QGroupBox>
#include <preferences/Preferences.h>

//---------------------------------------------------------------------------

SpectraWidgetSettingsDialog::SpectraWidgetSettingsDialog(QWidget *parent) : QDialog(parent)
{
    _accepted = false;
    this->setWindowTitle("Spectra Widget Settings");
    createLayout();
}

//---------------------------------------------------------------------------

SpectraWidgetSettingsDialog::~SpectraWidgetSettingsDialog()
{
  
}

//---------------------------------------------------------------------------

void SpectraWidgetSettingsDialog::createLayout()
{
    bool log10 = Preferences::inst()->getValue(STR_PFR_LOG_10).toBool();
    bool detailed = Preferences::inst()->getValue(STR_PFR_DETAILED_FIT_SPEC).toBool();
	bool fit_int_matrix = Preferences::inst()->getValue(STR_PFR_SHOW_FIT_INT_MATRIX).toBool();
    bool fit_int_nnls = Preferences::inst()->getValue(STR_PFR_SHOW_FIT_INT_NNLS).toBool();
    bool max_chan = Preferences::inst()->getValue(STR_PFR_SHOW_MAX_CHAN_SPEC).toBool();
    bool black_bg = Preferences::inst()->getValue(STR_PFR_SPECTRA_BLACK_BG).toBool();
    
    _chkLog10 = new QCheckBox("Display Y axis in Log10");
    _chkLog10->setChecked(log10);
    _chkDetailedFit = new QCheckBox("Show Details Fit Spectra (Ka, Kb, L, M lines)");
    _chkDetailedFit->setChecked(detailed);
	_chkFit_Int_Matrix = new QCheckBox("Show Fit_Int_Matrix (Summed per pixel fitting)");
	_chkFit_Int_Matrix->setChecked(fit_int_matrix);
    _chkFit_Int_NNLS = new QCheckBox("Show Fit_Int_NNLS (Summed per pixel fitting)");
    _chkFit_Int_NNLS->setChecked(fit_int_nnls);
    _chkMax_Chan = new QCheckBox("Show Max Channels Spectras");
    _chkMax_Chan->setChecked(max_chan);
    _chkSpec_Blk_Bg = new QCheckBox("Show Background as black");
    _chkSpec_Blk_Bg->setChecked(black_bg);

    

    QVBoxLayout* chklayout = new QVBoxLayout();
    chklayout->addWidget(_chkLog10);
    chklayout->addWidget(_chkDetailedFit);
	chklayout->addWidget(_chkFit_Int_Matrix);
    chklayout->addWidget(_chkFit_Int_NNLS);
    chklayout->addWidget(_chkMax_Chan);
    chklayout->addWidget(_chkSpec_Blk_Bg);


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

//---------------------------------------------------------------------------

void SpectraWidgetSettingsDialog::onAccepted()
{
	_accepted = true;
    Preferences::inst()->setValue(STR_PFR_LOG_10, _chkLog10->isChecked());
    Preferences::inst()->setValue(STR_PFR_DETAILED_FIT_SPEC, _chkDetailedFit->isChecked());
	Preferences::inst()->setValue(STR_PFR_SHOW_FIT_INT_MATRIX, _chkFit_Int_Matrix->isChecked());
    Preferences::inst()->setValue(STR_PFR_SHOW_FIT_INT_NNLS, _chkFit_Int_NNLS->isChecked());
    Preferences::inst()->setValue(STR_PFR_SHOW_MAX_CHAN_SPEC, _chkMax_Chan->isChecked());
    Preferences::inst()->setValue(STR_PFR_SPECTRA_BLACK_BG, _chkSpec_Blk_Bg->isChecked());
    Preferences::inst()->save();
	close();
}

//---------------------------------------------------------------------------

void SpectraWidgetSettingsDialog::onCancel()
{
    close();
}

//---------------------------------------------------------------------------
