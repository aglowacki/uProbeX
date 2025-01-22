/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/AbstractImageWidget.h"

#include <QAction>
#include <QActionGroup>
#include <QColorDialog>
#include <QComboBox>
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

//---------------------------------------------------------------------------

AbstractImageWidget::AbstractImageWidget(int rows, int cols, QWidget* parent)
: QWidget(parent)
{
   // Background
   m_treeModel = nullptr;
   m_annotationToolbar = nullptr;
   m_coordinateModel = nullptr;
   m_imageWidgetToolBar = nullptr;
   m_range = nullptr;
   m_imageWidthDim = nullptr;
   m_imageHeightDim = nullptr;
   m_tabWidget = new QTabWidget(this);
   //m_tabWidget->setPalette(pal);
   //m_tabWidget->setAutoFillBackground(true);
   //m_tabWidget->addTab(layoutWidget, QIcon(), "Annotations");
   //QPalette pal = this->palette();
   //pal.setColor(this->backgroundRole(), Qt::white);
   //this->setPalette(pal);
   //setAutoFillBackground(true);

   createActions();

   //Setup a QTreeView and AnnotationTreeModel for Annotations

   m_treeModel = new AnnotationTreeModel();
   connect(m_treeModel, &AnnotationTreeModel::dataChanged, this, &AbstractImageWidget::modelDataChanged);

   m_selectionModel = new QItemSelectionModel(m_treeModel);

   m_annoTreeView = new QTreeView();
   //m_annoTreeView->setPalette(pal);
   //m_annoTreeView->setAutoFillBackground(true);
   m_annoTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
   m_annoTreeView->setAnimated(true);
   m_annoTreeView->setModel(m_treeModel);
   m_annoTreeView->setHeaderHidden(true);
   m_annoTreeView->setSelectionModel(m_selectionModel);
   m_annoTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(m_annoTreeView, &QTreeView::customContextMenuRequested, this, &AbstractImageWidget::treeContextMenu);
   connect(m_annoTreeView, &QTreeView::doubleClicked, this, &AbstractImageWidget::treeDoubleClicked);

   m_imageViewWidget = new ImageViewWidget(rows, cols);
   m_imageViewWidget->setSceneModel(m_treeModel);
   m_imageViewWidget->setSceneSelectionModel(m_selectionModel);
   m_imageViewWidget->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(m_imageViewWidget, &ImageViewWidget::customContextMenuRequested, this, &AbstractImageWidget::viewContextMenu);

   createAnnotationToolBar();


   m_annotationsEnabled = true;
   //add it in parent class so you can control what tab it is on.
      //m_lblPixelXCoordinate = nullptr;
}

//---------------------------------------------------------------------------

AbstractImageWidget::~AbstractImageWidget()
{

    m_imageViewWidget->setSceneSelectionModel(nullptr);
   
    if(m_treeModel != nullptr)
    {
        delete m_treeModel;
        m_treeModel = nullptr;
    }

   /*
   if(m_annotationToolbar != nullptr)
   {
      delete m_annotationToolbar;
      m_annotationToolbar = nullptr;
   }

   if(m_imageWidgetToolBar != nullptr)
   {
      delete m_imageWidgetToolBar;
      m_imageWidgetToolBar = nullptr;
   }
   */
}

//---------------------------------------------------------------------------

void AbstractImageWidget::addMarker()
{

   MarkerGraphicsItem* annotation = new MarkerGraphicsItem();
   insertAndSelectAnnotation(m_treeModel, m_annoTreeView, m_selectionModel, annotation);

}

//---------------------------------------------------------------------------

void AbstractImageWidget::addRuler()
{

   RulerGraphicsItem* annotation = new RulerGraphicsItem();
   insertAndSelectAnnotation(m_treeModel, m_annoTreeView, m_selectionModel, annotation);

}

//---------------------------------------------------------------------------

void AbstractImageWidget::appendAnnotationTab(bool bToolbar)
{
    QVBoxLayout* infoLayout = new QVBoxLayout();
    if (bToolbar)
    {
        infoLayout->addWidget(m_annotationToolbar->getToolBar());
    }
    infoLayout->addWidget(m_annoTreeView);

    m_treeTabWidget = new QWidget(this);
    //m_treeTabWidget->setPalette(pal);
    //m_treeTabWidget->setAutoFillBackground(true);
    m_treeTabWidget->setLayout(infoLayout);

    m_tabWidget->addTab(m_treeTabWidget, QIcon(), "Annotations");
}

//---------------------------------------------------------------------------

void AbstractImageWidget::appendTab(QWidget *widget, QString name)
{
    m_tabWidget->addTab(widget, QIcon(), name);
}

//---------------------------------------------------------------------------

void AbstractImageWidget::createActions()
{

   m_addRulerAction = new QAction("Add Ruler", this);
   m_addMarkerAction = new QAction("Add Marker", this);
   m_duplicateAction = new QAction("Duplicate", this);
   m_deleteAction = new QAction("Delete", this);
   m_showRulerDialogAction = new QAction("Ruler Units", this);

   connect(m_addRulerAction, &QAction::triggered, this, &AbstractImageWidget::addRuler);
   connect(m_addMarkerAction, &QAction::triggered, this, &AbstractImageWidget::addMarker);

   connect(m_duplicateAction, &QAction::triggered, this, &AbstractImageWidget::duplicateItem);
   connect(m_deleteAction, &QAction::triggered, this, &AbstractImageWidget::deleteItem);
   connect(m_showRulerDialogAction, &QAction::triggered, this, &AbstractImageWidget::showRulerUnitsDialog);

}

//---------------------------------------------------------------------------

void AbstractImageWidget::createToolBar(ImageViewWidget* imageViewWidget, bool create_image_nav)
{

    m_toolbar = new QToolBar(this);
    m_toolbar->setFloatable(true);
    m_toolbar->setMovable(true);

    m_imageWidgetToolBar = new ImageViewToolBar(imageViewWidget);

    m_toolbar->addWidget(m_imageWidgetToolBar->getToolBar());
    m_toolbar->addSeparator();
    if (create_image_nav)
    {

        createRangeWidget();
        m_toolbar->addWidget(m_range);

        m_labelWidthAction = m_toolbar->addWidget(new QLabel(" Width :"));
        m_imageWidthDimAction = m_toolbar->addWidget(m_imageWidthDim);
        m_labelHeightAction = m_toolbar->addWidget(new QLabel(" Height :"));
        m_imageHeightDimAction = m_toolbar->addWidget(m_imageHeightDim);
    }
    
}

//---------------------------------------------------------------------------

void AbstractImageWidget::createAnnotationToolBar()
{

   m_annotationToolbar = new AnnotationToolBarWidget();

   connect(m_annotationToolbar, &AnnotationToolBarWidget::clickRuler, this, &AbstractImageWidget::addRuler);
   connect(m_annotationToolbar, &AnnotationToolBarWidget::clickMarker, this, &AbstractImageWidget::addMarker);

   connect(m_annotationToolbar, &AnnotationToolBarWidget::enabledStateChanged, this, &AbstractImageWidget::setAnnotationsEnabled);

}

//---------------------------------------------------------------------------

void AbstractImageWidget::createRangeWidget()
{

   m_range = new RangeWidget();
   m_range->setMaximum(0);
   m_range->setMinimum(0);
   connect(m_range, &RangeWidget::valueChanged, this, &AbstractImageWidget::rangeChanged);

   m_imageHeightDim = new QComboBox();
   m_imageHeightDim->setEditable(true);

   connect(m_imageHeightDim, &QComboBox::activated, this, &AbstractImageWidget::imageHeightDimChanged);

   m_imageWidthDim = new QComboBox();
   m_imageWidthDim->setEditable(true);
   connect(m_imageWidthDim, &QComboBox::activated, this, &AbstractImageWidget::imageWidthDimChanged);

}

//---------------------------------------------------------------------------

void AbstractImageWidget::deleteAllItems()
{
    
    QModelIndex topLeft;
    QModelIndex bottomRight;
    int rows = m_treeModel->rowCount(QModelIndex());
    int cols = m_treeModel->columnCount(QModelIndex());
    topLeft = m_treeModel->index(0, 0, QModelIndex());
    bottomRight = m_treeModel->index(rows-1, cols-1, QModelIndex());

    QItemSelection selection(topLeft, bottomRight);
    m_selectionModel->select(selection, QItemSelectionModel::Select);

    if (m_selectionModel->hasSelection())
    {
        QModelIndexList selectedIndexes = m_selectionModel->selectedRows();

        for (int i = selectedIndexes.count() - 1; i >= 0; i--)
        {
            QModelIndex index = selectedIndexes[i];
            AbstractGraphicsItem* item = static_cast<AbstractGraphicsItem*>(index.internalPointer());

            if (item != nullptr)
            {
                emit deletedAnnotation(item);
            }
            
            m_treeModel->removeRow(index.row(), index);
        }
    }
}


//---------------------------------------------------------------------------

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
            AbstractGraphicsItem* item = static_cast<AbstractGraphicsItem*>(index.internalPointer());

            if (item != nullptr)
            {
                emit deletedAnnotation(item);
            }

            m_treeModel->removeRow(index.row(), index);
            
         }
      }
   }

}

//---------------------------------------------------------------------------

void AbstractImageWidget::displayContextMenu(QWidget* parent,
                                             const QPoint& pos)
{

   if (m_annotationsEnabled == false)
      return;

   QMenu menu(parent);
   menu.addAction(m_addMarkerAction);
   menu.addAction(m_addRulerAction);

   if (m_treeModel != nullptr && m_treeModel->rowCount() > 0)
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
   if (result == nullptr)
   {
      m_selectionModel->clearSelection();
   }

}

//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------

QLayout* AbstractImageWidget::generateDefaultLayout(bool add_tab_widget)
{

   QVBoxLayout* mainLayout = new QVBoxLayout();

   QSplitter* splitter = new QSplitter();
   splitter->setOrientation(Qt::Horizontal);

   splitter->addWidget(m_imageViewWidget);
   splitter->setStretchFactor(0, 1);
   if (add_tab_widget)
   {
      splitter->addWidget(m_tabWidget);
      splitter->setStretchFactor(1, 1);
   }
   createToolBar(m_imageViewWidget);

   mainLayout->addWidget(m_toolbar);
   mainLayout->addWidget(splitter);

   return mainLayout;

}

//---------------------------------------------------------------------------

QLayout* AbstractImageWidget::getImageViewLayout()
{

   return m_imageViewWidget->layout();

}

//---------------------------------------------------------------------------

AnnotationTreeModel* AbstractImageWidget::getAnnotationModel()
{

   return m_treeModel;

}

//---------------------------------------------------------------------------

ImageViewWidget* AbstractImageWidget::imageViewWidget() const
{

   return m_imageViewWidget;

}

//---------------------------------------------------------------------------

void AbstractImageWidget::insertAndSelectAnnotation(AnnotationTreeModel* treeModel,
                                                    QTreeView* annoTreeView,
                                                    QItemSelectionModel* selectionModel,
                                                    AbstractGraphicsItem* annotation,
                                                    bool centerAnno)
{

    QPointF annoPos(0.0, 0.0);
    if (centerAnno)
    {
        annoPos = m_imageViewWidget->getCenterPoint();
    }
    
    annotation->setPos(annoPos);

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

//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------

void AbstractImageWidget::modelDataChanged(const QModelIndex& topLeft,
                                           const QModelIndex& bottomRight)
{

   m_annoTreeView->resizeColumnToContents(topLeft.column());
   m_annoTreeView->resizeColumnToContents(bottomRight.column());

}

//---------------------------------------------------------------------------

void AbstractImageWidget::setAnnotationModel(AnnotationTreeModel *model)
{

   m_treeModel = model;
   m_annoTreeView->setModel(m_treeModel);
   m_imageViewWidget->setSceneModel(m_treeModel);

}

//---------------------------------------------------------------------------

void AbstractImageWidget::setCoordinateModel(CoordinateModel *model)
{
   m_coordinateModel = model; 
   if(m_imageViewWidget != nullptr)
   {
      m_imageViewWidget->setCoordinateModel(model);
   }

}

//---------------------------------------------------------------------------

void AbstractImageWidget::setHeightDims(int h)
{

    if (m_imageHeightDim != nullptr)
    {
        if (h > (m_imageHeightDim->count() - 1)) return;

        disconnect(m_imageHeightDim, &QComboBox::activated, this, &AbstractImageWidget::imageHeightDimChanged);

        m_imageHeightDim->setCurrentIndex(h);

        connect(m_imageHeightDim, &QComboBox::activated, this, &AbstractImageWidget::imageHeightDimChanged);
    }
}

//---------------------------------------------------------------------------

void AbstractImageWidget::setNumberOfImages(int images)
{
    if (m_range != nullptr)
    {
        m_range->setMinimum(1);
        m_range->setMaximum(images);
    }
}

//---------------------------------------------------------------------------

void AbstractImageWidget::setAnnotationsEnabled(bool value)
{

   m_annoTreeView->setEnabled(value);
   m_imageViewWidget->sceneEnableAnnotations(value);
   m_annotationsEnabled = value;

}

//---------------------------------------------------------------------------

void AbstractImageWidget::setRank(int m_rankDim)
{
    if (m_imageWidthDim != nullptr && m_imageHeightDim != nullptr)
    {
        m_imageWidthDim->clear();

        m_imageHeightDim->clear();

        for (int i = 0; i < m_rankDim; i++)
        {
            m_imageWidthDim->addItem(tr("dim %1").arg(i));
            m_imageHeightDim->addItem(tr("dim %1").arg(i));
        }
    }
}

//---------------------------------------------------------------------------

void AbstractImageWidget::setRangeWidgetStartIndex(int index)
{
    if (m_range != nullptr)
    {
        m_range->setValue(index);
    }
}

//---------------------------------------------------------------------------

int AbstractImageWidget::getRangeWidgetStartIndex()
{
    if (m_range != nullptr)
    {
        return m_range->value();
    }
    return 0;
}

//---------------------------------------------------------------------------

void AbstractImageWidget::setRangeWidgetVisible(bool m_rangeVisible)
{
    if (m_range != nullptr)
    {
        m_range->setRangeWidgetVisible(false);
    }
}

//---------------------------------------------------------------------------

void AbstractImageWidget::setToolBarVisible(bool visible)
{

  m_toolbar->setVisible(visible);

}

//---------------------------------------------------------------------------

void AbstractImageWidget::setWidthDims(int w)
{
    if (m_imageWidthDim != nullptr)
    {
        if (w > (m_imageWidthDim->count() - 1)) return;

        disconnect(m_imageWidthDim, &QComboBox::activated, this, &AbstractImageWidget::imageWidthDimChanged);
        m_imageWidthDim->setCurrentIndex(w);
        connect(m_imageWidthDim, &QComboBox::activated, this, &AbstractImageWidget::imageWidthDimChanged);
    }
}

//---------------------------------------------------------------------------

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
      m_imageViewWidget->setSceneUnitsLabel(dialog->getUnitLabel());
      m_imageViewWidget->setSceneUnitsPerPixelX(dialog->getUnitsPerPixelX());
      m_imageViewWidget->setSceneUnitsPerPixelY(dialog->getUnitsPerPixelY());
      m_imageViewWidget->sceneUpdateModel();
      emit rulerUnitsChanged(dialog->getUnitLabel(),
                             dialog->getUnitsPerPixelX(),
                             dialog->getUnitsPerPixelY());
   }

    // Delete dialog
    delete dialog;

}

//---------------------------------------------------------------------------

void AbstractImageWidget::treeContextMenu(const QPoint& pos)
{

   displayContextMenu(m_annoTreeView, m_annoTreeView->viewport()->mapToGlobal(pos));

}


//---------------------------------------------------------------------------

void AbstractImageWidget::treeDoubleClicked(const QModelIndex& index)
{

   if (index.isValid())
   {
      AbstractGraphicsItem* item =
            static_cast<AbstractGraphicsItem*>(index.internalPointer());

      if (item != nullptr)
      {
         QVariant data = item->data(0, index.column());
         if (data.typeId() == QMetaType::QColor)
         {
            QColor color = QColorDialog::getColor(data.toString(), this);
            if (color.isValid())
            {
               item->setData(index, color);
            }
         }
         else if (data.typeId() == QMetaType::QIcon)
         {
            QDialog* custom_dialog = item->get_custom_dialog();
            if(custom_dialog != nullptr)
            {
               custom_dialog->show();
            }

         }
      }
   }

}

//---------------------------------------------------------------------------

void AbstractImageWidget::updateFrame(QImage *img)
{
    if (img != nullptr)
    {
        // Create pixmap from image
        m_imageViewWidget->setScenetPixmap(QPixmap::fromImage(img->convertToFormat(QImage::Format_RGB32)));
    }

}

//---------------------------------------------------------------------------

void AbstractImageWidget::viewContextMenu(const QPoint& pos)
{

   displayContextMenu(m_imageViewWidget, m_imageViewWidget->mapToGlobal(pos));

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------