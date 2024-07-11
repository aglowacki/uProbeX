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
    _first = true;
    _running = false;
}

//---------------------------------------------------------------------------

void AnnimateSlideWidget::setAnimWidget(QWidget* w, QWidget* container_widget) 
{
    if(w != nullptr)
    {
        _anim_widget = w; 
        _anim_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        _anim_hide = new QPropertyAnimation(_anim_widget, "minimumWidth");
        connect(_anim_hide, &QPropertyAnimation::finished, this, &AnnimateSlideWidget::onFinished);
        _anim_hide->setDuration(250);

        _anim_show = new QPropertyAnimation(_anim_widget, "minimumWidth");
        connect(_anim_show, &QPropertyAnimation::finished, this, &AnnimateSlideWidget::onFinished);
        _anim_show->setDuration(100);
    }

    QHBoxLayout *layout = new QHBoxLayout();
    if(container_widget != nullptr)
    {
        layout->addWidget(container_widget);
    }
    else
    {
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