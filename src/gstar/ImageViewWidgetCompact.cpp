/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/ImageViewWidgetCompact.h"

using namespace gstar;

//---------------------------------------------------------------------------

ImageViewWidgetCompact::ImageViewWidgetCompact(int rows, int cols , QWidget* parent) : ImageViewWidget(rows, cols, parent)
{

    _main_layout = nullptr;
    _sub_window.scene->removeDefaultPixmap();
    _sub_window.setImageLabelVisible(false);
    _sub_window.setCountsVisible(false);

    // Create main layout and add widgets
    createSceneAndView(rows,cols);
    createLayout();

}

//---------------------------------------------------------------------------

ImageViewWidgetCompact::~ImageViewWidgetCompact()
{

}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::setGlobalContrast(bool val)
{
    
    _sub_window.btn_contrast->setVisible(val);
    
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::clickCursor()
{

    _sub_window.scene->setZoomModeToNone();
    _sub_window.view->viewport()->setCursor(Qt::ArrowCursor);
	
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::customCursor(QCursor cursor)
{
    
    _sub_window.view->viewport()->setCursor(cursor);

}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::setSelectorVisible(bool val)
{

    _sub_window.setImageLabelVisible(val);

}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::setCountsVisible(bool val)
{
    
    _sub_window.setCountsVisible(val);

}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::clickFill(bool checked)
{

    // State is used by the resizeEvent raised below.
    //  Make sure state is set before calling resizeEvent method.
    m_fillState = checked;
   
    // Get out of zoom mode
    clickCursor();

    // Set scene mode
    if (checked == true) 
    {
        _sub_window.scene->setZoomModeToFit();
    }

    // Set regular cursor
    _sub_window.view->viewport()->setCursor(Qt::ArrowCursor);

    resizeEvent(nullptr);
    m_zoomPercent->setCurrentIndex(-1);

}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::clickZoomIn()
{

    _sub_window.scene->setZoomModeToZoomIn();
    _sub_window.view->viewport()->setCursor(m_zoomInCursor);

}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::clickZoomOriginal()
{

    
    _sub_window.view->resetTransform();
    _sub_window.view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _sub_window.view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	

   updateZoomPercentage();

}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::clickZoomOut()
{

    _sub_window.scene->setZoomModeToZoomOut();
    _sub_window.view->viewport()->setCursor(m_zoomOutCursor);

}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::createLayout()
{

    QPixmap pmap(90, 90);
    pmap.fill(Qt::blue);
   

    for(auto itr : _pixmaps)
    {
        _sub_window.scene->removeItem(itr);
        delete itr;
    }
    _pixmaps.clear();
    for (int i = 0; i < _grid_rows; i++)
    {
        for (int j = 0; j < _grid_cols; j++)
        {
            _pixmaps.emplace_back(new QGraphicsPixmapItem(pmap));
            QRectF bbox = _pixmaps.back()->boundingRect();
            float width = bbox.width() + 10.0;
            float height = bbox.height() + 10.0;
            _pixmaps.back()->setPos(j * width, i * height);
            _sub_window.scene->addItem(_pixmaps.back());
        }
    }

    _sub_window.scene->setSceneRect(_sub_window.scene->itemsBoundingRect());

    if(m_coordWidget == nullptr)
    {
        m_coordWidget = new CoordinateWidget();
    }

    // Main Layout
    if(_main_layout == nullptr)
    {
        _main_layout = new QVBoxLayout();
        _main_layout->addItem(_sub_window.layout);
        _main_layout->addWidget(m_coordWidget);
        _main_layout->setSpacing(0);
        _main_layout->setContentsMargins(0, 0, 0, 0);
        // Set widget's layout
        setLayout(_main_layout);

    }

    _sub_window.scene->update();
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::createSceneAndView(int rows, int cols)
{
	_grid_rows = rows;
	_grid_cols = cols;

    
	connect(_sub_window.scene, &ImageViewScene::zoomIn, this, &ImageViewWidgetCompact::zoomIn);
    connect(_sub_window.scene, &ImageViewScene::zoomInRect, this, &ImageViewWidgetCompact::zoomInRect);
    connect(_sub_window.scene, &ImageViewScene::zoomOut, this, &ImageViewWidgetCompact::zoomOut);
    //connect(_sub_window.scene, &ImageViewScene::sceneRectChanged, this, &ImageViewWidgetCompact::sceneRectUpdated);
    connect(_sub_window.scene, &ImageViewScene::onMouseMoveEvent, this, &ImageViewWidgetCompact::onMouseMoveEvent);
    
    connect(_sub_window.cb_image_label, &QComboBox::currentTextChanged, this, &ImageViewWidgetCompact::onComboBoxChange);

    connect(&_sub_window, &SubImageWindow::redraw_event, this, &ImageViewWidgetCompact::subwindow_redraw);
	
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::setUnitLabel(int idx, QString label)
{
    
    _sub_window.counts_coord_widget->setUnitsLabel(label);
    _sub_window.counts_stats_widget->setUnitsLabel(label);

}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::setUnitLabels(QString label)
{

    _sub_window.counts_coord_widget->setUnitsLabel(label);
    _sub_window.counts_stats_widget->setUnitsLabel(label);

}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::newGridLayout(int rows, int cols)
{

    disconnect(_sub_window.scene, &ImageViewScene::zoomIn, this, &ImageViewWidgetCompact::zoomIn);
    disconnect(_sub_window.scene, &ImageViewScene::zoomInRect, this, &ImageViewWidgetCompact::zoomInRect);
    disconnect(_sub_window.scene, &ImageViewScene::zoomOut, this, &ImageViewWidgetCompact::zoomOut);
    //disconnect(_sub_window.scene, &ImageViewScene::sceneRectChanged, this, &ImageViewWidgetCompact::sceneRectUpdated);
    disconnect(_sub_window.scene, &ImageViewScene::onMouseMoveEvent, this, &ImageViewWidgetCompact::onMouseMoveEvent);
    disconnect(_sub_window.cb_image_label, &QComboBox::currentTextChanged, this, &ImageViewWidgetCompact::onComboBoxChange);
    disconnect(&_sub_window, &SubImageWindow::redraw_event, this, &ImageViewWidgetCompact::subwindow_redraw);

    createSceneAndView(rows, cols);
    createLayout();
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::subwindow_redraw(SubImageWindow* win)
{
    //_sub_window.scene->update();
    emit parent_redraw(0);
}


//---------------------------------------------------------------------------

void ImageViewWidgetCompact::onComboBoxChange(QString lbl)
{
    /*
    QObject* obj = sender();
    if(_sub_window.cb_image_label == obj)
    {
        emit cbLabelChanged(lbl, i);
    }
    */
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::clearLabels()
{

    _sub_window.cb_image_label->clear();

}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::addLabel(QString lbl)
{
    
    _sub_window.cb_image_label->addItem(lbl);
    
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::setLabel(QString lbl)
{
    
    _sub_window.cb_image_label->setCurrentText(lbl);
}

//---------------------------------------------------------------------------

qreal ImageViewWidgetCompact::getCurrentZoomPercent()
{
    qreal wp = 0;
    
    QTransform t = _sub_window.view->transform();
    QRectF tImage = t.mapRect(_sub_window.scene->pixRect());

    wp = tImage.width() / _sub_window.scene->pixRect().width() * 100.0;
    
    return wp;
}

//---------------------------------------------------------------------------

QPointF ImageViewWidgetCompact::getCenterPoint() const
{
    
    return _sub_window.view->mapToScene(_sub_window.view->viewport()->geometry()).boundingRect().center();

}

//---------------------------------------------------------------------------

QRectF ImageViewWidgetCompact::getSceneRect()
{  
    
    return _sub_window.scene->sceneRect();
    
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::onMouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    int x = event->scenePos().x();
    int y = event->scenePos().y();
    m_coordWidget -> setCoordinate(x,y);
    _sub_window.counts_coord_widget->setCoordinate(x, y, 0);
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::resizeEvent(QResizeEvent* event)
{

    Q_UNUSED(event)

    if (m_fillState == false) return;


    QRectF r(0, 0, (_sub_window.scene->sceneRect()).width(), (_sub_window.scene->sceneRect()).height());

    _sub_window.view->fitInView(r, Qt::KeepAspectRatio);

    _sub_window.view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _sub_window.view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    updateZoomPercentage();
    update();
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::connectRoiGraphicsItemToMouseEvents(RoiMaskGraphicsItem* roi)
{
    connect(_sub_window.scene, &ImageViewScene::onMousePressEvent, roi, &RoiMaskGraphicsItem::onMousePressEvent);
    connect(_sub_window.scene, &ImageViewScene::onMouseMoveEvent, roi, &RoiMaskGraphicsItem::onMouseMoveEvent);
    connect(_sub_window.scene, &ImageViewScene::onMouseReleaseEvent, roi, &RoiMaskGraphicsItem::onMouseReleaseEvent);
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::disconnectRoiGraphicsItemToMouseEvents(RoiMaskGraphicsItem* roi)
{
    disconnect(_sub_window.scene, &ImageViewScene::onMousePressEvent, roi, &RoiMaskGraphicsItem::onMousePressEvent);
    disconnect(_sub_window.scene, &ImageViewScene::onMouseMoveEvent, roi, &RoiMaskGraphicsItem::onMouseMoveEvent);
    disconnect(_sub_window.scene, &ImageViewScene::onMouseReleaseEvent, roi, &RoiMaskGraphicsItem::onMouseReleaseEvent);
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::setSceneModel(QAbstractItemModel* model)
{
    _sub_window.scene->setModel(model, false);
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::setSceneSelectionModel(QItemSelectionModel* selectionModel)
{
    
    _sub_window.scene->setSelectionModel(selectionModel);
    
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::setSceneModelAndSelection(QAbstractItemModel* model, QItemSelectionModel* selectionModel)
{
    _sub_window.scene->setModel(model, false);
    _sub_window.scene->setSelectionModel(selectionModel);
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::sceneEnableAnnotations(bool state)
{
    
    _sub_window.scene->enableAnnotations(state);
    
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::setSceneUnitsLabel(QString label)
{
    
    _sub_window.scene->setUnitsLabel(label);
    
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::setSceneUnitsPerPixelX(double val)
{
    
    _sub_window.scene->setUnitsPerPixelX(val);
    
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::setSceneUnitsPerPixelY(double val)
{
    
    _sub_window.scene->setUnitsPerPixelY(val);
    
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::sceneUpdateModel()
{
    
    _sub_window.scene->updateModel();
    
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::setScenePixmap(const QPixmap& p)
{
 //   _sub_window.scene->setPixmap(p);
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::setSubScenePixmap(int idx, const QPixmap& p)
{
    if(false == p.isNull())
    {
        if(idx > -1 && idx < _pixmaps.size())
        {
          _pixmaps[idx]->setPixmap(p);
        }
        // re position them
        if(_pixmaps.size() > 0)
        {
            QRectF bbox = _pixmaps[0]->boundingRect();
            float width = bbox.width() + 10.0;
            float height = bbox.height() + 10.0;
                    
            int n = 0;
            for (int i = 0; i < _grid_rows; i++)
            {
                for (int j = 0; j < _grid_cols; j++)
                {
                    _pixmaps[n]->setPos(j * width, i * height);
                    n++;
                }
            }
        }
        _sub_window.scene->setSceneRect(_sub_window.scene->itemsBoundingRect());
    }
  //  _sub_window.scene->update();
}

//---------------------------------------------------------------------------

QGraphicsView* ImageViewWidgetCompact::view()
{

    return _sub_window.view;

}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::zoomIn(QGraphicsItem* zoomObject)
{
	
	_sub_window.view->fitInView(zoomObject, Qt::KeepAspectRatio);

		// Force update scroll bars
	_sub_window.view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	_sub_window.view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	
   updateZoomPercentage();

   clickCursor();
   m_fillState = false;
   emit resetZoomToolBar();
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::zoomInRect(QRectF zoomRect, QGraphicsSceneMouseEvent* event)
{

    qreal wp = getCurrentZoomPercent();
    if (wp >= 800) return;

    //int zoomRecPer = 500 - wp;
    int zoomWidth = zoomRect.normalized().width();
    int zoomHeight = zoomRect.normalized().height();
    // Zoom in

    
    if ((!zoomRect.isEmpty() || !zoomRect.normalized().isEmpty()) && (zoomWidth > 10 && zoomHeight > 10))
    {
        
        _sub_window.view->fitInView(QRectF(_sub_window.view->mapToScene(zoomRect.topLeft().toPoint()), _sub_window.view->mapToScene(zoomRect.bottomRight().toPoint())), Qt::KeepAspectRatio);
        
        /*
        QRect viewport = m_view -> rect();

        float xscale = viewport.width()  / zoomRect.normalized().width();
        float yscale = viewport.height() / zoomRect.normalized().height();

        // To preserve aspect ratio in the scaled image,
        // pick the smallest of two dimensions as scaling factor.
        float scalev = xscale;
        if (xscale > yscale) {
            scalev = yscale;
        }

        m_view -> scale(scalev, scalev);

        // Center the zoomed-in image at the center of zoom selection
        QPointF imageonScene = m_view -> mapToScene(zoomRect.normalized().topLeft().toPoint());
        m_view -> centerOn(imageonScene);
        */
    }

    else
    {
        // Without zoom rectangle, scale using fixed value
        _sub_window.view->scale(1.50, 1.50);
        _sub_window.view->centerOn(event->lastScenePos());
        
    }
    
    // Force update scroll bars
    _sub_window.view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _sub_window.view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    updateZoomPercentage();
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::zoomOut()
{

   qreal wp = getCurrentZoomPercent();

   if (wp <= 12.5) return;
   
    _sub_window.view->scale(.66, .66);

    // Force update scroll bars
    _sub_window.view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _sub_window.view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   
    updateZoomPercentage();

}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::zoomValueChanged(int val)
{

   bool isOK = false;
   float value = m_zoomPercent->currentText().toFloat(&isOK);
   if (!isOK || value < 12.5 || value > 800) {
      m_zoomPercent->removeItem(m_zoomPercent->currentIndex());
      updateZoomPercentage();
      return;
   }

   
    QTransform t = _sub_window.view->transform();
    QRectF image = _sub_window.scene->pixRect();
    QRectF tImage = t.mapRect(image);

    qreal sx = (value / 100 * image.width()) / tImage.width();
    qreal sy = (value / 100 * image.height()) / tImage.height();

    qreal s = sx;
    if (sy < sx) s = sy;
    
    _sub_window.view->scale(s, s);

}

//---------------------------------------------------------------------------

QString ImageViewWidgetCompact::getLabelAt(int idx)
{
    if(idx > -1 && idx < _sub_window.cb_image_label->count())
    {
        return _sub_window.cb_image_label->itemText(idx);
    }
    return "";
    //return _sub_window.cb_image_label->currentText();
    // TODO: have a list of elements that were selected to display. 

}

//---------------------------------------------------------------------------

 void ImageViewWidgetCompact::getMouseTrasnformAt(int idx, CountsLookupTransformer** counts_lookup, CountsStatsTransformer** counts_stats)
{
    *counts_lookup = _sub_window.counts_lookup;
    *counts_stats = _sub_window.counts_stats;
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::setCountsTrasnformAt(unsigned int idx, const ArrayXXr<float>& normalized)
{
    
    if(_sub_window.counts_lookup != nullptr)
    {
        _sub_window.counts_lookup->setCounts(normalized);
    }
    if(_sub_window.counts_stats != nullptr)
    {
        _sub_window.counts_stats->setCounts(normalized);
    }

}

//---------------------------------------------------------------------------

std::vector<QString> ImageViewWidgetCompact::getLabelList()
{
    std::vector<QString> label_list;
    label_list.push_back(_sub_window.cb_image_label->currentText());

    return label_list;
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::restoreLabels(const std::vector<QString>& labels)
{
    bool found = false;
    QComboBox* cb = _sub_window.cb_image_label;
    if(labels.size() > 0)
    {
        QString ilabel = labels[0];
        for(int j=0; j < cb->count(); j++)
        {
            if(cb->itemText(j) == ilabel)
            {
                cb->setCurrentText(ilabel);
                found = true;
                break;
            }
        }
    }
    if(false == found)
    {
        if(cb->count() > 0)
        {
            cb->setCurrentText(cb->itemText(0));
        }
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::resetCoordsToZero()
{

    _sub_window.counts_coord_widget->setCoordinate(0, 0, 0);
    _sub_window.counts_stats_widget->setCoordinate(0, 0, 0);

}

//---------------------------------------------------------------------------

bool ImageViewWidgetCompact::getMinMaxAt(int grid_idx, float &counts_min, float &counts_max)
{
	
    if (_sub_window.contrast_updated())
    {
        counts_min = _sub_window.contrast_min();
        counts_max = _sub_window.contrast_max();
        return true;
    }
	
    return false;
}

//---------------------------------------------------------------------------