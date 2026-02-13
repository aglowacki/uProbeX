/*-----------------------------------------------------------------------------
 * Copyright (c) 2026, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ContrastWidget.h>
#include "core/str_defines.h"
#include "preferences/Preferences.h"

//---------------------------------------------------------------------------

ContrastWidget::ContrastWidget(QWidget* parent) : QWidget(parent)
{

    _min_contrast_perc = 0;
	_max_contrast_perc = 1.0;
    
     _global_contrast_chk = new QCheckBox("Global Contrast");
    _global_contrast_chk->setChecked(true);
    connect(_global_contrast_chk, &QCheckBox::checkStateChanged, this, &ContrastWidget::on_global_contrast_changed);
    
    _contrast_widget = new gstar::MinMaxSlider();
    _contrast_widget->setMinimumWidth(100);
    _contrast_widget->setMaximumHeight(70);
    _contrast_widget->setContentsMargins(0, 0, 0, 0);
    connect(_contrast_widget, &gstar::MinMaxSlider::min_max_val_changed, this, &ContrastWidget::on_min_max_contrast_changed);

    _cb_contrast = new QComboBox();
    _cb_contrast->addItem(STR_FULL_IMAGE);
    _cb_contrast->addItem(STR_CENTER_2_3_IMAGE);
    _cb_contrast->addItem(STR_CENTER_1_3_IMAGE);
    _cb_contrast->addItem(STR_CENTER_1_4_IMAGE);
    _cb_contrast->addItem(STR_CENTER_1_6_IMAGE);
    //_cb_contrast->setMaximumWidth(200);

    QString saved_contrast = Preferences::inst()->getValue(STR_PREF_SAVED_CONTRAST).toString();
    if(saved_contrast == STR_CENTER_2_3_IMAGE
    || saved_contrast == STR_CENTER_1_3_IMAGE
    || saved_contrast == STR_CENTER_1_4_IMAGE
    || saved_contrast == STR_CENTER_1_6_IMAGE)
    {
        _cb_contrast->setCurrentText(saved_contrast);
    }
    else
    {
        saved_contrast = STR_FULL_IMAGE;
    }
    set_contrast_changed(saved_contrast);
    connect(_cb_contrast, &QComboBox::currentTextChanged, this, &ContrastWidget::on_contrast_changed);


    QHBoxLayout* hbox_contrast = new QHBoxLayout();
    hbox_contrast->addWidget(_cb_contrast);
   // hbox_contrast->addWidget(_chk_log_color);
    hbox_contrast->addWidget(_global_contrast_chk);
    hbox_contrast->addWidget(_contrast_widget);

    this->setLayout(hbox_contrast);
}

//---------------------------------------------------------------------------

ContrastWidget::~ContrastWidget()
{

}

//---------------------------------------------------------------------------

void ContrastWidget::set_contrast_changed(QString val)
{
    Preferences::inst()->setValue(STR_PREF_SAVED_CONTRAST, val);
    Preferences::inst()->save();
    if(val == STR_FULL_IMAGE)
    {
         _global_contrast_chk->setVisible(true);
        _contrast_widget->setVisible(true);
    }
    else
    {
         _global_contrast_chk->setVisible(false);
        _contrast_widget->setVisible(false);
    }
}

//---------------------------------------------------------------------------

void ContrastWidget::on_contrast_changed(QString val)
{

    set_contrast_changed(val);
    emit call_redraw();

}

//---------------------------------------------------------------------------

void ContrastWidget::on_min_max_contrast_changed()
{

	_min_contrast_perc = _contrast_widget->getUserMin() / 100.0;
    _max_contrast_perc = _contrast_widget->getUserMax() / 100.0;
	emit call_redraw();

}

//---------------------------------------------------------------------------

void ContrastWidget::on_global_contrast_changed(Qt::CheckState state)
{
    if (state == Qt::CheckState::Checked)
    {
        _contrast_widget->setEnabled(true);
        emit global_contrast_change(true);
    }
    else
    {
        _contrast_widget->setEnabled(false);
        emit global_contrast_change(false);
    }
}

//---------------------------------------------------------------------------