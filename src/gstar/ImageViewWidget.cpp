/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/ImageViewWidget.h>
#include <QDebug>
#include <QLabel>
#include <QComboBox>
#include <QListView>
#include <QGraphicsView>
#include <QResizeEvent>

using namespace gstar;

/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/

ImageViewWidget::~ImageViewWidget()
{

	for (auto& itr : m_scene)
	{
		delete itr;
	}
	m_scene.clear();

	for (auto& itr : m_view)
	{
		delete itr;
	}
	m_view.clear();

    for (auto& itr : _cb_image_label)
    {
        delete itr;
    }
    _cb_image_label.clear();

    //this memeory is deleted by the model, just need to clear the vector
    _counts_lookup.clear();

    for (auto& itr : _counts_coord_model)
    {
        delete itr;
    }
    _counts_coord_model.clear();

    for (auto& itr : _counts_coord_widget)
    {
        delete itr;
    }
    _counts_coord_widget.clear();

}

/*---------------------------------------------------------------------------*/

bool ImageViewWidget::getMouseLeaveState()
{

   return m_mouseLeaveState;

}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::clickCursor()
{

   // Set scene mode
	for (auto& itr : m_scene)
	{
		itr->setZoomModeToNone();
	}

   // Set regular cursor
   for (auto& itr : m_view)
   {
	   itr->viewport()->setCursor(Qt::ArrowCursor);
   }
}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::clickFill(bool checked)
{

   // State is used by the resizeEvent raised below.
   //  Make sure state is set before calling resizeEvent method.
   m_fillState = checked;
   
   // Get out of zoom mode
   clickCursor();

   // Set scene mode
   if (checked == true) 
   {
	   for (auto& itr : m_scene)
	   {
		   itr->setZoomModeToFit();
	   }
		resizeEvent(nullptr);
		m_zoomPercent->setCurrentIndex(-1);
   }
   
   // Set regular cursor
   for (auto &itr : m_view)
   {
	   itr->viewport()->setCursor(Qt::ArrowCursor);
   }

}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::clickZoomIn()
{

	for (auto& itr : m_scene)
	{
		itr->setZoomModeToZoomIn();
	}

	for (auto& itr : m_view)
	{
		itr->viewport()->setCursor(m_zoomInCursor);
	}

}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::clickZoomOriginal()
{

	for (auto& itr : m_view)
	{
		itr->resetMatrix();
		itr->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		itr->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}
  

   updateZoomPercentage();

}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::clickZoomOut()
{

   // Set zoom out mode
	for (auto& itr : m_scene)
	{
		itr->setZoomModeToZoomOut();
	}
   for (auto& itr : m_view)
   {
	   itr->viewport()->setCursor(m_zoomOutCursor);
   }
}

/*---------------------------------------------------------------------------*/

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
           QVBoxLayout *vb = new QVBoxLayout();
           int idx = (i*_grid_cols) + j;

           vb->addWidget(_counts_coord_widget[idx]);
           vb->addWidget(m_view[idx]);
           vb->addWidget(_cb_image_label[idx]);

           //_cb_image_label
           _image_view_grid_layout->addItem(vb, i, j);
	   }
   }
   
   _main_layout->addItem(_image_view_grid_layout);
   _main_layout->addWidget(m_coordWidget);

   // Set widget's layout
   setLayout(_main_layout);

}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::createSceneAndView(int rows, int cols)
{
	_grid_rows = rows;
	_grid_cols = cols;

	m_view.resize(_grid_rows*_grid_cols);
	m_scene.resize(_grid_rows*_grid_cols);

    _cb_image_label.resize(_grid_rows*_grid_cols);
    _counts_lookup.resize(_grid_rows*_grid_cols);
    _counts_coord_model.resize(_grid_rows*_grid_cols);
    _counts_coord_widget.resize(_grid_rows*_grid_cols);

	for (int i = 0; i < _grid_rows * _grid_cols; i++)
	{
		// Initialize scene
		ImageViewScene* scene = new ImageViewScene();
        // scene->setSceneRect(scene->itemsBoundingRect());


		connect(scene, SIGNAL(zoomIn(QRectF, QGraphicsSceneMouseEvent*)), this, SLOT(zoomIn(QRectF, QGraphicsSceneMouseEvent*)));
		connect(scene, SIGNAL(zoomIn(QGraphicsItem*)), this, SLOT(zoomIn(QGraphicsItem*)));
		connect(scene, SIGNAL(zoomOut()), this, SLOT(zoomOut()));
		connect(scene, SIGNAL(sceneRectChanged(const QRectF&)),
			this, SLOT(sceneRectUpdated(const QRectF&)));

		connect(scene, SIGNAL(mouseOverPixel(int, int)),
			this, SLOT(mouseOverPixel(int, int)));

		// Initialize view
		QGraphicsView *view = new QGraphicsView();
		view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
		view->setScene(scene);

        connect(scene, SIGNAL(mouseOverPixel(int, int)),
                this, SLOT(mouseOverPixel(int, int)));

		m_scene[i] = scene;
		m_view[i] = view;

        QComboBox *cb_label = new QComboBox(this);
        connect(cb_label, SIGNAL(currentIndexChanged(QString)), this, SLOT(onComboBoxChange(QString)));

        CountsLookupTransformer* counts_lookup = new gstar::CountsLookupTransformer();
        CoordinateModel* counts_coord_model = new gstar::CoordinateModel(counts_lookup);
        gstar::CoordinateWidget *counts_coord_widget = new gstar::CoordinateWidget();
        counts_coord_widget->setModel(counts_coord_model);
        counts_coord_widget->setLabel("Counts:", "Min:", "Max:");
        counts_coord_widget->setUnitsLabel("cts/s");

        _cb_image_label[i] = cb_label;
        _counts_lookup[i] = counts_lookup;
        _counts_coord_model[i] = counts_coord_model;
        _counts_coord_widget[i] = counts_coord_widget;
	}
}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::newGridLayout(int rows, int cols)
{

    for (int i = 0; i < _grid_rows * _grid_cols; i++)
    {
        delete m_scene[i];
        delete m_view[i];
        delete _cb_image_label[i];
        //delete _counts_lookup[i]; // deleted in coord_model
        delete _counts_coord_model[i];
        delete _counts_coord_widget[i];
    }
    m_scene.clear();
    m_view.clear();
    _cb_image_label.clear();
    _counts_lookup.clear();
    _counts_coord_model.clear();
    _counts_coord_widget.clear();

    delete m_coordWidget;
    delete _main_layout;
    //create new layout
    createSceneAndView(rows, cols);
    createLayout();
}

/*---------------------------------------------------------------------------*/

CoordinateWidget* ImageViewWidget::coordinateWidget()
{

   return m_coordWidget;

}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::enterEvent(QEvent * event)
{

   m_mouseLeaveState = false;
   QWidget::enterEvent(event);

}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::onComboBoxChange(QString lbl)
{
    QObject* obj = sender();
    for(int i =0; i< _cb_image_label.size(); i++)
    {
        if(_cb_image_label[i] == obj)
        {
            emit cbLabelChanged(lbl, i);
            break;
        }
    }
}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::clearLabels()
{
    for(auto &itr : _cb_image_label)
    {
        itr->clear();
    }
}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::addLabel(QString lbl)
{
    for(auto &itr : _cb_image_label)
    {
        itr->addItem(lbl);
    }
}

/*---------------------------------------------------------------------------*/

qreal ImageViewWidget::getCurrentZoomPercent()
{

   QTransform t = m_view[0]->transform();
   QRectF tImage = t.mapRect(m_scene[0]->pixRect());

   qreal wp = tImage.width() / m_scene[0]->pixRect().width() * 100.0;

   return wp;

}

/*---------------------------------------------------------------------------*/

QPointF ImageViewWidget::getCenterPoint() const
{

   const QRectF centerRect = m_view[0]->mapToScene(m_view[0]->viewport()->geometry()).boundingRect();

   return centerRect.center();

}

/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/

void ImageViewWidget::setCoordinateModel(CoordinateModel* model)
{

   m_coordWidget->setModel(model);

}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::mouseOverPixel(int x, int y)
{

    m_coordWidget -> setCoordinate(x,y);
    for(auto &itr : _counts_coord_widget)
    {
        itr->setCoordinate(x, y, 0);
    }

}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::resizeEvent(QResizeEvent* event)
{

   Q_UNUSED(event)

   if (m_fillState == false) return;
   
   // Get image size
   QRectF r(0, 0, (m_scene[0]->sceneRect()).width(), (m_scene[0]->sceneRect()).height());
   for (auto &itr : m_view)
   {
	   itr->fitInView(r, Qt::KeepAspectRatio);

	   itr->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	   itr->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   }
   updateZoomPercentage();
   update();
}

/*---------------------------------------------------------------------------*/

ImageViewScene* ImageViewWidget::scene(int grid_idx)
{

   // Return current scene
    if(m_scene.size() > grid_idx)
    {
        return m_scene[grid_idx];
    }
    return nullptr;

}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::sceneRectUpdated(const QRectF& rect)
{

   Q_UNUSED(rect)

   // Force a resize
   resizeEvent(nullptr);

}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::setZoomPercentWidget(QComboBox* zoomPercent)
{

   m_zoomPercent  = zoomPercent;

   connect(m_zoomPercent,
           SIGNAL(currentIndexChanged(int)),
           this,
           SLOT(zoomValueChanged()));

   updateZoomPercentage();

}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::updateZoomPercentage()
{

   if (m_zoomPercent == nullptr) return;

   qreal wp = getCurrentZoomPercent();


   disconnect(m_zoomPercent,
              SIGNAL(currentIndexChanged(int)),
              this,
              SLOT(zoomValueChanged()));

   m_zoomPercent->setEditText(QString("%1").arg(wp, 0, 'f',  0));

   connect(m_zoomPercent,
           SIGNAL(currentIndexChanged(int)),
           this,
           SLOT(zoomValueChanged()));

}

/*---------------------------------------------------------------------------*/

QGraphicsView* ImageViewWidget::view()
{

   // Return current scene
   return m_view[0];

}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::zoomIn(QGraphicsItem* zoomObject)
{
	for (auto & itr : m_view)
	{
		itr->fitInView(zoomObject, Qt::KeepAspectRatio);

		// Force update scroll bars
		itr->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		itr->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}
   updateZoomPercentage();

   clickCursor();
   m_fillState = false;
   emit resetZoomToolBar();
}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::zoomIn(QRectF zoomRect, QGraphicsSceneMouseEvent* event)
{

   qreal wp = getCurrentZoomPercent();
   if (wp >= 800) return;

   //int zoomRecPer = 500 - wp;
   int zoomWidth = zoomRect.normalized().width();
   int zoomHeight = zoomRect.normalized().height();
   qDebug()<<"ZoomIn Rect Width "<<zoomWidth<<" height "<<zoomHeight;
   // Zoom in
   //qDebug()<<"ZoomIn Rect Width per "<<zoomRecPer<<" height "<<zoomRecPer;
   if ((!zoomRect.isEmpty() || !zoomRect.normalized().isEmpty())
       && (zoomWidth > 10 && zoomHeight > 10) )
   {
	   for (auto & itr : m_view)
	   {
		   itr->fitInView(QRectF(itr->mapToScene(zoomRect.topLeft().toPoint()), itr->mapToScene(zoomRect.bottomRight().toPoint())),   Qt::KeepAspectRatio);
	   }
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
	   for (auto & itr : m_view)
	   {
		   itr->scale(1.50, 1.50);
		   itr->centerOn(event->lastScenePos());
	   }
   }
   for (auto & itr : m_view)
   {
	   // Force update scroll bars
	   itr->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	   itr->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   }
   updateZoomPercentage();

}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::zoomOut()
{

   qreal wp = getCurrentZoomPercent();

   if (wp <= 12.5) return;
   for (auto & itr : m_view)
   {
	   itr->scale(.66, .66);

	   // Force update scroll bars
	   itr->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	   itr->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   }
   updateZoomPercentage();

}

/*---------------------------------------------------------------------------*/

void ImageViewWidget::zoomValueChanged()
{

   bool isOK = false;
   float value = m_zoomPercent->currentText().toFloat(&isOK);
   if (!isOK || value < 12.5 || value > 800) {
      m_zoomPercent->removeItem(m_zoomPercent->currentIndex());
      updateZoomPercentage();
      return;
   }

   QTransform t = m_view[0]->transform();
   QRectF image = m_scene[0]->pixRect();
   QRectF tImage = t.mapRect(image);

   qreal sx = (value/100 * image.width()) / tImage.width();
   qreal sy = (value/100 * image.height()) / tImage.height();

   qreal s = sx;
   if (sy < sx) s = sy;

   for (auto & itr : m_view)
   {
	   itr->scale(s, s);
   }
}

/*---------------------------------------------------------------------------*/

