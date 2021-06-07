﻿/*-----------------------------------------------------------------------------
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
   LiveMapsElementsWidget(QString ip="127.0.0.1", QString port="43434", QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~LiveMapsElementsWidget();

   QString getIpAddress(){return _qline_ip_addr->text();}

   QString getPort(){return _qline_port->text();}

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

   std::vector<std::shared_ptr<MapsH5Model> > _maps_h5_models;

   std::shared_ptr<MapsH5Model> _currentModel;

   data_struct::Stream_Block *_last_packet;

   std::string _prev_dataset_name;

   int _num_images;
};


/*---------------------------------------------------------------------------*/

#endif /* LiveLiveMapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/
