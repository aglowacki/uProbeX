/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <core/TIFFController.h>
#include <core/TIFFModel.h>
#include <core/TIFFWidget.h>
#include <core/AbstractWindowModel.h>

#include <preferences/Preferences.h>
//#include <preferences/PVAttributesGroup.h>

#include <proc/ColorSwapProcessStage.h>
#include <proc/TIFFSourceStage.h>
#include <proc/DisplayStage.h>
#include <proc/Histogram.h>


#include <Array.h>
#include <QDateTime>

using gstar::ImageViewWidget;
using gstar::Array;

/*---------------------------------------------------------------------------*/

TIFFController::TIFFController(gstar::AbstractImageWidget* tiffWidget,
                               AbstractWindowModel* tiffModel,
                               Preferences* preferences)
{

   m_tiffWidget = tiffWidget;
   m_tiffModel = tiffModel;
   m_preferences = preferences;

   m_tiffSourceStage = NULL;
   m_processStage = NULL;
   m_displayStage = NULL;
   m_histogram = NULL;

   openPipeline();

}

/*---------------------------------------------------------------------------*/

TIFFController::~TIFFController()
{

   closePipeline();

   if (m_tiffModel != NULL) delete m_tiffModel;
   // the widget is deleted by the qt.

}

/*---------------------------------------------------------------------------*/

void TIFFController::closePipeline()
{

   // Only proceede to close file, if hdf5 source stage is not null.
   if (m_tiffSourceStage == NULL) return;

   m_tiffSourceStage->stop();

     delete m_tiffSourceStage;

     m_tiffSourceStage = NULL;

   disconnect(m_tiffWidget,
              SIGNAL(rangeChanged(int, int)),
              this,
              SLOT(imageIndexChanged(int, int)));

   disconnect(m_tiffWidget,
              SIGNAL(imageWidthDimChanged(int)),
              this,
              SLOT(imageWidthDimUpdated(int)));

   disconnect(m_tiffWidget,
              SIGNAL(imageHeightDimChanged(int)),
              this,
              SLOT(imageHeightDimUpdated(int)));


   if (m_displayStage != NULL)
   {
      disconnect(m_displayStage,
                 SIGNAL(newFrame(const gstar::Array*)),
                 this,
                 SLOT(newFrameAvailable(const gstar::Array*)));

      delete m_displayStage;
      m_displayStage = NULL;
   }

   if (m_histogram != NULL)
   {
      delete m_histogram;
      m_histogram = NULL;
   }

}

/*---------------------------------------------------------------------------*/

AbstractWindowModel* TIFFController::getModel()
{

   return m_tiffModel;

}


gstar::AbstractImageWidget* TIFFController::getWidget()
{

   return m_tiffWidget;

}

/*---------------------------------------------------------------------------*/

void TIFFController::imageHeightDimUpdated(int h)
{

   if (m_tiffSourceStage == NULL) return;

   m_tiffSourceStage->setHeightDimension(h);
   m_tiffWidget->setWidthDims(m_tiffSourceStage->getWidthDimension());
   m_tiffWidget->setNumberOfImages(m_tiffModel->getNumberOfImages());

}

/*---------------------------------------------------------------------------*/

void TIFFController::imageIndexChanged(int index, int id)
{

   Q_UNUSED(id);

   if (m_tiffSourceStage == NULL) return;

   m_tiffSourceStage->setImageIndex(index);

}

/*---------------------------------------------------------------------------*/

void TIFFController::imageWidthDimUpdated(int w)
{

   if (m_tiffSourceStage == NULL) return;

   m_tiffSourceStage->setWidthDimension(w);
   m_tiffWidget->setHeightDims(m_tiffSourceStage->getHeightDimension());
   m_tiffWidget->setNumberOfImages(m_tiffModel->getNumberOfImages());

}

/*---------------------------------------------------------------------------*/

void TIFFController::newFrameAvailable(const gstar::Array* image)
{

   m_tiffWidget->updateFrame(image);
   if (m_histogram != NULL)
   {
      emitHistogram(m_histogram->getCounts());
      m_tiffModel->setHistogram(m_histogram->getCounts());
   }

   QApplication::restoreOverrideCursor();

}

/*---------------------------------------------------------------------------*/

void TIFFController::openPipeline()
{

   QApplication::setOverrideCursor(Qt::WaitCursor);

   try
   {
      m_tiffSourceStage = new TIFFSourceStage(m_tiffModel);

   }
   catch (std::string& s)
   {
      throw;
   }

   // Initialize the histogram object
   //double bins =
   //      (m_preferences->readValueKey(Preferences::HISTBins)).toDouble();
   int arrayType = m_tiffModel->getDataType();

   switch (arrayType)
   {
   case dstar::Char:
      m_histogram = new Histogram(255 , 255);
      break;
   case dstar::UnsignedChar:
      m_histogram = new Histogram(255 , 255);
      break;
   case dstar::Short:
      m_histogram = new Histogram(255, 65535);
      break;
   case dstar::UnsignedShort:
      m_histogram = new Histogram(255, 65535);
      break;
   case dstar::Short12:
      m_histogram = new Histogram(255, 4095);
      break;
   }

   m_tiffWidget->setRank(m_tiffModel->getRank());
   m_tiffWidget->setNumberOfImages(m_tiffModel->getNumberOfImages());
   m_tiffWidget->setRangeWidgetStartIndex(m_tiffModel->getNumberOfImages());
   m_tiffWidget->setHeightDims(m_tiffSourceStage->getHeightDimension());
   m_tiffWidget->setWidthDims(m_tiffSourceStage->getWidthDimension());

   connect(m_tiffWidget,
           SIGNAL(rangeChanged(int, int)),
           this,
           SLOT(imageIndexChanged(int, int)));

   connect(m_tiffWidget,
           SIGNAL(imageWidthDimChanged(int)),
           this,
           SLOT(imageWidthDimUpdated(int)));

   connect(m_tiffWidget,
           SIGNAL(imageHeightDimChanged(int)),
           this,
           SLOT(imageHeightDimUpdated(int)));

   // Create the color swap process stage
   m_processStage = new ColorSwapProcessStage();
   m_processStage->setRGBSequence(RED, GREEN, BLUE);

   // Create final stage for display
   m_displayStage = new DisplayStage();

   // Connect display stage to slot
   connect(m_displayStage,
           SIGNAL(newFrame(const gstar::Array*)),
           this,
           SLOT(newFrameAvailable(const gstar::Array*)));

   m_tiffSourceStage->setNextStage(m_processStage);
   m_processStage->setNextStage(m_displayStage);

   // Start area detector thread.
   m_tiffSourceStage->start();

}

/*---------------------------------------------------------------------------*/

void TIFFController::updateCTF(gstar::ColorTransferFunction ctf)
{

   Q_UNUSED(ctf);

   if (m_processStage == NULL) return;

   // Re-load the current image
   m_tiffSourceStage->setImageIndex(m_tiffSourceStage->getCurrentImageIndex());

}

/*---------------------------------------------------------------------------*/

