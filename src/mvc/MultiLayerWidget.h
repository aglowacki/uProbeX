/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MultiLayerWidget_H
#define MultiLayerWidget_H

/*---------------------------------------------------------------------------*/

#include <QComboBox>
#include <gstar/AbstractImageWidget.h>

/*---------------------------------------------------------------------------*/

class MultiLayerWidget : public gstar::AbstractImageWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   MultiLayerWidget(QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~MultiLayerWidget();


protected:

   void createLayout();

};


/*---------------------------------------------------------------------------*/

#endif /* MapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/