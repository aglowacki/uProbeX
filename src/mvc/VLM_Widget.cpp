/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/VLM_Widget.h>

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

#include "gstar/Annotation/UProbeMarkerGraphicsItem.h"
#include "gstar/Annotation/UProbeRegionGraphicsItem.h"
#include "gstar/Annotation/ScanRegionGraphicsItem.h"
#include "gstar/Annotation/MarkerGraphicsItem.h"
#include "gstar/Annotation/AbstractGraphicsItem.h"
#include "gstar/Annotation/EmptyGraphicsItem.h"
#include "gstar/GStarResource.h"
#include "gstar/ImageViewToolBar.h"

#include <preferences/Preferences.h>
#include <preferences/PreferencesExport.h>
#include <preferences/AttributeGroup.h>
#include <solver/Solver.h>

#include <core/PythonRegionCaller.h>
#include <core/ShellRegionCaller.h>
//#include <core/CAEmitDataChangeHandler.h>

#include <mvc/SolverWidget.h>
#include <mvc/SolverProfileWidget.h>
#include <mvc/TIFF_Model.h>
#include "core/ColorMap.h"

#include "mvc/H5ImageModel.h"

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

static const int ID_LINEAR = 0;
static const int ID_NELDER_MEAD = 1;
static const int ID_PYTHON = 2;

//---------------------------------------------------------------------------

VLM_Widget::VLM_Widget(QWidget* parent)
: AbstractImageWidget(1,1,parent)
{

   _init();

}

//---------------------------------------------------------------------------

VLM_Widget::VLM_Widget(QString dataset_name, QWidget* parent) : AbstractImageWidget(1,1,parent)
{
   m_datasetPath = dataset_name;
   _init();
}

//---------------------------------------------------------------------------

VLM_Widget::~VLM_Widget()
{

    if (_scan_region_link_dialog != nullptr)
    {
        delete _scan_region_link_dialog;
    }

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

//---------------------------------------------------------------------------

void VLM_Widget::_init()
{
   _avail_scans = nullptr;
   m_microProbePvSet = false;
//   m_pvXHandler = nullptr;
//   m_pvYHandler = nullptr;
   m_solverWidget = nullptr;
   m_calSelectionModel = nullptr;
   m_lightToMicroCoordModel = nullptr;
   m_coordinateModel = nullptr;
   m_solver = nullptr;
   _btnSetBackground = nullptr;
   _scan_region_link_dialog = new ScanRegionLinkDialog();
   m_solverParameterParse = new SolverParameterParse();

   checkMicroProbePVs();
   createLayout();
   createActions();
   createMicroProbeMenu();
   _createSolver();
   m_imageViewWidget->clickFill(true);

   m_grabbingPvsX = false;
   m_grabbingPvsY = false;
}

//---------------------------------------------------------------------------

void VLM_Widget::addCalibration()
{

   UProbeMarkerGraphicsItem* annotation = new UProbeMarkerGraphicsItem();
   annotation->setMouseOverPixelCoordModel(m_coordinateModel);
   annotation->setLightToMicroCoordModel(m_lightToMicroCoordModel);

   insertAndSelectAnnotation(m_calTreeModel,
                             m_calAnnoTreeView,
                             m_calSelectionModel,
                             annotation);

}

//---------------------------------------------------------------------------

void VLM_Widget::addTopWindowPoints()
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
   annotation->setPropertyValue(DEF_STR_DISPLAY_NAME, btmRight);
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
   annotation->setPropertyValue(DEF_STR_DISPLAY_NAME, btmLeft);

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
   annotation->setPropertyValue(DEF_STR_DISPLAY_NAME, topRight);

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
   annotation->setPropertyValue(DEF_STR_DISPLAY_NAME, topLeft);


}

//---------------------------------------------------------------------------

void VLM_Widget::onConfigRegionLink()
{
    _scan_region_link_dialog->show();
}

//---------------------------------------------------------------------------

void VLM_Widget::onAddMicroProbeRegion()
{
   ScanRegionGraphicsItem* annotation = new ScanRegionGraphicsItem();

   connect(annotation, &ScanRegionGraphicsItem::planRemoved, this, &VLM_Widget::onScanRemoved);
   //UProbeRegionGraphicsItem* annotation = new UProbeRegionGraphicsItem();
   annotation->setMouseOverPixelCoordModel(m_coordinateModel);
   annotation->setLightToMicroCoordModel(m_lightToMicroCoordModel);

   insertAndSelectAnnotation(m_mpTreeModel,
                             m_mpAnnoTreeView,
                             m_mpSelectionModel,
                             annotation);

}

//---------------------------------------------------------------------------

void VLM_Widget::addMicroProbeRegion(gstar::UProbeRegionGraphicsItem* annotation)
{
   annotation->setMouseOverPixelCoordModel(m_coordinateModel);
   annotation->setLightToMicroCoordModel(m_lightToMicroCoordModel);

   insertAndSelectAnnotation(m_mpTreeModel,
                             m_mpAnnoTreeView,
                             m_mpSelectionModel,
                             annotation);
}

//---------------------------------------------------------------------------

void VLM_Widget::_createLightToMicroCoords(int id)
{
    ITransformer* lightTransformer = nullptr;
    QMap<QString, double> allCoefs;

    if (m_lightToMicroCoordModel != nullptr)
    {
        lightTransformer = m_lightToMicroCoordModel->getTransformer();
        if (lightTransformer != nullptr)
        {
            delete lightTransformer;
            m_lightToMicroCoordModel->setTransformer(nullptr);
        }
        lightTransformer = nullptr;
    }

   if(id == ID_LINEAR)
   {
      if (lightTransformer == nullptr)
      {
         lightTransformer = new LinearCoordTransformer();
      }
   }
   else if (id == ID_NELDER_MEAD)
   {
      QStringList coefList = Preferences::inst()->getValue(STR_PRF_NMCoefficient).toStringList();
      if (m_solverParameterParse->parseSolverCoefList(coefList))
      {
         m_solverParameterParse->getTransform(allCoefs);
      }
      else
      {
         //generate defaults
         SV_CoordTransformer c;
         allCoefs = c.getAllCoef();
      }
      

      if (lightTransformer == nullptr)
      {
         lightTransformer = new SV_CoordTransformer();
      }
   }
   else
   {
      QStringList coefList = Preferences::inst()->getValue(STR_PRF_PythonCoefficient).toStringList();

      if (false == m_solverParameterParse->parseSolverCoefList(coefList))
      {
         Preferences::inst()->setValue(STR_PRF_SolverCheckedID, 0);
         _createSolver();
         return;
      }
      m_solverParameterParse->getTransform(allCoefs);

      if (lightTransformer == nullptr)
      {
         QString pythonFileName = Preferences::inst()->getValue(STR_PRF_PythonSolverName).toString();
         if (pythonFileName.isEmpty())
         {
               QMessageBox::critical(nullptr, "Error",
                  "Must have a python script having a transform function, using default transformer right now.");
               Preferences::inst()->setValue(STR_PRF_SolverCheckedID, 0);
               _createSolver();
               return;
         }
         QFileInfo fileInfo = QFileInfo(pythonFileName);
         lightTransformer = new PythonTransformer(
               fileInfo.path(),
               fileInfo.baseName(),
               QString("my_transform"));
      }
   }

   if (lightTransformer->Init(allCoefs))
   {
      if (m_lightToMicroCoordModel != nullptr)
      {
         m_lightToMicroCoordModel->setTransformer(lightTransformer);
      }
      else
      {
         m_lightToMicroCoordModel = new gstar::CoordinateModel(lightTransformer);
      }
   }
   else
   {
      QMessageBox::critical(nullptr, "uProbeX", "Error initializeing Transformer!");
      logW << "Could not init Transformer\n";
   }

   m_lightToMicroCoordWidget->setModel(m_lightToMicroCoordModel);

}

//---------------------------------------------------------------------------

void VLM_Widget::_createSolver()
{

    QMap<QString, double> dict_options;

    if (m_solver == nullptr)
    {
        m_solver = new Solver();
    }

    int id = Preferences::inst()->getValue(STR_PRF_SolverCheckedID).toInt();

    _createLightToMicroCoords(id);

/*
if (id == ID_LINEAR)
   {
      LinearSolver* ls = new LinearSolver();
      m_solver->setImpl(ls);
   }
   else
*/
    if (id == ID_LINEAR || id == ID_NELDER_MEAD)
   {
      NelderMeadSolver* nm = new NelderMeadSolver();

      ///nm->setTransformer();
      QStringList optionList = Preferences::inst()->getValue(STR_PRF_NMOptions).toStringList();
      if (false == m_solverParameterParse->parseSolverOptionList(optionList))
      {
         logE << "Error reading options for NM solver\n";
         // Initialize with the default option
         dict_options = nm->getOptions();
      }
      else
      {
         m_solverParameterParse->getOptions(dict_options);
      }

      m_solver->setImpl(nm);
   }
   else
   {
      PythonSolver* ps = new PythonSolver();

      QString pythonFileName = Preferences::inst()->getValue(STR_PRF_PythonSolverName).toString();
      QFileInfo fileInfo = QFileInfo(pythonFileName);

      QStringList optionList = Preferences::inst()->getValue(STR_PRF_PythonOptions).toStringList();

      if (pythonFileName.isEmpty()
         || false == m_solverParameterParse->parseSolverOptionList(optionList)
         || false == ps->initialPythonSolver(fileInfo.path(),
               fileInfo.baseName(),
               QString("my_solver")))
      {
         logE << "Error reading options for python solver, reverting to NelderMeadSolver\n";
         QMessageBox::critical(nullptr, "uProbeX", "Error initializeing Python solver,  reverting to NelderMeadSolver");
         Preferences::inst()->setValue(STR_PRF_SolverCheckedID, 0);
         _createSolver();
         return;
      }
      m_solverParameterParse->getOptions(dict_options);
      m_solver->setImpl(ps);

   }
   ITransformer* trans = m_lightToMicroCoordModel->getTransformer();
   m_solver->setAllCoef(trans->getAllCoef());
   m_solver->setOptions(dict_options);
   m_solver->setTransformer(trans);

}

//---------------------------------------------------------------------------

UProbeRegionGraphicsItem* VLM_Widget::getSelectedRegion()
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

//---------------------------------------------------------------------------

void VLM_Widget::zoomMicroProbeRegion()
{
   UProbeRegionGraphicsItem* item = getSelectedRegion();
   if (item != nullptr) {
      item->zoomToRegion();
   }
}

//---------------------------------------------------------------------------

void VLM_Widget::onExportSelectedRegionInformation()
{
    exportSelectedRegionInformation(nullptr, nullptr);
}

//---------------------------------------------------------------------------

void VLM_Widget::exportSelectedRegionInformation(QList<QString>* summaryInformation, QList<QString>* summaryWarnings)
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
      bool zoomToRegion = Preferences::inst()->getValue(STR_PRF_ExportZoomToRegion).toBool();
      QString exportDirectoryName = Preferences::inst()->getValue(STR_PRF_ExportDirectoryInDataset).toString();

      bool printNameOnImage = Preferences::inst()->getValue(STR_PRF_ExportPrintNameOnExportedImage).toBool();
      bool printPredictedXYOnImage = Preferences::inst()->getValue(STR_PRF_ExportPrintPxPyOnExportedImage).toBool();
      bool printWidthHeightOnImage = Preferences::inst()->getValue(STR_PRF_ExportPrintWidthHeightOnExportedImage).toBool();
      int selectedXMLExportOption = Preferences::inst()->getValue(STR_PRF_ExportSelectedXmlOption).toInt();
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

//---------------------------------------------------------------------------

void VLM_Widget::exportRegionXMLAndImage(UProbeRegionGraphicsItem* item,
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
         maxWidth = std::max(fm.horizontalAdvance(itemName), maxWidth);
      }
      if (printPredictedXYOnImage) {
         maxWidth = std::max(fm.horizontalAdvance(predPositionXText), maxWidth);
         maxWidth = std::max(fm.horizontalAdvance(predPositionYText), maxWidth);
      }
      if (printWidthHeightOnImage) {
         maxWidth = std::max(fm.horizontalAdvance(widthText), maxWidth);
         maxWidth = std::max(fm.horizontalAdvance(heightText), maxWidth);
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

//---------------------------------------------------------------------------

void VLM_Widget::callbackPvXUpdatedFloat(float val)
{

   //logI<<"float x val = "<<val;
   callbackPvXUpdatedDouble((double) val);

}

//---------------------------------------------------------------------------

void VLM_Widget::callbackPvYUpdatedFloat(float val)
{

   //logI<<"float y val = "<<val;
   callbackPvYUpdatedDouble((double) val);

}

//---------------------------------------------------------------------------

void VLM_Widget::callbackPvXUpdatedDouble(double val)
{

   if(m_calSelectionModel == nullptr || m_grabbingPvsX == false)
      return;

   //logI<<"double x val = "<<val;

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
      logW<<"No marker selection for callbackPvXUpdatedDouble";
   }

   m_grabbingPvsX = false;

}

//---------------------------------------------------------------------------

void VLM_Widget::callbackPvYUpdatedDouble(double val)
{

   if(m_calSelectionModel == nullptr || m_grabbingPvsY == false)
      return;

   //logI<<"double y val = "<<val;

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
      logW<<"No marker selection for callbackPvYUpdatedDouble";
   }

   m_grabbingPvsY = false;

}

//---------------------------------------------------------------------------

void VLM_Widget::calModelDataChanged(const QModelIndex& topLeft,
                                    const QModelIndex& bottomRight)
{

   m_calAnnoTreeView->resizeColumnToContents(topLeft.column());
   m_calAnnoTreeView->resizeColumnToContents(bottomRight.column());

}

//---------------------------------------------------------------------------

void VLM_Widget::createActions()
{

   m_addCalibrationAction = new QAction("Add Point", this);
   connect(m_addCalibrationAction, &QAction::triggered, this, &VLM_Widget::addCalibration);

   m_addMicroPrboeRegionAction = new QAction("Add Region", this);   
   connect(m_addMicroPrboeRegionAction,&QAction::triggered,this,&VLM_Widget::onAddMicroProbeRegion);

   m_zoomMicroProbeRegionAction = new QAction("Zoom to Region", this);
   connect(m_zoomMicroProbeRegionAction,&QAction::triggered,this,&VLM_Widget::zoomMicroProbeRegion);

   m_exportMicroProbeRegionInfoAction = new QAction("Export Region Information", this);
   connect(m_exportMicroProbeRegionInfoAction,&QAction::triggered,this,&VLM_Widget::onExportSelectedRegionInformation);

   _linkRegionToDatasetAction = new QAction("Link to Dataset", this);
   connect(_linkRegionToDatasetAction, &QAction::triggered, this, &VLM_Widget::linkRegionToDataset);

   m_grabMicroProbePVAction = new QAction("Grab MicroProbe PV", this);
   connect(m_grabMicroProbePVAction,&QAction::triggered,this,&VLM_Widget::grabMicroProbePV);

   _queueMicroPrboeRegionAction = new QAction("Queue Scan Region", this);   
   connect(_queueMicroPrboeRegionAction,&QAction::triggered,this,&VLM_Widget::onQueueMicroProbeRegion);
}

//---------------------------------------------------------------------------

void VLM_Widget::linkRegionToDataset()
{
    QModelIndexList selectedIndexes = m_mpSelectionModel->selectedRows();
    if (selectedIndexes.size() == 1)
    {
        // When sum
        for (const QModelIndex& index : selectedIndexes)
        {
            if (index.isValid()) 
            {
                AbstractGraphicsItem* abstractItem = static_cast<AbstractGraphicsItem*>(index.internalPointer());
                UProbeRegionGraphicsItem* item = dynamic_cast<UProbeRegionGraphicsItem*>(abstractItem);
                if (item != nullptr)
                {
                    QGraphicsScene* scene = item->scene();
                   
                    QPoint pp = item->pos().toPoint();
                    QRect rr = item->boundingRectMarker().toRect();
                    pp.setX(pp.x() + rr.x());
                    pp.setY(pp.y() + rr.y());
                    QSize size(rr.width(), rr.height());
                    QRect rect = QRect(pp, size);
                    QRectF saved_recft = scene->sceneRect();
                    scene->setSceneRect(rect);
                    QImage image(size, QImage::Format_ARGB32);
                    image.fill(Qt::transparent);
                    QPainter painter(&image);
                    item->hide();
                    scene->render(&painter);
                    item->show();
                    //QString filepath = _model->getDataPath();
                    //QString fileName = "c:\\temp\\file_name.png"; 
                    //if (false == image.save("c:\\temp\\file_name.png"))
                    //{
                    //    QMessageBox::critical(this, "Error saving", "Error saving pixmap");
                    //}

                    scene->setSceneRect(saved_recft);
                    emit onLinkRegionToDataset(item->displayName(), _model->getDataPath(), image);
                }
            }
        }
    }
    else
    {
        QMessageBox::warning(this, "Select 1 region", "Please select only 1 region to link.");
    }
}

//---------------------------------------------------------------------------

void VLM_Widget::cancelUpdatedSolverVariables()
{

   //emit cancelSolverVariableUpdate();

}

//---------------------------------------------------------------------------

void VLM_Widget::createCalibrationTab()
{

   m_calTreeModel = new gstar::AnnotationTreeModel();

   connect(m_calTreeModel, &gstar::AnnotationTreeModel::dataChanged, this, &VLM_Widget::calModelDataChanged);

   m_calSelectionModel = new QItemSelectionModel(m_calTreeModel);

   m_calAnnoTreeView = new QTreeView();
   //m_calAnnoTreeView->setAutoFillBackground(true);
   m_calAnnoTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
   m_calAnnoTreeView->setAnimated(true);
   m_calAnnoTreeView->setModel(m_calTreeModel);
   m_calAnnoTreeView->setHeaderHidden(true);
   m_calAnnoTreeView->setSelectionModel(m_calSelectionModel);
   m_calAnnoTreeView->setContextMenuPolicy(Qt::CustomContextMenu);

//   m_calAnnoTreeView->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);

   connect(m_calAnnoTreeView, &QTreeView::customContextMenuRequested, this, &VLM_Widget::treeContextMenu);

   connect(m_calAnnoTreeView, &QTreeView::doubleClicked, this, &VLM_Widget::treeDoubleClicked);

   m_btnAddCalibration = new QPushButton("Add Calibration Point");
   connect(m_btnAddCalibration, &QPushButton::clicked, this, &VLM_Widget::addCalibration);

   m_btnAddTopWindowPoints = new QPushButton("Add Top Window Points");
   connect(m_btnAddTopWindowPoints, &QPushButton::clicked, this, &VLM_Widget::addTopWindowPoints);

   m_btnAddBottomWindowPoints = new QPushButton("Add Bottom Window Points");
   connect(m_btnAddBottomWindowPoints, &QPushButton::clicked, this, &VLM_Widget::addTopWindowPoints);

   _btnSetBackground = new QPushButton("Set Background Image");
   connect(_btnSetBackground, &QPushButton::clicked, this, &VLM_Widget::onUpdateBackgroundImage);

   _btnSetBackground->setVisible(false);

   _btnCaptureBackground = new QPushButton("Capture Background Image");
   connect(_btnCaptureBackground, &QPushButton::clicked, this, &VLM_Widget::onCaptureBackgroundImage);

   m_btnConfigRegionLink = new QPushButton("Config Region Link");
   connect(m_btnConfigRegionLink, &QPushButton::clicked, this, &VLM_Widget::onConfigRegionLink);
   

   _btnCaptureBackground->setVisible(false);

   m_btnRunSolver = new QPushButton("Update Transform");
   connect(m_btnRunSolver, &QPushButton::clicked, this, &VLM_Widget::openSolver);


   QVBoxLayout* infoLayout = new QVBoxLayout();
   QHBoxLayout* buttonLayout = new QHBoxLayout();
   QHBoxLayout* buttonLayout2 = new QHBoxLayout();
   QHBoxLayout* buttonLayout3 = new QHBoxLayout();
   QHBoxLayout* offsetLayout = new QHBoxLayout();

   buttonLayout->addWidget(m_btnAddCalibration);
   buttonLayout->addWidget(m_btnRunSolver);
   buttonLayout2->addWidget(m_btnAddTopWindowPoints);
   buttonLayout2->addWidget(m_btnAddBottomWindowPoints);
   buttonLayout3->addWidget(_btnSetBackground);
   buttonLayout3->addWidget(m_btnConfigRegionLink);
   //buttonLayout3->addWidget(_btnCaptureBackground);
   infoLayout->addLayout(offsetLayout);
   infoLayout->addLayout(buttonLayout);
   infoLayout->addLayout(buttonLayout2);
   infoLayout->addLayout(buttonLayout3);
   infoLayout->addWidget(m_calAnnoTreeView);

   m_calibrationTabWidget = new QWidget(this);
   //m_calibrationTabWidget->setAutoFillBackground(true);
   m_calibrationTabWidget->setLayout(infoLayout);

}

//---------------------------------------------------------------------------

void VLM_Widget::CallPythonFunc()
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

//---------------------------------------------------------------------------

void VLM_Widget::createMicroProbeMenu()
{
  
   QStringList rList = Preferences::inst()->getValue(STR_PRF_RegionMenuList)
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
                     logW<<"Error initializing shell call: "<<attr->getName().toStdString() << "\n";
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
                     logW<<"Error initializing python call: "<<attr->getName().toStdString() << "\n";
                  }
               }               

               if(prc != nullptr)
               {
                  connect(action,&QAction::triggered,this,&VLM_Widget::CallPythonFunc);

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

//---------------------------------------------------------------------------

void VLM_Widget::createMicroProbeTab()
{

   m_mpTreeModel = new gstar::AnnotationTreeModel();

   connect(m_mpTreeModel, &gstar::AnnotationTreeModel::dataChanged, this, &VLM_Widget::microModelDataChanged);

   m_mpSelectionModel = new QItemSelectionModel(m_mpTreeModel);

   m_mpAnnoTreeView = new QTreeView();
   //m_mpAnnoTreeView->setAutoFillBackground(true);
   m_mpAnnoTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
   m_mpAnnoTreeView->setAnimated(true);
   m_mpAnnoTreeView->setModel(m_mpTreeModel);
   m_mpAnnoTreeView->setHeaderHidden(true);
   m_mpAnnoTreeView->setSelectionModel(m_mpSelectionModel);
   m_mpAnnoTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(m_mpAnnoTreeView, &QTreeView::customContextMenuRequested, this, &VLM_Widget::treeContextMenu);
   connect(m_mpAnnoTreeView, &QTreeView::doubleClicked, this, &VLM_Widget::treeDoubleClicked);
   connect(m_mpAnnoTreeView, &QTreeView::clicked, this, &VLM_Widget::treeDoubleClicked);


   m_btnAddMicroProbe = new QPushButton("Add Micro Probe Region");
   connect(m_btnAddMicroProbe, &QPushButton::clicked, this, &VLM_Widget::onAddMicroProbeRegion);

   QHBoxLayout* btnLayout = new QHBoxLayout();
   btnLayout->addWidget(m_btnAddMicroProbe);

   QVBoxLayout* infoLayout = new QVBoxLayout();
   infoLayout->addItem(btnLayout);
   infoLayout->addWidget(m_mpAnnoTreeView);

   m_microProbeTabWidget = new QWidget(this);
   //m_microProbeTabWidget->setAutoFillBackground(true);
   m_microProbeTabWidget->setLayout(infoLayout);

}

//---------------------------------------------------------------------------

void VLM_Widget::createLayout()
{

   connect(m_tabWidget, &QTabWidget::currentChanged, this, &VLM_Widget::tabIndexChanged);

   createCalibrationTab();
   createMicroProbeTab();

   QLayout* layout = generateDefaultLayout();

   //m_range->setRangeWidgetVisible(false);
   //m_labelWidthAction->setVisible(false);
   //m_imageWidthDimAction->setVisible(false);
   //m_labelHeightAction->setVisible(false);
   //m_imageHeightDimAction->setVisible(false);

   m_tabWidget->addTab(m_calibrationTabWidget, QIcon(), "Setup");
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
   m_imageViewWidget->setCountsVisible(false);
   setLayout(layout);

   m_tabWidget->setCurrentIndex(MICROPROBE_IDX);
}

//---------------------------------------------------------------------------

void VLM_Widget::displayContextMenu(QWidget* parent,
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
               menu.addAction(_queueMicroPrboeRegionAction);
               menu.addAction(m_zoomMicroProbeRegionAction);
               menu.addAction(m_exportMicroProbeRegionInfoAction);
               menu.addAction(_linkRegionToDatasetAction);
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

void VLM_Widget::checkMicroProbePVs()
{
/*
   try
   {

      QString microX = Preferences::inst()->getValue(STR_PRF_MicroProbeXPv).toString();
      QString microY = Preferences::inst()->getValue(STR_PRF_MicroProbeYPv).toString();

      if (microX.length() > 0 && microY.length() > 0)
      {

         ChannelAccess* ca = ChannelAccess::getInstance();


         m_pvX = ca->getPV(microX.toLatin1().data(), 1, 1);
         m_pvY = ca->getPV(microY.toLatin1().data(), 1, 1);

         switch(m_pvX->getType())
         {
         case DBR_FLOAT:
            m_pvXHandler = new CAEmitDataChangeHandler<float, VLM_Widget>(
                                         this, &VLM_Widget::callbackPvXUpdatedFloat);
            m_pvYHandler = new CAEmitDataChangeHandler<float, VLM_Widget>(
                                         this, &VLM_Widget::callbackPvYUpdatedFloat);
            m_pvX->subscribeDataChangeEvent(
                     (CAEmitDataChangeHandler<float, VLM_Widget>*)m_pvXHandler);
            m_pvY->subscribeDataChangeEvent(
                     (CAEmitDataChangeHandler<float, VLM_Widget>*)m_pvYHandler);
            //pvType = FLOAT
            m_microProbePvSet = true;
            logW<<"Set micro pv true";
            break;
         case DBR_DOUBLE:
            m_pvXHandler = new CAEmitDataChangeHandler<double, VLM_Widget>(
                                         this, &VLM_Widget::callbackPvXUpdatedDouble);
            m_pvYHandler = new CAEmitDataChangeHandler<double, VLM_Widget>(
                                         this, &VLM_Widget::callbackPvYUpdatedDouble);
            m_pvX->subscribeDataChangeEvent(
                     (CAEmitDataChangeHandler<double, VLM_Widget>*)m_pvXHandler);
            m_pvY->subscribeDataChangeEvent(
                     (CAEmitDataChangeHandler<double, VLM_Widget>*)m_pvYHandler);
            //pvType = DOUBLE
            m_microProbePvSet = true;
            logW<<"Set micro pv true";
            break;
         default:
            m_microProbePvSet = false;
            logW<<"Set micro pv false";
         }

         m_pvX->asynGet(1, (CAEmitDataChangeHandler<int, VLM_Widget>*)m_pvXHandler);
         m_pvY->asynGet(1, (CAEmitDataChangeHandler<int, VLM_Widget>*)m_pvYHandler);
      }
   }
   catch(...)
   {
      logW<<"Failed to initialize PV's";
   }
   */
}

//---------------------------------------------------------------------------

void VLM_Widget::deleteItem()
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
            //logI<<"index "<<index.row();
            treeModel->removeRow(index.row(), index);
         }
      }
   }

}

//---------------------------------------------------------------------------

void VLM_Widget::duplicateItem()
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

//---------------------------------------------------------------------------

void VLM_Widget::getMarkerInfo(QXmlStreamWriter* xmlWriter)
{

   QModelIndex first = m_calTreeModel->index(0,0,QModelIndex());
   gstar::AbstractGraphicsItem* groupPtr =
         (gstar::AbstractGraphicsItem*)first.internalPointer();
   if(groupPtr == nullptr)
      return;
   std::list<gstar::AbstractGraphicsItem*> clist = groupPtr->childList();

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

//---------------------------------------------------------------------------

void VLM_Widget::getRegionMarkerInfo(QXmlStreamWriter* xmlWriter)
{

   QModelIndex first = m_mpTreeModel->index(0,0,QModelIndex());
   gstar::AbstractGraphicsItem* groupPtr =
         (gstar::AbstractGraphicsItem*)first.internalPointer();
   if(groupPtr == nullptr)
      return;
   std::list<gstar::AbstractGraphicsItem*> clist = groupPtr->childList();

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

//---------------------------------------------------------------------------

QString VLM_Widget::getModelFileName()
{

   return m_pathFile;

}

//---------------------------------------------------------------------------

bool VLM_Widget::getMarkerCoordinatePoints(QList < QMap<QString,double> >&
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
      logW<<"No Points to run solver on!\n";
      return false;
   }


   std::list<gstar::AbstractGraphicsItem*> clist = groupPtr->childList();

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

         QVariant vname = child->propertyValue(DEF_STR_DISPLAY_NAME);
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

//---------------------------------------------------------------------------

void VLM_Widget::grabMicroProbePV()
{
/*
   try
   {
      //set mouse to wait
      m_grabbingPvsX = true;
      m_grabbingPvsY = true;

      m_pvX->asynGet(1, (CAEmitDataChangeHandler<double, VLM_Widget>*)m_pvXHandler);
      m_pvY->asynGet(1, (CAEmitDataChangeHandler<double, VLM_Widget>*)m_pvYHandler);
   }
   catch(...)
   {
      logW<<"Error calling caget for PV's";
   }
   */
}

//---------------------------------------------------------------------------

void VLM_Widget::microModelDataChanged(const QModelIndex& topLeft,
                                      const QModelIndex& bottomRight)
{

   m_mpAnnoTreeView->resizeColumnToContents(topLeft.column());
   m_mpAnnoTreeView->resizeColumnToContents(bottomRight.column());

}

//---------------------------------------------------------------------------

void VLM_Widget::setModel(VLM_Model* model)
{
    if (_model != model)
    {
        _model = model;
        if (_model != nullptr)
        {
            if (_model->loaded())
            {
                updateFrame(_model->getImage());
                setCoordinateModel(_model->getCoordModel());
            }
            restoreMarkerLoaded();
        }
    }
}

//---------------------------------------------------------------------------

void VLM_Widget::preferenceChanged()
{

   // Set the precision
   m_coordinateModel->setTransformerPrecision( Preferences::inst()->getValue(STR_PRF_DecimalPrecision).toInt());
   m_lightToMicroCoordModel->setTransformerPrecision( Preferences::inst()->getValue(STR_PRF_DecimalPrecision).toInt());
   /*
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
   */
   checkMicroProbePVs();

}

//---------------------------------------------------------------------------

void VLM_Widget::restoreMarkerLoaded()
{

   // get from model
  // parseXML();
	if (_model == nullptr)
	{
		return;
	}

   auto regionMarkersLoaded = _model->getRegionMarkers();

   while (m_mpTreeModel->rowCount() > 0)
   {
       QModelIndex first = m_mpTreeModel->index(0, 0, QModelIndex());
       m_mpTreeModel->removeRow(0, first);
   }

   for(int i = regionMarkersLoaded.size()-1; i>=0; i--)
   {

      QMap<QString, QString> marker = regionMarkersLoaded.at(i);

      UProbeRegionGraphicsItem* annotation = new UProbeRegionGraphicsItem(marker);
      annotation->setMouseOverPixelCoordModel(m_coordinateModel);
      annotation->setLightToMicroCoordModel(m_lightToMicroCoordModel);

      reloadAndSelectAnnotation(m_mpTreeModel,
                                m_mpAnnoTreeView,
                                m_mpSelectionModel,
                                annotation,
                                QPointF(marker[UPROBE_REAL_POS_X].toDouble(),
                                marker[UPROBE_REAL_POS_Y].toDouble()));

      annotation->setPropertyValue(UPROBE_NAME,QVariant(marker[UPROBE_NAME]));
      annotation->setPropertyValue(UPROBE_PRED_POS_X,marker[UPROBE_PRED_POS_X]);
      annotation->setPropertyValue(UPROBE_PRED_POS_Y,marker[UPROBE_PRED_POS_Y]);
   }

   tabIndexChanged(CALIBRATION_IDX);

   auto markersLoaded = _model->getMarkers();

   while (m_calTreeModel->rowCount() > 0)
   {
       QModelIndex first = m_calTreeModel->index(0, 0, QModelIndex());
       m_calTreeModel->removeRow(0, first);
   }

   for (int i = markersLoaded.size() - 1; i >= 0; i--)
   {

       QMap<QString, QString> marker = markersLoaded.at(i);

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
       annotation->setPropertyValue(UPROBE_NAME, QVariant(marker[UPROBE_NAME]));
       annotation->setPropertyValue(UPROBE_LIGHT_POS_X, marker[UPROBE_LIGHT_POS_X]);
       annotation->setPropertyValue(UPROBE_LIGHT_POS_Y, marker[UPROBE_LIGHT_POS_Y]);
   }

   tabIndexChanged(MICROPROBE_IDX);

}

//---------------------------------------------------------------------------
/*
void VLM_Widget::openSolver()
{
    
    QList< QMap<QString, double> > coordPoints;
    SolverProfileWidget solverWidget;
    connect(&solverWidget, &SolverWidget::useUpdatedVariables, this, &VLM_Widget::useUpdatedSolverVariables );
    connect(&solverWidget, &SolverWidget::cancelUpdatedVariables, this, &VLM_Widget::cancelUpdatedSolverVariables );

    if (getMarkerCoordinatePoints(coordPoints))
    {
        solverWidget.setCoordinatePoints(&coordPoints);
    }

    if (solverWidget.exec() == QDialog::Accepted)
    {

    }
}
*/

//---------------------------------------------------------------------------

void VLM_Widget::openSolver()
{
   emit solverStart();

   QList< QMap<QString, double> > coordPoints;
   if (!getMarkerCoordinatePoints(coordPoints))
      return;
   if (m_solver == nullptr)
   {
       return;
   }

   m_solver->setCoordPoints(coordPoints);
   
   QMap<QString, double> newMinCoefs;

   if(m_lightToMicroCoordModel == nullptr)
   {
      QMessageBox::critical(0, "uProbeX", "Error: Light to Micro Transformation Model not set!");
      return;
   }
   ITransformer *trans = m_lightToMicroCoordModel->getTransformer();
   QMap<QString, double> minCoefs = trans->getAllCoef();
   m_solver->setTransformer(trans);
   m_solver->setAllCoef(minCoefs);
   m_solver->setMinCoef(minCoefs);

   QApplication::setOverrideCursor(Qt::WaitCursor);
   bool retVal = m_solver->run();
   QApplication::restoreOverrideCursor();

   if(m_solverWidget != nullptr)
      delete m_solverWidget;
   m_solverWidget = nullptr;

   m_solverWidget = new SolverWidget();
   connect(m_solverWidget, &SolverWidget::useUpdatedVariables, this, &VLM_Widget::useUpdatedSolverVariables );
   connect(m_solverWidget, &SolverWidget::cancelUpdatedVariables, this, &VLM_Widget::cancelUpdatedSolverVariables );

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

//---------------------------------------------------------------------------
/*

void PreferencesSolverOption::runSolver()
{

    QMap<QString, double> minCoefs;
    QMap<QString, double> newMinCoefs;
    QMap<QString, double> allCoefs;
    QMap<QString, double> options;
    QList< QMap<QString, double> > coordPoints;

    AbstractSolver* impl = m_solver->getImpl();
    SolverParameterWidget* widget;

    if (impl != nullptr)
    {
        m_solver->setImpl(nullptr);
        delete impl;
        impl = nullptr;
    }

    if (m_buttonGroup->checkedId() == NM_SELECTED)
    {
        widget = m_NMSolverWidget;

        m_solver->setImpl(new NelderMeadSolver());

    }
    else if (m_buttonGroup->checkedId() == PY_SELECTED)
    {
        widget = m_pythonSolverWidget;

        impl = new PythonSolver();
        //      QFileInfo fileInfo = QFileInfo(getPythonSolverPath());
        //      if(false ==((PythonSolver*)impl)->initialPythonSolver(fileInfo.path(),
        //                                                            fileInfo.baseName(),
        //                                                            "my_solver"))
        //      {
        //         delete impl;
        //         logW<<"Error initializing python solver";
        //         QMessageBox::critical(nullptr,"Error", "Error Initializing Python Solver!");
        //         return;
        //      }
        
        m_solver->setImpl(impl);
    }

    getSolverPropertiesFromModel(widget,
        allCoefs,
        minCoefs,
        options);

    m_transformer->Init(allCoefs);
    m_solver->setTransformer(m_transformer);
    m_solver->setAllCoef(allCoefs);
    m_solver->setOptions(options);
    m_solver->setMinCoef(minCoefs);

    if (m_windowList.size() > 0)
    {
        VLM_Widget* sws = (VLM_Widget*)m_windowList.at(0);
        sws->getMarkerCoordinatePoints(coordPoints);
    }
    else
    {
        QMessageBox::critical(nullptr, "Solver Error", "Could not find active SWS workspace!");
        return;
    }

    m_solver->setCoordPoints(coordPoints);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool retVal = m_solver->run();
    QApplication::restoreOverrideCursor();

    if (m_solverWidget != nullptr)
        delete m_solverWidget;
    m_solverWidget = nullptr;

    m_solverWidget = new SolverWidget();
    connect(m_solverWidget,
        &useUpdatedVariables,
        this,
        &useUpdatedSolverVariables);

    newMinCoefs = m_solver->getMinCoef();
    m_solverWidget->setCoefs(minCoefs, newMinCoefs);
    m_solverWidget->setStatusString(m_solver->getLastErrorMessage());

    if (retVal)
    {
        m_solverWidget->setUseBtnEnabled(true);
    }
    else
    {
        m_solverWidget->setUseBtnEnabled(false);
    }

    m_solverWidget->show();

}
*/

//---------------------------------------------------------------------------

void VLM_Widget::setLightToMicroCoordModel(gstar::CoordinateModel *model)
{

   m_lightToMicroCoordModel = model;
   m_lightToMicroCoordModel->setTransformerPrecision(
         Preferences::inst()->getValue(STR_PRF_DecimalPrecision).toInt());

   m_lightToMicroCoordWidget->setModel(model);
   /*
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
   */
}

//---------------------------------------------------------------------------

void VLM_Widget::setCoordinateModel(gstar::CoordinateModel *model)
{

	if (m_coordinateModel != nullptr)
	{
		disconnect(m_coordinateModel, &gstar::CoordinateModel::transformOutput, this,	&VLM_Widget::updatedPixelToLight);
	}

	m_coordinateModel = model;
    if (m_coordinateModel != nullptr)
    {
        m_coordinateModel->setTransformerPrecision(Preferences::inst()->getValue(STR_PRF_DecimalPrecision).toInt());
    }

   if(m_imageViewWidget != nullptr)
   {
      m_imageViewWidget->setCoordinateModel(model);

      connect(m_coordinateModel, &gstar::CoordinateModel::transformOutput, this,	&VLM_Widget::updatedPixelToLight);
   }

}

//---------------------------------------------------------------------------

//void VLM_Widget::setMarker(QString filepath)
//{
//
//   m_pathFile = filepath;
//   QString originalPath;
//
//   try
//   {
//      QStringList slist = m_pathFile.split('.');
//      if(slist.length() > 0)
//      {
//         m_datasetPath = slist[0]+".xml";
//         originalPath = m_datasetPath;
//
//         QString autosavedTemporaryFile = getTemporaryDatasetPath();
//         QFile autosavedTmpFile(autosavedTemporaryFile);
//         if(autosavedTmpFile.exists()) {
//            QFileInfo fileInfo(autosavedTmpFile);
//            QDateTime lastModified = fileInfo.lastModified();
//
//            QMessageBox msgBox;
//            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
//            msgBox.setDefaultButton(QMessageBox::No);
//            msgBox.setIcon(QMessageBox::Warning);
//            msgBox.setText("It looks like last time this application was used something went wrong, "
//                           "would you like to restore auto-safe data from " + lastModified.toString());
//            int ret = msgBox.exec();
//
//            if (ret == QMessageBox::Yes)
//            {
//               m_datasetPath = autosavedTemporaryFile;
//            }
//         }
//      }
//      else
//         throw std::string("Error:: Could not identify file!");
//   }
//   catch (std::string& s)
//   {
//      throw s;
//   }
//   catch (...)
//   {
//      throw std::string("Failed to open SWS workspace!");
//   }
//
//   restoreMarkerLoaded();
//   m_datasetPath = originalPath;
//
//}

//---------------------------------------------------------------------------

void VLM_Widget::saveScreenShot()
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

//---------------------------------------------------------------------------

QString VLM_Widget::getTemporaryDatasetPath()
{
   return m_datasetPath + ".tmp";
}

//---------------------------------------------------------------------------

void VLM_Widget::cleanUpTemoraryXMLFiles()
{
   QString temporaryDatasetPath = getTemporaryDatasetPath();

   QFile file(temporaryDatasetPath);

   file.remove();
}

//---------------------------------------------------------------------------

bool VLM_Widget::verifySaveIsRequired()
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


//---------------------------------------------------------------------------

void VLM_Widget::saveTemporaryXMLCoordinateInfo()
{
   QString tempDatasetPath = getTemporaryDatasetPath();

   saveXMLCoordinateInfo(tempDatasetPath);
}

//---------------------------------------------------------------------------

void VLM_Widget::saveXMLCoordinateInfo()
{
   saveXMLCoordinateInfo(m_datasetPath);

   // Auto safe data is no longer needed
   cleanUpTemoraryXMLFiles();
}

//---------------------------------------------------------------------------

void VLM_Widget::saveXMLCoordinateInfo(QString path)
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

//---------------------------------------------------------------------------

void VLM_Widget::writeXMLSaveData(QIODevice* device)
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

//---------------------------------------------------------------------------

void VLM_Widget::tabIndexChanged(int index)
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

//---------------------------------------------------------------------------

void VLM_Widget::treeContextMenu(const QPoint& pos)
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

//---------------------------------------------------------------------------

void VLM_Widget::updatedPixelToLight(double x, double y, double z)
{

   if(false == m_imageViewWidget->getMouseLeaveState())
   {
      m_lightToMicroCoordWidget->setCoordinate(x,y,z);
   }

}

//--------------------------------------------------------------------------

void VLM_Widget::load_live_coord_settings()
{
   QMap<QString, double> vars;
   QStringList names = Preferences::inst()->getValue(STR_PRF_LiveCoefficientNames).toStringList();
   QStringList values = Preferences::inst()->getValue(STR_PRF_LiveCoefficientVals).toStringList();
   if(names.size() == values.size())
   {
      for (int i = 0; i < names.size(); i++)
      {
         vars.insert(names.at(i), values.at(i).toDouble());
      }
   }
   m_lightToMicroCoordModel->getTransformer()->Init(vars);
   
}

//--------------------------------------------------------------------------

void VLM_Widget::useUpdatedSolverVariables(const QMap<QString, double> vars)
{
   
   m_lightToMicroCoordModel->getTransformer()->Init(vars);

   QStringList names;
   QStringList values;
   for (auto name : vars.keys())
   {
      names.append(name);
      values.append(QString::number(vars.value(name)));
   }
   Preferences::inst()->setValue(STR_PRF_LiveCoefficientNames, names);
   Preferences::inst()->setValue(STR_PRF_LiveCoefficientVals, values);

}

/*--------------------------------------------------------------------------*/

void VLM_Widget::updateTreeView()
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

void VLM_Widget::updateContextMenus()
{

   for(RegionCaller *prc : m_actionMap.values())
   {
      delete prc;
   }
   m_actionMap.clear();
   createMicroProbeMenu();

}

/*--------------------------------------------------------------------------*/

void VLM_Widget::windowChanged(Qt::WindowStates oldState,
                               Qt::WindowStates newState)
{

   Q_UNUSED(oldState);

   if(Qt::WindowMaximized || Qt::WindowActive == newState)
   {
      m_imageViewWidget->resizeEvent(nullptr);
   }

}

//--------------------------------------------------------------------------

void VLM_Widget::setEnableChangeBackground(bool val)
{
    if (_btnSetBackground != nullptr)
    {   
        _btnSetBackground->setVisible(val);
    }
}

//--------------------------------------------------------------------------

void VLM_Widget::onUpdateBackgroundImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Background Image", "", "Image Files (*.jpg *.png *.bmp *.tiff *.tif *.h5 *.h50)");
    if (fileName.length() > 0)
    {
        
        QImageReader img_reader(fileName);
        QImage image = img_reader.read();
        if (image.width() > 0 && image.height() > 0)
        {
            updateFrame(&image);
        }
        else
        {
            if (fileName.endsWith(".tiff") || fileName.endsWith(".tif"))
            {
                TIFF_Model tmodel;
           
                if (tmodel.load(fileName))
                {
                    updateFrame(tmodel.getImage());
                }
                else
                {
                    logE << "Failed to load tiff : " << fileName.toStdString() << "\n";
                }
            }
            else if (fileName.endsWith(".h5") || fileName.endsWith(".h50"))
            {
               MapsH5Model h5model;
               H5ImageModel h5image_model;
               if(h5model.load(fileName))
               {
                  ArrayXXr<float> normalized;
                  GenerateImageProp props;
                  props.analysis_type = h5model.getAnalyzedTypes().front();
                  props.element = STR_TOTAL_FLUORESCENCE_YIELD;
                  props.selected_colormap = ColorMap::inst()->get_color_map(Preferences::inst()->getValue(STR_COLORMAP).toString());
    
                  m_imageViewWidget->scene(0)->setPixmap(h5model.gen_pixmap(props, normalized));
                  if(normalized.rows() > 0 && normalized.cols() > 0)
                  {
                     if (m_imageHeightDim != nullptr && m_imageWidthDim != nullptr)
                     {
                           m_imageHeightDim->setCurrentText(QString::number(normalized.rows()));
                           m_imageWidthDim->setCurrentText(QString::number(normalized.cols()));
                     }
                     m_imageViewWidget->resetCoordsToZero();
                     m_imageViewWidget->setCountsTrasnformAt(0, normalized);

                     m_imageViewWidget->clearLabels();
                     std::vector<std::string> element_lines;
                     gen_insert_order_list(element_lines);

                     data_struct::Fit_Count_Dict<float> element_counts;
                     h5model.getAnalyzedCounts(props.analysis_type, element_counts);
                     // insert in z order
                     for (std::string el_name : element_lines)
                     {
                        if(element_counts.count(el_name) > 0)
                        {
                              QString val = QString(el_name.c_str());
                              m_imageViewWidget->addLabel(val);
                        }
                     }
                  }
               }
               else if (h5image_model.load(fileName))
               {
                  m_imageViewWidget->scene(0)->setPixmap(h5image_model.gen_pixmap());
               }
               else
                {
                    logE << "Failed to load hdf5 : " << fileName.toStdString() << "\n";
                }
            }
            else
            {
                logE << "Failed to load image : " << fileName.toStdString() << "\n";
            }
        }
    }
}

//--------------------------------------------------------------------------

void VLM_Widget::onCaptureBackgroundImage()
{
   
}

//--------------------------------------------------------------------------

void VLM_Widget::setAvailScans(std::map<QString, BlueskyPlan> * avail_scans)
{
    _avail_scans = avail_scans;
    _scan_region_link_dialog->setAvailScans(avail_scans);
}

//--------------------------------------------------------------------------

void VLM_Widget::onQueueMicroProbeRegion()
{
   ScanRegionGraphicsItem* item = static_cast<ScanRegionGraphicsItem*>(getSelectedRegion());
   if (item != nullptr)
   {
      const std::vector<QString> propList = _scan_region_link_dialog->property_list();
      BlueskyPlan plan;
      QJsonObject scan_link;
      if(item->isQueued())
      {
         plan = item->getPlan();
      }

      QString scan_name = Preferences::inst()->getValue(STR_PREF_LAST_SCAN_LINK_SELECTED).toString();
	   QJsonArray scan_link_profiles = Preferences::inst()->getValue(STR_PREF_SCAN_LINK_PROFILES).toJsonArray();
      for (const auto &scan_link_ref : scan_link_profiles)
      {
         scan_link  = scan_link_ref.toObject();
         if (scan_link.contains(STR_SCAN_TYPE))
         {
            if(item->isQueued())
            {
               break;
            }
            else
            {
               QString l_scan_name = scan_link[STR_SCAN_TYPE].toString();
               if(scan_name == l_scan_name)
               {
                  if(_avail_scans->count(l_scan_name) > 0)
                  {
                     plan = _avail_scans->at(l_scan_name);
                     plan.type = l_scan_name;
                     plan.name = item->displayName();
                     break;
                  }
                  else
                  {
                     QMessageBox::warning(this, "Error Queuing Scan", "Could not Queue scan region. Please check if you are connected to ZMQ (Press Update).");
                     return;
                  }
               }
            }
         }
      }

      for(const auto& itr: propList)
      {
         if(scan_link.contains(itr))
         {
            QString scan_link_value = scan_link.value(itr).toString();

            for(auto &param : plan.parameters)
            {
               if(param.name == scan_link_value)
               {
                  QString val = item->getValueAsString(itr);
                  if(val.length() > 0)
                  {
                     param.default_val = val;
                  }
                  break;
               }
            }
         }
      }
      
      if(item->isQueued())
      {
         emit onScanUpdated(plan, item); 
      }
      else
      {
         emit onQueueScan(plan, item); 
      }
   }
}

//---------------------------------------------------------------------------

void VLM_Widget::updatePlan(const BlueskyPlan &plan)
{

   QModelIndex first = m_mpTreeModel->index(0,0,QModelIndex());
   gstar::AbstractGraphicsItem* groupPtr = static_cast<gstar::AbstractGraphicsItem*>(first.internalPointer());
   if(groupPtr == nullptr)
   {
      return;
   }
   std::list<gstar::AbstractGraphicsItem*> clist = groupPtr->childList();

   for (gstar::AbstractGraphicsItem* child : clist)
   {
      if (child != nullptr)
      {
         ScanRegionGraphicsItem* item = static_cast<ScanRegionGraphicsItem*>(child);
         if(item != nullptr)
         {
            if(item->getPlan().uuid == (plan.uuid))
            {
               item->setPlan(plan);
               break;
            }
         }
      }
   }
}

//---------------------------------------------------------------------------

void VLM_Widget::removePlan(const BlueskyPlan &plan)
{
   if(plan.uuid.length() == 0)
   {
      return;
   }
   QModelIndex first = m_mpTreeModel->index(0,0,QModelIndex());
   gstar::AbstractGraphicsItem* groupPtr = static_cast<gstar::AbstractGraphicsItem*>(first.internalPointer());
   if(groupPtr == nullptr)
   {
      return;
   }
   std::list<gstar::AbstractGraphicsItem*> clist = groupPtr->childList();

   int idx = 0;
   for (gstar::AbstractGraphicsItem* child : clist)
   {
      if (child != nullptr)
      {
         ScanRegionGraphicsItem* item = static_cast<ScanRegionGraphicsItem*>(child);
         if(item != nullptr)
         {
            BlueskyPlan item_plan = item->getPlan();
            if(item_plan.uuid == plan.uuid)
            {
               // we do this so when we call delete it doesn't send event to double delete
               item_plan.uuid = "";
               item->setPlan(item_plan);
               QModelIndex child_index = m_mpTreeModel->index(idx, 0,first);
               m_mpTreeModel->removeRow(idx, child_index);
               break;
            }
         }
      }
      idx ++;
   }
}

//---------------------------------------------------------------------------

void VLM_Widget::widgetChanged(bool enable)
{
   // More widget change after solver run could be added here
   //m_btnRunSolver -> setEnabled(enable);

}

//---------------------------------------------------------------------------

void VLM_Widget::setPlanFilename(QString uuid, QString filename)
{
   if(uuid.length() == 0)
   {
      return;
   }
   QModelIndex first = m_mpTreeModel->index(0,0,QModelIndex());
   gstar::AbstractGraphicsItem* groupPtr = static_cast<gstar::AbstractGraphicsItem*>(first.internalPointer());
   if(groupPtr == nullptr)
   {
      return;
   }
   std::list<gstar::AbstractGraphicsItem*> clist = groupPtr->childList();
   int idx = 0;
   for (gstar::AbstractGraphicsItem* child : clist)
   {
      if (child != nullptr)
      {
         ScanRegionGraphicsItem* item = static_cast<ScanRegionGraphicsItem*>(child);
         if(item != nullptr)
         {
            BlueskyPlan* item_plan = item->getPlanPtr();
            if(item_plan->uuid == uuid)
            {
               item_plan->filename = filename;
               break;
            }
         }
      }
      idx ++;
   }

}

//---------------------------------------------------------------------------

void VLM_Widget::loadScanRegionLinks(QString dir)
{

   QString loadname = dir + QDir::separator() + STR_AUTO_SAVE_SCAN_REGION_NAME;
   QFile autoFile(loadname);
   if (!autoFile.open(QIODevice::ReadOnly))
   {
       //logW << "Couldn't open save file: " << roi_file_name.toStdString();
       return;
   }
   QJsonObject rootJson = QJsonDocument::fromJson(autoFile.readAll()).object();
   autoFile.close();

   if (rootJson.contains(STR_SCAN_REGIONS) && rootJson[STR_SCAN_REGIONS].isArray())
   {
      QJsonArray regions = rootJson[STR_SCAN_REGIONS].toArray();
      for (int i = 0; i < regions.size(); ++i)
      {
         double real_pos_x = 0.;
         double real_pos_y = 0.;
         BlueskyPlan item_plan;
         QJsonObject json_region_object = regions[i].toObject();
         ScanRegionGraphicsItem* annotation = new ScanRegionGraphicsItem();
         if(json_region_object.contains(UPROBE_NAME))
         {
            annotation->setPropertyValue(UPROBE_NAME, json_region_object.value(UPROBE_NAME).toString());
         }
         if(json_region_object.contains(UPROBE_COLOR))
         {
            annotation->setPropertyValue(UPROBE_COLOR, json_region_object.value(UPROBE_COLOR).toVariant());
         }
         if(json_region_object.contains(UPROBE_REAL_POS_X))
         {
            real_pos_x = json_region_object.value(UPROBE_REAL_POS_X).toDouble();
            annotation->setPropertyValue(UPROBE_REAL_POS_X, real_pos_x);
         }
         if(json_region_object.contains(UPROBE_REAL_POS_Y))
         {
            real_pos_y =  json_region_object.value(UPROBE_REAL_POS_Y).toDouble();
            annotation->setPropertyValue(UPROBE_REAL_POS_Y, real_pos_y);
         }
         if(json_region_object.contains(UPROBE_PRED_POS_X))
         {
            annotation->setPropertyValue(UPROBE_PRED_POS_X, json_region_object.value(UPROBE_PRED_POS_X).toDouble());
         }
         if(json_region_object.contains(UPROBE_PRED_POS_Y))
         {
            annotation->setPropertyValue(UPROBE_PRED_POS_Y, json_region_object.value(UPROBE_PRED_POS_Y).toDouble());
         }
         if(json_region_object.contains(UPROBE_RECT_TLX))
         {
            annotation->setPropertyValue(UPROBE_RECT_TLX, json_region_object.value(UPROBE_RECT_TLX).toDouble());
         }
         if(json_region_object.contains(UPROBE_RECT_TLY))
         {
            annotation->setPropertyValue(UPROBE_RECT_TLY, json_region_object.value(UPROBE_RECT_TLY).toDouble());
         }
         if(json_region_object.contains(UPROBE_RECT_W))
         {
            annotation->setPropertyValue(UPROBE_RECT_W, json_region_object.value(UPROBE_RECT_W).toDouble());
         }
         if(json_region_object.contains(UPROBE_RECT_H))
         {
            annotation->setPropertyValue(UPROBE_RECT_H, json_region_object.value(UPROBE_RECT_H).toDouble());
         }
         if(json_region_object.contains(STR_REGION_PLAN))
         {
            QJsonObject json_region_plan = json_region_object.value(STR_REGION_PLAN).toObject();
            if(json_region_plan.contains(STR_SCAN_REGION_PLAN_NAME))
            {
               item_plan.name = json_region_plan.value(STR_SCAN_REGION_PLAN_NAME).toString();
            }
            if(json_region_plan.contains(STR_SCAN_REGION_PLAN_UUID))
            {
               item_plan.uuid = json_region_plan.value(STR_SCAN_REGION_PLAN_UUID).toString();
            }
            if(json_region_plan.contains(STR_SCAN_REGION_PLAN_TYPE))
            {
               item_plan.type = json_region_plan.value(STR_SCAN_REGION_PLAN_TYPE).toString();
            }
            if(json_region_plan.contains(STR_SCAN_REGION_PLAN_DESC))
            {
               item_plan.description = json_region_plan.value(STR_SCAN_REGION_PLAN_DESC).toString();
            }
            if(json_region_plan.contains(STR_SCAN_REGION_PLAN_FILENAME))
            {
               item_plan.filename = json_region_plan.value(STR_SCAN_REGION_PLAN_FILENAME).toString();
            }
            if(json_region_plan.contains(STR_SCAN_REGION_PLAN_MODULE))
            {
               item_plan.module = json_region_plan.value(STR_SCAN_REGION_PLAN_MODULE).toString();
            }
            if(json_region_plan.contains(STR_SCAN_REGION_PLAN_USER))
            {
               item_plan.user = json_region_plan.value(STR_SCAN_REGION_PLAN_USER).toString();
            }
            if(json_region_plan.contains(STR_SCAN_REGION_PLAN_RESULT))
            {
               QJsonObject json_result = json_region_plan.value(STR_SCAN_REGION_PLAN_RESULT).toObject();
               if(json_result.contains(STR_SCAN_REGION_PLAN_RESULT_EXIT_STATUS))
               {
                  item_plan.result.exit_status = json_result.value(STR_SCAN_REGION_PLAN_RESULT_EXIT_STATUS).toString();
               }
               if(json_result.contains(STR_SCAN_REGION_PLAN_RESULT_RUN_UUIDS))
               {
                  item_plan.result.run_uids = json_result.value(STR_SCAN_REGION_PLAN_RESULT_RUN_UUIDS).toString();
               }
               if(json_result.contains(STR_SCAN_REGION_PLAN_RESULT_TIME_START))
               {
                  item_plan.result.time_start = json_result.value(STR_SCAN_REGION_PLAN_RESULT_TIME_START).toDouble();
               }
               if(json_result.contains(STR_SCAN_REGION_PLAN_RESULT_TIME_STOP))
               {
                  item_plan.result.time_stop = json_result.value(STR_SCAN_REGION_PLAN_RESULT_TIME_STOP).toDouble();
               }
               if(json_result.contains(STR_SCAN_REGION_PLAN_RESULT_MSG))
               {
                  item_plan.result.msg = json_result.value(STR_SCAN_REGION_PLAN_RESULT_MSG).toString();
               }
               if(json_result.contains(STR_SCAN_REGION_PLAN_RESULT_TRACEBACK))
               {
                  item_plan.result.traceback = json_result.value(STR_SCAN_REGION_PLAN_RESULT_TRACEBACK).toString();
               }
            }
            if(json_region_plan.contains(STR_SCAN_REGION_PLAN_PARAMS))
            {
            
               QJsonArray json_region_plan_params = json_region_plan.value(STR_SCAN_REGION_PLAN_PARAMS).toArray();
         
               for (int i = 0; i < json_region_plan_params.size(); ++i)
               {
                  BlueskyParam param; 
                  QJsonObject json_param = json_region_plan_params[i].toObject();
                  if(json_param.contains(STR_SCAN_REGION_PLAN_PARAM_NAME))
                  {
                     param.name = json_param.value(STR_SCAN_REGION_PLAN_PARAM_NAME).toString();
                  }
                  if(json_param.contains(STR_SCAN_REGION_PLAN_PARAM_KIND))
                  {
                     QString strkind = json_param.value(STR_SCAN_REGION_PLAN_PARAM_KIND).toString();
                     if(strkind == "Bool")
                     {
                        param.kind = BlueskyParamType::Bool;
                     }
                     if(strkind == "Double")
                     {
                        param.kind = BlueskyParamType::Double;
                     }
                     if(strkind == "String")
                     {
                        param.kind = BlueskyParamType::String;
                     }
                  }
                  if(json_param.contains(STR_SCAN_REGION_PLAN_PARAM_VAL))
                  {
                     param.default_val = json_param.value(STR_SCAN_REGION_PLAN_PARAM_VAL).toString();
                  }
                  if(json_param.contains(STR_SCAN_REGION_PLAN_PARAM_DESC))
                  {
                     param.description = json_param.value(STR_SCAN_REGION_PLAN_PARAM_DESC).toString();
                  }
                  item_plan.parameters.push_back(param);
               }
            }
            annotation->setPlan(item_plan);
         }

         annotation->setMouseOverPixelCoordModel(m_coordinateModel);
         annotation->setLightToMicroCoordModel(m_lightToMicroCoordModel);
   
         reloadAndSelectAnnotation(m_mpTreeModel,
                                 m_mpAnnoTreeView,
                                 m_mpSelectionModel,
                                 annotation,
                                 QPointF(real_pos_x, real_pos_y));
   
      }
   }
   else
   {
      logW<<"Could not load auto save scan regions file : "<<loadname.toStdString()<<"\n";
   } 
}

//---------------------------------------------------------------------------

void VLM_Widget::saveScanRegionLinks(QString dir)
{
   QString savename = dir + QDir::separator() + STR_AUTO_SAVE_SCAN_REGION_NAME;

   QJsonObject rootJson;

   QJsonArray json_regions;

   QModelIndex first = m_mpTreeModel->index(0,0,QModelIndex());
   gstar::AbstractGraphicsItem* groupPtr = static_cast<gstar::AbstractGraphicsItem*>(first.internalPointer());
   if(groupPtr == nullptr)
   {
      return;
   }
   std::list<gstar::AbstractGraphicsItem*> clist = groupPtr->childList();
   for (gstar::AbstractGraphicsItem* child : clist)
   {
      if (child != nullptr)
      {
         ScanRegionGraphicsItem* item = static_cast<ScanRegionGraphicsItem*>(child);
         if(item != nullptr)
         {
            QJsonObject json_region_object;
            
            json_region_object[UPROBE_COLOR] = child->propertyValue(UPROBE_COLOR).toString();
            json_region_object[UPROBE_REAL_POS_X] = child->x();
            json_region_object[UPROBE_REAL_POS_Y] = child->y();
            json_region_object[UPROBE_PRED_POS_X] = child->propertyValue(UPROBE_PRED_POS_X).toDouble();
            json_region_object[UPROBE_PRED_POS_Y] = child->propertyValue(UPROBE_PRED_POS_Y).toDouble();
            json_region_object[UPROBE_RECT_TLX] = child->boundingRectMarker().left();
            json_region_object[UPROBE_RECT_TLY] = child->boundingRectMarker().top();
            json_region_object[UPROBE_RECT_W] = child->boundingRectMarker().width();
            json_region_object[UPROBE_RECT_H] = child->boundingRectMarker().height();
            json_region_object[UPROBE_NAME] = child->propertyValue(UPROBE_NAME).toString();

            // save region box scan plan
            BlueskyPlan* item_plan = item->getPlanPtr();
            if(item_plan != nullptr)
            {
               QJsonObject json_region_plan;
               QJsonArray json_region_plan_params;
               
               json_region_plan[STR_SCAN_REGION_PLAN_NAME] = item_plan->name;
               json_region_plan[STR_SCAN_REGION_PLAN_UUID] = item_plan->uuid;
               json_region_plan[STR_SCAN_REGION_PLAN_TYPE] = item_plan->type;
               json_region_plan[STR_SCAN_REGION_PLAN_DESC] = item_plan->description;
               json_region_plan[STR_SCAN_REGION_PLAN_FILENAME] = item_plan->filename;
               json_region_plan[STR_SCAN_REGION_PLAN_MODULE] = item_plan->module;
               json_region_plan[STR_SCAN_REGION_PLAN_USER] = item_plan->user;
               
               QJsonObject json_result;
               json_result[STR_SCAN_REGION_PLAN_RESULT_EXIT_STATUS] = item_plan->result.exit_status;
               json_result[STR_SCAN_REGION_PLAN_RESULT_RUN_UUIDS] = item_plan->result.run_uids;
               json_result[STR_SCAN_REGION_PLAN_RESULT_TIME_START] = item_plan->result.time_start;
               json_result[STR_SCAN_REGION_PLAN_RESULT_TIME_STOP] = item_plan->result.time_stop;
               json_result[STR_SCAN_REGION_PLAN_RESULT_MSG] = item_plan->result.msg;
               json_result[STR_SCAN_REGION_PLAN_RESULT_TRACEBACK] = item_plan->result.traceback;
               json_region_plan[STR_SCAN_REGION_PLAN_RESULT] = json_result;

               for(auto itr : item_plan->parameters)
               {
                  QJsonObject json_param;
                  json_param[STR_SCAN_REGION_PLAN_PARAM_NAME] = itr.name;
                  if(itr.kind == BlueskyParamType::Bool)
                  {
                     json_param[STR_SCAN_REGION_PLAN_PARAM_KIND] = "Bool";
                  }
                  else if(itr.kind == BlueskyParamType::Double)
                  {
                     json_param[STR_SCAN_REGION_PLAN_PARAM_KIND] = "Double";
                  }
                  else if(itr.kind == BlueskyParamType::String)
                  {
                     json_param[STR_SCAN_REGION_PLAN_PARAM_KIND] = "String";
                  }
                  json_param[STR_SCAN_REGION_PLAN_PARAM_VAL] = itr.default_val;
                  json_param[STR_SCAN_REGION_PLAN_PARAM_DESC] = itr.description;
                  json_region_plan_params.append(json_param);
               }

               json_region_plan[STR_SCAN_REGION_PLAN_PARAMS] = json_region_plan_params;
               json_region_object[STR_REGION_PLAN] = json_region_plan;
               json_regions.append(json_region_object);
            }
         }
      }
   }

   rootJson[STR_SCAN_REGIONS] = json_regions;
   QByteArray save_data = QJsonDocument(rootJson).toJson();

   
   QFile saveFile(savename);
   if (!saveFile.open(QIODevice::WriteOnly))
   {
      logW << "Couldn't open save file: " << savename.toStdString();
   }
   else
   {
      saveFile.write(save_data);
      saveFile.close();
   }
}

//---------------------------------------------------------------------------

/*
void VLM_Widget::solverVariableUpdate()
{

}

void uProbeX::solverVariableUpdate(double valX, double valY)
{

    int id = Preferences::inst()->getValue(STR_PRF_SolverCheckedID).toInt();

    if (id == 0)
    {
        QStringList coefList = Preferences::inst()->getValue(STR_PRF_NMCoefficient).toStringList();
        QStringList newAttrs;

        saveXYToCoefficient(valX, valY, coefList, newAttrs);

        Preferences::inst()->setValue(STR_PRF_NMCoefficient,
            newAttrs);
    }
    else
    {
        QStringList coefList = Preferences::inst()->getValue(STR_PRF_PythonCoefficient).toStringList();
        QStringList newAttrs;

        saveXYToCoefficient(valX, valY, coefList, newAttrs);

        Preferences::inst()->setValue(STR_PRF_PythonCoefficient,
            newAttrs);
    }
    solverEnd();

}


void uProbeX::saveXYToCoefficient(double& valX,
    double& valY,
    QStringList& coefList,
    QStringList& newAttrs)
{

    for (int i = 0; i < coefList.size(); i++)
    {
        QString attr = coefList.at(i);
        QStringList l = attr.split(",");
        if (l.size() != 4)  continue;

        if (l.at(0) == "m2xfm_x")
        {
            newAttrs.append(QString("%1,%2,%3,%4").arg(l.at(0))
                .arg(QString::number(valX))
                .arg(l.at(2))
                .arg(l.at(3)));
        }
        else if (l.at(0) == "m2xfm_y")
        {
            newAttrs.append(QString("%1,%2,%3,%4").arg(l.at(0))
                .arg(QString::number(valY))
                .arg(l.at(2))
                .arg(l.at(3)));
        }
        else
        {
            newAttrs.append(attr);
        }
    }

}
*/