/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MDA_WIDGET_H
#define MDA_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <gstar/AbstractImageWidget.h>

#include <mvc/MDA_Model.h>

/*---------------------------------------------------------------------------*/

/**
 * @brief When open the acquisition window, the widget is showing for capturing
 * the image from the area detector writer, the window will also be updated to
 * show the image.
 */
class MDA_Widget
: public gstar::AbstractImageWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   MDA_Widget(QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~MDA_Widget();

   //void setModel(MapsH5Model* model);
   void setModel(MDA_Model* model);

   MDA_Model *getModel(){return _model;}

   void redrawCounts();

public slots:

   /**
    * @brief Accept the window change state
    * @param oldState
    * @param newState
    */
   void windowChanged(Qt::WindowStates oldState, Qt::WindowStates newState);

   void model_updated();

protected:

   /**
    * @brief Create layout
    */
   void createLayout();

   MDA_Model *_model;

};


/*---------------------------------------------------------------------------*/

#endif /* MapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/
