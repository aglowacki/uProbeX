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

   QTextEdit *_textEdit;

   QProgressBar *_progressBar;

   NetStreamWorker* _streamWorker;

   QLineEdit *_qline_ip_addr;

   QPushButton *_btn_update;
};


/*---------------------------------------------------------------------------*/

#endif /* LiveLiveMapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/
