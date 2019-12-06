/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/PerPixelFitWidget.h>

/*---------------------------------------------------------------------------*/

PerPixelFitWidget::PerPixelFitWidget(QWidget *parent) : QWidget(parent)
{

    createLayout();

}

/*---------------------------------------------------------------------------*/

PerPixelFitWidget::~PerPixelFitWidget()
{

 
}

/*---------------------------------------------------------------------------*/

void PerPixelFitWidget::createLayout()
{

    _progressBar = new QProgressBar();
    _progressBar->setRange(0,100);

    _btn_run = new QPushButton("Run");
    _btn_cancel = new QPushButton("Cancel");


    QHBoxLayout* buttonlayout = new QHBoxLayout();
    buttonlayout->addWidget(_btn_run);
    buttonlayout->addWidget(_btn_cancel);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addItem(buttonlayout);
    layout->addWidget(_progressBar);

    setLayout(layout);

}

/*---------------------------------------------------------------------------*/

void PerPixelFitWidget::updateFileList(QStringList file_list)
{

}

/*---------------------------------------------------------------------------*/
