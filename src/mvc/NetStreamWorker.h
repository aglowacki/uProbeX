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
#include <string>
/*---------------------------------------------------------------------------*/


class NetStreamWorker : public QThread
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   NetStreamWorker(QString str_ip, QObject* parent = nullptr) : QThread(parent)
   {

       std::string conn_str = "tcp://"+str_ip.toStdString()+":43434";
       //std::string conn_str = "tcp://192.168.0.254:43434";
       _context = new zmq::context_t(1);
       _zmq_socket = new zmq::socket_t(*_context, ZMQ_SUB);
       _zmq_socket->connect(conn_str);
       _zmq_socket->setsockopt(ZMQ_SUBSCRIBE, "XRF-Counts", 10);
       _zmq_socket->setsockopt(ZMQ_RCVTIMEO, 1000); //set timeout to 1000ms

   }

   /**
    * Destructor.
    */
   ~NetStreamWorker()
   {
       if(_zmq_socket != nullptr)
       {
           _zmq_socket->close();
           delete _zmq_socket;
       }
       if (_context != nullptr)
       {
           _context->close();
           delete _context;
       }
       _zmq_socket = nullptr;
       _context = nullptr;
   }

public slots:
    void run() override
    {
        _running = true;
        data_struct::Stream_Block *new_packet;
        zmq::message_t token, message;
        while(_running)
        {
            _zmq_socket->recv(&token);
            std::string s1 ((char*)token.data(), token.size());
            if(s1 == "XRF-Counts")
            {
                if(_zmq_socket->recv(&message))
                {
                    new_packet = _serializer.decode_counts((char*)message.data(), message.size());
                    emit newData(new_packet);
                }
            }
        }
        _zmq_socket->close();
    }
    void stop() {_running = false;}

signals:
    void newData(data_struct::Stream_Block *new_packet);

protected:

    bool _running;

    zmq::context_t *_context;

    zmq::socket_t *_zmq_socket;

    io::net::Basic_Serializer _serializer;

};


/*---------------------------------------------------------------------------*/

#endif /* NET_STREAM_WORKER_H */

/*---------------------------------------------------------------------------*/
