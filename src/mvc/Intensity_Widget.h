/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef INTENSITY_WIDGET_H
#define INTENSITY_WIDGET_H

/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/

class Intensity_Widget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   Intensity_Widget(QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~Intensity_Widget();


protected:

   void createLayout();

};


/*---------------------------------------------------------------------------*/

#endif /* MapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/