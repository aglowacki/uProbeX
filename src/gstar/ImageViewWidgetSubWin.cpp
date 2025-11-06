/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/ImageViewWidgetSubWin.h"
#include <QSpacerItem>
using namespace gstar;

//---------------------------------------------------------------------------

ImageViewWidgetSubWin::ImageViewWidgetSubWin(int rows, int cols , QWidget* parent) : ImageViewWidget(rows, cols, parent)
{
   // Create main layout and add widgets
    createSceneAndView(rows,cols);
    createLayout();

}

//---------------------------------------------------------------------------

ImageViewWidgetSubWin::~ImageViewWidgetSubWin()
{

    _sub_windows.clear();

}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::setGlobalContrast(bool val)
{
    for (auto& itr : _sub_windows)
    {
        itr.btn_contrast->setVisible(val);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::clickCursor()
{

   // Set scene mode
    for (auto& itr : _sub_windows)
	{
		itr.scene->setZoomModeToNone();
        itr.view->viewport()->setCursor(Qt::ArrowCursor);
	}

}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::customCursor(QCursor cursor)
{
    for (auto& itr : _sub_windows)
    {
        itr.view->viewport()->setCursor(cursor);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::setSelectorVisible(bool val)
{
    for (auto& itr : _sub_windows)
    {
        itr.setImageLabelVisible(val);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::setCountsVisible(bool val)
{
    for (auto& itr : _sub_windows)
    {
        itr.setCountsVisible(val);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::clickFill()
{
   
    // Get out of zoom mode
    clickCursor();

    for (auto& itr : _sub_windows)
    {
        // Set scene mode
        itr.scene->setZoomModeToFit();

        // Set regular cursor
        itr.view->viewport()->setCursor(Qt::ArrowCursor);
    }

    resizeEvent(nullptr);
    m_zoomPercent->setCurrentIndex(-1);

}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::clickZoomIn()
{

    for (auto& itr : _sub_windows)
    {
        itr.scene->setZoomModeToZoomIn();
        itr.view->viewport()->setCursor(m_zoomInCursor);
    }

}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::clickZoomOriginal()
{

    for (auto& itr : _sub_windows)
	{
		itr.view->resetTransform();
		itr.view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		itr.view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}
  

   updateZoomPercentage();

}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::clickZoomOut()
{

    // Set zoom out mode
    for (auto& itr : _sub_windows)
    {
        itr.scene->setZoomModeToZoomOut();
        itr.view->viewport()->setCursor(m_zoomOutCursor);
    }

}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::createLayout()
{

    // Layout
    _main_layout = new QVBoxLayout();
    _main_layout->setSpacing(0);
    _main_layout->setContentsMargins(0, 0, 0, 0);

    _image_view_grid_layout = new QGridLayout();
    _image_view_grid_layout->setContentsMargins(0, 0, 0, 0);
    _image_view_grid_layout->setVerticalSpacing(0);
    _image_view_grid_layout->setHorizontalSpacing(0);
   
    for (int i = 0; i < _grid_rows; i++)
    {
        //QHBoxLayout *new_row = new QHBoxLayout();
        //new_row->setContentsMargins(0, 0, 0, 0);
        //_main_layout->addItem(new_row);
        for (int j = 0; j < _grid_cols; j++)
        {
            int idx = (i * _grid_cols) + j;
            //_cb_image_label
            //new_row->addItem(_sub_windows[idx].layout);
            _image_view_grid_layout->addItem(_sub_windows[idx].layout, i, j);
        }
        //new_row->addItem(new QSpacerItem(1000,10,QSizePolicy::Maximum));
    }

    _main_layout->addItem(_image_view_grid_layout);
    if(m_coordWidget == nullptr)
    {
        m_coordWidget = new CoordinateWidget();
    }
    _main_layout->addWidget(m_coordWidget);


    // Set widget's layout
    setLayout(_main_layout);

}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::createSceneAndView(int rows, int cols)
{
	_grid_rows = rows;
	_grid_cols = cols;

    _sub_windows.resize(_grid_rows * _grid_cols);

	for (auto &itr : _sub_windows)
	{
		connect(itr.scene, &ImageViewScene::zoomIn, this, &ImageViewWidgetSubWin::zoomIn);
		connect(itr.scene, &ImageViewScene::zoomInRect, this, &ImageViewWidgetSubWin::zoomInRect);
		connect(itr.scene, &ImageViewScene::zoomOut, this, &ImageViewWidgetSubWin::zoomOut);
		//connect(itr.scene, &ImageViewScene::sceneRectChanged, this, &ImageViewWidgetSubWin::sceneRectUpdated);
        connect(itr.scene, &ImageViewScene::onMouseMoveEvent, this, &ImageViewWidgetSubWin::onMouseMoveEvent);
       
        connect(itr.cb_image_label, &QComboBox::currentTextChanged, this, &ImageViewWidgetSubWin::onComboBoxChange);

		connect(&itr, &SubImageWindow::redraw_event, this, &ImageViewWidgetSubWin::subwindow_redraw);
	}
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::setUnitLabel(int idx, QString label)
{
    if (idx > -1 && idx < _sub_windows.size())
    {
        _sub_windows[idx].counts_coord_widget->setUnitsLabel(label);
       // _sub_windows[idx].counts_stats_widget->setUnitsLabel(label);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::setUnitLabels(QString label)
{
    for (auto& itr : _sub_windows)
    {
        itr.counts_coord_widget->setUnitsLabel(label);
        //itr.counts_stats_widget->setUnitsLabel(label);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::newGridLayout(int rows, int cols)
{

	for (auto &itr : _sub_windows)
	{
		disconnect(itr.scene, &ImageViewScene::zoomIn, this, &ImageViewWidgetSubWin::zoomIn);
		disconnect(itr.scene, &ImageViewScene::zoomInRect, this, &ImageViewWidgetSubWin::zoomInRect);
		disconnect(itr.scene, &ImageViewScene::zoomOut, this, &ImageViewWidgetSubWin::zoomOut);
		//disconnect(itr.scene, &ImageViewScene::sceneRectChanged, this, &ImageViewWidgetSubWin::sceneRectUpdated);
		disconnect(itr.scene, &ImageViewScene::onMouseMoveEvent, this, &ImageViewWidgetSubWin::onMouseMoveEvent);
		disconnect(itr.cb_image_label, &QComboBox::currentTextChanged, this, &ImageViewWidgetSubWin::onComboBoxChange);
		disconnect(&itr, &SubImageWindow::redraw_event, this, &ImageViewWidgetSubWin::subwindow_redraw);
	}
    _sub_windows.clear();

    delete _main_layout;
    //create new layout
    createSceneAndView(rows, cols);
    createLayout();
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::subwindow_redraw(SubImageWindow* win)
{
    int idx = 0;
    for (auto &itr : _sub_windows)
	{
        if (&itr == win)
        {
            emit parent_redraw(idx);
            break;
        }
        idx ++;
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::onComboBoxChange(QString lbl)
{
    QObject* obj = sender();
    for(int i =0; i< _sub_windows.size(); i++)
    {
        if(_sub_windows[i].cb_image_label == obj)
        {
            emit cbLabelChanged(lbl, i);
            break;
        }
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::clearLabels()
{
    for(auto &itr : _sub_windows)
    {
        itr.cb_image_label->clear();
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::addLabel(QString lbl)
{
    for(auto &itr : _sub_windows)
    {
        itr.cb_image_label->addItem(lbl);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::setLabel(QString lbl)
{
    for(auto &itr : _sub_windows)
    {
        itr.cb_image_label->setCurrentText(lbl);
    }
}

//---------------------------------------------------------------------------

qreal ImageViewWidgetSubWin::getCurrentZoomPercent()
{
    qreal wp = 0;
    if (_sub_windows.size() > 0)
    {
        QTransform t = _sub_windows[0].view->transform();
        QRectF tImage = t.mapRect(_sub_windows[0].scene->pixRect());

        wp = tImage.width() / _sub_windows[0].scene->pixRect().width() * 100.0;
    }
    return wp;
}

//---------------------------------------------------------------------------

QPointF ImageViewWidgetSubWin::getCenterPoint() const
{
    const QRectF centerRect;
    if (_sub_windows.size() > 0)
    {
        return _sub_windows[0].view->mapToScene(_sub_windows[0].view->viewport()->geometry()).boundingRect().center();
    }
    return centerRect.center();

}

//---------------------------------------------------------------------------

QRectF ImageViewWidgetSubWin::getSceneRect()
{  
    const QRectF rect;
    if (_sub_windows.size() > 0)
    {
        // Get image size
        return _sub_windows[0].scene->sceneRect();
    }
    return rect;
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::onMouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    int x = event->scenePos().x();
    int y = event->scenePos().y();
    m_coordWidget -> setCoordinate(x,y);
    for(auto &itr : _sub_windows)
    {
        itr.counts_coord_widget->setCoordinate(x, y, 0);
    }

}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::resizeEvent(QResizeEvent* event)
{

   Q_UNUSED(event)
   
   if (_sub_windows.size() > 0)
   {
       // Get image size
       QRectF r(0, 0, (_sub_windows[0].scene->sceneRect()).width(), (_sub_windows[0].scene->sceneRect()).height());
       for (auto& itr : _sub_windows)
       {
           itr.view->fitInView(r, Qt::KeepAspectRatio);
           itr.view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
           itr.view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
       }
   }
   updateZoomPercentage();
   update();
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::connectRoiGraphicsItemToMouseEvents(RoiMaskGraphicsItem* roi)
{
    connect(_sub_windows[0].scene, &ImageViewScene::onMousePressEvent, roi, &RoiMaskGraphicsItem::onMousePressEvent);
    connect(_sub_windows[0].scene, &ImageViewScene::onMouseMoveEvent, roi, &RoiMaskGraphicsItem::onMouseMoveEvent);
    connect(_sub_windows[0].scene, &ImageViewScene::onMouseReleaseEvent, roi, &RoiMaskGraphicsItem::onMouseReleaseEvent);
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::disconnectRoiGraphicsItemToMouseEvents(RoiMaskGraphicsItem* roi)
{
    disconnect(_sub_windows[0].scene, &ImageViewScene::onMousePressEvent, roi, &RoiMaskGraphicsItem::onMousePressEvent);
    disconnect(_sub_windows[0].scene, &ImageViewScene::onMouseMoveEvent, roi, &RoiMaskGraphicsItem::onMouseMoveEvent);
    disconnect(_sub_windows[0].scene, &ImageViewScene::onMouseReleaseEvent, roi, &RoiMaskGraphicsItem::onMouseReleaseEvent);
}
//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::setSceneModel(QAbstractItemModel* model)
{
    bool multi_win =  (_sub_windows.size() > 1) ? true : false;
    for (auto& itr : _sub_windows)
    {
        itr.scene->setModel(model, multi_win);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::setSceneSelectionModel(QItemSelectionModel* selectionModel)
{
    for (auto& itr : _sub_windows)
    {
        itr.scene->setSelectionModel(selectionModel);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::setSceneModelAndSelection(QAbstractItemModel* model, QItemSelectionModel* selectionModel)
{
    bool multi_win = (_sub_windows.size() > 1) ? true : false;
    for (auto& itr : _sub_windows)
    {
        itr.scene->setModel(model, multi_win);
        itr.scene->setSelectionModel(selectionModel);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::sceneEnableAnnotations(bool state)
{
    for (auto& itr : _sub_windows)
    {
        itr.scene->enableAnnotations(state);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::setSceneUnitsLabel(QString label)
{
    for (auto& itr : _sub_windows)
    {
        itr.scene->setUnitsLabel(label);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::setSceneUnitsPerPixelX(double val)
{
    for (auto& itr : _sub_windows)
    {
        itr.scene->setUnitsPerPixelX(val);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::setSceneUnitsPerPixelY(double val)
{
    for (auto& itr : _sub_windows)
    {
        itr.scene->setUnitsPerPixelY(val);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::sceneUpdateModel()
{
    for (auto& itr : _sub_windows)
    {
        itr.scene->updateModel();
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::setScenePixmap(const QPixmap& p)
{
    for (auto& itr : _sub_windows)
    {
        itr.scene->setPixmap(p);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::setSubScenePixmap(int idx, const QPixmap& p)
{
    if(idx > -1 && idx < _sub_windows.size())
    {
        _sub_windows[idx].scene->setPixmap(p);
    }
}

//---------------------------------------------------------------------------

QGraphicsView* ImageViewWidgetSubWin::view()
{

    if (_sub_windows.size() > 0)
    {
        return _sub_windows[0].view;
    }
    // Return current scene
    return nullptr;

}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::zoomIn(QGraphicsItem* zoomObject)
{
	for (auto & itr : _sub_windows)
	{
		itr.view->fitInView(zoomObject, Qt::KeepAspectRatio);

		// Force update scroll bars
		itr.view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		itr.view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}
   updateZoomPercentage();

   clickCursor();
   emit resetZoomToolBar();
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::zoomInRect(QRectF zoomRect, QGraphicsSceneMouseEvent* event)
{

    qreal wp = getCurrentZoomPercent();
    if (wp >= 800) return;

    //int zoomRecPer = 500 - wp;
    int zoomWidth = zoomRect.normalized().width();
    int zoomHeight = zoomRect.normalized().height();
    // Zoom in

    for (auto& itr : _sub_windows)
    {
        if ((!zoomRect.isEmpty() || !zoomRect.normalized().isEmpty()) && (zoomWidth > 10 && zoomHeight > 10))
        {
            
            itr.view->fitInView(QRectF(itr.view->mapToScene(zoomRect.topLeft().toPoint()), itr.view->mapToScene(zoomRect.bottomRight().toPoint())), Qt::KeepAspectRatio);
            
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
            itr.view->scale(1.50, 1.50);
            itr.view->centerOn(event->lastScenePos());
            
        }
        
        // Force update scroll bars
        itr.view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        itr.view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        
    }

   updateZoomPercentage();

}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::zoomOut()
{

   qreal wp = getCurrentZoomPercent();

   if (wp <= 12.5) return;
   for (auto & itr : _sub_windows)
   {
	   itr.view->scale(.66, .66);

	   // Force update scroll bars
	   itr.view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	   itr.view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   }
   updateZoomPercentage();

}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::zoomValueChanged(int val)
{

   bool isOK = false;
   float value = m_zoomPercent->currentText().toFloat(&isOK);
   if (!isOK || value < 12.5 || value > 800) {
      m_zoomPercent->removeItem(m_zoomPercent->currentIndex());
      updateZoomPercentage();
      return;
   }

   if (_sub_windows.size() > 0)
   {
       QTransform t = _sub_windows[0].view->transform();
       QRectF image = _sub_windows[0].scene->pixRect();
       QRectF tImage = t.mapRect(image);

       qreal sx = (value / 100 * image.width()) / tImage.width();
       qreal sy = (value / 100 * image.height()) / tImage.height();

       qreal s = sx;
       if (sy < sx) s = sy;

       for (auto& itr : _sub_windows)
       {
           itr.view->scale(s, s);
       }
   }
}

//---------------------------------------------------------------------------

QString ImageViewWidgetSubWin::getLabelAt(int idx)
{
    if(idx < _sub_windows.size())
    {
        return _sub_windows[idx].cb_image_label->currentText();
    }
    return QString();
}

//---------------------------------------------------------------------------

 void ImageViewWidgetSubWin::getMouseTrasnformAt(int idx, CountsLookupTransformer** counts_lookup, CountsStatsTransformer** counts_stats)
{
    if(idx < _sub_windows.size())
    {
        *counts_lookup = _sub_windows[idx].counts_lookup;
        *counts_stats = _sub_windows[idx].counts_stats;
    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::setCountsTrasnformAt(unsigned int idx, const ArrayXXr<float>& normalized)
{
    if(idx < _sub_windows.size())
    {
        if(_sub_windows[idx].counts_lookup != nullptr)
        {
            _sub_windows[idx].counts_lookup->setCounts(normalized);
        }
        if(_sub_windows[idx].counts_stats != nullptr)
        {
            _sub_windows[idx].counts_stats->setCounts(normalized);
        }
        //_sub_windows[idx].on_update_min_max(normalized.minCoeff(), normalized.maxCoeff(), false);
    }
}

//---------------------------------------------------------------------------

std::vector<QString> ImageViewWidgetSubWin::getLabelList()
{
    std::vector<QString> label_list;
    for( auto& itr : _sub_windows)
    {
        label_list.push_back(itr.cb_image_label->currentText());
    }
    return label_list;
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::restoreLabels(const std::vector<QString>& labels)
{

    for(int i=0; i< _sub_windows.size(); i++)
    {
        bool found = false;
        QComboBox* cb = _sub_windows[i].cb_image_label;
        if(i < labels.size())
        {
            QString ilabel = labels[i];
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
            int cnt = cb->count();
            if(i < cnt)
            {
                cb->setCurrentText(cb->itemText(i));
            }
        }

    }
}

//---------------------------------------------------------------------------

void ImageViewWidgetSubWin::resetCoordsToZero()
{

    for( auto& itr : _sub_windows)
    {
        itr.counts_coord_widget->setCoordinate(0, 0, 0);
        itr.counts_stats_widget->setCoordinate(0, 0, 0);
    }
}

//---------------------------------------------------------------------------

bool ImageViewWidgetSubWin::getMinMaxAt(int grid_idx, float &counts_min, float &counts_max)
{
	if (grid_idx > -1 && grid_idx < _sub_windows.size())
	{
		if (_sub_windows[grid_idx].contrast_updated())
		{
			counts_min = _sub_windows[grid_idx].contrast_min();
			counts_max = _sub_windows[grid_idx].contrast_max();
            return true;
		}
	}
    return false;
}

//---------------------------------------------------------------------------