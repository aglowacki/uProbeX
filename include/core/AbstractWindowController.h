/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef TXM_ABSTRACT_WINDOW_CONTROLLER_H
#define TXM_ABSTRACT_WINDOW_CONTROLLER_H

/*---------------------------------------------------------------------------*/

#include <preferences/Preferences.h>

#include <QObject>

class AbstractWindowModel;

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

public slots:

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

};

/*---------------------------------------------------------------------------*/

#endif /* TXM_ABSTRACT_WINDOW_CONTROLLER_H */

/*---------------------------------------------------------------------------*/
