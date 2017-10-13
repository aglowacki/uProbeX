/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef NET_STREAM_WORKER_H
#define NET_STREAM_WORKER_H

/*---------------------------------------------------------------------------*/

#include <QObject>
#include "io/net/zmq_subscriber.h"

/*---------------------------------------------------------------------------*/


class NetStreamWorker : public QObject
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   NetStreamWorker(QObject* parent = NULL);

   /**
    * Destructor.
    */
   ~NetStreamWorker();

public slots:
    void doWork();

    void stop() {_running = false;}

signals:
    void newData(data_struct::xrf::Stream_Block *new_packet);

protected:

    bool _running;

    io::net::Zmq_Subscriber* _subscriber;

};


/*---------------------------------------------------------------------------*/

#endif /* NET_STREAM_WORKER_H */

/*---------------------------------------------------------------------------*/
