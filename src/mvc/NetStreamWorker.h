/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef NET_STREAM_WORKER_H
#define NET_STREAM_WORKER_H

/*---------------------------------------------------------------------------*/

#include <QThread>
#include "io/net/zmq_subscriber.h"

/*---------------------------------------------------------------------------*/


class NetStreamWorker : public QThread
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
    void run() override
    {
        _running = true;
        data_struct::xrf::Stream_Block *new_packet;
        while(_running)
        {
            _subscriber->get_counts(new_packet);
            emit newData(new_packet);
        }
    }
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
