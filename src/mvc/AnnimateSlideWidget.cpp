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
}

//---------------------------------------------------------------------------

void AnnimateSlideWidget::setAnimWidget(QWidget* w, QString btn_name) 
{
    if(w != nullptr)
    {
        if(_btn_hover == nullptr)
        {
            _btn_hover = new QPushButton(btn_name);
            _btn_hover->setVisible(false);
        }
        _anim_widget = w; 
    }

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(_btn_hover);
    layout->addWidget(_anim_widget);
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);
}

//---------------------------------------------------------------------------

void AnnimateSlideWidget::setAnimEnabled(bool val)
{
    if(val)
    {
        //_btn_hover->setVisible(true);
        _anim_enabled = val;
    }
    else
    {
        _btn_hover->setVisible(false);
        _anim_enabled = val;
    }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------