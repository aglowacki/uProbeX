/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/ImageViewScene.h"

#include "gstar/Annotation/AbstractGraphicsItem.h"
#include "gstar/AnnotationTreeModel.h"
#include "gstar/Annotation/RoiMaskGraphicsItem.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <typeinfo>

#include<QDebug>

using namespace gstar;

//---------------------------------------------------------------------------

ImageViewScene::ImageViewScene(QWidget* parent) : QGraphicsScene(parent)
{

   Q_UNUSED(parent)

   // Initialize pointers to nullptr
   m_pixItem = nullptr;
   m_model = nullptr;
   m_selectionModel = nullptr;
   m_zoomSelection = nullptr;
   _is_multi_scene = false;
   m_unitsLabel = "";
   m_unitsPerPixelX = 1.0;
   m_unitsPerPixelY = 1.0;

   // Initialize mode
   m_mode = None;

   // Set initial pixmap
   QPixmap p(1024, 1024);
   p.fill(Qt::gray);
   m_pixItem = addPixmap(p);
   setSceneRect(m_pixItem -> boundingRect());

   // Connect selectionChanged signal to annotation slot
   connect(this, &ImageViewScene::selectionChanged, this, &ImageViewScene::sceneSelectionChanged);

}

//---------------------------------------------------------------------------

ImageViewScene::~ImageViewScene()
{

}

//---------------------------------------------------------------------------

void ImageViewScene::addAnnotationsFromModel()
{

   if (typeid(*m_model) != typeid(AnnotationTreeModel))
   {
      return;
   }

   AnnotationTreeModel* model = dynamic_cast<AnnotationTreeModel*>(m_model);
   QModelIndex firstIndex = model->index(0,0);
   if (false == firstIndex.isValid())
   {
      return;
   }

   AbstractGraphicsItem* cItem = static_cast<AbstractGraphicsItem*>(
                                          firstIndex.internalPointer());

   if (cItem == nullptr)
   {
      return;
   }

   AbstractGraphicsItem* rootItem = cItem->parent();

   if (rootItem == nullptr)
   {
      return;
   }

   removeAllAnnotationItems();

   recursiveAddAnnotation(rootItem);

}

//---------------------------------------------------------------------------

const QAbstractItemModel* ImageViewScene::getModel() const
{

   return m_model;

}

//---------------------------------------------------------------------------

const QString ImageViewScene::getUnitsLabel() const
{

   return m_unitsLabel;

}

//---------------------------------------------------------------------------

double ImageViewScene::getUnitsPerPixelX() const
{

   return m_unitsPerPixelX;

}

//---------------------------------------------------------------------------

double ImageViewScene::getUnitsPerPixelY() const
{

   return m_unitsPerPixelY;

}

//---------------------------------------------------------------------------

void ImageViewScene::modelRowsInserted(const QModelIndex& parent,
                                      int start,
                                      int end)
{

   Q_UNUSED(start);
   Q_UNUSED(end);
   if (parent.isValid())
   {
      const QAbstractItemModel* pm = parent.model();
      if (typeid(*pm) == typeid(AnnotationTreeModel))
      {
         AbstractGraphicsItem* cItem = nullptr;
         AbstractGraphicsItem* item =
            static_cast<AbstractGraphicsItem*>(parent.internalPointer());

         if (item != nullptr)
         {
            cItem = item->child(start);
            if (cItem != nullptr)
            {
                if (_is_multi_scene)
                {
                    AbstractGraphicsItem* clone = cItem->duplicate();

                    clone->linkProperties(cItem->properties());

                    cItem->linkProperties(clone->properties());

                    cItem->appendLinkedDisplayChild(clone);

                    addItem(clone);
                }
                else
                {
                    addItem(cItem);// drawing works but updates down't on multiple displays...
                }
            }
         }
      }
   }
}

//---------------------------------------------------------------------------

void ImageViewScene::modelRowsRemoved(const QModelIndex& parent, int start, int end)
{

    Q_UNUSED(start);
    Q_UNUSED(end);
    if (parent.isValid())
    {
        const QAbstractItemModel* pm = parent.model();
        if (typeid(*pm) == typeid(AnnotationTreeModel))
        {
            AbstractGraphicsItem* cItem = nullptr;
            AbstractGraphicsItem* item =
                static_cast<AbstractGraphicsItem*>(parent.internalPointer());

            if (item != nullptr)
            {
                for (int i = start; i <= end; i++)
                {
                    cItem = item->child(i);
                    if (cItem != nullptr)
                    {
                        cItem->unlinkAllAnnotations();
                        QList<QGraphicsItem*> gitems = this->items();
                        AbstractGraphicsItem* toRemove = nullptr;
                        for (auto itr : cItem->getLinkedDisplayChildren())
                        {
                            if (gitems.contains(itr))
                            {
                                removeItem(itr);
                                toRemove = itr;
                                break;
                            }
                        }
                        if (toRemove != nullptr)
                        {
                            cItem->removeLinkedDisplayChild(toRemove);
                        }
                    }
                }
            }
        }
    }

}

//---------------------------------------------------------------------------

void ImageViewScene::enableAnnotations(bool state)
{

   if (typeid(*m_model) != typeid(AnnotationTreeModel))
   {
      return;
   }

   AnnotationTreeModel* model = dynamic_cast<AnnotationTreeModel*>(m_model);
   QModelIndex firstIndex = model->index(0,0);
   if (false == firstIndex.isValid())
   {
      return;
   }

   AbstractGraphicsItem* cItem = static_cast<AbstractGraphicsItem*>(
                                          firstIndex.internalPointer());

   if (cItem == nullptr)
   {
      return;
   }

   AbstractGraphicsItem* rootItem = cItem->parent();

   if (rootItem == nullptr)
   {
      return;
   }

   rootItem->setEnabled(state);
   recursiveSetEnabled(rootItem, state);

   // Reset view; just in case...
   setSceneRect(m_pixItem -> boundingRect());

}

//---------------------------------------------------------------------------
/*
void ImageViewScene::lockROIs(bool state)
{

   // Clear selection
   m_selectionModel -> clearSelection();

   // Set flags on gRectItems
   for (int i = 0 ; i < m_rois.size() ; i++) {
      m_rois.at(i)->setFlag(QGraphicsItem::ItemIsMovable, !state);
      m_rois.at(i)->setFlag(QGraphicsItem::ItemIsSelectable, !state);
      m_rois.at(i)->setFlag(QGraphicsItem::ItemSendsGeometryChanges, !state);
   }

}
*/

//---------------------------------------------------------------------------

void ImageViewScene::modelSelectionChanged(const QItemSelection& selected,
                                           const QItemSelection& deselected)
{

   if (m_selectionModel == nullptr || m_model == nullptr)
   {
      return;
   }

   disconnect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &ImageViewScene::modelSelectionChanged);

   disconnect(this, &ImageViewScene::selectionChanged, this, &ImageViewScene::sceneSelectionChanged);

   clearFocus();

   foreach (QModelIndex dIndex, deselected.indexes())
   {
      AbstractGraphicsItem* item =
            (AbstractGraphicsItem*)(dIndex.internalPointer());
      item->setSelected(false);
   }

   foreach (QModelIndex sIndex, selected.indexes())
   {
      AbstractGraphicsItem* item =
            (AbstractGraphicsItem*)(sIndex.internalPointer());
      item->setSelected(true);
   }

   connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &ImageViewScene::modelSelectionChanged);

   connect(this, &ImageViewScene::selectionChanged, this, &ImageViewScene::sceneSelectionChanged);

}

//---------------------------------------------------------------------------

void ImageViewScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{

   // Zoom in mode
   if ((m_mode == ZoomIn) && (m_zoomSelection != nullptr))
   {

      /// Get attached view
      QGraphicsView* view = qobject_cast<QGraphicsView*> (
         m_zoomSelection -> parent() -> parent());
      Q_ASSERT(view);

      // Resize rubber band based on initial mouse press and current mouse drag
      QRectF rect(m_zoomOrigin, view -> mapFromScene(event -> scenePos()));
      m_zoomSelection -> setGeometry(rect.toRect().normalized());

      // Rubber band should only becomes visible, if the mouse is dragged.
      if (!m_zoomSelection -> isVisible())
      {
         m_zoomSelection -> show();
      }
      m_zoomRect = rect;

   }
   else
   {
      int x = event->scenePos().x();
      int y = event->scenePos().y();
      if(m_pixItem != nullptr)
      {
         if(x > -1 && x < m_pixItem->pixmap().width())
         {
            if(y > -1 && y < m_pixItem->pixmap().height())
            {
               //QImage img = m_pixItem->pixmap().toImage();
               //QRgb val = img.pixel(x, y);
                              // emit the mouse over pixel location
               //emit mouseOverPixel(x, y);
               emit onMouseMoveEvent(event);
            }
         }
      }
      QGraphicsScene::mouseMoveEvent(event);
   }

}

//---------------------------------------------------------------------------

void ImageViewScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{

   // Left click and mode is ZoomIn
   if ((event -> button() == Qt::LeftButton) && (m_mode == ZoomIn))
   {
      // Get attached view
      QGraphicsView* view = qobject_cast<QGraphicsView*>(
         event -> widget() -> parent());
      Q_ASSERT(view);
      // Construct a rubber band object for the view.
      m_zoomSelection = new QRubberBand(QRubberBand::Rectangle,
                                        view -> viewport());
      m_zoomOrigin = view -> mapFromScene(event -> scenePos());

      // Store the size of rubber band rectangle in a QRectF object.
      m_zoomRect = QRectF(m_zoomOrigin, m_zoomOrigin);
   }

   // Left click and mode is ZoomOut
   else if ((event -> button() == Qt::LeftButton) && (m_mode == ZoomOut))
   {
      // Emit zoomOut signal
      emit zoomOut();
   }

   // Propagate event
   if ((event -> button() == Qt::LeftButton ||
        event -> button() == Qt::RightButton) &&
       (m_mode == None || m_mode == Fit))
   {
      QGraphicsScene::mousePressEvent(event);
   }

   emit onMousePressEvent(event);
}

//---------------------------------------------------------------------------

void ImageViewScene::updateZoom(QGraphicsItem* zoomObject)
{
   emit zoomIn(zoomObject);
}

//---------------------------------------------------------------------------

void ImageViewScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{

   if ((m_mode == ZoomIn) && m_zoomSelection != nullptr)
   {
      delete m_zoomSelection;
      m_zoomSelection = nullptr;
      emit zoomInRect(m_zoomRect, event);
   }
   else
   {
      QGraphicsScene::mouseReleaseEvent(event);
   }

   emit onMouseReleaseEvent(event);
}

//---------------------------------------------------------------------------

QRectF ImageViewScene::pixRect()
{

   return m_pixItem -> boundingRect();

}

//---------------------------------------------------------------------------

void ImageViewScene::recursiveAddAnnotation(AbstractGraphicsItem* item)
{

    if (item != nullptr)
    {
        if (_is_multi_scene)
        {
            AbstractGraphicsItem* clone = item->duplicate();
            clone->linkProperties(item->properties());
            item->linkProperties(clone->properties());
            addItem(clone);
        }
        else
        {
            addItem(item);
        }
        foreach(AbstractGraphicsItem * cItem, item->childList())
        {
            recursiveAddAnnotation(cItem);
        }
    }
}

//---------------------------------------------------------------------------

void ImageViewScene::recursiveSetEnabled(AbstractGraphicsItem* item, bool state)
{

   item->setVisible(state);
   foreach (AbstractGraphicsItem* cItem, item->childList())
   {
      cItem->setEnabled(state);
      recursiveSetEnabled(cItem, state);
   }

}

//---------------------------------------------------------------------------

void ImageViewScene::removeAllAnnotationItems()
{

   QList<QGraphicsItem*> allItems = QGraphicsScene::items();
   foreach(QGraphicsItem* item, allItems)
   {
      if(item != m_pixItem)
         removeItem(item);
   }

}

//---------------------------------------------------------------------------

void ImageViewScene::removeAllGraphicsItems()
{

   QList<QGraphicsItem*> allItems = QGraphicsScene::items();
   foreach(QGraphicsItem* item, allItems)
   {
      removeItem(item);
   }

}

//---------------------------------------------------------------------------

void ImageViewScene::sceneSelectionChanged()
{
    if (m_model != nullptr && m_selectionModel != nullptr)
    {

        disconnect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &ImageViewScene::modelSelectionChanged);

        disconnect(this, &ImageViewScene::selectionChanged, this, &ImageViewScene::sceneSelectionChanged);

        m_selectionModel->clear();

        QList<QGraphicsItem*> sItems = selectedItems();
        foreach(QGraphicsItem * item, sItems)
        {
            AbstractGraphicsItem* tItem = (AbstractGraphicsItem*)item;
            AbstractGraphicsItem* parent = tItem->parent();
            if (parent != nullptr)
            {
                QModelIndex groupIndex =
                    m_model->index(parent->row(), 0, QModelIndex());
                if (groupIndex.isValid())
                {
                    const QAbstractItemModel* mmodel = groupIndex.model();
                    if (mmodel != nullptr)
                    {
                        QModelIndex cIndex = mmodel->index(tItem->row(), 0);
                        if (cIndex.isValid())
                        {
                            for (int i = 0; i < tItem->columnCount(); i++)
                            {
                                cIndex = mmodel->index(tItem->row(), i);
                                m_selectionModel->select(cIndex, QItemSelectionModel::Select);
                            }
                        }
                    }
                }
            }
        }
    }

    if (m_selectionModel != nullptr)
    {
        connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &ImageViewScene::modelSelectionChanged);
    }
    connect(this, &ImageViewScene::selectionChanged, this, &ImageViewScene::sceneSelectionChanged);

}

//---------------------------------------------------------------------------

void ImageViewScene::setPixmap(QPixmap p)
{

    // Set pixmap
    if (m_pixItem != nullptr)
    {
        if (p.width() > 0 && p.height() > 0)
        {
            // Check against old image size and reset scene if necessary
            if ((m_pixItem->boundingRect()).width() != p.width() ||
                (m_pixItem->boundingRect()).height() != p.height())
            {
                m_pixItem->setPixmap(p);
                setSceneRect(m_pixItem->boundingRect());
                //updateROIs();
            }
            // New and old images have matching dimensions
            else
            {
                m_pixItem->setPixmap(p);
            }
        }
    }
    // Set first pixmap
    else
    {
        m_pixItem = addPixmap(p);
        //updateROIs();
    }

}

//---------------------------------------------------------------------------

void ImageViewScene::setMode(Mode mode)
{

   // Set mode for mouse clicks
   m_mode = mode;

}

//---------------------------------------------------------------------------

void ImageViewScene::setModel(QAbstractItemModel* model, bool is_multi_scene)
{

   // Remove old connections
   if (m_model != nullptr)
   {
      disconnect(m_model, &QAbstractItemModel::rowsInserted, this, &ImageViewScene::modelRowsInserted);

      disconnect(m_model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &ImageViewScene::modelRowsRemoved);
   }

   removeAllAnnotationItems();

   // Set model
   m_model = model;
   _is_multi_scene = is_multi_scene;

   if (m_model != nullptr)
   {
       addAnnotationsFromModel();
       // Connect signals/slots to inserting and removing ROIs
       connect(m_model, &QAbstractItemModel::rowsInserted, this, &ImageViewScene::modelRowsInserted);

       connect(m_model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &ImageViewScene::modelRowsRemoved);
   }
}

//---------------------------------------------------------------------------

void ImageViewScene::setSelectionModel(QItemSelectionModel* selectionModel)
{

   // Remove old connections
   if (m_selectionModel != nullptr)
   {
      disconnect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &ImageViewScene::modelSelectionChanged);
   }

   // Set ROI selection model
   m_selectionModel = selectionModel;

   if (m_selectionModel != nullptr)
   {
       // Set new connections
       connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &ImageViewScene::modelSelectionChanged);
   }
}

//---------------------------------------------------------------------------

void ImageViewScene::setUnitsLabel(QString label)
{

   m_unitsLabel = label;

}

//---------------------------------------------------------------------------

void ImageViewScene::setUnitsPerPixelX(double x)
{

   m_unitsPerPixelX = x;

}

//---------------------------------------------------------------------------

void ImageViewScene::setUnitsPerPixelY(double y)
{

   m_unitsPerPixelY = y;

}

//---------------------------------------------------------------------------

void ImageViewScene::setZoomModeToFit()
{

   m_mode = Fit;

}

//---------------------------------------------------------------------------

void ImageViewScene::setZoomModeToNone()
{

   m_mode = None;

}

//---------------------------------------------------------------------------

void ImageViewScene::setZoomModeToZoomIn()
{

   m_mode = ZoomIn;

}

//---------------------------------------------------------------------------

void ImageViewScene::setZoomModeToZoomOut()
{

   m_mode = ZoomOut;

}

//---------------------------------------------------------------------------

void ImageViewScene::updateModel()
{

   if (typeid(*m_model) == typeid(AnnotationTreeModel))
   {
      AnnotationTreeModel* aModel = dynamic_cast<AnnotationTreeModel*>(m_model);
      aModel->calculate();
   }

}

//---------------------------------------------------------------------------
/*
void ImageViewScene::updateROIs()
{

   // Model index
   QModelIndex index;

   // Update each ROI
   for (int i = 0 ; i < m_rois.size() ; i++) {

      // Get and recalculate ROI positions and size
      index = m_model -> index(i, ROITableModel::X);
      double x = (m_model -> data(index)).toDouble() *
                 pixRect().width();
      index = m_model -> index(i, ROITableModel::Y);
      double y = (m_model -> data(index)).toDouble() *
                 pixRect().height();
      index = m_model -> index(i, ROITableModel::Width);
      double w = (m_model -> data(index)).toDouble() *
                 pixRect().width();
      index = m_model -> index(i, ROITableModel::Height);
      double h = (m_model -> data(index)).toDouble() *
                 pixRect().height();

      // Update graphics item
      m_rois.at(i) -> setX(x);
      m_rois.at(i) -> setY(y);
      m_rois.at(i) -> setWidth(w);
      m_rois.at(i) -> setHeight(h);

   }

}
*/
//---------------------------------------------------------------------------
