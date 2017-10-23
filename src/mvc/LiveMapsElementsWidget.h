/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef LIVE_MAPS_ELEMENTS_WIDGET_H
#define LIVE_MAPS_ELEMENTS_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QThread>
#include <QTextEdit>
#include <QProgressBar>
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

public slots:
   void newDataArrived(data_struct::xrf::Stream_Block *new_packet);

protected:

   /**
    * @brief Create layout
    */
   void createLayout();

   QTextEdit *_textEdit;

   QProgressBar *_progressBar;

   NetStreamWorker* _streamWorker;

};


/*---------------------------------------------------------------------------*/

#endif /* LiveLiveMapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/
