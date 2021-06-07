/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef IntensityWidget_H
#define IntensityWidget_H

#include <QWidget>

/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/

class IntensityWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   IntensityWidget(QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~IntensityWidget();


protected:

   void createLayout();

};


/*---------------------------------------------------------------------------*/

#endif /* MapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/