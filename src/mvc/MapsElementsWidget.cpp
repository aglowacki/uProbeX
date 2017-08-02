/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsElementsWidget.h>

#include <ImageViewWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QDebug>

using gstar::AbstractImageWidget;
using gstar::ImageViewWidget;

/*---------------------------------------------------------------------------*/

MapsElementsWidget::MapsElementsWidget(QWidget* parent)
: AbstractImageWidget(parent)
{

    _model = nullptr;
   createLayout();

}

/*---------------------------------------------------------------------------*/

MapsElementsWidget::~MapsElementsWidget()
{

    if(_model != nullptr)
    {
        delete _model;
    }
    _model = nullptr;

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::createLayout()
{

   QLayout* layout = generateDefaultLayout();
   //appendAnnotationTab();
   setLayout(layout);

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::setModel(MapsModel* model)
{

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::windowChanged(Qt::WindowStates oldState,
                               Qt::WindowStates newState)
{
   Q_UNUSED(oldState);

   if(Qt::WindowMaximized || Qt::WindowActive == newState)
   {
      m_imageViewWidget->resizeEvent(NULL);
   }

}

/*---------------------------------------------------------------------------*/
