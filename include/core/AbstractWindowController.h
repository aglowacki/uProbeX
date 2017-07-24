/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef TXM_ABSTRACT_WINDOW_CONTROLLER_H
#define TXM_ABSTRACT_WINDOW_CONTROLLER_H

/*---------------------------------------------------------------------------*/

#include <ColorTransferFunction.h>
#include <preferences/Preferences.h>

#include <QObject>

class AbstractWindowModel;
class ProcessStage;
class DisplayStage;
class Histogram;

namespace gstar
{
   class Array;
   class ImageViewWidget;
   class AbstractImageWidget;
}


/*---------------------------------------------------------------------------*/

/**
 * @brief Abstract window controller class.
 */
class AbstractWindowController
: public QObject
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   AbstractWindowController();

   /**
    * Destructor.
    */
   ~AbstractWindowController();

   /**
    * Get model.
    */
   virtual AbstractWindowModel* getModel() = 0;

   /**
    * @brief getWidget
    * @return
    */
   virtual gstar::AbstractImageWidget* getWidget() = 0;

   /**
    * @brief getHistogram
    * @return
    */
   Histogram* getHistogram();

public slots:

   /**
    * Update CTF.
    */
   virtual void updateCTF(gstar::ColorTransferFunction) = 0;

signals:

   /**
    * Emit histogram change.
    */
   void emitHistogram(QList<int> hist);

protected:

   /**
    * Image view widget
    */
   gstar::ImageViewWidget* m_widget;

   /**
    * Preferences
    */
   Preferences* m_preferences;

   /**
    * For configuring histogram plot;
    */
   Histogram* m_histogram;

   /**
    * Display stage
    */
   DisplayStage* m_displayStage;

   /**
    * Process stage either swap or changing to RGB
    */
   ProcessStage* m_processStage;

};

/*---------------------------------------------------------------------------*/

#endif /* TXM_ABSTRACT_WINDOW_CONTROLLER_H */

/*---------------------------------------------------------------------------*/
