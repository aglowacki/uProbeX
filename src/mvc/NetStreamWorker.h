/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef NET_STREAM_WORKER_H
#define NET_STREAM_WORKER_H

/*---------------------------------------------------------------------------*/

#include <QThread>
#include "support/zmq/zmq.hpp"
#include "io/net/basic_serializer.h"

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
        zmq::message_t token, message;
        while(_running)
        {
            _zmq_socket->recv(&token);
            _zmq_socket->recv(&message);
            new_packet = _serializer.decode_counts((char*)message.data(), message.size());
            emit newData(new_packet);
        }
    }
    void stop() {_running = false;}

signals:
    void newData(data_struct::xrf::Stream_Block *new_packet);

protected:

    bool _running;

    zmq::context_t *_context;

    zmq::socket_t *_zmq_socket;

    io::net::Basic_Serializer _serializer;

};


/*---------------------------------------------------------------------------*/

#endif /* NET_STREAM_WORKER_H */

/*---------------------------------------------------------------------------*/
