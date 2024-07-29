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
#include "mvc/BlueskyPlan.h"
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

       std::string conn_str = "tcp://"+str_ip.toStdString()+":60615";
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

    bool get_avail_scans(std::unordered_map<QString, BlueskyPlan> &plans, QString &msg)
    {
        bool ret = false;
        zmq::message_t message;
        QJsonDocument doc;
        QJsonObject obj;
        QJsonObject params;
        obj["method"] = "plans_allowed";
        params["user_group"] = "primary";
        obj["params"] = params;
        doc.setObject(obj);
        QByteArray msg_arr = doc.toJson();
        _zmq_comm_socket->send(msg_arr.data(), msg_arr.length());

        _zmq_comm_socket->recv(&message);
        QJsonObject reply = QJsonDocument::fromJson(QString::fromUtf8((char*)message.data(), message.size()).toUtf8()).object();
        if(reply.contains("success"))
        {
            if(reply["success"].toString() == "true")
            {
                ret = true;
            }
        }
        if(reply.contains("plans_allowed"))
        {
            QJsonObject iojb = reply["plans_allowed"].toObject();
            for( auto itr : iojb)
            {
                QJsonObject pobj = itr.toObject();
                if(pobj.contains("name"))
                {
                    plans[pobj["name"].toString()].name = pobj["name"].toString();
                    if(pobj.contains("description"))
                    {
                        plans[pobj["name"].toString()].description = pobj["description"].toString();
                    }
                    if(pobj.contains("module"))
                    {
                        plans[pobj["name"].toString()].module = pobj["module"].toString();
                    }
                    if(pobj.contains("parameters"))
                    {
                        QJsonArray params_obj = pobj["parameters"].toArray();
                        for( auto itr2 : params_obj)
                        {
                            QJsonObject param = itr2.toObject();
                            if(param.contains("name"))
                            {
                                plans[pobj["name"].toString()].parameters[param["name"].toString()].name = param["name"].toString();
                                if(param.contains("default"))
                                {
                                    plans[pobj["name"].toString()].parameters[param["name"].toString()].default_val = param["default"].toString();
                                }
                                if(param.contains("description"))
                                {
                                    plans[pobj["name"].toString()].parameters[param["name"].toString()].description = param["description"].toString();
                                }
                            }
                        }
                    }
                }
                
            }
        }
        if(reply.contains("msg"))
        {
            msg = reply["msg"].toString();
        }
        return ret;
    }

    void get_queued_scans()
    {

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
                    QJsonObject rootJson = QJsonDocument::fromJson(QString::fromUtf8((char*)message.data(), message.size()).toUtf8()).object();
                    if(rootJson.contains("msg"))
                    {
                        emit newData(rootJson["msg"].toString());
                    }
                    else
                    {
                    //    logI<<data.toStdString()<<"\n"; // may cause issues coming from a thread
                    }
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

#endif /*  */

//---------------------------------------------------------------------------
