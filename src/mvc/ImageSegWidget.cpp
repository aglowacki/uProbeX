/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ImageSegWidget.h>

//---------------------------------------------------------------------------

ImageSegWidget::ImageSegWidget(QWidget* parent)
: AbstractImageWidget(1,1,parent)
{
    _draw_action_mode = gstar::DRAW_ACTION_MODES::OFF;
    _selected_roi = nullptr;
    _mouse_down = false;
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
   
   
   connect(m_selectionModel, &QItemSelectionModel::currentChanged, this, &ImageSegWidget::currentRoiChanged);
   /*
   connect(m_imageViewWidget->scene(), &gstar::ImageViewScene::onMouseMoveEvent, this, &ImageSegWidget::mouseOverPixel);
   connect(m_imageViewWidget->scene(), &gstar::ImageViewScene::onMousePressEvent, this, &ImageSegWidget::mousePressEvent);
   connect(m_imageViewWidget->scene(), &gstar::ImageViewScene::onMouseReleaseEvent, this, &ImageSegWidget::mouseReleaseEvent);
   */
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
/*
void ImageSegWidget::mouseOverPixel(int x, int y)
{
    if (_selected_roi != nullptr && _mouse_down)
    {
        if (_action_mode == ROI_ACTION_MODES::ADD)
        {
            _selected_roi->add_to_roi(x, y, _roi_brush_size);
            //QRectF rect(qreal(x), qreal(y), qreal(_roi_brush_size.width()), qreal(_roi_brush_size.height()));
            //m_imageViewWidget->view()->update(x, y, _roi_brush_size.width(), _roi_brush_size.height());
            //m_imageViewWidget->view()->update(->sceneRect());
            //m_imageViewWidget->scene()->invalidate(x, y, _roi_brush_size.width(), _roi_brush_size.height());
            //m_imageViewWidget->redrawSubWindows();
            //m_imageViewWidget->scene()->updateModel();
        }
        else if (_action_mode == ROI_ACTION_MODES::ERASE)
        {
            _selected_roi->erase_from_roi(x, y, _roi_brush_size);
        }
    }
}

//---------------------------------------------------------------------------

void ImageSegWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mouse_down = true;
        if (_selected_roi != nullptr)
        {
            if (_action_mode == ROI_ACTION_MODES::ADD)
            {
                _selected_roi->add_to_roi(event->pos().x(), event->pos().y(), _roi_brush_size);
            }
            else if (_action_mode == ROI_ACTION_MODES::ERASE)
            {
                _selected_roi->erase_from_roi(event->pos().x(), event->pos().y(), _roi_brush_size);
            }
        }
    }
}

//---------------------------------------------------------------------------

void ImageSegWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mouse_down = false;
    }
}
*/
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
        disconnect(m_imageViewWidget->scene(), &gstar::ImageViewScene::onMousePressEvent, _selected_roi, &gstar::RoiMaskGraphicsItem::onMousePressEvent);
        disconnect(m_imageViewWidget->scene(), &gstar::ImageViewScene::onMouseMoveEvent, _selected_roi, &gstar::RoiMaskGraphicsItem::onMouseMoveEvent);
        disconnect(m_imageViewWidget->scene(), &gstar::ImageViewScene::onMouseReleaseEvent, _selected_roi, &gstar::RoiMaskGraphicsItem::onMouseReleaseEvent);
    }

    insertAndSelectAnnotation(m_treeModel, m_annoTreeView, m_selectionModel, roi, false);
    // auto select the new roi
    _selected_roi = roi;
    _selected_roi->setBrushSize(_roi_brush_size);
    _selected_roi->setDrawAction(_draw_action_mode);

    connect(m_imageViewWidget->scene(), &gstar::ImageViewScene::onMousePressEvent, _selected_roi, &gstar::RoiMaskGraphicsItem::onMousePressEvent);
    connect(m_imageViewWidget->scene(), &gstar::ImageViewScene::onMouseMoveEvent, _selected_roi, &gstar::RoiMaskGraphicsItem::onMouseMoveEvent);
    connect(m_imageViewWidget->scene(), &gstar::ImageViewScene::onMouseReleaseEvent, _selected_roi, &gstar::RoiMaskGraphicsItem::onMouseReleaseEvent);
}

//---------------------------------------------------------------------------

void ImageSegWidget::currentRoiChanged(const QModelIndex& current, const QModelIndex& previous)
{
    if (_selected_roi != nullptr)
    {
        disconnect(m_imageViewWidget->scene(), &gstar::ImageViewScene::onMousePressEvent, _selected_roi, &gstar::RoiMaskGraphicsItem::onMousePressEvent);
        disconnect(m_imageViewWidget->scene(), &gstar::ImageViewScene::onMouseMoveEvent, _selected_roi, &gstar::RoiMaskGraphicsItem::onMouseMoveEvent);
        disconnect(m_imageViewWidget->scene(), &gstar::ImageViewScene::onMouseReleaseEvent, _selected_roi, &gstar::RoiMaskGraphicsItem::onMouseReleaseEvent);
    }

    gstar::AbstractGraphicsItem* abstractItem = static_cast<gstar::AbstractGraphicsItem*>(current.internalPointer());
    // will be nullptr if not the right class
    _selected_roi = dynamic_cast<gstar::RoiMaskGraphicsItem*>(abstractItem);

    if (_selected_roi != nullptr)
    {
        _selected_roi->setBrushSize(_roi_brush_size);
        _selected_roi->setDrawAction(_draw_action_mode);
        connect(m_imageViewWidget->scene(), &gstar::ImageViewScene::onMousePressEvent, _selected_roi, &gstar::RoiMaskGraphicsItem::onMousePressEvent);
        connect(m_imageViewWidget->scene(), &gstar::ImageViewScene::onMouseMoveEvent, _selected_roi, &gstar::RoiMaskGraphicsItem::onMouseMoveEvent);
        connect(m_imageViewWidget->scene(), &gstar::ImageViewScene::onMouseReleaseEvent, _selected_roi, &gstar::RoiMaskGraphicsItem::onMouseReleaseEvent);
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