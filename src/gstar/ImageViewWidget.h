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
#include <QAction>
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
   ~ImageViewWidget();

   /**
    * @brief coordinateWidget
    * @return
    */
   CoordinateWidget* coordinateWidget();

   /**
    * Even when the mouse enter the widget
    */
   void enterEvent(QEvent * event);

   /**
    * Widget for controlling image setting. 
    */
   QToolBar* getImageControlsWidget();

   /**
    * @brief getCenterPoint
    * @return
    */
   QPointF getCenterPoint() const;

   QRectF getSceneRect();

   /**
    * Even when the mouse leave the widget
    */
   void leaveEvent(QEvent * event);

   /**
    * getMouseLeaveState
    */
   bool getMouseLeaveState();

   void setCoordsVisible(bool val);

   void setSelectorVisible(bool val);

   void setCountsVisible(bool val);

   /**
    * Returns a pointer to the scene that is currently visualized in the view.
    * If no scene is currently visualized, 0 is returned.
    *
    * @return the current scene.
    */
   ImageViewScene* scene(int grid_idx = 0);

   void setSceneModel(QAbstractItemModel* model);

   void setSceneSelectionModel(QItemSelectionModel* selectionModel);

   void setSceneModelAndSelection(QAbstractItemModel* model, QItemSelectionModel* selectionModel);

   void sceneEnableAnnotations(bool state);

   void setSceneUnitsLabel(QString label);

   void setSceneUnitsPerPixelX(double val);

   void setSceneUnitsPerPixelY(double val);

   void sceneUpdateModel();

   void setScenetPixmap(QPixmap p);

   void setCountsTrasnformAt(int idx, const ArrayXXr<float>& normalized);

   /**
    * sets the coordinate model for coordinate widget
    */
   void setCoordinateModel(CoordinateModel* model);

   /**
    *  Set the number of images for range widget.
    */
   void setNumberOfImages(int images);

   /**
    * Set the dim of the two comboxes.
    */
   void setRank(int m_rankDim);

   /**
    * Set the enable status of the rangeWidget.
    */
   void setRangeWidgetEnable(bool m_rangeEnable = false);

   /**
    * Set starting index for the range widget
    */
   void setRangeWidgetStartIndex(int index);

   /**
    * Set the visible status of the rangeWidget.
    */
   void setRangewidgetVisible(bool m_rangeVisible = false);

   /**
    * The widget to display current zoom value.
    *
    * This widget is created in ImageViewToolBar.
    */
   void setZoomPercentWidget(QComboBox* zoomPercent);


   void setUnitLabel(int idx, QString label);

   void setUnitLabels(QString label);

   void redrawSubWindows();

   /**
    * @ brief view
    *
    */
   QGraphicsView* view();

   int getViewCount(){return _grid_rows * _grid_cols;}

   QString getLabelAt(int idx);

   void getMouseTrasnformAt(int idx, CountsLookupTransformer** counts_lookup, CountsStatsTransformer** counts_stats);

   void resetCoordsToZero();

   std::vector<QString> getLabelList();

   void restoreLabels(const std::vector<QString>& labels);

   bool set_null_mouse_pos;

   void clearLabels();

   void addLabel(QString lbl);

   void setLabel(QString lbl);
   
   void setGlobalContrast(bool val);

   void getMinMaxAt(int grid_idx, float &counts_min, float &counts_max);

   QImage generate_img(ArrayXXr<float>& int_img, QVector<QRgb>& colormap);

public slots:

   /**
    * Handle click on cursor tool bar icon
    */
   void clickCursor();

   /**
    * Handle click on zoom in tool bar icon. Switch to zoom in mode.
    */
   void clickZoomIn();

   /**
    * Handle click on fill tool bar icon. Switch to fill mode.
    *
    * @param check - The flag for the fill button state. 
    */
   void clickFill(bool check);
   /**
    * Handle click on zoom back to original tool bar icon.
    */
   void clickZoomOriginal();

   /**
    * Handle click on zoom out tool bar icon. Switch to zoom out mode.
    */
   void clickZoomOut();

   void customCursor(QCursor cursor);

//protected:

   /**
    * Overridden resize handler
    *
    * See Qt documentation
    */
   void resizeEvent(QResizeEvent* event);

   void newGridLayout(int rows, int cols);

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

   void parent_redraw();

protected slots:

   /**
    * The coordinates that when mouse moves over
    *
    * @param x - x coordinate.
    * @param y - y coordinate.
    */
   //virtual void mouseOverPixel(int x, int y);
    void onMouseMoveEvent(QGraphicsSceneMouseEvent* event);

private slots:

   /**
    * Handle change to scene rectangle
    *
    * @param rect - scene rectangle
    */
   void sceneRectUpdated(const QRectF& rect);

   /**
    * Handle change to image zoom percentage factor.
    */
   void updateZoomPercentage();

   /**
    * A slot that performs a zoom to a particular item on the scene.
    *
    * @param zoomObject - object on the scene to zoom into.
    */
   void zoomIn(QGraphicsItem* zoomObject);

   /**
    * Zoom in the region of the image selected by the user.
    * In case no selection was made, zoom-in on the image
    * by a fixed scale and anchor the image at center.
    *
    * @param zoomRect - zoom selection rectangle.
    */
   void zoomInRect(QRectF zoomRect, QGraphicsSceneMouseEvent* event);

   /**
    * Zoom out
    */
   void zoomOut();

   /**
    * Called when zoom percentage is updated by the user.
    */
   void zoomValueChanged(int val);

   void onComboBoxChange(QString lbl);

private:

   /**
    * @brief getCurrentZoomPercent
    * @return
    */
   qreal getCurrentZoomPercent();
   
   /**
    * Create layout for widget
    */
   void createLayout();

   /**
    * Create graphics scene and graphics view
    */
   void createSceneAndView(int rows, int cols);

   /**
    * Create and setup the rangewidget.
    */
   void createRangewidget();


private:

   /**
    * Coordinate widget
    */
   CoordinateWidget* m_coordWidget;

   /**
    * Widget
    */
   QWidget* m_widget;

   std::vector<SubImageWindow> _sub_windows;

   /**
    * Zoom in cursor
    */
   QCursor m_zoomInCursor;

   /**
    * Zoom out cursor
    */
   QCursor m_zoomOutCursor;

   /**
    * Zoom percent combobox
    */
   QComboBox* m_zoomPercent;

   /**
    * Fill action requires us to maintain a state
    *  that is used during resize event of the widget
    */
   bool m_fillState;

   /**
    * Zoom percent combobox
    */
   bool m_mouseLeaveState;

   QGridLayout *_image_view_grid_layout;

   int _grid_rows;

   int _grid_cols;

   QVBoxLayout* _main_layout;

};

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
