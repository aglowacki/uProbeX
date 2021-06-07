/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef TXM_ABSTRACT_IMAGE_WIDGET_H
#define TXM_ABSTRACT_IMAGE_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <QWidget>

#include "gstar/AnnotationTreeModel.h"
#include "gstar/Annotation/AbstractGraphicsItem.h"
#include "gstar/Annotation/MarkerGraphicsItem.h"
#include "gstar/Annotation/RulerGraphicsItem.h"
#include "gstar/AnnotationToolBarWidget.h"
#include "gstar/ImageViewWidget.h"
#include "gstar/ImageViewToolBar.h"
#include "gstar/RangeWidget.h"
#include "gstar/RulerUnitsDialog.h"

class QAbstractTableModel;
class QComboBox;
class QItemSelectionModel;
class QLabel;
class QTabWidget;
class QToolBar;
class QTreeView;
class QModelIndex;


/*---------------------------------------------------------------------------*/

namespace gstar
{

/**
 * @brief When open the acquisition window, the widget is showing for capturing
 * the image from the area detector writer, the window will also be updated to
 * show the image.
 */
class AbstractImageWidget
: public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   AbstractImageWidget(int rows = 1, int cols = 1, QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~AbstractImageWidget();

   /**
    * @brief appendTab
    * @param widget
    * @param name
    */
   void appendTab(QWidget *widget, QString name);

   /**
    * @brief getImageViewWidget
    * @return
    */
   ImageViewWidget* imageViewWidget() const;

   /**
    * @brief getAnnotationModel
    * @return
    */
   AnnotationTreeModel* getAnnotationModel();

   /**
    * @brief set coord model
    */
   virtual void setCoordinateModel(CoordinateModel *model);

   /**
    * @brief set range widget visible status
    */
   void setRangeWidgetVisible(bool m_rangeVisible);

   /**
    * @brief setToolBarVisible
    * @param visible
    */
   void setToolBarVisible(bool visible);

public slots:

   /**
    * @brief setAnnotationModel
    * @param model
    */
   void setAnnotationModel(AnnotationTreeModel* model);

   /**
    *  Set height dimension.
    *
    *  @param d - Dimension
    */
   void setHeightDims(int d);

   /**
    * Set number of images that can be viewed.
    *
    * @param i # of images.
    */
   void setNumberOfImages(int i);

   /**
    * Total number of dimension in the image
    *
    * @param r - Rank or number of dimension for the image.
    */
   void setRank(int r);
 
   /**
    * The current index of the image loaded in the widget.
    *
    * @param index - Image number.
    */
   void setRangeWidgetStartIndex(int index);

   int getRangeWidgetStartIndex() const;

   /**
    * Set the dimension number for the width of the image.
    *
    * @param w - dimension number. 
    */
   void setWidthDims(int w);

   /**
    * Updat the frame when new image shows.
    *
    */
   void updateFrame(const Array* image);

   /**
    * @brief updateFrame
    * @param img
    */
   void updateFrame(QImage *img);

signals:
  
   /**
    * The height dimension was modified
    *
    * @param The new dimension
    */
   void imageHeightDimChanged(int);

   /**
    * The width dimension was modified
    *
    * @param The new dimension
    */
   void imageWidthDimChanged(int);

   /**
    * The range of images that can be viewed in the widget changed.
    *
    * @param The new value.
    * @param Id
    */
   void rangeChanged(int, int);

   /**
    * @brief rulerUnitsChanged
    */
   void rulerUnitsChanged(QString, double, double);

protected slots:

   /**
    * @brief addMarker
    */
   void addMarker();

   /**
    * @brief addRuler
    */
   void addRuler();

   /**
    * @brief deleteItem
    */
   virtual void deleteItem();

   /**
    * @brief duplicateItem
    */
   virtual void duplicateItem();

   /**
    * @brief getImageViewLayout
    * @return
    */
   QLayout* getImageViewLayout();

   /**
    * @brief modelDataChanged
    */
   void modelDataChanged(const QModelIndex &, const QModelIndex &);

   /**
    * @brief setAnnotationsEnabled
    * @param value
    */
   void setAnnotationsEnabled(bool value);

   /**
    * @brief showRulerUnitsDialog
    */
   void showRulerUnitsDialog();

   /**
    * @brief treeContextMenu
    * @param pos
    */
   void treeContextMenu(const QPoint &pos);

   /**
    * @brief treeDoubleClicked
    * @param index
    */
   void treeDoubleClicked(const QModelIndex &index);

   /**
    * @brief viewContextMenu
    * @param pos
    */
   void viewContextMenu(const QPoint& pos);

protected:

   /**
    * @brief appendAnnotationTab
    */
   void appendAnnotationTab();

   /**
    * @brief createActions
    */
   virtual void createActions();

   /**
    * Virtual method that must be overidden by base classes.
    */
   virtual QLayout* generateDefaultLayout();

   /**
    * Create tool bar
    */
   virtual void createToolBar(ImageViewWidget* imageViewWidget);

   /**
    * @brief createAnnotationToolBar
    */
   void createAnnotationToolBar();

   /**
    * Range widget
    */
   void createRangeWidget();

   /**
    * @brief displayContextMenu
    * @param parent
    * @param pos
    */
   virtual void displayContextMenu(QWidget* parent, const QPoint& pos);

   /**
    * @brief insertAndSelectAnnotation
    * @param annotation
    */
   void insertAndSelectAnnotation(AnnotationTreeModel* treeModel,
                                  QTreeView* annoTreeView,
                                  QItemSelectionModel* selectionModel,
                                  AbstractGraphicsItem* annotation);

   void reloadAndSelectAnnotation(AnnotationTreeModel* treeModel,
                                  QTreeView* annoTreeView,
                                  QItemSelectionModel* selectionModel,
                                  AbstractGraphicsItem* annotation,
                                  QPointF pos);

protected:

   /**
    * @brief m_addMarkerAction
    */
   QAction* m_addMarkerAction;

   /**
    * @brief m_addRulerAction
    */
   QAction* m_addRulerAction;

   /**
    * @brief m_annotationsEnabled
    */
   bool m_annotationsEnabled;

   /**
   * @brief m_coordinateModel
   */
   gstar::CoordinateModel* m_coordinateModel;

   /**
    * @brief m_deleteAction
    */
   QAction* m_deleteAction;

   /**
    * @brief m_duplicateAction
    */
   QAction* m_duplicateAction;

   /**
    * @brief m_showRulerDialogAction
    */
   QAction* m_showRulerDialogAction;

   /**
    * @brief m_annotationToolbar
    */
   AnnotationToolBarWidget* m_annotationToolbar;

   /**
    * Pick dimension for image height.
    */
   QComboBox* m_imageHeightDim;

   /**
    * Pick dimension for image width.
    */
   QComboBox* m_imageWidthDim;

   /**
    * Toolbar contains action for manipulating image settings such as zoom.
    */
   ImageViewToolBar* m_imageWidgetToolBar;

   /**
    * Widget for viewing/manipulating image
    */
   ImageViewWidget* m_imageViewWidget;

   /**
    * Range widget.
    */
   RangeWidget* m_range;

   /**
    * Label m_labelWidth.
    */
   QAction* m_labelWidthAction;

   /**
    * Label m_labelHeight.
    */
   QAction* m_labelHeightAction;

   /**
    * Label m_labelHeight.
    */
   QAction* m_imageWidthDimAction;

   /**
    * Label m_labelHeight.
    */
   QAction* m_imageHeightDimAction;

   /**
     * @brief m_selectionModel
     */
   QItemSelectionModel* m_selectionModel;

   /**
    * @brief m_tabWidget
    */
   QTabWidget* m_tabWidget;

   /**
    * Toolbar
    */
   QToolBar* m_toolbar;

   /**
    * Toolbar
    */
   QToolBar* m_scanToolbar;

   /**
    * @brief m_annoTreeView
    */
   QTreeView* m_annoTreeView;

   /**
    * @brief m_treeModel
    */
   AnnotationTreeModel* m_treeModel;

   /**
    * @brief m_treeTabWidget
    */
   QWidget* m_treeTabWidget;

};

}

/*---------------------------------------------------------------------------*/

#endif /* ImageWidget_H_ */

/*---------------------------------------------------------------------------*/
