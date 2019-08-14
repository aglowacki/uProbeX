/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/TIFFWidget.h>

#include <gstar/ImageViewWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QDebug>

using gstar::AbstractImageWidget;
using gstar::ImageViewWidget;

/*---------------------------------------------------------------------------*/

TIFFWidget::TIFFWidget(QWidget* parent)
: AbstractImageWidget(1,1,parent)
{

   createLayout();

}

/*---------------------------------------------------------------------------*/

TIFFWidget::~TIFFWidget()
{


}

/*---------------------------------------------------------------------------*/

void TIFFWidget::createLayout()
{

   QLayout* layout = generateDefaultLayout();
   appendAnnotationTab();
   setLayout(layout);

}

/*---------------------------------------------------------------------------*/

void TIFFWidget::windowChanged(Qt::WindowStates oldState,
                               Qt::WindowStates newState)
{
   Q_UNUSED(oldState);

   if(Qt::WindowMaximized || Qt::WindowActive == newState)
   {
      m_imageViewWidget->resizeEvent(nullptr);
   }

}

/*---------------------------------------------------------------------------*/
