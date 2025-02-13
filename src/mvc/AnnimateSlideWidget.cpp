/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/AnnimateSlideWidget.h>
#include <QHBoxLayout>
//---------------------------------------------------------------------------

AnnimateSlideWidget::AnnimateSlideWidget(QWidget *parent) : QWidget(parent)
{
    _anim_widget = nullptr;
    _anim_enabled = true;
    _isShown = true;
    _first = true;
    _cur_state = SlideState::SlideOut;
}

//---------------------------------------------------------------------------

void AnnimateSlideWidget::setAnimWidget(QWidget* w, QWidget* container_widget) 
{
    _btn_toggle = new QPushButton("|");
    _btn_toggle->setMaximumWidth(10);
    connect(_btn_toggle, &QPushButton::pressed, this, &AnnimateSlideWidget::onToggle);
    if(w != nullptr)
    {
        _anim_widget = w; 
        _anim_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        _anim_hide = new QPropertyAnimation(_anim_widget, "minimumWidth");
        _anim_hide->setDuration(100);

        _anim_show = new QPropertyAnimation(_anim_widget, "minimumWidth");
        _anim_show->setDuration(100);
    }

    QHBoxLayout *layout = new QHBoxLayout();
    if(container_widget != nullptr)
    {
        layout->addWidget(container_widget);
        layout->addWidget(_btn_toggle);
    }
    else
    {
        layout->addWidget(_btn_toggle);
        layout->addWidget(_anim_widget);
    }
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);
}

//---------------------------------------------------------------------------

void AnnimateSlideWidget::setAnimEnabled(bool val)
{
    if(val)
    {
        _anim_enabled = val;
    }
    else
    {
        _anim_enabled = val;
    }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------