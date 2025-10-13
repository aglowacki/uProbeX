/*-----------------------------------------------------------------------------
 * Copyright (c) 2025, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_IMAGE_VIEW_WIDGET_COMPACT_H
#define GSTAR_IMAGE_VIEW_WIDGET_COMPACT_H

//---------------------------------------------------------------------------

#include "gstar/ImageViewWidget.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

namespace gstar
{
/**
 * @brief The ImageViewWidgetCompact class ...
 */
class ImageViewWidgetCompact
: public ImageViewWidget
{

   Q_OBJECT

public:

   /**
    * Constructor
    *
    * @param parent the Qt parent widget.
    */
   ImageViewWidgetCompact(int rows = 1, int cols = 1, QWidget* parent = 0);

   /**
    * Destructor
    */
   virtual ~ImageViewWidgetCompact();

   virtual QPointF getCenterPoint() const;

   virtual QRectF getSceneRect();

   virtual void setSelectorVisible(bool val);

   virtual void setCountsVisible(bool val);

   virtual QGraphicsView* view();

   virtual ImageViewScene* scene(int grid_idx = 0);

   virtual void setSceneModel(QAbstractItemModel* model);

   virtual void setSceneSelectionModel(QItemSelectionModel* selectionModel);

   virtual void setSceneModelAndSelection(QAbstractItemModel* model, QItemSelectionModel* selectionModel);

   virtual void sceneEnableAnnotations(bool state);

   virtual void setSceneUnitsLabel(QString label);

   virtual void setSceneUnitsPerPixelX(double val);

   virtual void setSceneUnitsPerPixelY(double val);

   virtual void sceneUpdateModel();

   virtual void setScenetPixmap(QPixmap p);

   virtual void setCountsTrasnformAt(unsigned int idx, const ArrayXXr<float>& normalized);

   virtual void setUnitLabel(int idx, QString label);

   virtual void setUnitLabels(QString label);

   virtual QString getLabelAt(int idx);

   virtual void getMouseTrasnformAt(int idx, CountsLookupTransformer** counts_lookup, CountsStatsTransformer** counts_stats);

   virtual void resetCoordsToZero();

   virtual std::vector<QString> getLabelList();

   virtual void restoreLabels(const std::vector<QString>& labels);

   virtual void clearLabels();

   virtual void addLabel(QString lbl);

   virtual void setLabel(QString lbl);
   
   virtual void setGlobalContrast(bool val);

   virtual bool getMinMaxAt(int grid_idx, float &counts_min, float &counts_max);

public slots:

   /**
    * Handle click on cursor tool bar icon
    */
   virtual void clickCursor();

   /**
    * Handle click on zoom in tool bar icon. Switch to zoom in mode.
    */
   virtual void clickZoomIn();

   /**
    * Handle click on fill tool bar icon. Switch to fill mode.
    *
    * @param check - The flag for the fill button state. 
    */
   virtual void clickFill(bool check);
   /**
    * Handle click on zoom back to original tool bar icon.
    */
   virtual void clickZoomOriginal();

   /**
    * Handle click on zoom out tool bar icon. Switch to zoom out mode.
    */
   virtual void clickZoomOut();

   virtual void customCursor(QCursor cursor);

   virtual void resizeEvent(QResizeEvent* event);

   virtual void newGridLayout(int rows, int cols);

protected slots:

   virtual void onMouseMoveEvent(QGraphicsSceneMouseEvent* event);

   virtual void subwindow_redraw(SubImageWindow*);

private slots:

   /**
    * A slot that performs a zoom to a particular item on the scene.
    *
    * @param zoomObject - object on the scene to zoom into.
    */
   virtual void zoomIn(QGraphicsItem* zoomObject);

   /**
    * Zoom in the region of the image selected by the user.
    * In case no selection was made, zoom-in on the image
    * by a fixed scale and anchor the image at center.
    *
    * @param zoomRect - zoom selection rectangle.
    */
   virtual void zoomInRect(QRectF zoomRect, QGraphicsSceneMouseEvent* event);

   /**
    * Zoom out
    */
   virtual void zoomOut();

   /**
    * Called when zoom percentage is updated by the user.
    */
   virtual void zoomValueChanged(int val);

   virtual void onComboBoxChange(QString lbl);

private:

   /**
    * @brief getCurrentZoomPercent
    * @return
    */
   qreal getCurrentZoomPercent();
   
   /**
    * Create layout for widget
    */
   virtual void createLayout();

   /**
    * Create graphics scene and graphics view
    */
   virtual void createSceneAndView(int rows, int cols);

   /**
    * Create and setup the rangewidget.
    */
   void createRangewidget();

private:

   SubImageWindow _sub_window;
   
};

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
