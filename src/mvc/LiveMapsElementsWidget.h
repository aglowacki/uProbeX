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
#include <QPushButton>
#include <QLineEdit>
#include "NetStreamWorker.h"
#include "mvc/MapsElementsWidget.h"

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

   void updateIp();

protected:

   /**
    * @brief Create layout
    */
   void createLayout();

   MapsElementsWidget *_mapsElementsWidget;

   QTextEdit *_textEdit;

   QProgressBar *_progressBar;

   NetStreamWorker* _streamWorker;

   QLineEdit *_qline_ip_addr;

   QPushButton *_btn_update;

   MapsH5Model _currentModel;
};


/*---------------------------------------------------------------------------*/

#endif /* LiveLiveMapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/
