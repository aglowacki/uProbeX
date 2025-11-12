/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_IMAGE_VIEW_WIDGET_H
#define GSTAR_IMAGE_VIEW_WIDGET_H

//---------------------------------------------------------------------------

#include "gstar/CoordinateWidget.h"
#include "gstar/ImageViewScene.h"
#include "gstar/RangeWidget.h"

#include <QResizeEvent>
#include <QCursor>
#include <QGraphicsView>
#include <QIcon>
#include <QPixmap>
#include <QStandardItemModel>
#include <QTableView>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QListView>
#include <QComboBox>
#include <gstar/CountsLookupTransformer.h>
#include "gstar/SubImageWindow.h"
#include "gstar/Annotation/RoiMaskGraphicsItem.h"


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

namespace gstar
{
/**
 * @brief The ImageViewWidget class ...
 */
class ImageViewWidget
: public QWidget
{

   Q_OBJECT

public:

   enum ZoomMode
   {
      ZOOM_NONE,
      ZOOM_IN,
      ZOOM_OUT,
      ZOOM_FIT
   };

   /**
    * Constructor
    *
    * @param parent the Qt parent widget.
    */
   ImageViewWidget(int rows = 1, int cols = 1, QWidget* parent = 0);

   /**
    * Destructor
    */
   virtual ~ImageViewWidget();

   CoordinateWidget* coordinateWidget();

   void setCoordsVisible(bool val);

   void setCoordinateModel(CoordinateModel* model);

   QImage generate_img(ArrayXXr<float>& int_img, QVector<QRgb>& colormap);

   int getViewCount(){return _grid_rows * _grid_cols;}

   void enterEvent(QEvent * event);
   
   void leaveEvent(QEvent * event);

   virtual void connectRoiGraphicsItemToMouseEvents(RoiMaskGraphicsItem* roi) = 0;

   virtual void disconnectRoiGraphicsItemToMouseEvents(RoiMaskGraphicsItem* roi) = 0;
   
   virtual QRectF getSceneRect() = 0;

   virtual void setZoomPercentWidget(QComboBox* zoomPercent);

   virtual QPointF getCenterPoint() const = 0;

   virtual void setSelectorVisible(bool val) = 0;

   virtual void setCountsVisible(bool val) = 0;

   virtual void setSceneModel(QAbstractItemModel* model) = 0;

   virtual void setSceneSelectionModel(QItemSelectionModel* selectionModel) = 0;

   virtual void setSceneModelAndSelection(QAbstractItemModel* model, QItemSelectionModel* selectionModel) = 0;

   virtual void sceneEnableAnnotations(bool state) = 0;

   virtual void setSceneUnitsLabel(QString label) = 0;

   virtual void setSceneUnitsPerPixelX(double val) = 0;

   virtual void setSceneUnitsPerPixelY(double val) = 0;

   virtual void sceneUpdateModel() = 0;

   virtual void setScenePixmap(const QPixmap& p) = 0;

   virtual void setSubScenePixmap(int idx, const QPixmap& p) = 0;

   virtual void setCountsTrasnformAt(unsigned int idx, const ArrayXXr<float>& normalized) = 0;

   virtual void setUnitLabel(int idx, QString label) = 0;

   virtual void setUnitLabels(QString label) = 0;

   virtual QGraphicsView* view() = 0;

   virtual QString getLabelAt(int idx) = 0;

   virtual void getMouseTrasnformAt(int idx, CountsLookupTransformer** counts_lookup, CountsStatsTransformer** counts_stats) = 0;

   virtual void resetCoordsToZero() = 0;

   virtual std::vector<QString> getLabelList() = 0;

   virtual void restoreLabels(const std::vector<QString>& labels) = 0;

   virtual void clearLabels() = 0;

   virtual void addLabel(QString lbl) = 0;

   virtual void setLabel(QString lbl) = 0;
   
   virtual void setGlobalContrast(bool val) = 0;

   virtual bool getMinMaxAt(int grid_idx, float &counts_min, float &counts_max) = 0;

   bool set_null_mouse_pos;

public slots:

   /**
    * Handle click on cursor tool bar icon
    */
   virtual void clickCursor() = 0;

   /**
    * Handle click on zoom in tool bar icon. Switch to zoom in mode.
    */
   virtual void clickZoomIn() = 0;

   /**
    * Handle click on fill tool bar icon. Switch to fill mode.
    *
    * @param check - The flag for the fill button state. 
    */
   virtual void clickFill() = 0;
   /**
    * Handle click on zoom back to original tool bar icon.
    */
   virtual void clickZoomOriginal() = 0;

   /**
    * Handle click on zoom out tool bar icon. Switch to zoom out mode.
    */
   virtual void clickZoomOut() = 0;

   virtual void customCursor(QCursor cursor) = 0;

   virtual void resizeEvent(QResizeEvent* event) = 0;

   virtual void newGridLayout(int rows, int cols) = 0;

signals:

   /**
    ** This signal is emitted whenever the combox1's value is changed.
    * The new value is passed in value.
    * @param value the widget's new value.
    */
   void valueChangedWidth(int value);

   /**
    ** This signal is emitted whenever the combox1's value is changed.
    * The new value is passed in value.
    * @param value the widget's new value.
    */
   void valueChangedHeight(int value);

   /**
    ** This signal is emitted whenever the combox1's value is changed.
    * The new value is passed in value.
    * @param value the widget's new value.
    */
   void imageViewSignal(int value, int id);

   /**
    * Signal is emitted whenever the zoom toolbar should be reset.
    */
   void resetZoomToolBar();

   void cbLabelChanged(QString value, int viewIdx);

   void parent_redraw(int idx);

protected slots:

   virtual void onMouseMoveEvent(QGraphicsSceneMouseEvent* event) = 0;

   virtual void subwindow_redraw(SubImageWindow*) = 0;

   void updateZoomPercentage();

private slots:

   /**
    * Handle change to scene rectangle
    *
    * @param rect - scene rectangle
    */
   void sceneRectUpdated(const QRectF& rect);

   /**
    * A slot that performs a zoom to a particular item on the scene.
    *
    * @param zoomObject - object on the scene to zoom into.
    */
   virtual void zoomIn(QGraphicsItem* zoomObject) = 0;

   /**
    * Zoom in the region of the image selected by the user.
    * In case no selection was made, zoom-in on the image
    * by a fixed scale and anchor the image at center.
    *
    * @param zoomRect - zoom selection rectangle.
    */
   virtual void zoomInRect(QRectF zoomRect, QGraphicsSceneMouseEvent* event) = 0;

   /**
    * Zoom out
    */
   virtual void zoomOut() = 0;

   /**
    * Called when zoom percentage is updated by the user.
    */
   virtual void zoomValueChanged(int val) = 0;

   virtual void onComboBoxChange(QString lbl) = 0;

private:

   /**
    * @brief getCurrentZoomPercent
    * @return
    */
   virtual qreal getCurrentZoomPercent() = 0;
   
   /**
    * Create layout for widget
    */
   virtual void createLayout() = 0;

   /**
    * Create graphics scene and graphics view
    */
   virtual void createSceneAndView(int rows, int cols) = 0;

protected:

   CoordinateWidget* m_coordWidget;

   QWidget* m_widget;

   QCursor m_zoomInCursor;

   QCursor m_zoomOutCursor;

   QComboBox* m_zoomPercent;

   int _grid_rows;

   int _grid_cols;

   QVBoxLayout* _main_layout;
};

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
