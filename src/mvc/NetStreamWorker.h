/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef NET_STREAM_WORKER_H
#define NET_STREAM_WORKER_H

//---------------------------------------------------------------------------

#include <QThread>
#include <zmq.hpp>
#include "io/net/basic_serializer.h"
#include <string>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>

//---------------------------------------------------------------------------


class NetStreamWorker : public QThread
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   NetStreamWorker(zmq::context_t *context, QString str_ip, QString port, QString qserv_addr, QObject* parent = nullptr) : QThread(parent)
   {
        _zmq_qserv_socket = nullptr;

       std::string conn_str = "tcp://"+str_ip.toStdString()+":"+port.toStdString();
       _zmq_socket = new zmq::socket_t(*context, ZMQ_SUB);
       _zmq_socket->connect(conn_str);
       _zmq_socket->set(zmq::sockopt::subscribe, "XRF-Counts");
       _zmq_socket->set(zmq::sockopt::subscribe, "XRF-Spectra");
       _zmq_socket->set(zmq::sockopt::subscribe, "XRF-Counts-and-Spectra");
       _zmq_socket->set(zmq::sockopt::rcvtimeo, 1000); //set timeout to 1000ms
        if(qserv_addr.length() > 0)
        {
            std::string conn_str2 = "tcp://"+qserv_addr.toStdString()+":60625";
            _zmq_qserv_socket = new zmq::socket_t(*context, ZMQ_SUB);
            _zmq_qserv_socket->connect(conn_str2);
            _zmq_qserv_socket->set(zmq::sockopt::subscribe, "QS_Console");
            _zmq_qserv_socket->set(zmq::sockopt::rcvtimeo, 1000); //set timeout to 1000ms
        }
        else
        {
            logW<<"QServer address is not set. Can not connect to QServer\n";
        }

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
       _zmq_socket = nullptr;

       if(_zmq_qserv_socket != nullptr)
       {
           _zmq_qserv_socket->close();
           delete _zmq_qserv_socket;
       }
       _zmq_qserv_socket = nullptr;
   }

public slots:
    void run() override
    {
        _running = true;
        data_struct::Stream_Block<float> *new_packet;
        zmq::message_t token, message;
        zmq::recv_result_t res1, res2;
        unsigned int sleepAtVal = 1;
        if(_zmq_qserv_socket != nullptr)
        {
            sleepAtVal++;
        }

        while(_running)
        {
            unsigned int toSleepVal = 0;
            res1 = _zmq_socket->recv(token, zmq::recv_flags::dontwait);
            if(res1.has_value() && (EAGAIN != res1.value()))
            {
                std::string s1 ((char*)token.data(), token.size());
                if(s1 == "XRF-Counts-and-Spectra")
                {
                    zmq::recv_result_t res1_2 = _zmq_socket->recv(message);
                    if(res1_2.has_value())
                    {
                        new_packet = _serializer.decode_counts_and_spectra((char*)message.data(), message.size());
                        emit newData(new_packet);
                    }
                }
                else if(s1 == "XRF-Counts")
                {
                    zmq::recv_result_t res1_2 = _zmq_socket->recv(message);
                    if(res1_2.has_value())
                    {
                        new_packet = _serializer.decode_counts((char*)message.data(), message.size());
                        emit newData(new_packet);
                    }
                }
                else if(s1 == "XRF-Spectra")
                {
                    zmq::recv_result_t res1_2 = _zmq_socket->recv(message);
                    if(res1_2.has_value())
                    {
                        new_packet = _serializer.decode_spectra((char*)message.data(), message.size());
                        emit newData(new_packet);
                    }
                }
            }
            else
            {
                toSleepVal ++;
            }
            
            if(_zmq_qserv_socket != nullptr)
            {
                res2 = _zmq_qserv_socket->recv(token, zmq::recv_flags::dontwait);
                if(res2.has_value() && (EAGAIN != res2.value()))
                {
                    std::string s1 ((char*)token.data(), token.size());
                    if(s1 == "QS_Console")
                    {
                        zmq::recv_result_t res2_2 = _zmq_qserv_socket->recv(message);
                        if(res2_2.has_value())
                        {
                            QJsonObject rootJson = QJsonDocument::fromJson(QString::fromUtf8((char*)message.data(), message.size()).toUtf8()).object();
                            if(rootJson.contains("msg"))
                            {
                                QString msg = rootJson["msg"].toString();
                                msg.chop(1);
                                emit newStringData(msg);
                            }
                            else
                            {
                            //    logI<<data.toStdString()<<"\n"; // may cause issues coming from a thread
                            }
                        }
                    }
                }
                else
                {
                    toSleepVal++;
                }
            }
            if(toSleepVal >= sleepAtVal)
            { 
                QThread::sleep(1.0);
                
            }
        }
        _zmq_socket->close();
        if(_zmq_qserv_socket != nullptr)
        {
             _zmq_qserv_socket->close();
        }
    }
    void stop() {_running = false;}

signals:
    void newData(data_struct::Stream_Block<float> *new_packet);

    void newStringData(QString);

protected:

    bool _running;

    zmq::socket_t *_zmq_socket;

    zmq::socket_t *_zmq_qserv_socket;

    io::net::Basic_Serializer<float> _serializer;

};


//---------------------------------------------------------------------------

#endif /* NET_STREAM_WORKER_H */

//---------------------------------------------------------------------------
