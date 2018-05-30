/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/AbstractImageWidget.h"

#include "gstar/Annotation/AbstractGraphicsItem.h"
#include "gstar/Annotation/MarkerGraphicsItem.h"
#include "gstar/Annotation/RulerGraphicsItem.h"
#include "gstar/AnnotationTreeModel.h"
#include "gstar/AnnotationToolBarWidget.h"
#include "gstar/ImageViewWidget.h"
#include "gstar/ImageViewToolBar.h"
#include "gstar/RangeWidget.h"
#include "gstar/RulerUnitsDialog.h"

#include <QAction>
#include <QActionGroup>
#include <QColorDialog>
#include <QComboBox>
#include <QDebug>
#include <QItemSelectionModel>
#include <QMenu>
#include <QMessageBox>
#include <QModelIndex>
#include <QSplitter>
#include <QTabBar>
#include <QTableWidget>
#include <QToolBar>
#include <QTreeView>
#include <QVBoxLayout>

using namespace gstar;

/*---------------------------------------------------------------------------*/

AbstractImageWidget::AbstractImageWidget(QWidget* parent)
: QWidget(parent)
{
   
   // Background
   m_annotationToolbar = NULL;
   m_coordinateModel = NULL;
   m_imageWidgetToolBar = NULL;
   QPalette pal = this->palette();
   pal.setColor(this->backgroundRole(), Qt::white);
   this->setPalette(pal);
   setAutoFillBackground(true);

   createActions();


   //Setup a QTreeView and AnnotationTreeModel for Annotations

   m_treeModel = new AnnotationTreeModel();
   connect(m_treeModel,
           SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
           this,
           SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));

   m_selectionModel = new QItemSelectionModel(m_treeModel);

   m_annoTreeView = new QTreeView();
   m_annoTreeView->setPalette(pal);
   m_annoTreeView->setAutoFillBackground(true);
   m_annoTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
   m_annoTreeView->setAnimated(true);
   m_annoTreeView->setModel(m_treeModel);
   m_annoTreeView->setHeaderHidden(true);
   m_annoTreeView->setSelectionModel(m_selectionModel);
   m_annoTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(m_annoTreeView,
          SIGNAL(customContextMenuRequested(const QPoint &)),
          this,
          SLOT(treeContextMenu(const QPoint &)));
   connect(m_annoTreeView,
           SIGNAL(doubleClicked(const QModelIndex &)),
           this,
           SLOT(treeDoubleClicked(const QModelIndex &)));

   m_imageViewWidget = new ImageViewWidget();
   m_imageViewWidget->scene()->setModel(m_treeModel);
   m_imageViewWidget->scene()->setSelectionModel(m_selectionModel);
   m_imageViewWidget->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(m_imageViewWidget,
          SIGNAL(customContextMenuRequested(const QPoint &)),
          this,
          SLOT(viewContextMenu(const QPoint &)));

   createAnnotationToolBar();

   m_tabWidget = new QTabWidget();
   m_tabWidget->setPalette(pal);
   m_tabWidget->setAutoFillBackground(true);

   m_annotationsEnabled = true;
   //add it in parent class so you can control what tab it is on.
   //m_tabWidget->addTab(layoutWidget, QIcon(), "Annotations");

   //m_lblPixelXCoordinate = NULL;

}

/*---------------------------------------------------------------------------*/

AbstractImageWidget::~AbstractImageWidget()
{

   if(m_treeModel != NULL)
   {
      delete m_treeModel;
      m_treeModel = NULL;
   }

   if(m_annotationToolbar != NULL)
   {
      delete m_annotationToolbar;
      m_annotationToolbar = NULL;
   }

   if(m_imageWidgetToolBar != NULL)
   {
      delete m_imageWidgetToolBar;
      m_imageWidgetToolBar = NULL;
   }

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::addMarker()
{

   MarkerGraphicsItem* annotation = new MarkerGraphicsItem();
   insertAndSelectAnnotation(m_treeModel, m_annoTreeView, m_selectionModel, annotation);

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::addRuler()
{

   RulerGraphicsItem* annotation = new RulerGraphicsItem();
   insertAndSelectAnnotation(m_treeModel, m_annoTreeView, m_selectionModel, annotation);

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::appendAnnotationTab()
{

   QVBoxLayout* infoLayout = new QVBoxLayout();
   infoLayout->addWidget(m_annotationToolbar->getToolBar());
   infoLayout->addWidget(m_annoTreeView);

   m_treeTabWidget = new QWidget(this);
   //m_treeTabWidget->setPalette(pal);
   m_treeTabWidget->setAutoFillBackground(true);
   m_treeTabWidget->setLayout(infoLayout);

   m_tabWidget->addTab(m_treeTabWidget, QIcon(), "Annotations");

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::appendTab(QWidget *widget, QString name)
{
    m_tabWidget->addTab(widget, QIcon(), name);
}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::createActions()
{

   m_addRulerAction = new QAction("Add Ruler", this);
   m_addMarkerAction = new QAction("Add Marker", this);
   m_duplicateAction = new QAction("Duplicate", this);
   m_deleteAction = new QAction("Delete", this);
   m_showRulerDialogAction = new QAction("Ruler Units", this);

   connect(m_addRulerAction,
           SIGNAL(triggered()),
           this,
           SLOT(addRuler()));
   connect(m_addMarkerAction,
           SIGNAL(triggered()),
           this,
           SLOT(addMarker()));

   connect(m_duplicateAction,
           SIGNAL(triggered()),
           this,
           SLOT(duplicateItem()));
   connect(m_deleteAction,
           SIGNAL(triggered()),
           this,
           SLOT(deleteItem()));
   connect(m_showRulerDialogAction,
           SIGNAL(triggered()),
           this,
           SLOT(showRulerUnitsDialog()));

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::createToolBar(ImageViewWidget* imageViewWidget)
{

   m_toolbar = new QToolBar(this);
   m_toolbar->setFloatable(false);
   m_toolbar->setMovable(false);

   m_imageWidgetToolBar = new ImageViewToolBar(imageViewWidget);


   m_toolbar->addWidget(m_imageWidgetToolBar->getToolBar());
   m_toolbar->addSeparator();

   createRangeWidget();

   m_toolbar -> addWidget(m_range);
   m_labelWidthAction = m_toolbar -> addWidget(new QLabel(" Width :"));
   m_imageWidthDimAction = m_toolbar -> addWidget(m_imageWidthDim);
   m_labelHeightAction = m_toolbar -> addWidget(new QLabel(" Height :"));
   m_imageHeightDimAction = m_toolbar -> addWidget(m_imageHeightDim);

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::createScanToolBar()
{

//   m_scanToolbar = new QToolBar(this);
//   m_scanToolbar->setFloatable(false);
//   m_scanToolbar->setMovable(false);

//   m_imageScanToolBar = new ImageViewToolBar(m_imageViewWidget);


//   m_scanToolbar->addWidget(m_imageScanToolBar->getToolBar());

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::createAnnotationToolBar()
{

   m_annotationToolbar = new AnnotationToolBarWidget();

   connect(m_annotationToolbar,
           SIGNAL(clickRuler()),
           this,
           SLOT(addRuler()));
   connect(m_annotationToolbar,
           SIGNAL(clickMarker()),
           this,
           SLOT(addMarker()));

   connect(m_annotationToolbar,
           SIGNAL(enabledStateChanged(bool)),
           this,
           SLOT(setAnnotationsEnabled(bool)));

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::createRangeWidget()
{

   m_range = new RangeWidget();
   m_range->setMaximum(0);
   m_range->setMinimum(0);
   connect(m_range, 
           SIGNAL(valueChanged(int, int)),
           this, 
           SIGNAL(rangeChanged(int, int)));

   m_imageHeightDim = new QComboBox();
   m_imageHeightDim->setEditable(true);

   connect(m_imageHeightDim, 
           SIGNAL(activated(int)),
           this, 
           SIGNAL(imageHeightDimChanged(int)));

   m_imageWidthDim = new QComboBox();
   m_imageWidthDim->setEditable(true);
   connect(m_imageWidthDim, 
           SIGNAL(activated(int)),
           this, 
           SIGNAL(imageWidthDimChanged(int)));

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::deleteItem()
{

   if (m_selectionModel->hasSelection())
   {
      QModelIndexList selectedIndexes = m_selectionModel->selectedRows();

      //display message box
      QMessageBox msgBox;
      msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
      msgBox.setDefaultButton(QMessageBox::No);
      msgBox.setText("Are you sure you want to delete the selected items?");
      int ret = msgBox.exec();

      if (ret == QMessageBox::Yes)
      {
         for (int i = selectedIndexes.count() - 1; i >= 0; i--)
         {
            QModelIndex index = selectedIndexes[i];
            qDebug()<<"index "<<index.row();
            m_treeModel->removeRow(index.row(), index);
         }
      }
   }

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::displayContextMenu(QWidget* parent,
                                             const QPoint& pos)
{

   if (m_annotationsEnabled == false)
      return;

   QMenu menu(parent);
   menu.addAction(m_addMarkerAction);
   menu.addAction(m_addRulerAction);

   if (m_treeModel != NULL && m_treeModel->rowCount() > 0)
   {
      if (m_selectionModel->hasSelection())
      {
         menu.addSeparator();
         menu.addAction(m_duplicateAction);
         menu.addSeparator();
         menu.addAction(m_deleteAction);
      }
   }

   //ruler properties
   menu.addSeparator();
   menu.addAction(m_showRulerDialogAction);

   QAction* result = menu.exec(pos);
   if (result == NULL)
   {
      m_selectionModel->clearSelection();
   }

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::duplicateItem()
{

   if (m_selectionModel->hasSelection())
   {
      QModelIndexList selectedIndexes = m_selectionModel->selectedRows();

      m_selectionModel->clearSelection();
      foreach(QModelIndex index, selectedIndexes)
      {
         QModelIndex duplicateIndex = m_treeModel->duplicateNode(index);
         m_selectionModel->select(duplicateIndex, QItemSelectionModel::Select);
      }
   }

}

/*---------------------------------------------------------------------------*/

QLayout* AbstractImageWidget::generateDefaultLayout()
{

   QVBoxLayout* mainLayout = new QVBoxLayout();

   QSplitter* splitter = new QSplitter();
   splitter->setOrientation(Qt::Horizontal);

   splitter->addWidget(m_imageViewWidget);
   splitter->setStretchFactor(0, 1);
   splitter->addWidget(m_tabWidget);

   createToolBar(m_imageViewWidget);

   mainLayout->addWidget(m_toolbar);
   mainLayout->addWidget(splitter);

   //setLayout(mainLayout);
   return mainLayout;

}

/*---------------------------------------------------------------------------*/

QLayout* AbstractImageWidget::getImageViewLayout()
{

   return m_imageViewWidget->layout();

}

/*---------------------------------------------------------------------------*/

AnnotationTreeModel* AbstractImageWidget::getAnnotationModel()
{

   return m_treeModel;

}

/*---------------------------------------------------------------------------*/

ImageViewWidget* AbstractImageWidget::imageViewWidget() const
{

   return m_imageViewWidget;

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::insertAndSelectAnnotation(AnnotationTreeModel* treeModel,
                                                    QTreeView* annoTreeView,
                                                    QItemSelectionModel* selectionModel,
                                                    AbstractGraphicsItem* annotation)
{

   QPointF center = m_imageViewWidget->getCenterPoint();
   annotation->setPos(center);

   QModelIndex pIndex = treeModel->appendNode(annotation);

   if (pIndex.isValid())
   {
      annoTreeView->expand(pIndex);
      selectionModel->clear();

      for (int i = 0; i < annotation->columnCount(); i++)
      {
         QModelIndex index = treeModel->index(annotation->row(), i, pIndex);
         if (index.isValid())
         {
            selectionModel->select(index,QItemSelectionModel::Select);
         }
      }
   }

   int cCount = treeModel->columnCount();
   for(int i=0; i<cCount; i++)
   {
      annoTreeView->resizeColumnToContents(i);
   }

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::reloadAndSelectAnnotation(AnnotationTreeModel* treeModel,
                                                    QTreeView* annoTreeView,
                                                    QItemSelectionModel* selectionModel,
                                                    AbstractGraphicsItem* annotation,
                                                    QPointF pos)
{

   annotation->setPos(pos);

   QModelIndex pIndex = treeModel->appendNode(annotation);

   if (pIndex.isValid())
   {
      annoTreeView->expand(pIndex);
      selectionModel->clear();

      for (int i = 0; i < annotation->columnCount(); i++)
      {
         QModelIndex index = treeModel->index(annotation->row(), i, pIndex);

         if (index.isValid())
         {
            selectionModel->select(index,QItemSelectionModel::Select);
         }
      }

   }

   int cCount = treeModel->columnCount();
   for(int i=0; i<cCount; i++)
   {
      annoTreeView->resizeColumnToContents(i);
   }

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::modelDataChanged(const QModelIndex& topLeft,
                                           const QModelIndex& bottomRight)
{

   m_annoTreeView->resizeColumnToContents(topLeft.column());
   m_annoTreeView->resizeColumnToContents(bottomRight.column());

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::setAnnotationModel(AnnotationTreeModel *model)
{

   m_treeModel = model;
   m_annoTreeView->setModel(m_treeModel);
   m_imageViewWidget->scene()->setModel(m_treeModel);

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::setCoordinateModel(CoordinateModel *model)
{
   m_coordinateModel = model; 
   if(m_imageViewWidget != NULL)
   {
      m_imageViewWidget->setCoordinateModel(model);
   }

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::setHeightDims(int h)
{

   if (h > (m_imageHeightDim->count() - 1)) return;

   disconnect(m_imageHeightDim, 
             SIGNAL(activated(int)),
             this, 
             SLOT(imageHeightDimChanged(int)));

   m_imageHeightDim->setCurrentIndex(h);

   connect(m_imageHeightDim, 
           SIGNAL(activated(int)),
           this, 
           SIGNAL(imageHeightDimChanged(int)));

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::setNumberOfImages(int images)
{

   m_range->setMinimum(1);
   m_range->setMaximum(images);

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::setAnnotationsEnabled(bool value)
{

   m_annoTreeView->setEnabled(value);
   m_imageViewWidget->scene()->enableAnnotations(value);
   m_annotationsEnabled = value;

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::setRank(int m_rankDim)
{
   
   m_imageWidthDim->clear();
   m_imageHeightDim->clear();

   for (int i = 0 ; i < m_rankDim ; i++) 
   {
        m_imageWidthDim->addItem(tr("dim %1").arg(i));
        m_imageHeightDim->addItem(tr("dim %1").arg(i));
   }

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::setRangeWidgetStartIndex(int index)
{

   m_range->setValue(index);

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::setRangeWidgetVisible(bool m_rangeVisible)
{

   m_range->setRangeWidgetVisible(false);

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::setToolBarVisible(bool visible)
{

  m_toolbar->setVisible(visible);

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::setWidthDims(int w)
{

   if (w > (m_imageWidthDim->count() - 1)) return;


   disconnect(m_imageWidthDim, 
              SIGNAL(activated(int)),
              this,
              SIGNAL(imageWidthDimChanged(int)));

   m_imageWidthDim->setCurrentIndex(w);


   connect(m_imageWidthDim, 
           SIGNAL(activated(int)),
           this, 
           SIGNAL(imageWidthDimChanged(int)));

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::showRulerUnitsDialog()
{

   // Show GenerateSeriesDialog with current settings
   RulerUnitsDialog* dialog = new RulerUnitsDialog(this, Qt::Dialog);
   dialog->setUnitLabel(m_imageViewWidget->scene()->getUnitsLabel());
   dialog->setUnitsPerPixelX(m_imageViewWidget->scene()->getUnitsPerPixelX());
   dialog->setUnitsPerPixelY(m_imageViewWidget->scene()->getUnitsPerPixelY());

   // Update with current settings
   if (dialog->exec() == QDialog::Accepted)
   {
      m_imageViewWidget->scene()->setUnitsLabel(dialog->getUnitLabel());
      m_imageViewWidget->scene()->setUnitsPerPixelX(dialog->getUnitsPerPixelX());
      m_imageViewWidget->scene()->setUnitsPerPixelY(dialog->getUnitsPerPixelY());
      m_imageViewWidget->scene()->updateModel();
      emit rulerUnitsChanged(dialog->getUnitLabel(),
                             dialog->getUnitsPerPixelX(),
                             dialog->getUnitsPerPixelY());
   }

    // Delete dialog
    delete dialog;

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::treeContextMenu(const QPoint& pos)
{

   displayContextMenu(m_annoTreeView, m_annoTreeView->viewport()->mapToGlobal(pos));

}


/*---------------------------------------------------------------------------*/

void AbstractImageWidget::treeDoubleClicked(const QModelIndex& index)
{

   if (index.isValid())
   {
      AbstractGraphicsItem* item =
            static_cast<AbstractGraphicsItem*>(index.internalPointer());

      if (item != NULL)
      {
         QVariant data = item->data(0, index.column());
         if (data.type() == QVariant::Color)
         {
            QColor color = QColorDialog::getColor(data.toString(), this);
            if (color.isValid())
            {
               item->setData(index, color);
            }
         }
      }
   }

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::updateFrame(const Array* image)
{

   if (image == NULL)
      return;

   // Get type
   Array::DataType type = image->getType();

   // Get rank
   unsigned long rank = image->getRank();

   if (type != Array::BGRA32)
      return;

   // Get dimensions
   unsigned long long dims[2];
   image->getDims(dims, 2);

   // Check image properties; only process BGRA32 images
   if (rank != 2)
      return;

   if (dims[0] <= 0 || dims[1] <= 0)
      return;

   // Create image
   QImage img = QImage((uchar*) image->getBuffer(), dims[1], dims[0],
            QImage::Format_ARGB32);

   // Create pixmap from image
   m_imageViewWidget->scene()->setPixmap(
            QPixmap::fromImage(img.convertToFormat(QImage::Format_RGB32)));

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::updateFrame(QImage *img)
{

   // Create pixmap from image
   m_imageViewWidget->scene()->setPixmap(QPixmap::fromImage(img->convertToFormat(QImage::Format_RGB32)));

}

/*---------------------------------------------------------------------------*/

void AbstractImageWidget::viewContextMenu(const QPoint& pos)
{

   displayContextMenu(m_imageViewWidget, m_imageViewWidget->mapToGlobal(pos));

}

/*---------------------------------------------------------------------------*/
