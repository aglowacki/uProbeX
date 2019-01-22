/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/SWSWidget.h>

#include <gstar/ImageViewWidget.h>

#include <gstar/AnnotationTreeModel.h>
#include <gstar/CoordinateModel.h>
#include <gstar/CoordinateWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QTreeView>
#include <QFileInfo>
#include <QFileDialog>
#include <QMenu>
#include <QBuffer>

#include <QDebug>

#include "gstar/Annotation/UProbeMarkerGraphicsItem.h"
#include "gstar/Annotation/UProbeRegionGraphicsItem.h"
#include "gstar/Annotation/MarkerGraphicsItem.h"
#include "gstar/Annotation/AbstractGraphicsItem.h"
#include "gstar/Annotation/EmptyGraphicsItem.h"
#include "gstar/GStarResource.h"
#include "gstar/ImageViewToolBar.h"

#include <preferences/Preferences.h>
#include <preferences/PreferencesExport.h>
#include <preferences/AttributeGroup.h>
#include <preferences/CoordinateTransformGlobals.h>
#include <solver/Solver.h>

#include <core/PythonRegionCaller.h>
#include <core/ShellRegionCaller.h>
//#include <core/CAEmitDataChangeHandler.h>

#include <mvc/SolverWidget.h>
#include <preferences/PythonSolverProfileWidget.h>

using gstar::AbstractImageWidget;
using gstar::ImageViewWidget;
using gstar::MarkerGraphicsItem;
using gstar::UProbeMarkerGraphicsItem;
using gstar::CoordinateWidget;
using gstar::UProbeRegionGraphicsItem;
using gstar::ImageViewToolBar;
using namespace gstar;

enum TabIndex{
               CALIBRATION_IDX,
               MICROPROBE_IDX
             };

/*---------------------------------------------------------------------------*/

SWSWidget::SWSWidget(Solver *solver, Preferences* pref, QWidget* parent)
: AbstractImageWidget(parent)
{

   m_microProbePvSet = false;
//   m_pvXHandler = nullptr;
//   m_pvYHandler = nullptr;
   m_solverWidget = nullptr;
   m_coordinateModel = nullptr;
   m_calSelectionModel = nullptr;
   m_lightToMicroCoordModel = nullptr;
   m_solver = solver;
   m_preferences = pref;
   m_solverParameterParse = new SolverParameterParse();

   checkMicroProbePVs();
   createLayout();
   createActions();
   createMicroProbeMenu();
   m_imageViewWidget->clickFill(true);

   m_grabbingPvsX = false;
   m_grabbingPvsY = false;

}

/*---------------------------------------------------------------------------*/

SWSWidget::~SWSWidget()
{

   if(m_solverParameterParse != nullptr)
   {
      delete m_solverParameterParse;
   }
   m_solverParameterParse = nullptr;

   if(m_solverWidget != nullptr)
   {
      delete m_solverWidget;
   }
   m_solverWidget = nullptr;
/*
   if (m_pvXHandler != nullptr)
      delete m_pvXHandler;
   m_pvXHandler = nullptr;

   if (m_pvYHandler != nullptr)
      delete m_pvYHandler;
   m_pvYHandler = nullptr;
*/
   //if(m_lightToMicroCoordWidget != nullptr)
   //   delete m_lightToMicroCoordWidget;
   //m_lightToMicroCoordWidget = nullptr;
//   saveXMLCoordinateInfo();

}

/*---------------------------------------------------------------------------*/

void SWSWidget::addCalibration()
{

   UProbeMarkerGraphicsItem* annotation = new UProbeMarkerGraphicsItem();
   annotation->setMouseOverPixelCoordModel(m_coordinateModel);
   annotation->setLightToMicroCoordModel(m_lightToMicroCoordModel);

   insertAndSelectAnnotation(m_calTreeModel,
                             m_calAnnoTreeView,
                             m_calSelectionModel,
                             annotation);

}

/*---------------------------------------------------------------------------*/

void SWSWidget::addTopWindowPoints()
{

    QString btmRight = "A";
    QString btmLeft = "B";
    QString topRight = "C";
    QString topLeft = "D";

    if(QObject::sender() == m_btnAddBottomWindowPoints)
    {   
        btmRight = "E";
        btmLeft = "F";
        topRight = "G";
        topLeft = "H";
    }

    float offset = 1000;

    QPointF center = m_imageViewWidget->getCenterPoint();
    offset = center.x() - (center.x() / 20);

   //Add A
   UProbeMarkerGraphicsItem* annotation = new UProbeMarkerGraphicsItem();
   annotation->setMouseOverPixelCoordModel(m_coordinateModel);
   annotation->setLightToMicroCoordModel(m_lightToMicroCoordModel);

   insertAndSelectAnnotation(m_calTreeModel,
                             m_calAnnoTreeView,
                             m_calSelectionModel,
                             annotation);
   QPointF p = annotation->pos();
   annotation->setPropertyValue("DisplayName", btmRight);
   annotation->setPos(QPointF(p.x()+offset, p.y()+offset));

   //Add B
   annotation = new UProbeMarkerGraphicsItem();
   annotation->setMouseOverPixelCoordModel(m_coordinateModel);
   annotation->setLightToMicroCoordModel(m_lightToMicroCoordModel);

   insertAndSelectAnnotation(m_calTreeModel,
                             m_calAnnoTreeView,
                             m_calSelectionModel,
                             annotation);
   p = annotation->pos();
   annotation->setPos(QPointF(p.x()-offset, p.y()+offset));
   annotation->setPropertyValue("DisplayName", btmLeft);

   //Add C
   annotation = new UProbeMarkerGraphicsItem();
   annotation->setMouseOverPixelCoordModel(m_coordinateModel);
   annotation->setLightToMicroCoordModel(m_lightToMicroCoordModel);

   insertAndSelectAnnotation(m_calTreeModel,
                             m_calAnnoTreeView,
                             m_calSelectionModel,
                             annotation);
   p = annotation->pos();
   annotation->setPos(QPoint(p.x()+offset, p.y()-offset));
   annotation->setPropertyValue("DisplayName", topRight);

   //Add D
   annotation = new UProbeMarkerGraphicsItem();
   annotation->setMouseOverPixelCoordModel(m_coordinateModel);
   annotation->setLightToMicroCoordModel(m_lightToMicroCoordModel);

   insertAndSelectAnnotation(m_calTreeModel,
                             m_calAnnoTreeView,
                             m_calSelectionModel,
                             annotation);
   p = annotation->pos();
   annotation->setPos(QPoint(p.x()-offset, p.y()-offset));
   annotation->setPropertyValue("DisplayName", topLeft);


}

/*---------------------------------------------------------------------------*/

void SWSWidget::addMicroProbeRegion()
{

   UProbeRegionGraphicsItem* annotation = new UProbeRegionGraphicsItem();
   annotation->setMouseOverPixelCoordModel(m_coordinateModel);
   annotation->setLightToMicroCoordModel(m_lightToMicroCoordModel);

   insertAndSelectAnnotation(m_mpTreeModel,
                             m_mpAnnoTreeView,
                             m_mpSelectionModel,
                             annotation);

}

/*---------------------------------------------------------------------------*/

UProbeRegionGraphicsItem* SWSWidget::getSelectedRegion()
{
   QModelIndexList selectedIndexes = m_mpSelectionModel->selectedRows();
   if (selectedIndexes.size() == 1)
   {
      QModelIndex qModelIndex = selectedIndexes[0];
      if (qModelIndex.isValid()){
         // Currently selected region item.
         QModelIndex qModelIndex = selectedIndexes[0];
         AbstractGraphicsItem* abstractItem = static_cast<AbstractGraphicsItem*>(qModelIndex.internalPointer());
         return dynamic_cast<UProbeRegionGraphicsItem*>(abstractItem);
      }
   }

   return nullptr;
}

/*---------------------------------------------------------------------------*/

void SWSWidget::zoomMicroProbeRegion()
{
   UProbeRegionGraphicsItem* item = getSelectedRegion();
   if (item != nullptr) {
      item->zoomToRegion();
   }
}

/*---------------------------------------------------------------------------*/

void SWSWidget::exportSelectedRegionInformation(QList<QString>* summaryInformation, QList<QString>* summaryWarnings)
{
   QModelIndexList selectedIndexes = m_mpSelectionModel->selectedRows();
   if (selectedIndexes.size() > 0)
   {
      // When summary information is enabled no exports occur however a summary of all directories is genereated.
      bool summaryMode = (summaryInformation != nullptr && summaryWarnings != nullptr);
      if (!summaryMode)
      {
         summaryInformation = new QList<QString>();
         summaryWarnings = new QList<QString>();
         exportSelectedRegionInformation(summaryInformation, summaryWarnings);

         QString messageBoxText = "Listed is the summary of the export: <br/>";
         QString messageBoxDetailedText = "";

         if (summaryWarnings->size() > 0)
         {
            messageBoxDetailedText.append("\n Warnings: \n\n");
            for (int i = 0; i < summaryWarnings->size(); i++)
            {
               messageBoxDetailedText.append(" - " + summaryWarnings->at(i) + "\n");
            }

            messageBoxText.append(QString("<FONT COLOR=red>%1</FONT>").arg("Contains " + QString::number(summaryWarnings->size()) + " Warnings<br/>"));
         }

         if (summaryInformation->size() > 0)
         {
            messageBoxDetailedText.append("\n Information: \n\n");
            for (int i = 0; i < summaryInformation->size(); i++)
            {
               messageBoxDetailedText.append(" - " + summaryInformation->at(i) + "\n");
            }
            messageBoxText.append("Contains " + QString::number(summaryInformation->size()) + " Info notifications.<br/>");
         }

         messageBoxText += "<br/> Would you like to proceed?";

         QMessageBox msg;
         msg.setTextFormat(Qt::RichText);
         msg.setWindowTitle("Export Summary");
         msg.setDetailedText(messageBoxDetailedText);
         msg.setText(messageBoxText);
         msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
         msg.setDefaultButton(QMessageBox::Yes);
         msg.setIcon(QMessageBox::Question);

         delete summaryInformation;
         delete summaryWarnings;

         if (msg.exec() != QMessageBox::Yes)
         {
            // User does not want to proceed.
            return;
         }
      }

      // Gather all export configurations before proceeding.
      bool zoomToRegion = m_preferences->readValueKey(Preferences::ExportZoomToRegion).toBool();
      QString exportDirectoryName = m_preferences->readValueKey(Preferences::ExportDirectoryInDataset).toString();

      bool printNameOnImage = m_preferences->readValueKey(Preferences::ExportPrintNameOnExportedImage).toBool();
      bool printPredictedXYOnImage = m_preferences->readValueKey(Preferences::ExportPrintPxPyOnExportedImage).toBool();
      bool printWidthHeightOnImage = m_preferences->readValueKey(Preferences::ExportPrintWidthHeightOnExportedImage).toBool();
      int selectedXMLExportOption = m_preferences->readValueKey(Preferences::ExportSelectedXmlOption).toInt();
      bool seperateXMLFile = selectedXMLExportOption == PreferencesExport::sepereateFileXMLExport;
      bool singleXmlFileExport = selectedXMLExportOption == PreferencesExport::singleFileXMLExport;

      QXmlStreamWriter* xmlStreamWriter = nullptr;
      QFile* xmlFile = nullptr;
      QString filePath = QFileInfo(m_pathFile).absolutePath();

      // Create the desired directory for exports.
      if (exportDirectoryName != nullptr)
      {
         filePath += "/" + exportDirectoryName;
         QDir directory(filePath);
         if (directory.exists() == false)
         {
            directory.mkdir(filePath);
         }
      }

      QString fileBaseName = filePath + "/" + QFileInfo(m_pathFile).baseName();

      if (singleXmlFileExport)
      {
         QString xmlFileName = fileBaseName + ".export.xml";
         xmlFile = new QFile(xmlFileName);

         if (!summaryMode) {
            if (!xmlFile->open(QIODevice::WriteOnly))
            {
               QMessageBox::warning(0, "Write only", xmlFileName + " is in read only mode");
               return;
            }

            xmlStreamWriter = new QXmlStreamWriter(xmlFile);
            xmlStreamWriter->setAutoFormatting(true);
            xmlStreamWriter->setAutoFormattingIndent(true);
            xmlStreamWriter->writeStartDocument();
            xmlStreamWriter->writeStartElement("Region-List");
         } else {
            QString message = "Xml File: " + xmlFileName;
            if (xmlFile->exists())
            {
               summaryWarnings->append(message + " will be overriden");
            }
            else
            {
               summaryInformation->append(message + " will be created");
            }
         }
      }

      for(const QModelIndex &index : selectedIndexes)
      {
         if (index.isValid()){
            AbstractGraphicsItem* abstractItem = static_cast<AbstractGraphicsItem*>(index.internalPointer());
            UProbeRegionGraphicsItem* item = dynamic_cast<UProbeRegionGraphicsItem*>(abstractItem);
            if (item != nullptr)
            {
               QString itemName = item->getUProbeName();
               QString fileName = fileBaseName + "." + itemName + ".jpg";

               if (seperateXMLFile)
               {
                  QString xmlFileName = fileName + ".xml";
                  xmlFile = new QFile(xmlFileName);

                  if (!summaryMode)
                  {
                     if (!xmlFile->open(QIODevice::WriteOnly))
                     {
                        QMessageBox::warning(0, "Write only", xmlFileName + " is in read only mode");
                        return;
                     }
                     xmlStreamWriter = new QXmlStreamWriter(xmlFile);
                     xmlStreamWriter->setAutoFormatting(true);
                     xmlStreamWriter->setAutoFormattingIndent(true);
                     xmlStreamWriter->writeStartDocument();
                  }
                  else
                  {
                     QString message = "Xml File: " + xmlFileName;
                     if (xmlFile->exists())
                     {
                        summaryWarnings->append(message + " will be overriden");
                     }
                     else
                     {
                        summaryInformation->append(message + " will be created");
                     }
                  }
               }

               if (!summaryMode)
               {
                  exportRegionXMLAndImage(item,
                                          fileName,
                                          zoomToRegion,
                                          printNameOnImage,
                                          printPredictedXYOnImage,
                                          printWidthHeightOnImage,
                                          xmlStreamWriter);
               } else
               {
                  QFile exportImage(fileName);
                  QString message = "Image File: " + fileName;
                  if (exportImage.exists())
                  {
                     summaryWarnings->append(message + " will be overriden");
                  }
                  else
                  {
                     summaryInformation->append(message + " will be created");
                  }
               }

               if (seperateXMLFile)
               {
                  if (!summaryMode)
                  {                     
                     xmlStreamWriter->writeEndDocument();
                     delete xmlStreamWriter;
                  }
                  delete xmlFile;
               }
            }
         }
      }
      if (singleXmlFileExport)
      {
         if (!summaryMode) {
            xmlStreamWriter->writeEndElement();
            xmlStreamWriter->writeEndDocument();
            delete xmlStreamWriter;
         }
         delete xmlFile;
      }
   }
}

/*---------------------------------------------------------------------------*/

void SWSWidget::exportRegionXMLAndImage(UProbeRegionGraphicsItem* item,
                                        QString filePathToSave,
                                        bool zoomToRegion,
                                        bool printNameOnImage,
                                        bool printPredictedXYOnImage,
                                        bool printWidthHeightOnImage,
                                        QXmlStreamWriter* xmlStreamWriter)
{
   QString itemName = item->getUProbeName();
   QString widthText = "Width: " + item->propertyValue(UPROBE_WIDTH).toString();
   QString heightText = "Height: " + item->propertyValue(UPROBE_HEIGHT).toString();
   QString predPositionXText = "Px: " + item->propertyValue(UPROBE_PRED_POS_X).toString();
   QString predPositionYText = "Py: " + item->propertyValue(UPROBE_PRED_POS_Y).toString();

   int textLeftOffset = 5;
   int boundsIncreaseFactor = 5;
   int textLineOffset = 12;
   int textBoxHeight = 0;

   // Default blackbar multiplier
   int defaultMultiplier = 3;
   int multiplier = defaultMultiplier;

   if (printNameOnImage) multiplier++;
   if (printPredictedXYOnImage) multiplier++;
   if (printWidthHeightOnImage) multiplier++;

   // When some text needs to be printed
   if (multiplier > defaultMultiplier){
      textBoxHeight = (textLineOffset * multiplier);
   }

   // Clone the region and add to a new temporary model
   UProbeRegionGraphicsItem* clonedItem = item->cloneRegion();
   AnnotationTreeModel* tempTreeModel = new gstar::AnnotationTreeModel();
   tempTreeModel->appendNode(clonedItem);
   clonedItem->setPropertyValue(UPROBE_NAME,"");
   clonedItem->setPos(item->pos());
   m_imageViewWidget->scene()->setModel(tempTreeModel);

   QGraphicsView* view = m_imageViewWidget->view();

   // Caputre the original bounds
   QRectF originalBounds = view->mapToScene(view->rect()).boundingRect();
   QRectF bounds;
   if (zoomToRegion)
   {
      // Tightly fit the region in view
      view->fitInView(clonedItem, Qt::KeepAspectRatio);
      bounds = view->mapToScene(view->rect()).boundingRect();
      // Unzoom slightly
      QPointF topLeft = bounds.topLeft();
      QPointF bottomRight = bounds.bottomRight();
      topLeft.setX(topLeft.x() - boundsIncreaseFactor);
      topLeft.setY(topLeft.y() - boundsIncreaseFactor);
      bottomRight.setX(bottomRight.x() + boundsIncreaseFactor);
      bottomRight.setY(bottomRight.y() + boundsIncreaseFactor);
      bounds = QRectF(topLeft, bottomRight);
      view->fitInView(bounds,
                      Qt::KeepAspectRatio);
   } else {
      // Show all
      view->fitInView(
          view->scene()->itemsBoundingRect(),
          Qt::KeepAspectRatio);
   }
   // Get the new bounds.
   bounds = view->mapToScene(view->rect()).boundingRect();

   // The view should go within the target rect.
   QRectF target(0, 0, bounds.width(), bounds.height());

   // Set up image size to fit the region name information.
   int imageheight = bounds.height() + textBoxHeight;
   int imageWidth = bounds.width();

   // Create painter for painting into image
   QPainter* painter = new QPainter();

   // Predict size of the written text
   if (textBoxHeight > 0)
   {
      int maxWidth = 0;

      QFont painterFont = painter->font();
      QFontMetrics fm(painterFont);
      if (printNameOnImage) {
         maxWidth = std::max(fm.width(itemName), maxWidth);
      }
      if (printPredictedXYOnImage) {
         maxWidth = std::max(fm.width(predPositionXText), maxWidth);
         maxWidth = std::max(fm.width(predPositionYText), maxWidth);
      }
      if (printWidthHeightOnImage) {
         maxWidth = std::max(fm.width(widthText), maxWidth);
         maxWidth = std::max(fm.width(heightText), maxWidth);
      }

      maxWidth += textLeftOffset;
      // Increse image width if needed to fit text.
      if (imageWidth < maxWidth) {
         imageWidth = maxWidth;
      }
   }

   // Create a image to fit all the export info.
   QImage img(imageWidth, imageheight, QImage::Format_RGB32);
   img.fill(Qt::black);
   painter->begin(&img);

   // Render scene to image
   view -> render(painter, target);

   // Write the region name.  
   if (textBoxHeight > 0)
   {          
      painter->setPen(Qt::white);
      int textYPos = imageheight - textBoxHeight + (textLineOffset * 2);

      if (printNameOnImage) {
         painter->drawText(textLeftOffset, textYPos, itemName);
         textYPos += textLineOffset;
      }
      if (printPredictedXYOnImage) {
         painter->drawText(textLeftOffset, textYPos, predPositionXText);
         textYPos += textLineOffset;
         painter->drawText(textLeftOffset, textYPos, predPositionYText);
         textYPos += textLineOffset;
      }
      if (printWidthHeightOnImage) {
         painter->drawText(textLeftOffset, textYPos, widthText);
         textYPos += textLineOffset;
         painter->drawText(textLeftOffset, textYPos, heightText);
      }
   }

   // Save image to file
   QApplication::setOverrideCursor(Qt::WaitCursor);
   img.save(filePathToSave);
   QApplication::restoreOverrideCursor();

   // Restore last model
   m_imageViewWidget->scene()->setModel(m_mpTreeModel);
   view->fitInView(originalBounds, Qt::KeepAspectRatio);

   delete painter;
   delete tempTreeModel;  

   if (xmlStreamWriter != nullptr)
   {
      xmlStreamWriter->writeStartElement("Region");

      xmlStreamWriter->writeAttribute("name", itemName);

      xmlStreamWriter->writeStartElement(UPROBE_PRED_POS_X);
      xmlStreamWriter->writeCharacters(predPositionXText);
      xmlStreamWriter->writeEndElement();

      xmlStreamWriter->writeStartElement(UPROBE_PRED_POS_Y);
      xmlStreamWriter->writeCharacters(predPositionYText);
      xmlStreamWriter->writeEndElement();

      xmlStreamWriter->writeStartElement(UPROBE_WIDTH);
      xmlStreamWriter->writeCharacters(widthText);
      xmlStreamWriter->writeEndElement();


      xmlStreamWriter->writeStartElement(UPROBE_HEIGHT);
      xmlStreamWriter->writeCharacters(heightText);
      xmlStreamWriter->writeEndElement();

      xmlStreamWriter->writeEndElement();
   }
}

/*---------------------------------------------------------------------------*/

void SWSWidget::callbackPvXUpdatedFloat(float val)
{

   qDebug()<<"float x val = "<<val;
   callbackPvXUpdatedDouble((double) val);

}

/*---------------------------------------------------------------------------*/

void SWSWidget::callbackPvYUpdatedFloat(float val)
{

   qDebug()<<"float y val = "<<val;
   callbackPvYUpdatedDouble((double) val);

}

/*---------------------------------------------------------------------------*/

void SWSWidget::callbackPvXUpdatedDouble(double val)
{

   if(m_calSelectionModel == nullptr || m_grabbingPvsX == false)
      return;

   qDebug()<<"double x val = "<<val;

   QModelIndexList selectedIndexes = m_calSelectionModel->selectedRows();
   if (selectedIndexes.count() == 1)
   {
      QModelIndex index = selectedIndexes[0];
      UProbeMarkerGraphicsItem* ann =
            (UProbeMarkerGraphicsItem*)index.internalPointer();
      if(ann)
      {
         ann->setMeasuredXProperty(QVariant(val));
      }
   }
   else
   {
      //QMessageBox::warning(0, "Warning", "Please select only one annotation.");
      qDebug()<<"No marker selection for callbackPvXUpdatedDouble";
   }

   m_grabbingPvsX = false;

}

/*---------------------------------------------------------------------------*/

void SWSWidget::callbackPvYUpdatedDouble(double val)
{

   if(m_calSelectionModel == nullptr || m_grabbingPvsY == false)
      return;

   qDebug()<<"double y val = "<<val;

   QModelIndexList selectedIndexes = m_calSelectionModel->selectedRows();
   if (selectedIndexes.count() == 1)
   {
      QModelIndex index = selectedIndexes[0];
      UProbeMarkerGraphicsItem* ann =
            (UProbeMarkerGraphicsItem*)index.internalPointer();
      if(ann)
      {
         ann->setMeasuredYProperty(QVariant(val));
      }
   }
   else
   {
      //QMessageBox::warning(0, "Warning", "Please select only one annotation.");
      qDebug()<<"No marker selection for callbackPvYUpdatedDouble";
   }

   m_grabbingPvsY = false;

}

/*---------------------------------------------------------------------------*/

void SWSWidget::calModelDataChanged(const QModelIndex& topLeft,
                                    const QModelIndex& bottomRight)
{

   m_calAnnoTreeView->resizeColumnToContents(topLeft.column());
   m_calAnnoTreeView->resizeColumnToContents(bottomRight.column());

}

/*---------------------------------------------------------------------------*/

void SWSWidget::createActions()
{

   m_addCalibrationAction = new QAction("Add Point", this);

   connect(m_addCalibrationAction,
           SIGNAL(triggered()),
           this,
           SLOT(addCalibration()));

   m_addMicroPrboeRegionAction = new QAction("Add Region", this);   

   connect(m_addMicroPrboeRegionAction,
           SIGNAL(triggered()),
           this,
           SLOT(addMicroProbeRegion()));

   m_zoomMicroProbeRegionAction = new QAction("Zoom to Region", this);

   connect(m_zoomMicroProbeRegionAction,
           SIGNAL(triggered()),
           this,
           SLOT(zoomMicroProbeRegion()));

   m_exportMicroProbeRegionInfoAction = new QAction("Export Region Information", this);

   connect(m_exportMicroProbeRegionInfoAction,
           SIGNAL(triggered()),
           this,
           SLOT(exportSelectedRegionInformation()));

   m_grabMicroProbePVAction = new QAction("Grab MicroProbe PV", this);
   connect(m_grabMicroProbePVAction,
           SIGNAL(triggered()),
           this,
           SLOT(grabMicroProbePV()));
}

/*---------------------------------------------------------------------------*/

void SWSWidget::cancelUpdatedSolverVariables()
{

   emit cancelSolverVariableUpdate();

}

/*---------------------------------------------------------------------------*/

void SWSWidget::createCalibrationTab()
{

   m_calTreeModel = new gstar::AnnotationTreeModel();

   connect(m_calTreeModel,
           SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
           this,
           SLOT(calModelDataChanged(const QModelIndex &, const QModelIndex &)));

   m_calSelectionModel = new QItemSelectionModel(m_calTreeModel);

   m_calAnnoTreeView = new QTreeView();
   m_calAnnoTreeView->setAutoFillBackground(true);
   m_calAnnoTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
   m_calAnnoTreeView->setAnimated(true);
   m_calAnnoTreeView->setModel(m_calTreeModel);
   m_calAnnoTreeView->setHeaderHidden(true);
   m_calAnnoTreeView->setSelectionModel(m_calSelectionModel);
   m_calAnnoTreeView->setContextMenuPolicy(Qt::CustomContextMenu);

//   m_calAnnoTreeView->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);

   connect(m_calAnnoTreeView,
          SIGNAL(customContextMenuRequested(const QPoint &)),
          this,
          SLOT(treeContextMenu(const QPoint &)));

   connect(m_calAnnoTreeView,
           SIGNAL(doubleClicked(const QModelIndex &)),
           this,
           SLOT(treeDoubleClicked(const QModelIndex &)));

   m_btnAddCalibration = new QPushButton("Add Calibration Point");
   connect(m_btnAddCalibration,
           SIGNAL(clicked()),
           this,
           SLOT(addCalibration()));

   m_btnAddTopWindowPoints = new QPushButton("Add Top Window Points");
   connect(m_btnAddTopWindowPoints,
           SIGNAL(clicked()),
           this,
           SLOT(addTopWindowPoints()));

   m_btnAddBottomWindowPoints = new QPushButton("Add Bottom Window Points");
   connect(m_btnAddBottomWindowPoints,
           SIGNAL(clicked()),
           this,
           SLOT(addTopWindowPoints()));

   m_btnRunSolver = new QPushButton("Open Solver");
   connect(m_btnRunSolver,
           SIGNAL(clicked()),
           this,
           SLOT(openSolver()));

   QLabel* lblXOffset = new QLabel("2xfm:X ");
   m_xOffset = new QLineEdit();
   connect(m_xOffset,
           SIGNAL(editingFinished()),
           this,
           SLOT(offsetReturnPressed()));
   connect(m_xOffset,
           SIGNAL(returnPressed()),
           this,
           SLOT(offsetReturnPressed()));

   QLabel* lblYOffset = new QLabel("2xfm:Y ");
   m_yOffset = new QLineEdit();
   connect(m_yOffset,
           SIGNAL(editingFinished()),
           this,
           SLOT(offsetReturnPressed()));
   connect(m_xOffset,
           SIGNAL(returnPressed()),
           this,
           SLOT(offsetReturnPressed()));

   QVBoxLayout* infoLayout = new QVBoxLayout();
   QHBoxLayout* buttonLayout = new QHBoxLayout();
   QHBoxLayout* buttonLayout2 = new QHBoxLayout();
   QHBoxLayout* offsetLayout = new QHBoxLayout();
   offsetLayout->addWidget(lblXOffset);
   offsetLayout->addWidget(m_xOffset);
   offsetLayout->addWidget(lblYOffset);
   offsetLayout->addWidget(m_yOffset);
   buttonLayout->addWidget(m_btnAddCalibration);
   buttonLayout->addWidget(m_btnRunSolver);
   buttonLayout2->addWidget(m_btnAddTopWindowPoints);
   buttonLayout2->addWidget(m_btnAddBottomWindowPoints);
   infoLayout->addLayout(offsetLayout);
   infoLayout->addLayout(buttonLayout);
   infoLayout->addLayout(buttonLayout2);
   infoLayout->addWidget(m_calAnnoTreeView);

   m_calibrationTabWidget = new QWidget(this);
   m_calibrationTabWidget->setAutoFillBackground(true);
   m_calibrationTabWidget->setLayout(infoLayout);

}

/*---------------------------------------------------------------------------*/

void SWSWidget::CallPythonFunc()
{

   QAction *action = (QAction *)sender();
   RegionCaller *prc = m_actionMap[action];

   if(prc)
   {
      QModelIndexList selectedIndexes = m_mpSelectionModel->selectedRows();
      if (selectedIndexes.count() == 1)
      {
         QModelIndex index = selectedIndexes[0];
         UProbeRegionGraphicsItem* region =
               (UProbeRegionGraphicsItem*)index.internalPointer();
         if(region)
         {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            QVariant prop = region->propertyValue(UPROBE_NAME);

            if( false == prc->CallFunc(prop.toString(),
                                       region->getX(),
                                       region->getY(),
                                       region->getWidth(),
                                       region->getHeight(),
                                       region->getFactorX(),
                                       region->getFactorY()))
            {
               QApplication::restoreOverrideCursor();
               QMessageBox::critical(0, "Error", "Failed to call function");
            }
            QApplication::restoreOverrideCursor();
         }
      }
   }
}

/*---------------------------------------------------------------------------*/

void SWSWidget::createMicroProbeMenu()
{

   QStringList rList = m_preferences->readValueKey(Preferences::RegionMenuList)
         .toStringList();

   for(const QString &str : rList)
   {
      AttributeGroup grp;
      if(grp.fromString(str))
      {
         QFileInfo fInfo = QFileInfo(grp.getGroupName());
         for(int i=0; i< grp.count(); i++)
         {
            Attribute *attr = grp.getAttrAt(i);
            if(attr->getIsEnable())
            {
               QAction *action = new QAction(attr->getName(), this);

               QString execType = attr->getDescription();
               RegionCaller *prc = nullptr;

               if(execType == "shell")
               {
                  prc = new ShellRegionCaller();
                  if(prc->init(grp.getGroupName(), "", ""))
                  {
                     m_actionMap.insert(action, prc);
                  }
                  else
                  {
                     delete prc;
                     prc = nullptr;
                     qDebug()<<"Error initializing shell call: "
                            <<attr->getName();
                  }
               }               
               if(execType == "python")
               {
                  prc = new PythonRegionCaller();
                  if(prc->init(fInfo.path(), fInfo.baseName(), attr->getValue()))
                  {
                     m_actionMap.insert(action, prc);
                  }
                  else
                  {
                     delete prc;
                     prc = nullptr;
                     qDebug()<<"Error initializing python call: "
                            <<attr->getName();
                  }
               }               

               if(prc != nullptr)
               {
                  connect(action,
                          SIGNAL(triggered()),
                          this,
                          SLOT(CallPythonFunc()));

               }
               else
               {
                  delete action;
               }
            }
         }
      }
   }

}

/*---------------------------------------------------------------------------*/

void SWSWidget::createMicroProbeTab()
{

   m_mpTreeModel = new gstar::AnnotationTreeModel();

   connect(m_mpTreeModel,
           SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
           this,
           SLOT(microModelDataChanged(const QModelIndex &,
                                      const QModelIndex &)));

   m_mpSelectionModel = new QItemSelectionModel(m_mpTreeModel);

   m_mpAnnoTreeView = new QTreeView();
   m_mpAnnoTreeView->setAutoFillBackground(true);
   m_mpAnnoTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
   m_mpAnnoTreeView->setAnimated(true);
   m_mpAnnoTreeView->setModel(m_mpTreeModel);
   m_mpAnnoTreeView->setHeaderHidden(true);
   m_mpAnnoTreeView->setSelectionModel(m_mpSelectionModel);
   m_mpAnnoTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(m_mpAnnoTreeView,
          SIGNAL(customContextMenuRequested(const QPoint &)),
          this,
          SLOT(treeContextMenu(const QPoint &)));

   connect(m_mpAnnoTreeView,
           SIGNAL(doubleClicked(const QModelIndex &)),
           this,
           SLOT(treeDoubleClicked(const QModelIndex &)));

   m_btnAddMicroProbe = new QPushButton("Add Micro Probe Region");
   connect(m_btnAddMicroProbe,
           SIGNAL(clicked()),
           this,
           SLOT(addMicroProbeRegion()));

   QVBoxLayout* infoLayout = new QVBoxLayout();
   infoLayout->addWidget(m_btnAddMicroProbe);
   infoLayout->addWidget(m_mpAnnoTreeView);

   m_microProbeTabWidget = new QWidget(this);
   m_microProbeTabWidget->setAutoFillBackground(true);
   m_microProbeTabWidget->setLayout(infoLayout);

}

/*---------------------------------------------------------------------------*/

void SWSWidget::createLayout()
{

   connect(m_tabWidget,
           SIGNAL(currentChanged(int )),
           this,
           SLOT(tabIndexChanged(int )));

   createCalibrationTab();
   createMicroProbeTab();

   QLayout* layout = generateDefaultLayout();

   m_range->setRangeWidgetVisible(false);
   m_labelWidthAction->setVisible(false);
   m_imageWidthDimAction->setVisible(false);
   m_labelHeightAction->setVisible(false);
   m_imageHeightDimAction->setVisible(false);

   m_tabWidget->addTab(m_calibrationTabWidget, QIcon(), "Calibration");
   m_tabWidget->addTab(m_microProbeTabWidget, QIcon(), "MicroProbe");

   //set calibration as initial model
   m_imageViewWidget->scene()->setModel(m_calTreeModel);
   m_imageViewWidget->scene()->setSelectionModel(m_calSelectionModel);

   m_imageViewWidget->coordinateWidget()->setLabel("Light  X :",
                                                   "Light  Y :",
                                                   "Light  Z :");
   m_imageViewWidget->coordinateWidget()->setUnitsLabel("mm");

   m_lightToMicroCoordWidget = new CoordinateWidget();
   m_lightToMicroCoordWidget->setLabel("Micro X :", "Micro Y :", "Micro Z :");
   m_lightToMicroCoordWidget->setUnitsLabel("mm");
   m_imageViewWidget->layout()->addWidget(m_lightToMicroCoordWidget);

   setLayout(layout);

}

/*---------------------------------------------------------------------------*/

void SWSWidget::displayContextMenu(QWidget* parent,
                                   const QPoint& pos)
{

   gstar::AnnotationTreeModel* treeModel = nullptr;
   QItemSelectionModel* selectionModel = nullptr;
   QModelIndexList selectedIndexes;

   //if (m_annotationsEnabled == false)
   //   return;

   QMenu menu(parent);

   switch(m_tabWidget->currentIndex())
   {
      case CALIBRATION_IDX:
         menu.addAction(m_addCalibrationAction);
         treeModel = m_calTreeModel;
         selectionModel = m_calSelectionModel;
         selectedIndexes = m_calSelectionModel->selectedRows();
         if( true == m_microProbePvSet)
         {
            menu.addAction(m_grabMicroProbePVAction);
         }
         break;
      case MICROPROBE_IDX:
         menu.addAction(m_addMicroPrboeRegionAction);
         selectedIndexes = m_mpSelectionModel->selectedRows();

         // add preferences call menu items
         if (selectedIndexes.count() == 1)
         {
            QMap<QString, QAction*> actionNameMap;
            for(QAction* key : m_actionMap.keys())
            {
               actionNameMap[m_actionMap[key]->getFuncName()] = key;
            }
            for(const QString &key : actionNameMap.keys())
            {
               menu.addAction(actionNameMap[key]);
            }

            UProbeRegionGraphicsItem* item = getSelectedRegion();
            // Selected item is of proper type for zoom action.
            if (item != nullptr) {
               menu.addSeparator();
               menu.addAction(m_zoomMicroProbeRegionAction);
               menu.addAction(m_exportMicroProbeRegionInfoAction);
            }
         } else if (selectedIndexes.count() > 1)
         {
            menu.addSeparator();
            menu.addAction(m_exportMicroProbeRegionInfoAction);
         }
         treeModel = m_mpTreeModel;
         selectionModel = m_mpSelectionModel;
         break;
      default:
         return;
         break;
   }

   if (treeModel != nullptr && treeModel->rowCount() > 0)
   {
      if (selectionModel != nullptr && selectionModel->hasSelection())
      {
         bool addDuplicate = true;
         if (selectedIndexes.count() == 1)
         {
            QModelIndex qModelIndex = selectedIndexes[0];
            AbstractGraphicsItem* abstractItem = static_cast<AbstractGraphicsItem*>(qModelIndex.internalPointer());
            EmptyGraphicsItem* item = dynamic_cast<EmptyGraphicsItem*>(abstractItem);

            // Add duplicate when group is not selected.
            addDuplicate = (item == nullptr);
         }

         if (addDuplicate) {
            menu.addSeparator();
            menu.addAction(m_duplicateAction);
         }
         menu.addSeparator();
         menu.addAction(m_deleteAction);
      }
   }

   QAction* result = menu.exec(pos);
   if (result == nullptr)
   {
      if (selectionModel != nullptr)
         selectionModel->clearSelection();
   }

}

/*--------------------------------------------------------------------------*/

void SWSWidget::checkMicroProbePVs()
{
/*
   try
   {

      QString microX = m_preferences->readValueKey(Preferences::MicroProbeXPv).toString();
      QString microY = m_preferences->readValueKey(Preferences::MicroProbeYPv).toString();

      if (microX.length() > 0 && microY.length() > 0)
      {

         ChannelAccess* ca = ChannelAccess::getInstance();


         m_pvX = ca->getPV(microX.toLatin1().data(), 1, 1);
         m_pvY = ca->getPV(microY.toLatin1().data(), 1, 1);

         switch(m_pvX->getType())
         {
         case DBR_FLOAT:
            m_pvXHandler = new CAEmitDataChangeHandler<float, SWSWidget>(
                                         this, &SWSWidget::callbackPvXUpdatedFloat);
            m_pvYHandler = new CAEmitDataChangeHandler<float, SWSWidget>(
                                         this, &SWSWidget::callbackPvYUpdatedFloat);
            m_pvX->subscribeDataChangeEvent(
                     (CAEmitDataChangeHandler<float, SWSWidget>*)m_pvXHandler);
            m_pvY->subscribeDataChangeEvent(
                     (CAEmitDataChangeHandler<float, SWSWidget>*)m_pvYHandler);
            //pvType = FLOAT
            m_microProbePvSet = true;
            qDebug()<<"Set micro pv true";
            break;
         case DBR_DOUBLE:
            m_pvXHandler = new CAEmitDataChangeHandler<double, SWSWidget>(
                                         this, &SWSWidget::callbackPvXUpdatedDouble);
            m_pvYHandler = new CAEmitDataChangeHandler<double, SWSWidget>(
                                         this, &SWSWidget::callbackPvYUpdatedDouble);
            m_pvX->subscribeDataChangeEvent(
                     (CAEmitDataChangeHandler<double, SWSWidget>*)m_pvXHandler);
            m_pvY->subscribeDataChangeEvent(
                     (CAEmitDataChangeHandler<double, SWSWidget>*)m_pvYHandler);
            //pvType = DOUBLE
            m_microProbePvSet = true;
            qDebug()<<"Set micro pv true";
            break;
         default:
            m_microProbePvSet = false;
            qDebug()<<"Set micro pv false";
         }

         m_pvX->asynGet(1, (CAEmitDataChangeHandler<int, SWSWidget>*)m_pvXHandler);
         m_pvY->asynGet(1, (CAEmitDataChangeHandler<int, SWSWidget>*)m_pvYHandler);
      }
   }
   catch(...)
   {
      qDebug()<<"Failed to initialize PV's";
   }
   */
}

/*---------------------------------------------------------------------------*/

void SWSWidget::deleteItem()
{

   gstar::AnnotationTreeModel* treeModel;
   QItemSelectionModel* selectionModel;

   switch(m_tabWidget->currentIndex())
   {
      case CALIBRATION_IDX:
         treeModel = m_calTreeModel;
         selectionModel = m_calSelectionModel;
         break;
      case MICROPROBE_IDX:
         treeModel = m_mpTreeModel;
         selectionModel = m_mpSelectionModel;
         break;
      default:
         return;
         break;
   }

   if (selectionModel->hasSelection())
   {
      QModelIndexList selectedIndexes = selectionModel->selectedRows();

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
            treeModel->removeRow(index.row(), index);
         }
      }
   }

}

/*---------------------------------------------------------------------------*/

void SWSWidget::duplicateItem()
{

   gstar::AnnotationTreeModel* treeModel;
   QItemSelectionModel* selectionModel;

   switch(m_tabWidget->currentIndex())
   {
      case CALIBRATION_IDX:
         treeModel = m_calTreeModel;
         selectionModel = m_calSelectionModel;
         break;
      case MICROPROBE_IDX:
         treeModel = m_mpTreeModel;
         selectionModel = m_mpSelectionModel;
         break;
      default:
         return;
         break;
   }

   if (selectionModel->hasSelection())
   {
      QModelIndexList selectedIndexes = selectionModel->selectedRows();

      selectionModel->clearSelection();
      for(const QModelIndex &index : selectedIndexes)
      {
         QModelIndex duplicateIndex = treeModel->duplicateNode(index);
         selectionModel->select(duplicateIndex, QItemSelectionModel::Select);
      }
   }

}

/*---------------------------------------------------------------------------*/

void SWSWidget::getMarkerInfo(QXmlStreamWriter* xmlWriter)
{

   QModelIndex first = m_calTreeModel->index(0,0,QModelIndex());
   gstar::AbstractGraphicsItem* groupPtr =
         (gstar::AbstractGraphicsItem*)first.internalPointer();
   if(groupPtr == nullptr)
      return;
   QList<gstar::AbstractGraphicsItem*> clist = groupPtr->childList();

   for (gstar::AbstractGraphicsItem* child : clist)
   {
      if (child != nullptr)
      {
         xmlWriter->writeStartElement("marker");

         QVariant color = child->propertyValue(UPROBE_COLOR);
         xmlWriter->writeAttribute(UPROBE_COLOR, color.toString());

         QVariant lx = child->propertyValue(UPROBE_LIGHT_POS_X);
         xmlWriter->writeAttribute(UPROBE_LIGHT_POS_X, lx.toString());

         QString rx = QString::number(child->x(), 'g', 10);
         xmlWriter->writeAttribute(UPROBE_REAL_POS_X, rx);

         QVariant ly = child->propertyValue(UPROBE_LIGHT_POS_Y);
         xmlWriter->writeAttribute(UPROBE_LIGHT_POS_Y, ly.toString());

         QString ry = QString::number(child->y(), 'g', 10);
         xmlWriter->writeAttribute(UPROBE_REAL_POS_Y, ry);

         QVariant lz = child->propertyValue(UPROBE_LIGHT_POS_Z);
         xmlWriter->writeAttribute(UPROBE_LIGHT_POS_Z, lz.toString());

         QVariant mx = child->propertyValue(UPROBE_MICRO_POS_X);
         xmlWriter->writeAttribute(UPROBE_MICRO_POS_X, mx.toString());

         QVariant my = child->propertyValue(UPROBE_MICRO_POS_Y);
         xmlWriter->writeAttribute(UPROBE_MICRO_POS_Y, my.toString());

         QVariant px = child->propertyValue(UPROBE_PRED_POS_X);
         xmlWriter->writeAttribute(UPROBE_PRED_POS_X, px.toString());

         QVariant py = child->propertyValue(UPROBE_PRED_POS_Y);
         xmlWriter->writeAttribute(UPROBE_PRED_POS_Y, py.toString());

         QVariant makerName = child->propertyValue(UPROBE_NAME);
         xmlWriter->writeCharacters (makerName.toString());

         xmlWriter->writeEndElement();
      }
   }

}

/*---------------------------------------------------------------------------*/

void SWSWidget::getRegionMarkerInfo(QXmlStreamWriter* xmlWriter)
{

   QModelIndex first = m_mpTreeModel->index(0,0,QModelIndex());
   gstar::AbstractGraphicsItem* groupPtr =
         (gstar::AbstractGraphicsItem*)first.internalPointer();
   if(groupPtr == nullptr)
      return;
   QList<gstar::AbstractGraphicsItem*> clist = groupPtr->childList();

   for (gstar::AbstractGraphicsItem* child : clist)
   {
      if (child != nullptr)
      {
         xmlWriter->writeStartElement("regionmarker");

         QVariant color = child->propertyValue(UPROBE_COLOR);
         xmlWriter->writeAttribute(UPROBE_COLOR, color.toString());

         QString rx = QString::number(child->x(), 'g', 10);
         xmlWriter->writeAttribute(UPROBE_REAL_POS_X, rx);

         QString ry = QString::number(child->y(), 'g', 10);
         xmlWriter->writeAttribute(UPROBE_REAL_POS_Y, ry);

         QVariant px = child->propertyValue(UPROBE_PRED_POS_X);
         xmlWriter->writeAttribute(UPROBE_PRED_POS_X, px.toString());

         QVariant py = child->propertyValue(UPROBE_PRED_POS_Y);
         xmlWriter->writeAttribute(UPROBE_PRED_POS_Y, py.toString());

         QVariant topLeftX = child->boundingRectMarker().left();
         xmlWriter->writeAttribute(UPROBE_RECT_TLX, topLeftX.toString());

         QVariant topLeftY = child->boundingRectMarker().top();
         xmlWriter->writeAttribute(UPROBE_RECT_TLY, topLeftY.toString());

         QVariant rectWidth = child->boundingRectMarker().width();
         xmlWriter->writeAttribute(UPROBE_RECT_W, rectWidth.toString());

         QVariant rectHeight = child->boundingRectMarker().height();
         xmlWriter->writeAttribute(UPROBE_RECT_H, rectHeight.toString());

         QVariant makerName = child->propertyValue(UPROBE_NAME);
         xmlWriter->writeCharacters (makerName.toString());

         xmlWriter->writeEndElement();
      }
   }

}

/*---------------------------------------------------------------------------*/

QString SWSWidget::getModelFileName()
{

   return m_pathFile;

}

/*---------------------------------------------------------------------------*/

bool SWSWidget::getMarkerCoordinatePoints(QList < QMap<QString,double> >&
                                          listCoordPoints)
{
   bool ok = false;

   if (listCoordPoints.isEmpty()!=true)
       listCoordPoints.clear();

   QModelIndex first = m_calTreeModel->index(0,0,QModelIndex());
   gstar::AbstractGraphicsItem* groupPtr =
         (gstar::AbstractGraphicsItem*)first.internalPointer();
   if(groupPtr == nullptr)
   {
      QMessageBox::critical(0, "uProbeX", "No Points to run solver on!");
      return false;
   }


   QList<gstar::AbstractGraphicsItem*> clist = groupPtr->childList();

   for (gstar::AbstractGraphicsItem* child : clist)
   {
      QMap<QString,double> cPoint;
      if (child != nullptr)
      {
         QVariant vlx = child->propertyValue(UPROBE_LIGHT_POS_X);
         QVariant vly = child->propertyValue(UPROBE_LIGHT_POS_Y);
         QVariant vlz = child->propertyValue(UPROBE_LIGHT_POS_Z);
         QVariant vmx = child->propertyValue(UPROBE_MICRO_POS_X);
         QVariant vmy = child->propertyValue(UPROBE_MICRO_POS_Y);

         QVariant vname = child->propertyValue("DisplayName");
         QString name = vname.toString();

         if(vlx.isValid() && vly.isValid() && vlz.isValid() &&
            vmx.isValid() && vmy.isValid())
         {

            ok = false;
            double Lx = vlx.toDouble(&ok);
            if(!ok)
            {
               QMessageBox::critical(0, "uProbeX", "Error with calibration point: '"+name+"'' LX value");
               return false;
            }
            ok = false;
            double Ly = vly.toDouble(&ok);
            if(!ok)
            {
               QMessageBox::critical(0, "uProbeX", "Error with calibration point: '"+name+"'' LY value");
               return false;
            }
            ok = false;
            double Lz = vlz.toDouble(&ok);
            if(!ok)
            {
               QMessageBox::critical(0, "uProbeX", "Error with calibration point: '"+name+"'' LZ value");
               return false;
            }
            ok = false;
            double Mx = vmx.toDouble(&ok);
            if(!ok)
            {
               QMessageBox::critical(0, "uProbeX", "Error with calibration point: '"+name+"'' MX value");
               return false;
            }
            ok = false;
            double My = vmy.toDouble(&ok);
            if(!ok)
            {
               QMessageBox::critical(0, "uProbeX", "Error with calibration point: '"+name+"'' MY value");
               return false;
            }

            cPoint.insert("Lx", Lx);
            cPoint.insert("Ly", Ly);
            cPoint.insert("Lz", Lz);
            cPoint.insert("Mx", Mx);
            cPoint.insert("My", My);
            listCoordPoints.append(cPoint);
         }
         else
         {
            QMessageBox::critical(0, "uProbeX", "Error with calibration point: '"+name+"'");
            return false;
         }
      }
   }

   return true;

}

/*---------------------------------------------------------------------------*/

void SWSWidget::grabMicroProbePV()
{
/*
   try
   {
      //set mouse to wait
      m_grabbingPvsX = true;
      m_grabbingPvsY = true;

      m_pvX->asynGet(1, (CAEmitDataChangeHandler<double, SWSWidget>*)m_pvXHandler);
      m_pvY->asynGet(1, (CAEmitDataChangeHandler<double, SWSWidget>*)m_pvYHandler);
   }
   catch(...)
   {
      qDebug()<<"Error calling caget for PV's";
   }
   */
}

/*---------------------------------------------------------------------------*/

void SWSWidget::microModelDataChanged(const QModelIndex& topLeft,
                                      const QModelIndex& bottomRight)
{

   m_mpAnnoTreeView->resizeColumnToContents(topLeft.column());
   m_mpAnnoTreeView->resizeColumnToContents(bottomRight.column());

}

/*---------------------------------------------------------------------------*/

void SWSWidget::offsetReturnPressed()
{

   bool ok;
   double valX, valY;

   QString sxOff = m_xOffset->text();
   QString syOff = m_yOffset->text();

   QMessageBox msgBox;
   msgBox.setStandardButtons(QMessageBox::Ok);

   ok = false;
   valX = sxOff.toDouble(&ok);
   if(ok)
   {
      if(false == m_lightToMicroCoordModel->setTransformerVariable(
               CoordinateTransformGlobals::keyToString(
                  CoordinateTransformGlobals::m2xfm_x), valX))
      {
         msgBox.setText("Error setting value!");
         msgBox.exec();
      }
   }
   else
   {
       m_xOffset->setText("0");
   }

   ok = false;
   valY = syOff.toDouble(&ok);
   if(ok)
   {
      if(false == m_lightToMicroCoordModel->setTransformerVariable(
               CoordinateTransformGlobals::keyToString(
                  CoordinateTransformGlobals::m2xfm_y), valY))
      {
         QMessageBox msgBox;
         msgBox.setStandardButtons(QMessageBox::Ok);
         msgBox.setText("Error setting value!");
         msgBox.exec();
      }
   }
   else
   {
       m_yOffset->setText("0");
   }


   emit solverVariableUpdate(valX, valY);

   updateTreeView();

}

/*---------------------------------------------------------------------------*/

QMap<QString, QString> SWSWidget::parseMarker(QXmlStreamReader& xml)
{

   QMap<QString, QString> marker;

   QXmlStreamAttributes attributes = xml.attributes();

   if(attributes.hasAttribute(UPROBE_COLOR))
   {
    marker.insert(UPROBE_COLOR, attributes.value(UPROBE_COLOR).toString());
   }
   if(attributes.hasAttribute(UPROBE_LIGHT_POS_X))
   {
    marker.insert(UPROBE_LIGHT_POS_X,
                  attributes.value(UPROBE_LIGHT_POS_X).toString());
   }
   if(attributes.hasAttribute(UPROBE_REAL_POS_X))
   {
    marker.insert(UPROBE_REAL_POS_X,
                  attributes.value(UPROBE_REAL_POS_X).toString());
   }
   if(attributes.hasAttribute(UPROBE_LIGHT_POS_Y))
   {
    marker.insert(UPROBE_LIGHT_POS_Y,
                  attributes.value(UPROBE_LIGHT_POS_Y).toString());
   }
   if(attributes.hasAttribute(UPROBE_REAL_POS_Y))
   {
    marker.insert(UPROBE_REAL_POS_Y,
                  attributes.value(UPROBE_REAL_POS_Y).toString());
   }
   if(attributes.hasAttribute(UPROBE_LIGHT_POS_Z))
   {
    marker.insert(UPROBE_LIGHT_POS_Z,
                  attributes.value(UPROBE_LIGHT_POS_Z).toString());
   }
   if(attributes.hasAttribute(UPROBE_MICRO_POS_X))
   {
    marker.insert(UPROBE_MICRO_POS_X,
                  attributes.value(UPROBE_MICRO_POS_X).toString());
   }
   if(attributes.hasAttribute(UPROBE_MICRO_POS_Y))
   {
    marker.insert(UPROBE_MICRO_POS_Y,
                  attributes.value(UPROBE_MICRO_POS_Y).toString());
   }
   if(attributes.hasAttribute(UPROBE_PRED_POS_X))
   {
    marker.insert(UPROBE_PRED_POS_X,
                  attributes.value(UPROBE_PRED_POS_X).toString());
   }
   if(attributes.hasAttribute(UPROBE_PRED_POS_Y))
   {
    marker.insert(UPROBE_PRED_POS_Y,
                  attributes.value(UPROBE_PRED_POS_Y).toString());
   }

   xml.readNext();
   if(xml.tokenType() == QXmlStreamReader::Characters)
   {
      marker.insert(UPROBE_NAME, xml.text().toString());
   }

   return marker;

}

/*---------------------------------------------------------------------------*/

QMap<QString, QString> SWSWidget::parseRegionMarker(QXmlStreamReader& xml)
{


   QMap<QString, QString> marker;

   QXmlStreamAttributes attributes = xml.attributes();

   if(attributes.hasAttribute(UPROBE_COLOR))
   {
    marker.insert(UPROBE_COLOR, attributes.value(UPROBE_COLOR).toString());
   }
   if(attributes.hasAttribute(UPROBE_REAL_POS_X))
   {
    marker.insert(UPROBE_REAL_POS_X,
                  attributes.value(UPROBE_REAL_POS_X).toString());
   }
   if(attributes.hasAttribute(UPROBE_REAL_POS_Y))
   {
    marker.insert(UPROBE_REAL_POS_Y,
                  attributes.value(UPROBE_REAL_POS_Y).toString());
   }
   if(attributes.hasAttribute(UPROBE_PRED_POS_X))
   {
    marker.insert(UPROBE_PRED_POS_X,
                  attributes.value(UPROBE_PRED_POS_X).toString());
   }
   if(attributes.hasAttribute(UPROBE_PRED_POS_Y))
   {
    marker.insert(UPROBE_PRED_POS_Y,
                  attributes.value(UPROBE_PRED_POS_Y).toString());
   }
   if(attributes.hasAttribute(UPROBE_RECT_TLX))
   {
    marker.insert(UPROBE_RECT_TLX,
                  attributes.value(UPROBE_RECT_TLX).toString());
   }
   if(attributes.hasAttribute(UPROBE_RECT_TLY))
   {
    marker.insert(UPROBE_RECT_TLY,
                  attributes.value(UPROBE_RECT_TLY).toString());
   }
   if(attributes.hasAttribute(UPROBE_RECT_W))
   {
    marker.insert(UPROBE_RECT_W,
                  attributes.value(UPROBE_RECT_W).toString());
   }
   if(attributes.hasAttribute(UPROBE_RECT_H))
   {
    marker.insert(UPROBE_RECT_H,
                  attributes.value(UPROBE_RECT_H).toString());
   }

   xml.readNext();
   if(xml.tokenType() == QXmlStreamReader::Characters)
   {
      marker.insert(UPROBE_NAME, xml.text().toString());
   }

   return marker;

}

/*---------------------------------------------------------------------------*/

void SWSWidget::parseXML()
{

   QFile* file = new QFile(m_datasetPath);

   if (!file->exists())
   {
      return;
   }

   if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
   {
      QMessageBox::critical(this,
                            "SWSWidget::parseXML",
                            "Couldn't open maker xml",
                            QMessageBox::Ok);
      return;
   }

   QXmlStreamReader xml(file);

   while(!xml.atEnd() && !xml.hasError())
   {
      // Read next element
      QXmlStreamReader::TokenType token = xml.readNext();

      // If token is just StartDocument, we'll go to next
      if(token == QXmlStreamReader::StartDocument)
      {
         continue;
      }

      if(token == QXmlStreamReader::StartElement)
      {
         if(xml.name() == "markers")
         {
            continue;
         }
         if(xml.name() == "marker")
         {
            m_markersLoaded.prepend(this->parseMarker(xml));
            continue;
         }
         if(xml.name() == "regionmarker")
         {
            m_regionMarkersLoaded.prepend(this->parseRegionMarker(xml));
         }
      }
   }

   // Error handling.
   if(xml.hasError())
   {
      QMessageBox::critical(this,
                            "SWSWidget::parseXML",
                            xml.errorString(),
                            QMessageBox::Ok);
   }

   xml.clear();

}

/*---------------------------------------------------------------------------*/

void SWSWidget::preferenceChanged()
{

   // Set the precision
   m_coordinateModel->setTransformerPrecision(
         m_preferences->readValueKey(Preferences::DecimalPrecision).toInt());
   m_lightToMicroCoordModel->setTransformerPrecision(
         m_preferences->readValueKey(Preferences::DecimalPrecision).toInt());

   // Set the value in the calibration text linedit box
   if(m_lightToMicroCoordModel != nullptr)
   {
      double val;
      if(m_lightToMicroCoordModel->getTransformerVariable(
               QString("m2xfm_x"), &val))
      {
         m_xOffset->setText(QString::number(val));
      }
      if(m_lightToMicroCoordModel->getTransformerVariable(
               QString("m2xfm_y"), &val))
      {
         m_yOffset->setText(QString::number(val));
      }

      updateTreeView();

   }

   checkMicroProbePVs();

}

/*---------------------------------------------------------------------------*/

void SWSWidget::restoreMarkerLoaded()
{

   // First parse the XML tags
   parseXML();

   for(int i = m_markersLoaded.size()-1; i>=0; i--)
   {

      QMap<QString, QString> marker = m_markersLoaded.at(i);

      UProbeMarkerGraphicsItem* annotation = new UProbeMarkerGraphicsItem(marker);
      annotation->setMouseOverPixelCoordModel(m_coordinateModel);
      annotation->setLightToMicroCoordModel(m_lightToMicroCoordModel);

      reloadAndSelectAnnotation(m_calTreeModel,
                                m_calAnnoTreeView,
                                m_calSelectionModel,
                                annotation,
                                QPointF(marker[UPROBE_REAL_POS_X].toDouble(),
                                        marker[UPROBE_REAL_POS_Y].toDouble()));
      annotation->setPos(QPointF(marker[UPROBE_REAL_POS_X].toDouble(),
                         marker[UPROBE_REAL_POS_Y].toDouble()));
      annotation->setPropertyValue(UPROBE_NAME,QVariant(marker[UPROBE_NAME]));
      annotation->setPropertyValue(UPROBE_LIGHT_POS_X,marker[UPROBE_LIGHT_POS_X]);
      annotation->setPropertyValue(UPROBE_LIGHT_POS_Y,marker[UPROBE_LIGHT_POS_Y]);
   }

   tabIndexChanged(MICROPROBE_IDX);

   for(int i = m_regionMarkersLoaded.size()-1; i>=0; i--)
   {

      QMap<QString, QString> marker = m_regionMarkersLoaded.at(i);

      UProbeRegionGraphicsItem* annotation = new UProbeRegionGraphicsItem(marker);
      annotation->setMouseOverPixelCoordModel(m_coordinateModel);
      annotation->setLightToMicroCoordModel(m_lightToMicroCoordModel);

      reloadAndSelectAnnotation(m_mpTreeModel,
                                m_mpAnnoTreeView,
                                m_mpSelectionModel,
                                annotation,
                                QPointF(marker[UPROBE_REAL_POS_X].toDouble(),
                                marker[UPROBE_REAL_POS_Y].toDouble()));

      annotation->setPos(QPointF(marker[UPROBE_REAL_POS_X].toDouble(),
                         marker[UPROBE_REAL_POS_Y].toDouble()));
      annotation->setPropertyValue(UPROBE_NAME,QVariant(marker[UPROBE_NAME]));
      annotation->setPropertyValue(UPROBE_PRED_POS_X,marker[UPROBE_PRED_POS_X]);
      annotation->setPropertyValue(UPROBE_PRED_POS_Y,marker[UPROBE_PRED_POS_Y]);
   }

   tabIndexChanged(CALIBRATION_IDX);

}

/*---------------------------------------------------------------------------*/

void SWSWidget::openSolver()
{
    QList< QMap<QString, double> > coordPoints;
    if (!getMarkerCoordinatePoints(coordPoints))
    {
        QMessageBox::warning(nullptr, "Warning", "You need to add coordinate points before running solver");

        return;
    }
    PythonSolverProfileWidget solverWidget;
    solverWidget.setCoordinatePoints(&coordPoints);

    if (solverWidget.exec() == QDialog::Accepted)
    {

    }
}

/*---------------------------------------------------------------------------*/

void SWSWidget::runSolver()
{
   emit solverStart();

   QList< QMap<QString, double> > coordPoints;
   if (!getMarkerCoordinatePoints(coordPoints))
      return;
   m_solver->setCoordPoints(coordPoints);

   QMap<QString, double> minCoefs;
   QMap<QString, double> newMinCoefs;

   if(m_lightToMicroCoordModel != nullptr)
   {
      double val;
      QString fmx = CoordinateTransformGlobals::keyToString(
               CoordinateTransformGlobals::m2xfm_x);
      QString fmy = CoordinateTransformGlobals::keyToString(
               CoordinateTransformGlobals::m2xfm_y);
      if(m_lightToMicroCoordModel->getTransformerVariable(fmx, &val))
      {
         minCoefs.insert(fmx, val);
      }
      else
      {
         QMessageBox::critical(0, "uProbeX", "Error: Could not find m2xfm_x variable in transfomer!");
         return;
      }
      if(m_lightToMicroCoordModel->getTransformerVariable(fmy, &val))
      {
         minCoefs.insert(fmy, val);
      }
      else
      {
         QMessageBox::critical(0, "uProbeX", "Error: Could not find m2xfm_y variable in transfomer!");
         return;
      }
   }
   else
   {
      QMessageBox::critical(0, "uProbeX", "Error: Light to Micro Transformation Model not set!");
      return;
   }
   ITransformer *trans = m_lightToMicroCoordModel->getTransformer();
   m_solver->setTransformer(trans);
   m_solver->setAllCoef(trans->getAllCoef());
   m_solver->setMinCoef(minCoefs);

   QApplication::setOverrideCursor(Qt::WaitCursor);
   bool retVal = m_solver->run();
   QApplication::restoreOverrideCursor();

   if(m_solverWidget != nullptr)
      delete m_solverWidget;
   m_solverWidget = nullptr;

   m_solverWidget = new SolverWidget();
   connect(m_solverWidget,
     SIGNAL(useUpdatedVariables(const QMap<QString, double>)),
     this,
     SLOT(useUpdatedSolverVariables(const QMap<QString, double> )));

   connect(m_solverWidget,
     SIGNAL(cancelUpdatedVariables()),
     this,
     SLOT(cancelUpdatedSolverVariables()));

   newMinCoefs = m_solver->getMinCoef();
   m_solverWidget->setCoefs(minCoefs, newMinCoefs);
   m_solverWidget->setStatusString(m_solver->getLastErrorMessage());

   if(retVal)
   {
      m_solverWidget->setUseBtnEnabled(true);
   }
   else
   {
      m_solverWidget->setUseBtnEnabled(false);
   }

   m_solverWidget->show();

}

/*---------------------------------------------------------------------------*/

void SWSWidget::setLightToMicroCoordModel(gstar::CoordinateModel *model)
{

   m_lightToMicroCoordModel = model;
   m_lightToMicroCoordModel->setTransformerPrecision(
         m_preferences->readValueKey(Preferences::DecimalPrecision).toInt());

   m_lightToMicroCoordWidget->setModel(model);

   if(m_lightToMicroCoordModel != nullptr)
   {
      double val;
      if(m_lightToMicroCoordModel->getTransformerVariable(
               CoordinateTransformGlobals::keyToString(
                  CoordinateTransformGlobals::m2xfm_x), &val))
      {
         m_xOffset->setText(QString::number(val));
      }
      if(m_lightToMicroCoordModel->getTransformerVariable(
               CoordinateTransformGlobals::keyToString(
                  CoordinateTransformGlobals::m2xfm_y), &val))
      {
         m_yOffset->setText(QString::number(val));
      }
   }

}

/*---------------------------------------------------------------------------*/

void SWSWidget::setCoordinateModel(gstar::CoordinateModel *model)
{

   if(m_coordinateModel != nullptr)
   {
      disconnect(m_coordinateModel,
                 SIGNAL(transformOutput(double, double, double)),
                 this,
                 SLOT(updatedPixelToLight(double,double,double)));
   }

   m_coordinateModel = model;
   m_coordinateModel->setTransformerPrecision(
         m_preferences->readValueKey(Preferences::DecimalPrecision).toInt());

   if(m_imageViewWidget != nullptr)
   {
      m_imageViewWidget->setCoordinateModel(model);

      connect(m_coordinateModel,
              SIGNAL(transformOutput(double, double, double)),
              this,
              SLOT(updatedPixelToLight(double,double,double)));
   }

}

/*---------------------------------------------------------------------------*/

void SWSWidget::setMarker(QString filepath)
{

   m_pathFile = filepath;
   QString originalPath;

   try
   {
      QStringList slist = m_pathFile.split('.');
      if(slist.length() > 0)
      {
         m_datasetPath = slist[0]+".xml";
         originalPath = m_datasetPath;

         QString autosavedTemporaryFile = getTemporaryDatasetPath();
         QFile autosavedTmpFile(autosavedTemporaryFile);
         if(autosavedTmpFile.exists()) {
            QFileInfo fileInfo(autosavedTmpFile);
            QDateTime lastModified = fileInfo.lastModified();

            QMessageBox msgBox;
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText("It looks like last time this application was used something went wrong, "
                           "would you like to restore auto-safe data from " + lastModified.toString());
            int ret = msgBox.exec();

            if (ret == QMessageBox::Yes)
            {
               m_datasetPath = autosavedTemporaryFile;
            }
         }
      }
      else
         throw std::string("Error:: Could not identify file!");
   }
   catch (std::string& s)
   {
      throw s;
   }
   catch (...)
   {
      throw std::string("Failed to open SWS workspace!");
   }

   restoreMarkerLoaded();
   m_datasetPath = originalPath;

}

/*---------------------------------------------------------------------------*/

void SWSWidget::saveScreenShot()
{

   QString filePath = QFileInfo(m_pathFile).absolutePath();
   QString fileName = QFileDialog::getSaveFileName(this,
             tr("Export Frame"),
             filePath + "/" + QFileInfo(m_pathFile).baseName()+".jpg",
             tr("Image Files (*.jpg *.png *.bmp)"));

   QGraphicsView* view = m_imageViewWidget->view();

   // Get scene bounding rectangle
   QRectF bounds = view->mapToScene(view->rect()).boundingRect();

   // Create image of size matching scene
   QImage img(bounds.width(), bounds.height(), QImage::Format_RGB32);

   // Create painter for painting into image
   QPainter painter(&img);

   // Render scene to image
   view -> render(&painter);
   // Save image to file

   QApplication::setOverrideCursor(Qt::WaitCursor);
   img.save(fileName);
   QApplication::restoreOverrideCursor();

}

/*---------------------------------------------------------------------------*/

QString SWSWidget::getTemporaryDatasetPath()
{
   return m_datasetPath + ".tmp";
}

/*---------------------------------------------------------------------------*/

void SWSWidget::cleanUpTemoraryXMLFiles()
{
   QString temporaryDatasetPath = getTemporaryDatasetPath();

   QFile file(temporaryDatasetPath);

   file.remove();
}

/*---------------------------------------------------------------------------*/

bool SWSWidget::verifySaveIsRequired()
{
   bool saveRequired = true;

   // Read current save data.
   QFile file(m_datasetPath);
   if (file.open(QIODevice::ReadOnly)) {
      QByteArray savedByteArray = file.readAll();

      // Generate current save data
      QByteArray currentByteArray;
      QBuffer* currentStatusBuffer = new QBuffer(&currentByteArray);
      currentStatusBuffer->open(QIODevice::ReadWrite);
      writeXMLSaveData(currentStatusBuffer);

      // Compare save data to saved data
      if (savedByteArray.size() == currentByteArray.size()) {
         int indexesPassed = 0;
         for (int i = 0; i < savedByteArray.size(); i++) {
            if (savedByteArray[i] != currentByteArray[i]) {
               break;
            }
            indexesPassed++;
         }
         if (indexesPassed == savedByteArray.size()) {
            saveRequired = false;
         }
      }

      delete currentStatusBuffer;
   }

   return saveRequired;
}


/*---------------------------------------------------------------------------*/

void SWSWidget::saveTemporaryXMLCoordinateInfo()
{
   QString tempDatasetPath = getTemporaryDatasetPath();

   saveXMLCoordinateInfo(tempDatasetPath);
}

/*---------------------------------------------------------------------------*/

void SWSWidget::saveXMLCoordinateInfo()
{
   saveXMLCoordinateInfo(m_datasetPath);

   // Auto safe data is no longer needed
   cleanUpTemoraryXMLFiles();
}

/*---------------------------------------------------------------------------*/

void SWSWidget::saveXMLCoordinateInfo(QString path)
{
   QFile file(path);

   if (!file.open(QIODevice::WriteOnly))
   {
      QMessageBox::warning(0, "Read only", "The file is in read only mode");
   }
   else
   {
      writeXMLSaveData(&file);
   }
}

/*---------------------------------------------------------------------------*/

void SWSWidget::writeXMLSaveData(QIODevice* device)
{
   QXmlStreamWriter* xmlWriter = new QXmlStreamWriter();
   xmlWriter->setDevice(device);

   xmlWriter->writeStartDocument();
   xmlWriter->writeStartElement("markers");

   // Get the crossing marker information
   getMarkerInfo(xmlWriter);
   // Get the region marker information
   getRegionMarkerInfo(xmlWriter);

   xmlWriter->writeEndElement();
   xmlWriter->writeEndDocument();

   delete xmlWriter;
}

/*---------------------------------------------------------------------------*/

void SWSWidget::tabIndexChanged(int index)
{

   switch(index)
   {
      case CALIBRATION_IDX:
         m_imageViewWidget->scene()->setModel(m_calTreeModel);
         m_mpSelectionModel->clear();
         m_imageViewWidget->scene()->setSelectionModel(m_calSelectionModel);
         break;
      case MICROPROBE_IDX:
         m_imageViewWidget->scene()->setModel(m_mpTreeModel);
         m_calSelectionModel->clear();
         m_imageViewWidget->scene()->setSelectionModel(m_mpSelectionModel);
         break;
      default:
         break;
   }

}

/*---------------------------------------------------------------------------*/

void SWSWidget::treeContextMenu(const QPoint& pos)
{

   switch(m_tabWidget->currentIndex())
   {
      case CALIBRATION_IDX:
         displayContextMenu(m_calAnnoTreeView,
                            m_calAnnoTreeView->viewport()->mapToGlobal(pos));
         break;
      case MICROPROBE_IDX:
         displayContextMenu(m_mpAnnoTreeView,
                            m_mpAnnoTreeView->viewport()->mapToGlobal(pos));
         break;
      default:
         break;
   }

}

/*---------------------------------------------------------------------------*/

void SWSWidget::updatedPixelToLight(double x, double y, double z)
{

   if(false == m_imageViewWidget->getMouseLeaveState())
   {
      m_lightToMicroCoordWidget->setCoordinate(x,y,z);
   }

}

/*--------------------------------------------------------------------------*/

void SWSWidget::useUpdatedSolverVariables(const QMap<QString, double> vars)
{

   if(vars.contains(CoordinateTransformGlobals::keyToString(
                       CoordinateTransformGlobals::m2xfm_x))
      && vars.contains(CoordinateTransformGlobals::keyToString(
                          CoordinateTransformGlobals::m2xfm_y)) )
   {
      double newX = vars[CoordinateTransformGlobals::keyToString(
               CoordinateTransformGlobals::m2xfm_x)];
      double newY = vars[CoordinateTransformGlobals::keyToString(
               CoordinateTransformGlobals::m2xfm_y)];

      m_xOffset->setText(QString::number(newX));
      m_yOffset->setText(QString::number(newY));
      offsetReturnPressed();
   }

}

/*--------------------------------------------------------------------------*/

void SWSWidget::updateTreeView()
{

   switch(m_tabWidget->currentIndex())
   {
      case CALIBRATION_IDX:
           m_calAnnoTreeView->viewport()->update();
           for(int i = 0; i<m_calTreeModel->columnCount(QModelIndex()); i++)
           {
              m_calAnnoTreeView->resizeColumnToContents(i);
           }
           break;
      case MICROPROBE_IDX:
           m_mpAnnoTreeView->viewport()->update();
           for(int i = 0; i<m_mpTreeModel->columnCount(QModelIndex()); i++)
           {
              m_mpAnnoTreeView->resizeColumnToContents(i);
           }
           break;
      default:
           break;
   }

}

/*--------------------------------------------------------------------------*/

void SWSWidget::updateContextMenus()
{

   for(RegionCaller *prc : m_actionMap.values())
   {
      delete prc;
   }
   m_actionMap.clear();
   createMicroProbeMenu();

}

/*--------------------------------------------------------------------------*/

void SWSWidget::windowChanged(Qt::WindowStates oldState,
                               Qt::WindowStates newState)
{

   Q_UNUSED(oldState);

   if(Qt::WindowMaximized || Qt::WindowActive == newState)
   {
      m_imageViewWidget->resizeEvent(nullptr);
   }

}

/*--------------------------------------------------------------------------*/

void SWSWidget::widgetChanged(bool enable)
{
   // More widget change after solver run could be added here
   m_btnRunSolver -> setEnabled(enable);

}

/*---------------------------------------------------------------------------*/
