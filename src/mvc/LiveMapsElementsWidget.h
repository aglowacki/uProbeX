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
   LiveMapsElementsWidget(QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~LiveMapsElementsWidget();

public slots:

   void newDataArrived(data_struct::Stream_Block *new_packet);

   void updateIp();

   void image_changed(int start, int end);

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

   QLineEdit *_qline_port;

   QPushButton *_btn_update;

   std::vector<MapsH5Model*> _maps_h5_models;

   MapsH5Model *_currentModel;

   data_struct::Stream_Block *_last_packet;

   int _num_images;
};


/*---------------------------------------------------------------------------*/

#endif /* LiveLiveMapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/
