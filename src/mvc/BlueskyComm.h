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

    //---------------------------------------------------------------------------

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

    //---------------------------------------------------------------------------

    QByteArray gen_send_mesg(QString method, std::map<QString, QString> *params)
    {
        QJsonDocument doc;
        QJsonObject obj;
        obj["method"] = method;
        if(params != nullptr)
        {
            QJsonObject jparams;
            for( auto itr : *params )
            {
                jparams[itr.first] = itr.second ;
            }
            obj["params"] = jparams;
        }
        doc.setObject(obj);
        return doc.toJson();
    }

    //---------------------------------------------------------------------------

    QByteArray gen_send_mesg2(QString method, QJsonObject &params)
    {
        QJsonDocument doc;
        QJsonObject obj;
        obj["method"] = method;
        obj["params"] = params;
        doc.setObject(obj);
        return doc.toJson();
    }

    //---------------------------------------------------------------------------

    QJsonObject plan_to_json_item(const BlueskyPlan& plan)
    {
        /*
            {'name': 'count',
            'args': [['det1', 'det2']],
            'kwargs': {'num': 10, 'delay': 1},
            'item_type': 'plan',
            'user': 'qserver-cli',
            'user_group': 'primary',
            'item_uid': 'f66d959f-12e2-43a5-a67d-01b3d40b4f43'}
        */

        QJsonObject item;
        
        item["name"] = plan.type;
        item["item_type"] = "plan";
        QJsonObject meta;
        meta["name"] = plan.name;
        item["meta"] = meta;
        QJsonArray args;
        QJsonObject kwargs;
        for(auto itr: plan.parameters)
        {
            if(itr.second.name == "detectors") 
            {
                QJsonArray inner_args; // need this for bluesky or it doesn't work
                QStringList sarr = itr.second.default_val.split(",");
                for(auto sitr : sarr)
                {
                    inner_args.append(QJsonValue::fromVariant(sitr));
                }
                args.append(inner_args);
            }
            else if(itr.second.kind == BlueskyParamType::String && itr.second.default_val.length() > 0)
            {
                kwargs[itr.first] = itr.second.default_val;
            }
            else if(itr.second.kind == BlueskyParamType::Numeral && itr.second.default_val.length() > 0)
            {
                kwargs[itr.first] = QJsonValue::fromVariant(itr.second.default_val.toDouble());
            }
        }
        item["kwargs"] = kwargs;
        item["args"] = args;
       return item;
    }

    //---------------------------------------------------------------------------

    bool open_env(QString &msg)
    {
        // 
        bool ret = false;

        if(_zmq_comm_socket == nullptr)
        {
            return ret;
        }
        zmq::message_t message;
        QByteArray msg_arr = gen_send_mesg("environment_open", nullptr);
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
        if(reply.contains("msg"))
        {
            msg = reply["msg"].toString();
        }
        return ret;
    }

    //---------------------------------------------------------------------------

    bool close_env(QString &msg)
    {
        // 
        bool ret = false;
        if(_zmq_comm_socket == nullptr)
        {
            return ret;
        }
        zmq::message_t message;
        QByteArray msg_arr = gen_send_mesg("environment_close", nullptr);
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
        if(reply.contains("msg"))
        {
            msg = reply["msg"].toString();
        }
        return ret;
    }
    
    //---------------------------------------------------------------------------

    bool start_queue(QString &msg)
    {
        // 
        bool ret = false;
        if(_zmq_comm_socket == nullptr)
        {
            return ret;
        }
        zmq::message_t message;
        QByteArray msg_arr = gen_send_mesg("queue_start", nullptr);
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
        if(reply.contains("msg"))
        {
            msg = reply["msg"].toString();
        }
        return ret;
    }

    //---------------------------------------------------------------------------

    bool stop_queue(QString &msg)
    {
        // 
        bool ret = false;
        if(_zmq_comm_socket == nullptr)
        {
            return ret;
        }
        zmq::message_t message;
        QByteArray msg_arr = gen_send_mesg("queue_stop", nullptr);
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
        if(reply.contains("msg"))
        {
            msg = reply["msg"].toString();
        }
        return ret;
    }

    //---------------------------------------------------------------------------

    bool queue_plan(QString &msg, const BlueskyPlan& plan)
    {
        bool ret = false;
        if(_zmq_comm_socket == nullptr)
        {
            return ret;
        }
        zmq::message_t message;
        
        QJsonObject params;
        params["item"] = plan_to_json_item(plan);
        params["user"] = "uProbeX";
        params["user_group"] = "primary";
        QByteArray msg_arr = gen_send_mesg2("queue_item_add", params);
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
        if(reply.contains("msg"))
        {
            msg = reply["msg"].toString();
        }
        return ret;
    }
    
    //---------------------------------------------------------------------------
    
    bool movePlan(QString &msg, int srcRow, int destRow)
    {
        bool ret = false;
        if(_zmq_comm_socket == nullptr)
        {
            return ret;
        }
        zmq::message_t message;
        
        QJsonObject params;
        params["pos"] = srcRow;
        params["pos_dest"] = destRow;
        QByteArray msg_arr = gen_send_mesg2("queue_item_move", params);
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
        if(reply.contains("msg"))
        {
            msg = reply["msg"].toString();
        }
        return ret;
    }

    //---------------------------------------------------------------------------

    bool get_avail_scans(std::map<QString, BlueskyPlan> &plans, QString &msg)
    {
        bool ret = false;
        zmq::message_t message;
        
        //params["user_group"] = "primary";
        //obj["params"] = params;
        if(_zmq_comm_socket == nullptr)
        {
            return ret;
        }
        std::map<QString, QString> params;
        params["user_group"] = "primary";
        QByteArray msg_arr = gen_send_mesg("plans_allowed", &params); 
        _zmq_comm_socket->send(msg_arr.data(), msg_arr.length());

        _zmq_comm_socket->recv(&message);
        QJsonObject reply = QJsonDocument::fromJson(QString::fromUtf8((char*)message.data(), message.size()).toUtf8()).object();
        if(reply.contains("success"))
        {
            QString strReply = reply["success"].toString();
            if(strReply.length() > 0)
            {
                if(strReply == "true")
                {
                    ret = true;
                }
            }
            else
            {
                ret = reply["success"].toBool();
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
                                    if(param["default"].toString() != "None")
                                    {
                                        plans[pobj["name"].toString()].parameters[param["name"].toString()].default_val = param["default"].toString();
                                    }
                                }
                                if(param.contains("description"))
                                {
                                    plans[pobj["name"].toString()].parameters[param["name"].toString()].description = param["description"].toString();
                                }
                            }
                            if(param.contains("kind"))
                            {
                                QJsonObject kind = param["kind"].toObject();
                                if(kind.contains("value"))
                                {
                                    int tval = kind["value"].toInt();
                                    switch (tval)
                                    {
                                        case 1:
                                        plans[pobj["name"].toString()].parameters[param["name"].toString()].kind = BlueskyParamType::Numeral;
                                        break;
                                        case 3:
                                        plans[pobj["name"].toString()].parameters[param["name"].toString()].kind = BlueskyParamType::String;
                                        break;
                                        default:
                                        break;
                                    }
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

    //---------------------------------------------------------------------------

    bool get_queued_scans(QString &msg, std::vector<BlueskyPlan> &queued_plans, BlueskyPlan &running_plan)
    {
        bool ret = false;
        if(_zmq_comm_socket == nullptr)
        {
            return ret;
        }
        running_plan.name = "";
        running_plan.uuid = "";
        zmq::message_t message;
        QByteArray msg_arr = gen_send_mesg("queue_get", nullptr); 
        _zmq_comm_socket->send(msg_arr.data(), msg_arr.length());

        _zmq_comm_socket->recv(&message);
    
        QJsonObject reply = QJsonDocument::fromJson(QString::fromUtf8((char*)message.data(), message.size()).toUtf8()).object();
        if(reply.contains("success"))
        {
            QString strReply = reply["success"].toString();
            if(strReply.length() > 0)
            {
                if(strReply == "true")
                {
                    ret = true;
                }
            }
            else
            {
                ret = reply["success"].toBool();
            }
        }
        if(reply.contains("items"))
        {
            /*
            {'name': 'count',
            'args': [['det1', 'det2']],
            'kwargs': {'num': 10, 'delay': 1},
            'item_type': 'plan',
            'user': 'qserver-cli',
            'user_group': 'primary',
            'item_uid': 'f66d959f-12e2-43a5-a67d-01b3d40b4f43'}
            */
           queued_plans.clear();
            QJsonArray items = reply["items"].toArray();
            for( auto itr2 : items)
            {
                BlueskyPlan plan;
                QJsonObject param = itr2.toObject();
                if(param.contains("name"))
                {
                    plan.type = param["name"].toString();
                }
                if(param.contains("args"))
                {
                    //plan.args = param["args"].toString();
                }
                if(param.contains("kwargs"))
                {
                    QJsonObject kwargs = param["kwargs"].toObject();
                    //plan.parameters
                    for(auto pitr : kwargs.keys())
                    {
                        BlueskyParam bsp;
                        bsp.name = pitr;
                        bsp.default_val = kwargs[pitr].toString();
                        plan.parameters[pitr] = bsp;
                    }
                }
                if(param.contains("user"))
                {
                    plan.user = param["user"].toString();
                }
                if(param.contains("meta"))
                {
                    QJsonObject meta = param["meta"].toObject();
                    if(meta.contains("name"))
                    {
                        plan.name = meta["name"].toString();
                    }
                }
                if(param.contains("item_uid"))
                {
                    plan.uuid = param["item_uid"].toString();
                }
                queued_plans.push_back(plan);
            }
        }
        if(reply.contains("running_item"))
        {
            /*
            "name": "count",
            "args": [
                [
                    "det4",
                    "det3"
                ]
            ],
            "kwargs": {
                "num": 10,
                "delay": 1
            },
            "item_type": "plan",
            "user": "qserver-cli",
            "user_group": "primary",
            "item_uid": "dacdd8fc-2903-490d-8027-7510d43fb1e2",
            "properties": {
                "time_start": 1722889109.0853188
            }
            */

            QJsonObject running_item = reply["running_item"].toObject();
            if(running_item.contains("name"))
            {
                running_plan.name =  running_item["name"].toString();
            }
            if(running_item.contains("args"))
            {
                //running_plan.name =  running_item["args"].toString();
            }
            if(running_item.contains("kwargs"))
            {
                QJsonObject kwargs = running_item["kwargs"].toObject();
                for(auto pitr : kwargs.keys())
                {
                    BlueskyParam bsp;
                    bsp.name = pitr;
                    bsp.default_val = kwargs[pitr].toString();
                    running_plan.parameters[pitr] = bsp;
                }
            }
            if(running_item.contains("user"))
            {
                running_plan.user =  running_item["user"].toString();
            }
            if(running_item.contains("item_uid"))
            {
                running_plan.uuid =  running_item["item_uid"].toString();
            }
        }
        /*
        if(reply.contains("plan_queue_uid"))
        {

        }
        */
        return ret;
    }

    //---------------------------------------------------------------------------

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
                        QString msg = rootJson["msg"].toString();
                        msg.chop(1);
                        emit newData(msg);
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
