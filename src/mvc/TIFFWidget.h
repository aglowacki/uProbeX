/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef TXM_TIFF_WIDGET_H
#define TXM_TIFF_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <gstar/AbstractImageWidget.h>

class HDF5PropertyWidget;
class QAbstractTableModel;

/*---------------------------------------------------------------------------*/

/**
 * @brief When open the acquisition window, the widget is showing for capturing
 * the image from the area detector writer, the window will also be updated to
 * show the image.
 */
class TIFFWidget
: public gstar::AbstractImageWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   TIFFWidget(QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~TIFFWidget();

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

};


/*---------------------------------------------------------------------------*/

#endif /* HDF5Widget_H_ */

/*---------------------------------------------------------------------------*/
