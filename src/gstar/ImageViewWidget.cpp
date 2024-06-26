/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/ImageViewWidget.h>

using namespace gstar;

//---------------------------------------------------------------------------

ImageViewWidget::ImageViewWidget(int rows, int cols , QWidget* parent)
: QWidget(parent), m_fillState(false)
{

   m_coordWidget = nullptr;

   // Create scene and view
   createSceneAndView(rows,cols);

   // Create main layout and add widgets
   createLayout();

   // Zoom in/out cursors
   m_zoomInCursor = QCursor(QPixmap(":/images/zoomin.png"));
   m_zoomOutCursor = QCursor(QPixmap(":/images/zoomout.png"));

   m_zoomPercent = nullptr;
   m_mouseLeaveState = true;
   set_null_mouse_pos = true;

/*   m_widget = parent;
   m_widget->installEventFilter(this);*/

}

//---------------------------------------------------------------------------

ImageViewWidget::~ImageViewWidget()
{

    _sub_windows.clear();

}

//---------------------------------------------------------------------------

bool ImageViewWidget::getMouseLeaveState()
{

   return m_mouseLeaveState;

}

//---------------------------------------------------------------------------

void ImageViewWidget::setGlobalContrast(bool val)
{
    for (auto& itr : _sub_windows)
    {
        itr.btn_contrast->setVisible(val);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidget::clickCursor()
{

   // Set scene mode
    for (auto& itr : _sub_windows)
	{
		itr.scene->setZoomModeToNone();
        itr.view->viewport()->setCursor(Qt::ArrowCursor);
	}

}

//---------------------------------------------------------------------------

void ImageViewWidget::customCursor(QCursor cursor)
{
    for (auto& itr : _sub_windows)
    {
        itr.view->viewport()->setCursor(cursor);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidget::setCoordsVisible(bool val)
{
    m_coordWidget->setVisible(val, val, val);
}

//---------------------------------------------------------------------------

void ImageViewWidget::setSelectorVisible(bool val)
{
    for (auto& itr : _sub_windows)
    {
        itr.setImageLabelVisible(val);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidget::setCountsVisible(bool val)
{
    for (auto& itr : _sub_windows)
    {
        itr.setCountsVisible(val);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidget::clickFill(bool checked)
{

    // State is used by the resizeEvent raised below.
    //  Make sure state is set before calling resizeEvent method.
    m_fillState = checked;
   
    // Get out of zoom mode
    clickCursor();

    for (auto& itr : _sub_windows)
    {
        // Set scene mode
        if (checked == true) 
        {
            itr.scene->setZoomModeToFit();
        }

        // Set regular cursor
        itr.view->viewport()->setCursor(Qt::ArrowCursor);
    }

    resizeEvent(nullptr);
    m_zoomPercent->setCurrentIndex(-1);

}

//---------------------------------------------------------------------------

void ImageViewWidget::clickZoomIn()
{

    for (auto& itr : _sub_windows)
    {
        itr.scene->setZoomModeToZoomIn();
        itr.view->viewport()->setCursor(m_zoomInCursor);
    }

}

//---------------------------------------------------------------------------

void ImageViewWidget::clickZoomOriginal()
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

void ImageViewWidget::clickZoomOut()
{

    // Set zoom out mode
    for (auto& itr : _sub_windows)
    {
        itr.scene->setZoomModeToZoomOut();
        itr.view->viewport()->setCursor(m_zoomOutCursor);
    }

}

//---------------------------------------------------------------------------

void ImageViewWidget::createLayout()
{

   m_coordWidget = new CoordinateWidget();

   // Layout
   _main_layout = new QVBoxLayout();
   _main_layout->setContentsMargins(0, 0, 0, 0);

   _image_view_grid_layout = new QGridLayout();
   for (int i = 0; i < _grid_rows; i++)
   {
	   for (int j = 0; j < _grid_cols; j++)
	   {
           int idx = (i * _grid_cols) + j;
           //_cb_image_label
           _image_view_grid_layout->addItem(_sub_windows[idx].layout, i, j);
	   }
   }
   
   _main_layout->addItem(_image_view_grid_layout);
   _main_layout->addWidget(m_coordWidget);

   // Set widget's layout
   setLayout(_main_layout);

}

//---------------------------------------------------------------------------

void ImageViewWidget::createSceneAndView(int rows, int cols)
{
	_grid_rows = rows;
	_grid_cols = cols;

    _sub_windows.resize(_grid_rows * _grid_cols);

	for (auto &itr : _sub_windows)
	{
		connect(itr.scene, SIGNAL(zoomIn(QRectF, QGraphicsSceneMouseEvent*)), this, SLOT(zoomIn(QRectF, QGraphicsSceneMouseEvent*)));
		connect(itr.scene, SIGNAL(zoomIn(QGraphicsItem*)), this, SLOT(zoomIn(QGraphicsItem*)));
		connect(itr.scene, SIGNAL(zoomOut()), this, SLOT(zoomOut()));
		connect(itr.scene, SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(sceneRectUpdated(const QRectF&)));
        connect(itr.scene, &ImageViewScene::onMouseMoveEvent, this, &ImageViewWidget::onMouseMoveEvent);
       
        connect(itr.cb_image_label, &QComboBox::currentTextChanged, this, &ImageViewWidget::onComboBoxChange);

		connect(&itr, &SubImageWindow::redraw_event, this, &ImageViewWidget::parent_redraw);
	}
}

//---------------------------------------------------------------------------

QImage ImageViewWidget::generate_img(ArrayXXr<float>& int_img, QVector<QRgb>& colormap)
{
    float counts_max = int_img.maxCoeff();
    float counts_min = int_img.minCoeff();
    int width = static_cast<unsigned int>(int_img.cols());
    int height = static_cast<unsigned int>(int_img.rows());
    QImage image(width, height, QImage::Format_Indexed8);
    image.setColorTable(colormap);

    float max_min = counts_max - counts_min;
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            //first clamp the data to max min
            float cnts = int_img(row, col);
            cnts = std::min(counts_max, cnts);
            cnts = std::max(counts_min, cnts);
            //convert to pixel
            unsigned char data = static_cast<unsigned char>(((cnts - counts_min) / max_min) * 255.0);
            image.setPixel(col, row, (uint)data);
        }
    }
    return image;
}

//---------------------------------------------------------------------------

void ImageViewWidget::redrawSubWindows()
{
    for (auto& itr : _sub_windows)
    {
        itr.redraw();
    }
}

//---------------------------------------------------------------------------

void ImageViewWidget::setUnitLabel(int idx, QString label)
{
    if (idx > -1 && idx < _sub_windows.size())
    {
        _sub_windows[idx].counts_coord_widget->setUnitsLabel(label);
        _sub_windows[idx].counts_stats_widget->setUnitsLabel(label);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidget::setUnitLabels(QString label)
{
    for (auto& itr : _sub_windows)
    {
        itr.counts_coord_widget->setUnitsLabel(label);
        itr.counts_stats_widget->setUnitsLabel(label);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidget::newGridLayout(int rows, int cols)
{

	for (auto &itr : _sub_windows)
	{
		disconnect(itr.scene, SIGNAL(zoomIn(QRectF, QGraphicsSceneMouseEvent*)), this, SLOT(zoomIn(QRectF, QGraphicsSceneMouseEvent*)));
		disconnect(itr.scene, SIGNAL(zoomIn(QGraphicsItem*)), this, SLOT(zoomIn(QGraphicsItem*)));
		disconnect(itr.scene, SIGNAL(zoomOut()), this, SLOT(zoomOut()));
		disconnect(itr.scene, SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(sceneRectUpdated(const QRectF&)));
		disconnect(itr.scene, &ImageViewScene::onMouseMoveEvent, this, &ImageViewWidget::onMouseMoveEvent);
		disconnect(itr.cb_image_label, &QComboBox::currentTextChanged, this, &ImageViewWidget::onComboBoxChange);
		disconnect(&itr, &SubImageWindow::redraw_event, this, &ImageViewWidget::parent_redraw);
	}
    _sub_windows.clear();

    delete m_coordWidget;
    delete _main_layout;
    //create new layout
    createSceneAndView(rows, cols);
    createLayout();
}

//---------------------------------------------------------------------------

CoordinateWidget* ImageViewWidget::coordinateWidget()
{

   return m_coordWidget;

}

//---------------------------------------------------------------------------

void ImageViewWidget::enterEvent(QEvent * event)
{

   m_mouseLeaveState = false;
   QWidget::enterEvent(static_cast<QEnterEvent*>(event));

}

//---------------------------------------------------------------------------

void ImageViewWidget::onComboBoxChange(QString lbl)
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

void ImageViewWidget::clearLabels()
{
    for(auto &itr : _sub_windows)
    {
        itr.cb_image_label->clear();
    }
}

//---------------------------------------------------------------------------

void ImageViewWidget::addLabel(QString lbl)
{
    for(auto &itr : _sub_windows)
    {
        itr.cb_image_label->addItem(lbl);
    }
}

//---------------------------------------------------------------------------

qreal ImageViewWidget::getCurrentZoomPercent()
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

QPointF ImageViewWidget::getCenterPoint() const
{
    const QRectF centerRect;
    if (_sub_windows.size() > 0)
    {
        return _sub_windows[0].view->mapToScene(_sub_windows[0].view->viewport()->geometry()).boundingRect().center();
    }
    return centerRect.center();

}

//---------------------------------------------------------------------------

void ImageViewWidget::leaveEvent(QEvent * event)
{

   m_mouseLeaveState = true;

   if(set_null_mouse_pos)
        m_coordWidget -> setnullptr();

   QLayout* lout = layout();
   QWidget* w = lout->itemAt(lout->count()-1)->widget();
   CoordinateWidget* lightToMicroCoordWidget = dynamic_cast<CoordinateWidget*>(w);
   if(set_null_mouse_pos)
        lightToMicroCoordWidget->setnullptr();

   QWidget::leaveEvent(event);

}

//---------------------------------------------------------------------------

void ImageViewWidget::setCoordinateModel(CoordinateModel* model)
{

   m_coordWidget->setModel(model);

}

//---------------------------------------------------------------------------

void ImageViewWidget::onMouseMoveEvent(QGraphicsSceneMouseEvent* event)
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

void ImageViewWidget::resizeEvent(QResizeEvent* event)
{

   Q_UNUSED(event)

   if (m_fillState == false) return;
   
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

ImageViewScene* ImageViewWidget::scene(int grid_idx)
{

   // Return current scene
    if(_sub_windows.size() > grid_idx)
    {
        return _sub_windows[grid_idx].scene;
    }
    return nullptr;

}

//---------------------------------------------------------------------------

void ImageViewWidget::setSceneModel(QAbstractItemModel* model)
{
    bool multi_win =  (_sub_windows.size() > 1) ? true : false;
    for (auto& itr : _sub_windows)
    {
        itr.scene->setModel(model, multi_win);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidget::setSceneSelectionModel(QItemSelectionModel* selectionModel)
{
    for (auto& itr : _sub_windows)
    {
        itr.scene->setSelectionModel(selectionModel);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidget::setSceneModelAndSelection(QAbstractItemModel* model, QItemSelectionModel* selectionModel)
{
    bool multi_win = (_sub_windows.size() > 1) ? true : false;
    for (auto& itr : _sub_windows)
    {
        itr.scene->setModel(model, multi_win);
        itr.scene->setSelectionModel(selectionModel);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidget::sceneEnableAnnotations(bool state)
{
    for (auto& itr : _sub_windows)
    {
        itr.scene->enableAnnotations(state);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidget::setSceneUnitsLabel(QString label)
{
    for (auto& itr : _sub_windows)
    {
        itr.scene->setUnitsLabel(label);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidget::setSceneUnitsPerPixelX(double val)
{
    for (auto& itr : _sub_windows)
    {
        itr.scene->setUnitsPerPixelX(val);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidget::setSceneUnitsPerPixelY(double val)
{
    for (auto& itr : _sub_windows)
    {
        itr.scene->setUnitsPerPixelY(val);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidget::sceneUpdateModel()
{
    for (auto& itr : _sub_windows)
    {
        itr.scene->updateModel();
    }
}

//---------------------------------------------------------------------------

void ImageViewWidget::setScenetPixmap(QPixmap p)
{
    for (auto& itr : _sub_windows)
    {
        itr.scene->setPixmap(p);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidget::sceneRectUpdated(const QRectF& rect)
{

   Q_UNUSED(rect)

   // Force a resize
   resizeEvent(nullptr);

}

//---------------------------------------------------------------------------

void ImageViewWidget::setZoomPercentWidget(QComboBox* zoomPercent)
{

    m_zoomPercent  = zoomPercent;
    connect(m_zoomPercent, SIGNAL(currentIndexChanged(int)), this, SLOT(zoomValueChanged(int)));
    updateZoomPercentage();

}

//---------------------------------------------------------------------------

void ImageViewWidget::updateZoomPercentage()
{

    if (m_zoomPercent == nullptr) return;

    qreal wp = getCurrentZoomPercent();

    disconnect(m_zoomPercent, SIGNAL(currentIndexChanged(int)), this, SLOT(zoomValueChanged(int)));

    m_zoomPercent->setEditText(QString("%1").arg(wp, 0, 'f',  0));

    connect(m_zoomPercent, SIGNAL(currentIndexChanged(int)), this, SLOT(zoomValueChanged(int)));

}

//---------------------------------------------------------------------------

QGraphicsView* ImageViewWidget::view()
{

    if (_sub_windows.size() > 0)
    {
        return _sub_windows[0].view;
    }
    // Return current scene
    return nullptr;

}

//---------------------------------------------------------------------------

void ImageViewWidget::zoomIn(QGraphicsItem* zoomObject)
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
   m_fillState = false;
   emit resetZoomToolBar();
}

//---------------------------------------------------------------------------

void ImageViewWidget::zoomIn(QRectF zoomRect, QGraphicsSceneMouseEvent* event)
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

void ImageViewWidget::zoomOut()
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

void ImageViewWidget::zoomValueChanged(int val)
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

QString ImageViewWidget::getLabelAt(int idx)
{
    if(idx < _sub_windows.size())
    {
        return _sub_windows[idx].cb_image_label->currentText();
    }
    return QString();
}

//---------------------------------------------------------------------------

 void ImageViewWidget::getMouseTrasnformAt(int idx, CountsLookupTransformer** counts_lookup, CountsStatsTransformer** counts_stats)
{
    if(idx < _sub_windows.size())
    {
        *counts_lookup = _sub_windows[idx].counts_lookup;
        *counts_stats = _sub_windows[idx].counts_stats;
    }
}

//---------------------------------------------------------------------------

std::vector<QString> ImageViewWidget::getLabelList()
{
    std::vector<QString> label_list;
    for( auto& itr : _sub_windows)
    {
        label_list.push_back(itr.cb_image_label->currentText());
    }
    return label_list;
}

//---------------------------------------------------------------------------

void ImageViewWidget::restoreLabels(const std::vector<QString>& labels)
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

void ImageViewWidget::resetCoordsToZero()
{

    for( auto& itr : _sub_windows)
    {
        itr.counts_coord_widget->setCoordinate(0, 0, 0);
        itr.counts_stats_widget->setCoordinate(0, 0, 0);
    }
}

//---------------------------------------------------------------------------

void ImageViewWidget::getMinMaxAt(int grid_idx, float &counts_min, float &counts_max)
{
	if (grid_idx < _sub_windows.size())
	{
		if (_sub_windows[grid_idx].contrast_updated())
		{
			counts_min = _sub_windows[grid_idx].contrast_min();
			counts_max = _sub_windows[grid_idx].contrast_max();
		}
	}
}

//---------------------------------------------------------------------------