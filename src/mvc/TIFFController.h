/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef TXM_TIFF_CONTROLLER_H
#define TXM_TIFF_CONTROLLER_H

/*---------------------------------------------------------------------------*/

#include <mvc/AbstractWindowController.h>
#include <preferences/Preferences.h>


#include <QObject>
#include <QApplication>
#include <QMessageBox>

class AbstractWindowModel;
class TIFFWidget;

namespace gstar
{
   class AbstractImageWidget;
}
/*---------------------------------------------------------------------------*/

/**
 * @brief Controller to control the Tiff image pipeline with TiffWidget and
 * TiffModel.It will send out signals to connect these two classes for the
 * controll of how Tiff image is captured ,recorded, showed on the window.
 */
class TIFFController
: public AbstractWindowController
{

   Q_OBJECT

public:

   enum {
      BLUE,
      GREEN,
      RED
   };

   /**
    * @brief TIFFController
    * @param tiffWidget
    * @param tiffModel
    * @param preferences
    */
   TIFFController(gstar::AbstractImageWidget* tiffWidget,
                  AbstractWindowModel*  tiffModel,
                  Preferences* preferences);

   /**
    * TIFFController destructor
    */
   ~TIFFController();

   /**
    * @brief getModel return ths model
    * @return
    */
   AbstractWindowModel* getModel();

   /**
    * @brief getWidget
    * @return
    */
   gstar::AbstractImageWidget* getWidget();

public slots:

   /**
    * @brief Update the height of dim
    */
   void imageHeightDimUpdated(int h);

   /**
    * @brief Update the width of dim
    */
   void imageWidthDimUpdated(int w);

private:

   //TODO Making open and close pipeline method as private:
   //  Currently the only way to open a new TIFF file in the pipeline
   //  is through the TIFFModel and at this time it doesn't allow
   //   to open a file after construction of its instance. 
   
   /**
    * @brief Close TIFF pipeline
    */
   //void closePipeline();

   /**
    * @brief Create TIFF pipeline
    */
   //void openPipeline();


private slots:

   /**
    * @brief Update the current frame in the image widget.
    */
   void newFrameAvailable(const gstar::Array* image);

private:

   /**
    * @brief Create HDF5 widget;
    */
   gstar::AbstractImageWidget* m_tiffWidget;

   /**
    * @brief Create HDF5 model;
    */
   AbstractWindowModel* m_tiffModel;

};

/*---------------------------------------------------------------------------*/

#endif /* TXM_TIFF_CONTROLLER_H */

/*---------------------------------------------------------------------------*/

