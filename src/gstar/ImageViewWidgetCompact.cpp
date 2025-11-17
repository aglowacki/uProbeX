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
    _height_offset = 20.0;
    _sub_window.scene->removeDefaultPixmap();
    _sub_window.scene->setItemOffset(QPointF(0.0f,_height_offset)); 
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
   
    _pixmaps.clear();
    _clip_pixmaps.clear();
    _el_textitems.clear();
    _min_textitems.clear();
    _max_textitems.clear();
    _unit_textitems.clear();
    _raw_data_items.clear();
    _sub_window.scene->clear();

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
            _el_textitems.back()->setDefaultTextColor(Qt::white);
            _el_textitems.back()->setFont(_element_font);

            _min_textitems.back()->setPos(j * _spacer_width + 28, i * _spacer_height);
            _min_textitems.back()->setDefaultTextColor(Qt::white);
            _min_textitems.back()->setFont(_min_max_font);

            _max_textitems.back()->setPos(j * _spacer_width + 28, i * _spacer_height + 8);
            _max_textitems.back()->setDefaultTextColor(Qt::white);
            _max_textitems.back()->setFont(_min_max_font);

            _unit_textitems.back()->setPos(j * _spacer_width + 48, i * _spacer_height + 8);
            _unit_textitems.back()->setDefaultTextColor(Qt::white);
            _unit_textitems.back()->setFont(_min_max_font);

            //_sub_window.scene->addItem(_pixmaps.back());
            _sub_window.scene->addItem(_clip_pixmaps.back());
            _sub_window.scene->addItem(_el_textitems.back());
            _sub_window.scene->addItem(_min_textitems.back());
            _sub_window.scene->addItem(_max_textitems.back());
            _sub_window.scene->addItem(_unit_textitems.back());
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
            _spacer_height = p.height() + _height_offset;
            //_element_font.setPointSize(4); // TODO: calc from size of map
            int n = 0;
            for (int i = 0; i < _grid_rows; i++)
            {
                for (int j = 0; j < _grid_cols; j++)
                {
                    //_pixmaps[n]->setPos(j * _spacer_width, (i * _spacer_height) + _height_offset);
                    _clip_pixmaps[n]->setPos(j * _spacer_width, (i * _spacer_height) + _height_offset);
                    _el_textitems[n]->setPlainText(_sub_window.cb_image_label->itemText(n));
                    _el_textitems[n]->adjustSize();
                    
                    if(p.width() < 40)
                    {
                        _el_textitems[n]->setScale(.3);
                        _min_textitems[n]->setScale(.3);
                        _max_textitems[n]->setScale(.3);
                        _unit_textitems[n]->setScale(.3);

                        _el_textitems[n]->setPos(j * _spacer_width, (i * _spacer_height) );
                        _min_textitems[n]->setPos(j * _spacer_width + 8, i * _spacer_height);
                        _max_textitems[n]->setPos(j * _spacer_width + 8, i * _spacer_height + 2);
                        _unit_textitems[n]->setPos(j * _spacer_width + 28, i * _spacer_height + 2);
                    }
                    else
                    {
                        _el_textitems[n]->setScale(1.0);
                        _min_textitems[n]->setScale(1.0);
                        _max_textitems[n]->setScale(1.0);
                        _unit_textitems[n]->setScale(1.0);

                        _el_textitems[n]->setPos(j * _spacer_width, (i * _spacer_height) );
                        _min_textitems[n]->setPos(j * _spacer_width + 28, i * _spacer_height);
                        _max_textitems[n]->setPos(j * _spacer_width + 28, i * _spacer_height + 8);
                        _unit_textitems[n]->setPos(j * _spacer_width + 88, i * _spacer_height + 8);
                    }
                    n++;
                }
            }
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
        _unit_textitems[i]->adjustSize();
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
        _unit_textitems[i]->adjustSize();
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
    qreal wp = 0;
    if(_pixmaps.size() > 0)
    {
        wp = _pixmaps[0]->scale();
        wp *= 100;
    }
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

    qreal wp = getCurrentZoomPercent();
    if (wp >= 800) return;

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
        
        qreal scale = 1.0;
        if(_pixmaps.size() > 0)
        {
            scale = _pixmaps[0]->scale();
            scale *= 2.0;
            if(scale > 8.0)
            {
                scale = 8.0;
            }


            QGraphicsItem *item = _sub_window.scene->itemAt(event->scenePos(), QTransform());
            ClickablePixmapItem* selectedItem = qgraphicsitem_cast<ClickablePixmapItem*>(item);
            if (selectedItem) 
            {
                QPointF seletionPoint = selectedItem->last_local_intersection_point();
               // qDebug() << "Item clicked in the view, scene pos:" << selectionPoint<<"\n";
            
                QRectF bbox = _pixmaps[0]->boundingRect();
                qreal transX = ( (bbox.width() * 0.5) - seletionPoint.x() );
                qreal transY = ( (bbox.height() * 0.5) - seletionPoint.y() );
                
                if(transX > 0 && transX < (bbox.width() * 0.5))
                {
                    transX = 0;
                }
                if(transY > 0 && transY < (bbox.height() * 0.5))
                {
                    transY = 0;
                }
                 qDebug() << "box: " << bbox<<" : "<< seletionPoint << " :: "<<transX<<" "<<transY<<"\n";
                    _pixmaps[0]->setPos(transX, transY);
                    _pixmaps[0]->setScale(scale);
                
                /*
                for(auto itr : _pixmaps)
                {
                    itr->setPos(transX, transY);
                    itr->setScale(scale);
                }
                    */
            }
        }   
    }
    
    // Force update scroll bars
    //_sub_window.view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    //_sub_window.view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    updateMinMaxLabels();
    updateZoomPercentage();
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::zoomOut()
{

    qreal wp = getCurrentZoomPercent();

    if (wp <= 12.5) return;

    qreal scale = 1.0;
    QPointF npos;
    if(_pixmaps.size() > 0)
    {
        npos = _pixmaps[0]->pos();
        scale = _pixmaps[0]->scale();
    }
    scale *= 0.5;
    if(scale < 1.0)
    {
        scale = 1.0;
    }
    
    if(scale == 1.0)
    {
        npos = QPointF(0,0);
    }

    for(auto itr : _pixmaps)
    {
        itr->setPos(npos);
        itr->setScale(scale);

    }
    //_sub_window.view->scale(1.0, 1.0);

    // Force update scroll bars
    //_sub_window.view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    //_sub_window.view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    updateMinMaxLabels();
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

    value *= .01;
    
    /*
    QTransform t = _sub_window.view->transform();
    QRectF image = _sub_window.scene->pixRect();
    QRectF tImage = t.mapRect(image);

    qreal sx = (value / 100 * image.width()) / tImage.width();
    qreal sy = (value / 100 * image.height()) / tImage.height();

    qreal s = sx;
    if (sy < sx) s = sy;
    */
    

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
    updateMinMaxLabels();
    //_sub_window.view->scale(s, s);
    
}

//---------------------------------------------------------------------------

void ImageViewWidgetCompact::updateMinMaxLabels()
{
    if(_pixmaps.size() > 0 && _raw_data_items.count(0) > 0)
    {
        QPainterPath parentClipPath = _clip_pixmaps[0]->shape();

        // 2. Map the parent's clip path from parent coordinates to the child's local coordinates
        QPainterPath mappedClipPath = _pixmaps[0]->mapFromItem(_clip_pixmaps[0], parentClipPath);

        // 3. Get the child's bounding rectangle in child coordinates
        QRectF childRect = _pixmaps[0]->boundingRect();

        QPainterPath childPath;
        childPath.addRect(childRect); // or QPainterPath(childRect)

        QPainterPath intersectionPath = mappedClipPath.intersected(childPath);
        // 4. Calculate the intersection of the child's bounding rect and the mapped clip path
        //QPainterPath intersectionPath = mappedClipPath.intersected(QPainterPath(childRect));

        // The bounding rect of the intersection path is the visible/unclipped area
        QRectF bbox = intersectionPath.boundingRect();
        //logI<<bbox.x()<<" : "<<bbox.y()<<" : "<<bbox.width()<<" : "<<bbox.height()<<"\n";


        unsigned int start_row = std::max((unsigned int)bbox.y(), (unsigned int)0);
        unsigned int end_row = std::min((unsigned int)bbox.height(), (unsigned int)_raw_data_items[0].rows());
        unsigned int start_col = std::max((unsigned int)bbox.x(), (unsigned int)0);
        unsigned int end_rcol = std::min((unsigned int)bbox.width(), (unsigned int)_raw_data_items[0].cols());

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
                _min_textitems[idx]->adjustSize();
                _max_textitems[idx]->adjustSize();
            }
        }
    }
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
        _min_textitems[idx]->adjustSize();
        _max_textitems[idx]->adjustSize();
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