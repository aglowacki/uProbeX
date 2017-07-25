/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <core/TIFFController.h>
#include <core/TIFFWidget.h>
#include <core/AbstractWindowModel.h>

#include <preferences/Preferences.h>
//#include <preferences/PVAttributesGroup.h>

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

   m_tiffWidget->updateFrame(m_tiffModel->getImage());
   //openPipeline();

}

/*---------------------------------------------------------------------------*/

TIFFController::~TIFFController()
{

   //closePipeline();

   if (m_tiffModel != NULL) delete m_tiffModel;
   // the widget is deleted by the qt.

}

/*---------------------------------------------------------------------------*/
/*
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
*/
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

   //if (m_tiffSourceStage == NULL) return;

   //m_tiffSourceStage->setHeightDimension(h);
   //m_tiffWidget->setWidthDims(m_tiffSourceStage->getWidthDimension());
   m_tiffWidget->setNumberOfImages(m_tiffModel->getNumberOfImages());

}

/*---------------------------------------------------------------------------*/

void TIFFController::imageWidthDimUpdated(int w)
{

   //if (m_tiffSourceStage == NULL) return;

   //m_tiffSourceStage->setWidthDimension(w);
   //m_tiffWidget->setHeightDims(m_tiffSourceStage->getHeightDimension());
   m_tiffWidget->setNumberOfImages(m_tiffModel->getNumberOfImages());

}

/*---------------------------------------------------------------------------*/

void TIFFController::newFrameAvailable(const gstar::Array* image)
{

   m_tiffWidget->updateFrame(image);

   QApplication::restoreOverrideCursor();

}

/*---------------------------------------------------------------------------*/
/*
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
   int pixelByteSize = m_tiffModel->getPixelByteSize();

   switch (pixelByteSize)
   {
   case 1:
      m_histogram = new Histogram(255 , 255);
      break;
   case 2:
      m_histogram = new Histogram(255, 65535);
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


   m_tiffSourceStage->setNextStage(m_displayStage);
   //m_tiffSourceStage->setNextStage(m_processStage);
   //m_processStage->setNextStage(m_displayStage);

   // Start area detector thread.
   m_tiffSourceStage->start();

}
*/
/*---------------------------------------------------------------------------*/

