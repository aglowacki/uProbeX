/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_IMAGE_VIEW_SCENE_H
#define GSTAR_IMAGE_VIEW_SCENE_H

//---------------------------------------------------------------------------

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QItemSelectionModel>
#include <QRubberBand>
#include <QPixmap>
#include "gstar/Annotation/AbstractGraphicsItem.h"

//---------------------------------------------------------------------------

namespace gstar
{

/**
 * @brief The ImageViewScene class ...
 */
class ImageViewScene
: public QGraphicsScene
{

   Q_OBJECT

public:

   /**
    * Possible zoom modes.
    */
   enum Mode {
      None,        /*!< No zoom mode selected. */
      ZoomIn,      /*!< Zoom in mode. */
      ZoomOut,     /*!< Zoom out mode. */
      Fit          /*!< Fit mode; stretch or shrink image to fit in widget. */
   };

   /**
    * Constructor
    *
    * @param parent the Qt parent widget.
    */
   ImageViewScene(QWidget* parent = 0);

   /**
    * Destructor
    */
   virtual ~ImageViewScene();

   /**
    * Get bounding rectangle of current pixItem
    *
    * @return current pixItem boudning rectangle
    */
   QRectF pixRect();

   /**
    * Emits a signal to the view widget to zoom in to zoom object.
    *
    * @param zoomObject - item to zoom onto.
    */
   void updateZoom(QGraphicsItem* zoomObject);

   /**
    * @brief getModel
    * @return
    */
   const QAbstractItemModel* getModel() const;

   /**
    * @brief getUnitsLabel
    * @return
    */
   const QString getUnitsLabel() const;

   /**
    * @brief getUnitsPerPixelX
    * @return
    */
   double getUnitsPerPixelX() const;

   /**
    * @brief getUnitsPerPixelY
    * @return
    */
   double getUnitsPerPixelY() const;

   QGraphicsPixmapItem* getPixmapItem(){return m_pixItem;}

   /**
    * Set ROI movel
    *
    * @param model - ROI model
    */
   void setModel(QAbstractItemModel* model, bool is_multi_scene=false);

   /**
    * Set the selection model for ROIs
    *
    * @param selectionModel - ROI selection model
    */
   void setSelectionModel(QItemSelectionModel* selectionModel);

   void removeDefaultPixmap();
   
   void setItemOffset(QPointF off) {_item_offset = off;}
   
public slots:

   /**
    * Enable(show) / Disable(hide) annotations in scene
    *
    * @param state - true = enable, false = disable
    */
   void enableAnnotations(bool state);

   /**
    * Set zoom mode for view.
    *
    * @param mode - image zoom mode for view
    */
   void setMode(ImageViewScene::Mode mode);

   /**
    * Set current image to visualize.
    *
    * @param p - image to visualize
    */
   void setPixmap(const QPixmap& p);

   /**
    * @brief setUnitsLabel
    * @param label
    */
   void setUnitsLabel(QString label);

   /**
    * @brief setUnitsPerPixelX
    * @param x
    */
   void setUnitsPerPixelX(double x);

   /**
    * @brief setUnitsPerPixelY
    * @param y
    */
   void setUnitsPerPixelY(double y);

   /**
    * Set zoom mode to ImageViewScene::Fit
    */
   void setZoomModeToFit();

   /**
    * Set zoom mode to ImageViewScene::None
    */
   void setZoomModeToNone();

   /**
    * Set zoom mode to ImageViewScene::ZoomIn
    */
   void setZoomModeToZoomIn();

   /**
    * Set zoom mode to ImageViewScene::ZoomOut
    */
   void setZoomModeToZoomOut();

   /**
    * @brief updateModel
    */
   void updateModel();

signals:

   /**
    * The coordinates that when mouse moves over
    *
    * @param x - x coordinate.
    * @param y - y coordinate.
    */
   //void mouseOverPixel(int x, int y);

   /**
    * Signal that a zoom in event took place. The view onto the scene should
    * handle it.
    *
    * @param zoomRect - rubber band selection rectangle.
    */
   void zoomInRect(QRectF zoomRect, QGraphicsSceneMouseEvent* event);

   /**
    * Singal that is used to perform a zoom to a particular item on the view scene.
    *
    * @param zoomObject - item on view scene to zoom to.
    */
   void zoomIn(QGraphicsItem* zoomObject);

   /**
    * Signal that a zoom out event took place. The view onto the scene should
    * handle it.
    */
   void zoomOut();

   void onMousePressEvent(QGraphicsSceneMouseEvent*);

   void onMouseMoveEvent(QGraphicsSceneMouseEvent*);

   void onMouseReleaseEvent(QGraphicsSceneMouseEvent*);

protected:

   /**
    * @brief addItemsFromModel
    * @param model
    */
   void addAnnotationsFromModel();

   /**
    * Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

   /**
    * Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   void mousePressEvent(QGraphicsSceneMouseEvent* event);

   /**
    * Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

   /**
    * @brief removeAllAnnotationItems
    */
   void removeAllAnnotationItems();

   /**
    * @brief removeAllGraphicsItems
    */
   void removeAllGraphicsItems();

   /**
    * @brief recursiveAddAnnotation
    * @param item
    * @param state
    */
   void recursiveAddAnnotation(AbstractGraphicsItem* item);

   /**
    * @brief recursiveSetEnabled
    * @param item
    * @param state
    */
   void recursiveSetEnabled(AbstractGraphicsItem* item, bool state);

private slots:

   /**
    * @brief modelRowsInserted
    * @param parent
    * @param start
    * @param end
    */
   void modelRowsInserted(const QModelIndex& parent, int start, int end);

   void modelRowsRemoved(const QModelIndex& parent, int start, int end);

   /**
    * Internal slot called when the ROI selection in the scene is changed
    * interactively by the user. Reimplemented from QAbstractItemModel.
    * See Qt documentation.
    */
   void sceneSelectionChanged();

   /**
    * @brief selectionChanged
    * @param selected
    * @param deselected
    */
   void modelSelectionChanged(const QItemSelection& selected,
                              const QItemSelection& deselected);

private:

   /**
    * Zoom mode
    */
   Mode m_mode;

   /**
    * Pointer to pixmap item
    */
   QGraphicsPixmapItem* m_pixItem;

   /**
    * For drawing rubber-band on the image.
    */
   QRubberBand* m_zoomSelection;

   /**
    * Starting point of zoom selection
    */
   QPointF m_zoomOrigin;

   /**
    * Rubber band selection rect
    */
   QRectF m_zoomRect;

   /**
    * Annotation model
    */
   QAbstractItemModel* m_model;

   /**
    * Annotation selection model
    */
   QItemSelectionModel* m_selectionModel;

   /**
    * @brief m_unitsLabel
    */
   QString m_unitsLabel;

   /**
    * @brief m_unitsPerPixelX
    */
   double m_unitsPerPixelX;

   /**
    * @brief m_unitsPerPixelY
    */
   double m_unitsPerPixelY;

   bool _is_multi_scene;

   QPointF _item_offset;
};

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
