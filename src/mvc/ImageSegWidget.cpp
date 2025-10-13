/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ImageSegWidget.h>
#include "preferences/Preferences.h"

//---------------------------------------------------------------------------

ImageSegWidget::ImageSegWidget(QWidget* parent)
: AbstractImageWidget(1,1,parent)
{
    _draw_action_mode = gstar::DRAW_ACTION_MODES::OFF;
    _selected_roi = nullptr;
    _mouse_down = false;

    createLayout();
    createActions();

    //disconnect(m_imageViewWidget,&ImageViewWidget::customContextMenuRequested,this,&ImageSegWidget::viewContextMenu);
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
   
   
   connect(m_selectionModel, &QItemSelectionModel::currentChanged, this, &ImageSegWidget::currentRoiChanged);
   appendAnnotationTab(false);
   setLayout(layout);
}

//---------------------------------------------------------------------------

void ImageSegWidget::setImageFromArray(ArrayXXr<float>& img_arr, QVector<QRgb>& colormap)
{
    QImage image = m_imageViewWidget->generate_img(img_arr, colormap);
    if (Preferences::inst()->getValue(STR_INVERT_Y_AXIS).toBool())
    {
        image = image.mirrored(false, true);
    }
    setPixMap(QPixmap::fromImage(image.convertToFormat(QImage::Format_RGB32)));
}

//---------------------------------------------------------------------------

void ImageSegWidget::setPixMap(const QPixmap& pix)
{
    m_imageViewWidget->setScenePixmap(pix);
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

void ImageSegWidget::setActionMode(gstar::DRAW_ACTION_MODES mode)
{
    _draw_action_mode = mode;
    if (_selected_roi != nullptr)
    {
        _selected_roi->setBrushSize(_roi_brush_size);
        _selected_roi->setDrawAction(_draw_action_mode);
    }
}

//---------------------------------------------------------------------------

void ImageSegWidget::setRoiBrushSize(int val)
{
    if (val < 1)
    {
        val = 1;
    }
    _roi_brush_size = QSize(val, val);
    if (_selected_roi != nullptr)
    {
        _selected_roi->setBrushSize(_roi_brush_size);
        _selected_roi->setDrawAction(_draw_action_mode);
    }
}

//---------------------------------------------------------------------------

void ImageSegWidget::addRoiMask(gstar::RoiMaskGraphicsItem* roi)
{
    if (_selected_roi != nullptr)
    {
        m_imageViewWidget->disconnectRoiGraphicsItemToMouseEvents(_selected_roi);
    }

    insertAndSelectAnnotation(m_treeModel, m_annoTreeView, m_selectionModel, roi, false);
    // auto select the new roi
    _selected_roi = roi;
    if (_selected_roi != nullptr)
    {
        _selected_roi->setBrushSize(_roi_brush_size);
        _selected_roi->setDrawAction(_draw_action_mode);
    
        m_imageViewWidget->connectRoiGraphicsItemToMouseEvents(_selected_roi);
    }
}

//---------------------------------------------------------------------------

void ImageSegWidget::invertSelectedRoiMask()
{
    if (_selected_roi != nullptr)
    {
        _selected_roi->invertMask();
    }
}

//---------------------------------------------------------------------------

void ImageSegWidget::currentRoiChanged(const QModelIndex& current, const QModelIndex& previous)
{
    if (_selected_roi != nullptr)
    {
        m_imageViewWidget->disconnectRoiGraphicsItemToMouseEvents(_selected_roi);
    }

    gstar::AbstractGraphicsItem* abstractItem = static_cast<gstar::AbstractGraphicsItem*>(current.internalPointer());
    // will be nullptr if not the right class
    _selected_roi = dynamic_cast<gstar::RoiMaskGraphicsItem*>(abstractItem);

    if (_selected_roi != nullptr)
    {
        _selected_roi->setBrushSize(_roi_brush_size);
        _selected_roi->setDrawAction(_draw_action_mode);
        m_imageViewWidget->connectRoiGraphicsItemToMouseEvents(_selected_roi);
    }
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

//---------------------------------------------------------------------------

int ImageSegWidget::getROIsCount()
{
    std::vector<gstar::RoiMaskGraphicsItem*> list;
    QImage i;
    QColor q;
    gstar::RoiMaskGraphicsItem item(i, q, 0);
    if (m_treeModel != nullptr)
    {
        m_treeModel->get_all_of_type(item.classId(), list);
    }
    return list.size();
}

//---------------------------------------------------------------------------