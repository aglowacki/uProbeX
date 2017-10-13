/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef LIVE_MAPS_ELEMENTS_WIDGET_H
#define LIVE_MAPS_ELEMENTS_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QThread>
#include "NetStreamWorker.h"

/*---------------------------------------------------------------------------*/


class LiveMapsElementsWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   LiveMapsElementsWidget(QWidget* parent = NULL);

   /**
    * Destructor.
    */
   ~LiveMapsElementsWidget();

signals:
   void start_listening();

public slots:
   void newDataArrived(data_struct::xrf::Stream_Block *new_packet);

protected:

   /**
    * @brief Create layout
    */
   void createLayout();

   QThread _thread;

   NetStreamWorker* _streamWorker;

};


/*---------------------------------------------------------------------------*/

#endif /* LiveLiveMapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/
