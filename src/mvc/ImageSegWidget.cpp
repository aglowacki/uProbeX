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

   QLayout* layout = generateDefaultLayout(true);
   m_imageViewWidget->setCoordsVisible(false);
   m_imageViewWidget->setSelectorVisible(false);
   m_imageViewWidget->setCountsVisible(false);
   appendAnnotationTab();
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

void ImageSegWidget::clearAllRoiMasks()
{
    m_treeModel->clearAll();
}

//---------------------------------------------------------------------------

void ImageSegWidget::addRoiMask(gstar::RoiMaskGraphicsItem* roi)
{
    insertAndSelectAnnotation(m_treeModel, m_annoTreeView, m_selectionModel, roi);
}

//---------------------------------------------------------------------------

std::vector<gstar::RoiMaskGraphicsItem*> ImageSegWidget::getAllROIs()
{

    std::vector<gstar::RoiMaskGraphicsItem*> list;
    QImage i;
    QColor q;
    gstar::RoiMaskGraphicsItem item(i, q, 0);
    if (m_treeModel != nullptr)
    {
        m_treeModel->get_all_of_type(item.classId(), list);
    }
    return list;
}