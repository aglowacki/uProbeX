/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ImageSegWidget.h>

//---------------------------------------------------------------------------

ImageSegWidget::ImageSegWidget(QWidget* parent)
: AbstractImageWidget(1,1,parent)
{
    createLayout();
    createActions();
}

//---------------------------------------------------------------------------

ImageSegWidget::~ImageSegWidget()
{
    _first_pixmap_set = true;
}

//---------------------------------------------------------------------------

void ImageSegWidget::createLayout()
{

   QLayout* layout = generateDefaultLayout(false);
   m_imageViewWidget->setCoordsVisible(false);
   m_imageViewWidget->setSelectorVisible(false);
   m_imageViewWidget->setCountsVisible(false);
   setLayout(layout);
}

//---------------------------------------------------------------------------

void ImageSegWidget::setPixMap(QPixmap pix)
{
    m_imageViewWidget->scene(0)->setPixmap(pix);
    if (_first_pixmap_set)
    {
        m_imageViewWidget->clickFill(true);
        _first_pixmap_set = false;
    }
}

//---------------------------------------------------------------------------

void ImageSegWidget::windowChanged(Qt::WindowStates oldState,
                               Qt::WindowStates newState)
{

   Q_UNUSED(oldState);

   if(Qt::WindowMaximized || Qt::WindowActive == newState)
   {
      m_imageViewWidget->resizeEvent(nullptr);
   }

}

//---------------------------------------------------------------------------

void ImageSegWidget::widgetChanged(bool enable)
{
   // More widget change after solver run could be added here
   //m_btnRunSolver -> setEnabled(enable);

}

//---------------------------------------------------------------------------