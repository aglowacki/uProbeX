/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ImageStackControlWidget.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ImageStackControlWidget::ImageStackControlWidget(QWidget* parent) : QWidget(parent)
{

    createLayout();

}

/*---------------------------------------------------------------------------*/

ImageStackControlWidget::~ImageStackControlWidget()
{

}

/*---------------------------------------------------------------------------*/

void ImageStackControlWidget::createLayout()
{
    QVBoxLayout* vlayout = new QVBoxLayout();
    QHBoxLayout* hlayout = new QHBoxLayout();


    _left_btn =  new QPushButton();
    _left_btn->setIcon(QIcon(":/images/previous.png"));
    _image_name_cb = new QComboBox();
    _right_btn =  new QPushButton();
    _right_btn->setIcon(QIcon(":/images/next.png"));

    hlayout->addWidget(_left_btn);
    hlayout->addWidget(_image_name_cb);
    hlayout->addWidget(_right_btn);


    vlayout->addItem(hlayout);
    setLayout(vlayout);
}

/*---------------------------------------------------------------------------*/

void ImageStackControlWidget::setModel(MapsWorkspaceModel *model)
{
    _model = model;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
