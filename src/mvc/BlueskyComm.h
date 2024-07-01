/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef BLUESKY_COMM_H
#define BLUESKY_COMM_H

//---------------------------------------------------------------------------

#include <QThread>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "support/zmq/zmq.hpp"
#include <string>
//---------------------------------------------------------------------------


class BlueskyComm : public QThread
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   BlueskyComm(QString str_ip, QObject* parent = nullptr) : QThread(parent)
   {

       std::string conn_str = "tcp://"+str_ip.toStdString()+":60610"; // or 60615
       std::string lsn_str = "tcp://"+str_ip.toStdString()+":60625";
       _context = new zmq::context_t(1);
       _zmq_comm_socket = new zmq::socket_t(*_context, ZMQ_REQ);
       _zmq_comm_socket->connect(conn_str);
       _zmq_lsn_socket = new zmq::socket_t(*_context, ZMQ_SUB);
       _zmq_lsn_socket->connect(lsn_str);
       _zmq_lsn_socket->setsockopt(ZMQ_SUBSCRIBE, "QS_Console", 10);
       _zmq_lsn_socket->setsockopt(ZMQ_RCVTIMEO, 1000); //set timeout to 1000ms

   }

   /**
    * Destructor.
    */
   ~BlueskyComm()
   {
       if(_zmq_comm_socket != nullptr)
       {
           _zmq_comm_socket->close();
           delete _zmq_comm_socket;
       }
       if(_zmq_lsn_socket != nullptr)
       {
           _zmq_lsn_socket->close();
           delete _zmq_lsn_socket;
       }
       if (_context != nullptr)
       {
           _context->close();
           delete _context;
       }
       _zmq_comm_socket = nullptr;
       _zmq_lsn_socket = nullptr;
       _context = nullptr;
   }

public slots:
    void run() override
    {
        _running = true;
        zmq::message_t token, message;
        while(_running)
        {
            _zmq_lsn_socket->recv(&token);
            std::string s1 ((char*)token.data(), token.size());
            if(s1 == "QS_Console")
            {
                if(_zmq_lsn_socket->recv(&message))
                {
                    emit newData(QString::fromUtf8((char*)message.data(), message.size()));
                }
            }
        }
        _zmq_lsn_socket->close();
    }
    void stop() {_running = false;}

signals:
    void newData(const QString&);

protected:

    bool _running;

    zmq::context_t *_context;

    zmq::socket_t *_zmq_comm_socket;

    zmq::socket_t *_zmq_lsn_socket;

};


//---------------------------------------------------------------------------

#endif /* NET_STREAM_WORKER_H */

//---------------------------------------------------------------------------
