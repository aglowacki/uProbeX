/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/ImageViewWidgetCompact.h"
#include <QPainterPath>

using namespace gstar;

//---------------------------------------------------------------------------

ImageViewWidgetCompact::ImageViewWidgetCompact(int rows, int cols , QWidget* parent) : ImageViewWidget(rows, cols, parent)
{

    _main_layout = nullptr;
    _scale_bar_line = nullptr;
    _scale_bar_text = nullptr;
    _height_offset = 20.0;
    _cur_scale = 1.0;
    _sub_window.scene->removeDefaultPixmap();
    _sub_window.scene->setItemOffset(QPointF(0.0f,_height_offset)); 
    connect(_sub_window.scene, &ImageViewScene::onMousePressEvent, this, &ImageViewWidgetCompact::onMousePressEvent);
    _sub_window.setImageLabelVisible(false);
    _sub_window.setCountsVisible(false);
    
    _element_font = QFont("Ariel", 10, QFont::Bold);
    _min_max_font = QFont("Ariel", 6);
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

void ImageViewWidgetCompact::clickFill()
{
    // Set scene mode
    _sub_window.scene->setZoomModeToFit();
    
    // Set regular cursor
    _sub_window.view->viewport()->setCursor(Qt::ArrowCursor);

    //resizeEvent(nullptr);
    QRectF r(0, 0, (_sub_window.scene->sceneRect()).width(), (_sub_window.scene->sceneRect()).height());
    _sub_window.view->fitInView(r, Qt::KeepAspectRatio);
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

    m_zoomPercent->setCurrentText("100");
    zoomValueChanged(0);

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
   
    _pixmaps.clear();
    _clip_pixmaps.clear();
    _el_textitems.clear();
    _min_textitems.clear();
    _max_textitems.clear();
    _unit_textitems.clear();
    _raw_data_items.clear();
    _sub_window.scene->clear();
    _scale_bar_line = nullptr;
    _scale_bar_text = nullptr;

    QColor fg;
    QColor bg = _sub_window.scene->backgroundBrush().color();
    // If brush is NoBrush (the default — see ImageViewScene.cpp:41), fall back to the view's palette:
    if (_sub_window.scene->backgroundBrush().style() == Qt::NoBrush)
    {
        bg = _sub_window.view->palette().color(QPalette::Base);
    }
        // Rec. 601 luma, 0..255
    double luma = 0.299 * bg.redF() + 0.587 * bg.greenF() + 0.114 * bg.blueF();
    if(luma > 0.5)
    { 
        fg = Qt::black;
    }
    else
    {
        fg = Qt::white;
    }

    for (int i = 0; i < _grid_rows; i++)
    {
        for (int j = 0; j < _grid_cols; j++)
        {
            _clip_pixmaps.emplace_back(new ClipperItem(90,90));
            _pixmaps.emplace_back(new ClickablePixmapItem(pmap, _clip_pixmaps.back()));
            _el_textitems.emplace_back(new QGraphicsTextItem("E"));
            _min_textitems.emplace_back(new QGraphicsTextItem("m"));
            _max_textitems.emplace_back(new QGraphicsTextItem("M"));
            _unit_textitems.emplace_back(new QGraphicsTextItem("cts/s"));
            QRectF bbox = _pixmaps.back()->boundingRect();
            _spacer_width = bbox.width() + 10.0;
            _spacer_height = bbox.height() + 10.0;
            //_pixmaps.back()->setPos(j * _spacer_width, i * _spacer_height);
            _clip_pixmaps.back()->setPos(j * _spacer_width, i * _spacer_height);

            _el_textitems.back()->setPos(j * _spacer_width, i * _spacer_height);
            _el_textitems.back()->setDefaultTextColor(fg);
            _el_textitems.back()->setFont(_element_font);

            _min_textitems.back()->setPos(j * _spacer_width + 28, i * _spacer_height);
            _min_textitems.back()->setDefaultTextColor(fg);
            _min_textitems.back()->setFont(_min_max_font);

            _max_textitems.back()->setPos(j * _spacer_width + 28, i * _spacer_height + 8);
            _max_textitems.back()->setDefaultTextColor(fg);
            _max_textitems.back()->setFont(_min_max_font);

            _unit_textitems.back()->setPos(j * _spacer_width + 48, i * _spacer_height + 8);
            _unit_textitems.back()->setDefaultTextColor(fg);
            _unit_textitems.back()->setFont(_min_max_font);

            _sub_window.scene->storeStaticItem(_pixmaps.back());
            _sub_window.scene->addStaticItem(_clip_pixmaps.back());
            _sub_window.scene->addStaticItem(_el_textitems.back());
            _sub_window.scene->addStaticItem(_min_textitems.back());
            _sub_window.scene->addStaticItem(_max_textitems.back());
            _sub_window.scene->addStaticItem(_unit_textitems.back());
        }
    }

    _scale_bar_line = new QGraphicsLineItem();
    _scale_bar_line->setPen(QPen(fg, 2));
    _scale_bar_line->setVisible(false);
    _scale_bar_text = new QGraphicsTextItem();
    _scale_bar_text->setDefaultTextColor(fg);
    _scale_bar_text->setFont(_min_max_font);
    _scale_bar_text->setVisible(false);
    _sub_window.scene->addStaticItem(_scale_bar_line);
    _sub_window.scene->addStaticItem(_scale_bar_text);

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

void ImageViewWidgetCompact::setSubScenePixmap(int idx, const QPixmap& p)
{
    if(false == p.isNull())
    {   
        if(idx > -1 && idx < _pixmaps.size())
        {
            _pixmaps[idx]->setPixmap(p);
            _clip_pixmaps[idx]->updateSize(p.width(), p.height());
            _clip_pixmaps[idx]->update();
        
            _spacer_width = p.width() + 4.0;

            for (int n = 0; n < _grid_rows * _grid_cols; n++)
            {
                _el_textitems[n]->setPlainText(_sub_window.cb_image_label->itemText(n));
            }

            qreal effective_offset = computeLabelAreaHeight() + 2.0;
            _spacer_height = p.height() + effective_offset;

            int n = 0;
            for (int i = 0; i < _grid_rows; i++)
            {
                for (int j = 0; j < _grid_cols; j++)
                {
                    _clip_pixmaps[n]->setPos(j * _spacer_width, (i * _spacer_height) + effective_offset);
                    layoutCellLabels(n, i, j);
                    n++;
                }
            }
            updateScaleBar();
            _sub_window.scene->setSceneRect(_sub_window.scene->itemsBoundingRect());
            _sub_window.scene->update();
        }
    }

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

    for (int i = 0; i < _grid_rows * _grid_cols; i++)
    {
        _unit_textitems[i]->setPlainText(label);
        layoutCellLabels(i, i / _grid_cols, i % _grid_cols);
    }
    //_sub_window.counts_coord_widget->setUnitsLabel(label);
    //_sub_window.counts_stats_widget->setUnitsLabel(label);

}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::setUnitLabels(QString label)
{

    for (int i = 0; i < _grid_rows * _grid_cols; i++)
    {
        _unit_textitems[i]->setPlainText(label);
        layoutCellLabels(i, i / _grid_cols, i % _grid_cols);
    }
    //_sub_window.counts_coord_widget->setUnitsLabel(label);
    //_sub_window.counts_stats_widget->setUnitsLabel(label);

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
    /*
    qreal wp = 0;
    if(_pixmaps.size() > 0)
    {
        wp = _pixmaps[0]->scale();
        wp *= 100;
    }
    return wp;
    */
   return _cur_scale * 100.0;
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

}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::onMousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem *item = _sub_window.scene->itemAt(event->scenePos(), QTransform());
    ClickablePixmapItem* selectedItem = qgraphicsitem_cast<ClickablePixmapItem*>(item);
    if (selectedItem) 
    {
        QPointF seletionPoint = selectedItem->last_local_intersection_point();
        unsigned int r = (unsigned int)seletionPoint.y();
        unsigned int c = (unsigned int)seletionPoint.x();
        if(m_coordWidget->model() != nullptr)
        {
            double outX, outY, outZ;
            m_coordWidget->model()->runTransformer(c,r,0, &outX, &outY, &outZ);
            logI<<"motor X["<<c<<"]: "<< outX <<" , Y[ "<<r<<"]: "<< outY <<"\r\n";
        }
        else
        {
            logI<<"motor X["<<c<<"]: N/A , Y[ "<<r<<"]: N/A\r\n";
        }
        int i = 0;
        for(auto itr : _raw_data_items)
        {
            if(c >= 0 && r >= 0 && c < itr.second.cols() && r < itr.second.rows())
            {   
                logI<<_sub_window.cb_image_label->itemText(i).toStdString()<<": ";
                logit_s<<itr.second(r,c)<<"\r\n";
            }
            i++;
        }
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::resizeEvent(QResizeEvent* event)
{

    Q_UNUSED(event)

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
    _sub_window.scene->setModel(model, true);
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::setSceneSelectionModel(QItemSelectionModel* selectionModel)
{
    
    _sub_window.scene->setSelectionModel(selectionModel);
    
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::setSceneModelAndSelection(QAbstractItemModel* model, QItemSelectionModel* selectionModel)
{
    _sub_window.scene->setModel(model, true);
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

QGraphicsView* ImageViewWidgetCompact::view()
{

    return _sub_window.view;

}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::zoomIn(QGraphicsItem* zoomObject)
{
	
	//_sub_window.view->fitInView(zoomObject, Qt::KeepAspectRatio);

		// Force update scroll bars
	//_sub_window.view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	//_sub_window.view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	
   //updateZoomPercentage();

   //clickCursor();
   //emit resetZoomToolBar();
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::zoomInRect(QRectF zoomRect, QGraphicsSceneMouseEvent* event)
{
    //int zoomRecPer = 500 - wp;
    int zoomWidth = zoomRect.normalized().width();
    int zoomHeight = zoomRect.normalized().height();
    // Zoom in

    
    if ((!zoomRect.isEmpty() || !zoomRect.normalized().isEmpty()) && (zoomWidth > 10 && zoomHeight > 10))
    {
        /*
        qreal scale = 1.0;
        if(_pixmaps.size() > 0)
        {
            scale = _pixmaps[0]->scale();
        }
        scale *= 2.0;
        for(auto itr : _pixmaps)
        {
            itr->setScale(scale);

        }
        */
        //_sub_window.view->fitInView(QRectF(_sub_window.view->mapToScene(zoomRect.topLeft().toPoint()), _sub_window.view->mapToScene(zoomRect.bottomRight().toPoint())), Qt::KeepAspectRatio);
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
        if(_pixmaps.empty())
        {
            return;
        }
        if(_cur_scale * 2.0 > 8.0)
        {
            return;
        }

        QPointF anchorItemPos;
        QGraphicsItem *item = (event != nullptr) ? _sub_window.scene->itemAt(event->scenePos(), QTransform()) : nullptr;
        ClickablePixmapItem* selectedItem = qgraphicsitem_cast<ClickablePixmapItem*>(item);
        if (selectedItem)
        {
            anchorItemPos = selectedItem->last_local_intersection_point();
        }
        else
        {
            QRectF bbox = _pixmaps[0]->boundingRect();
            anchorItemPos = QPointF(bbox.width() * 0.5, bbox.height() * 0.5);
        }

        QTransform transform;
        transform.translate(anchorItemPos.x(), anchorItemPos.y());
        transform.scale(2.0, 2.0);
        transform.translate(-anchorItemPos.x(), -anchorItemPos.y());

        for(auto itr : _pixmaps)
        {
            itr->setTransform(itr->transform() * transform);
        }

        _cur_scale *= 2.0;
    }

    updateMinMaxLabels();
    updateScaleBar();
    updateZoomPercentage();
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::zoomOut(QGraphicsSceneMouseEvent* event)
{
    if(_pixmaps.empty())
    {
        return;
    }
    if(_cur_scale * 0.5 < 1.0)
    {
        if(_cur_scale != 1.0)
        {
            for(auto itr : _pixmaps)
            {
                itr->setTransform(QTransform());
            }
            _cur_scale = 1.0;
            updateMinMaxLabels();
            updateScaleBar();
            updateZoomPercentage();
        }
        return;
    }

    QPointF anchorItemPos;
    QGraphicsItem *item = (event != nullptr) ? _sub_window.scene->itemAt(event->scenePos(), QTransform()) : nullptr;
    ClickablePixmapItem* selectedItem = qgraphicsitem_cast<ClickablePixmapItem*>(item);
    if (selectedItem)
    {
        anchorItemPos = selectedItem->last_local_intersection_point();
    }
    else
    {
        QRectF bbox = _pixmaps[0]->boundingRect();
        anchorItemPos = QPointF(bbox.width() * 0.5, bbox.height() * 0.5);
    }

    QTransform transform;
    transform.translate(anchorItemPos.x(), anchorItemPos.y());
    transform.scale(0.5, 0.5);
    transform.translate(-anchorItemPos.x(), -anchorItemPos.y());
    for(auto itr : _pixmaps)
    {
        itr->setTransform(itr->transform() * transform);
    }

    _cur_scale *= 0.5;

    updateMinMaxLabels();
    updateScaleBar();
    updateZoomPercentage();
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::zoomValueChanged(int val)
{

    bool isOK = false;
    qreal value = m_zoomPercent->currentText().toFloat(&isOK);
    if (!isOK || value < 100 || value > 800) {
        m_zoomPercent->removeItem(m_zoomPercent->currentIndex());
        updateZoomPercentage();
        return;
    }
    if(_cur_scale == value *.01)
    {
        return;
    }
    _cur_scale = value * .01;
    
 
    /*

    for(auto itr : _pixmaps)
    {
        if(value == 1.0)
        {
            itr->setPos(QPointF(0, 0));
        }
        else
        {
            QRectF bbox = _pixmaps[0]->boundingRect();
            // 100 x 100 
            qreal nx = -(bbox.width() / value) / 2.0;
            qreal ny = -(bbox.height() / value) / 2.0;
            QPointF npos = QPointF(nx, ny);
            
            itr->setPos(npos);
        }
        itr->setScale(value);

    }
    */
    if(_pixmaps.size() > 0)
    {   
        QPointF anchorItemPos = QPointF(0,0);
        QTransform transform;
        transform.translate(anchorItemPos.x(), anchorItemPos.y());
        transform.scale(_cur_scale, _cur_scale);
        transform.translate(-anchorItemPos.x(), -anchorItemPos.y());
        for(auto itr : _pixmaps)
        {
            itr->setTransform(transform);
        }
        updateMinMaxLabels();
        updateScaleBar();
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::updateMinMaxLabels()
{
    if(_pixmaps.size() > 0 && _raw_data_items.count(0) > 0)
    {
        QRectF maskRect = rect();
        const QPixmap& pixmap = _pixmaps[0]->pixmap();
        
        QTransform maskToPixmap = _pixmaps[0]->sceneTransform().inverted();
        QRectF visibleRectInPixmap = maskToPixmap.mapRect(_clip_pixmaps[0]->mapRectToScene(maskRect));
        QRectF pixmapBounds(0, 0, pixmap.width(), pixmap.height());
        visibleRectInPixmap = visibleRectInPixmap.intersected(pixmapBounds);
        
        unsigned int start_row = qMax(0, (int)qFloor(visibleRectInPixmap.top()));
        unsigned int end_row = qMin(pixmap.height() - 1, (int)qCeil(visibleRectInPixmap.bottom()));
        unsigned int start_col = qMax(0, (int)qFloor(visibleRectInPixmap.left()));
        unsigned int end_rcol = qMin(pixmap.width() - 1, (int)qCeil(visibleRectInPixmap.right()));
        
/*
        QPainterPath parentClipPath = _clip_pixmaps[0]->shape();
        QPainterPath mappedClipPath = _pixmaps[0]->mapFromItem(_clip_pixmaps[0], parentClipPath);
        QRectF childRect = _pixmaps[0]->sceneBoundingRect();


        QPainterPath childPath;
        childPath.addRect(childRect); // or QPainterPath(childRect)

        QPainterPath intersectionPath = mappedClipPath.intersected(childPath);
        //QPainterPath intersectionPath = mappedClipPath.intersected(QPainterPath(childRect));

        // The bounding rect of the intersection path is the visible/unclipped area
        QRectF bbox = intersectionPath.boundingRect();
        //logI<<bbox.x()<<" : "<<bbox.y()<<" : "<<bbox.width()<<" : "<<bbox.height()<<"\n";


        unsigned int start_row = std::max((unsigned int)bbox.y(), (unsigned int)0);
        unsigned int end_row = std::min((unsigned int)bbox.height(), (unsigned int)_raw_data_items[0].rows());
        unsigned int start_col = std::max((unsigned int)bbox.x(), (unsigned int)0);
        unsigned int end_rcol = std::min((unsigned int)bbox.width(), (unsigned int)_raw_data_items[0].cols());
*/
        for(unsigned int idx = 0; idx < _pixmaps.size(); idx++)
        {
            if(_raw_data_items.count(idx) > 0)
            {
                float counts_max = std::numeric_limits<float>::min();
                float counts_min = std::numeric_limits<float>::max();
                for(unsigned int r = start_row; r < end_row; r++)
                {
                    for(unsigned int c = start_col; c < end_rcol; c++)
                    {
                        float val =_raw_data_items[idx](r,c);
                        counts_max = std::max(val, counts_max);
                        counts_min = std::min(val, counts_min);
                    }
                }
                
                QString minStr = "min: "+ QString::number(counts_min);
                QString maxStr = "max: "+ QString::number(counts_max);
                _min_textitems[idx]->setPlainText(minStr);
                _max_textitems[idx]->setPlainText(maxStr);
                int row = (int)idx / _grid_cols;
                int col = (int)idx % _grid_cols;
                layoutCellLabels((int)idx, row, col);
            }
        }
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::updateScaleBar()
{
    if(_scale_bar_line == nullptr || _scale_bar_text == nullptr)
    {
        return;
    }
    if(_pixmaps.empty())
    {
        return;
    }
    const QPixmap& pixmap = _pixmaps[0]->pixmap();
    if(pixmap.isNull() || pixmap.width() <= 0)
    {
        return;
    }

    // The rendered pixmap may be wider than the underlying data array — MapsH5Model::gen_pixmap
    // appends a colormap legend strip on the right. The scale bar describes data pixels (what the
    // user counts on the image), so use the raw data column count when available and convert back
    // to scene units via the pixmap-to-data ratio.
    int data_cols = pixmap.width();
    if(_raw_data_items.count(0) > 0 && _raw_data_items.at(0).cols() > 0)
    {
        data_cols = (int)_raw_data_items.at(0).cols();
    }

    /*
    int target = std::max(1, data_cols / 5);
    int magnitude = (int)std::pow(10.0, std::floor(std::log10((double)data_cols / 5.0)));
    if(magnitude < 1) magnitude = 1;
    int nice_pixels = (target / magnitude) * magnitude;
    if(nice_pixels < 1) nice_pixels = 1;
  */
 
    double target_pixels = (double)data_cols / 5.0;
    double mag = std::pow(10.0, std::floor(std::log10(target_pixels)));
    double frac = target_pixels / mag;
    double nice;
    if (frac < 1.5)      nice = 1.0;
    else if (frac < 3.5) nice = 2.0;
    else if (frac < 7.5) nice = 5.0;
    else                 nice = 10.0;
    double nice_units = nice * mag;
    double nice_pixels = nice_units;
/*
    qreal margin = pix.width() * 0.02;
    qreal bar_x = pix.left() + margin;
    qreal bar_y = pix.bottom() - margin;
*/

    // scene_per_data_px = (qreal)pixmap.width() / (qreal)data_cols;
    // if(nice_pixels <= 4)
    // {
    //     scene_per_data_px = 0.4;
    // }
    //qreal bar_length = (qreal)nice_pixels * scene_per_data_px * _cur_scale;
    qreal bar_x = 2.0;
    qreal bar_y = (qreal)_grid_rows * _spacer_height + 4.0;

    if(m_coordWidget->model() != nullptr)
    {
        double outX0, outX1, outY, outZ;
        m_coordWidget->model()->runTransformer(0.0, 0.0, 0.0, &outX0, &outY, &outZ);
        m_coordWidget->model()->runTransformer(nice_pixels, 0.0, 0.0, &outX1, &outY, &outZ);
        // reuse outY since outY and outZ not needed.
        outY = std::abs(outX1 - outX0);
        _scale_bar_text->setPlainText(QString("%1 um").arg(outY));
    }
    else
    {
        _scale_bar_text->setPlainText(QString("%1 px").arg(nice_pixels));
    }

    _scale_bar_line->setLine(bar_x, bar_y, bar_x + nice_pixels, bar_y);

    //_scale_bar_line->setLine(bar_x, bar_y, bar_x + bar_length, bar_y);


    _scale_bar_text->setPos(bar_x, bar_y + 1.0);
    _scale_bar_line->setVisible(true);
    _scale_bar_text->setVisible(true);
}

//---------------------------------------------------------------------------

qreal ImageViewWidgetCompact::computeLabelAreaHeight()
{
    if(_el_textitems.empty())
    {
        return 0.0;
    }

    const qreal padding = 4.0;
    qreal max_h = 0.0;

    for(size_t n = 0; n < _el_textitems.size(); n++)
    {
        _el_textitems[n]->setScale(1.0);
        _min_textitems[n]->setScale(1.0);
        _max_textitems[n]->setScale(1.0);
        _unit_textitems[n]->setScale(1.0);
        _el_textitems[n]->adjustSize();
        _min_textitems[n]->adjustSize();
        _max_textitems[n]->adjustSize();
        _unit_textitems[n]->adjustSize();

        qreal el_w = _el_textitems[n]->boundingRect().width();
        qreal min_w = _min_textitems[n]->boundingRect().width();
        qreal max_w = _max_textitems[n]->boundingRect().width();
        qreal unit_w = _unit_textitems[n]->boundingRect().width();
        qreal min_max_w = std::max(min_w, max_w);
        qreal needed_w = el_w + padding + min_max_w + padding + unit_w;

        qreal scale = 1.0;
        if(needed_w > _spacer_width && needed_w > 0)
        {
            scale = (qreal)_spacer_width / needed_w;
        }

        qreal el_h = _el_textitems[n]->boundingRect().height();
        qreal min_h = _min_textitems[n]->boundingRect().height();
        qreal max_h_text = _max_textitems[n]->boundingRect().height();
        qreal min_max_stack_h = (min_h * 0.55) + max_h_text;
        qreal cell_label_h = std::max(el_h, min_max_stack_h) * scale;
        max_h = std::max(max_h, cell_label_h);
    }

    return max_h;
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::layoutCellLabels(int n, int row, int col)
{
    if(n < 0 || n >= (int)_el_textitems.size())
    {
        return;
    }

    _el_textitems[n]->setScale(1.0);
    _min_textitems[n]->setScale(1.0);
    _max_textitems[n]->setScale(1.0);
    _unit_textitems[n]->setScale(1.0);

    _el_textitems[n]->adjustSize();
    _min_textitems[n]->adjustSize();
    _max_textitems[n]->adjustSize();
    _unit_textitems[n]->adjustSize();

    qreal el_w = _el_textitems[n]->boundingRect().width();
    qreal min_w = _min_textitems[n]->boundingRect().width();
    qreal max_w = _max_textitems[n]->boundingRect().width();
    qreal unit_w = _unit_textitems[n]->boundingRect().width();
    qreal min_max_w = std::max(min_w, max_w);

    const qreal padding = 4.0;
    qreal needed_w = el_w + padding + min_max_w + padding + unit_w;

    qreal scale = 1.0;
    if(needed_w > _spacer_width && needed_w > 0)
    {
        scale = (qreal)_spacer_width / needed_w;
    }

    _el_textitems[n]->setScale(scale);
    _min_textitems[n]->setScale(scale);
    _max_textitems[n]->setScale(scale);
    _unit_textitems[n]->setScale(scale);

    qreal cell_x = (qreal)col * _spacer_width;
    qreal cell_y = (qreal)row * _spacer_height;
    qreal scaled_padding = padding * scale;
    qreal scaled_el_w = el_w * scale;
    qreal scaled_min_max_w = min_max_w * scale;
    qreal scaled_min_h = _min_textitems[n]->boundingRect().height() * scale;

    _el_textitems[n]->setPos(cell_x, cell_y);
    qreal mid_x = cell_x + scaled_el_w + scaled_padding;
    _min_textitems[n]->setPos(mid_x, cell_y);
    _max_textitems[n]->setPos(mid_x, cell_y + scaled_min_h * 0.55);
    _unit_textitems[n]->setPos(mid_x + scaled_min_max_w + scaled_padding, cell_y);
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
    if(idx < _min_textitems.size())
    {
        _raw_data_items[idx] = ArrayXXr<float>(normalized);
        QString minStr = "min: "+ QString::number(normalized.minCoeff());
        QString maxStr = "max: "+ QString::number(normalized.maxCoeff());
        _min_textitems[idx]->setPlainText(minStr);
        _max_textitems[idx]->setPlainText(maxStr);
        int row = (int)idx / _grid_cols;
        int col = (int)idx % _grid_cols;
        layoutCellLabels((int)idx, row, col);
    }
}

//---------------------------------------------------------------------------

std::vector<QString> ImageViewWidgetCompact::getLabelList()
{
    std::vector<QString> label_list;
    for(int i=0; i< _sub_window.cb_image_label->count(); i++)
    { 
        label_list.push_back(_sub_window.cb_image_label->itemText(i));
    }
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

    //_sub_window.counts_coord_widget->setCoordinate(0, 0, 0);
    //_sub_window.counts_stats_widget->setCoordinate(0, 0, 0);

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