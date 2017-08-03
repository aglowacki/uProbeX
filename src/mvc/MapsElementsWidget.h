/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_ELEMENTS_WIDGET_H
#define MAPS_ELEMENTS_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <AbstractImageWidget.h>

#include <mvc/MapsH5Model.h>

class HDF5PropertyWidget;
class QAbstractTableModel;

/*---------------------------------------------------------------------------*/

/**
 * @brief When open the acquisition window, the widget is showing for capturing
 * the image from the area detector writer, the window will also be updated to
 * show the image.
 */
class MapsElementsWidget
: public gstar::AbstractImageWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   MapsElementsWidget(QWidget* parent = NULL);

   /**
    * Destructor.
    */
   ~MapsElementsWidget();

   void setModel(MapsH5Model* model);

public slots:

   /**
    * @brief Accept the window change state
    * @param oldState
    * @param newState
    */
   void windowChanged(Qt::WindowStates oldState, Qt::WindowStates newState);

protected:

   /**
    * @brief Create layout
    */
   void createLayout();

   MapsH5Model *_model;

};


/*---------------------------------------------------------------------------*/

#endif /* MapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/
