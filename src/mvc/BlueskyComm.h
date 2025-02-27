/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef BLUESKY_COMM_H
#define BLUESKY_COMM_H

//---------------------------------------------------------------------------

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <zmq.hpp>
#include "mvc/BlueskyPlan.h"
#include <string>

#include "core/defines.h"
//---------------------------------------------------------------------------


class BlueskyComm 
{

public:

   /**
    * Constructor.
    */
   BlueskyComm(zmq::context_t *context, QString str_ip) 
   {
        if(str_ip.length() > 0)
        {
            std::string conn_str = "tcp://"+str_ip.toStdString()+":60615";
            _zmq_comm_socket = new zmq::socket_t(*context, ZMQ_REQ);
            _zmq_comm_socket->connect(conn_str);
        }
        else
        {
            logW<<"QServer ip address is blank, Can not connect!\n";
        }
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
       _zmq_comm_socket = nullptr;
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
        
        if(plan.uuid.length() > 0)
        {
            item["item_uid"] = plan.uuid;
        }
        item["name"] = plan.type;
        item["item_type"] = "plan";
        QJsonObject meta;
        meta["name"] = plan.name;
        item["meta"] = meta;
        QJsonArray args;
        QJsonObject kwargs;
        for(auto itr: plan.parameters)
        {
            /*
            if(itr.name == "detectors") 
            {
                QJsonArray inner_args; // need this for bluesky or it doesn't work
                QStringList sarr = itr.default_val.split(",");
                for(auto sitr : sarr)
                {
                    inner_args.append(QJsonValue::fromVariant(sitr));
                }
                args.append(inner_args);
            }
            */
            if(itr.default_val.length() > 0)
            {
                kwargs[itr.name] = itr.default_val;
            }
            /*
                if(itr.kind == BlueskyParamType::String)
                {
                    kwargs[itr.name] = itr.default_val;
                }
                else if(itr.kind == BlueskyParamType::Bool)
                {
                    if(itr.default_val == "1")
                    {
                        itr.default_val = "True";
                    }
                    else
                    {
                        itr.default_val = "False";
                    }
                    QVariant v =  itr.default_val;
                    kwargs[itr.name] = QJsonValue::fromVariant(v.toBool());
                }
                else if(itr.kind == BlueskyParamType::Int)
                {
                    kwargs[itr.name] = QJsonValue::fromVariant(itr.default_val.toInt());
                }
                else if(itr.kind == BlueskyParamType::Double)
                {
                    kwargs[itr.name] = QJsonValue::fromVariant(itr.default_val.toDouble());
                }
            }
                */
        }
        item["kwargs"] = kwargs;
        item["args"] = args;
        //qDebug()<<item;
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
        zmq::message_t zmsg(msg_arr.data(), msg_arr.length());
        zmq::send_result_t s_res = _zmq_comm_socket->send(zmsg);

        zmq::recv_result_t r_res = _zmq_comm_socket->recv(message);
        if(r_res.has_value())
        {
            QJsonObject reply = QJsonDocument::fromJson(QByteArray::fromRawData((char*)message.data(), message.size())).object();
            if(reply.contains("success"))
            {
                QVariant v = reply.value("success").toVariant();
                QString strV = v.toString().toLower();
                if(strV == u"true" || strV == "true")
                {
                    ret = true;
                }
            }
            if(reply.contains("msg"))
            {
                msg = reply["msg"].toString();
            }
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
        zmq::message_t zmsg(msg_arr.data(), msg_arr.length());
        zmq::send_result_t s_res = _zmq_comm_socket->send(zmsg);

        zmq::recv_result_t r_res = _zmq_comm_socket->recv(message);
        if(r_res.has_value())
        {
            QJsonObject reply = QJsonDocument::fromJson(QByteArray::fromRawData((char*)message.data(), message.size())).object();
            if(reply.contains("success"))
            {
                QVariant v = reply.value("success").toVariant();
                QString strV = v.toString().toLower();
                if(strV == u"true" || strV == "true")
                {
                    ret = true;
                }
            }
            if(reply.contains("msg"))
            {
                msg = reply["msg"].toString();
            }
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
        zmq::message_t zmsg(msg_arr.data(), msg_arr.length());
        zmq::send_result_t s_res = _zmq_comm_socket->send(zmsg);

        zmq::recv_result_t r_res = _zmq_comm_socket->recv(message);
        if(r_res.has_value())
        {
            QJsonObject reply = QJsonDocument::fromJson(QByteArray::fromRawData((char*)message.data(), message.size())).object();
            if(reply.contains("success"))
            {
                QVariant v = reply.value("success").toVariant();
                QString strV = v.toString().toLower();
                if(strV == u"true" || strV == "true")
                {
                    ret = true;
                }
            }
            if(reply.contains("msg"))
            {
                msg = reply["msg"].toString();
            }
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
        zmq::message_t zmsg(msg_arr.data(), msg_arr.length());
        zmq::send_result_t s_res = _zmq_comm_socket->send(zmsg);

        zmq::recv_result_t r_res = _zmq_comm_socket->recv(message);
        if(r_res.has_value())
        {
            QJsonObject reply = QJsonDocument::fromJson(QByteArray::fromRawData((char*)message.data(), message.size())).object();
            if(reply.contains("success"))
            {
                QVariant v = reply.value("success").toVariant();
                QString strV = v.toString().toLower();
                if(strV == u"true" || strV == "true")
                {
                    ret = true;
                }
            }
            if(reply.contains("msg"))
            {
                msg = reply["msg"].toString();
            }
        }
        return ret;
    }

    //---------------------------------------------------------------------------

    bool queue_plan(QString &msg, BlueskyPlan& plan)
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
        zmq::message_t zmsg(msg_arr.data(), msg_arr.length());
        zmq::send_result_t s_res = _zmq_comm_socket->send(zmsg);
logI<<QString::fromUtf8(msg_arr).toStdString()<<"\n";
        zmq::recv_result_t r_res = _zmq_comm_socket->recv(message);
        if(r_res.has_value())
        {
            
            QJsonObject reply = QJsonDocument::fromJson(QByteArray::fromRawData((char*)message.data(), message.size())).object();
            
            if(reply.contains("success"))
            {
                QVariant v = reply.value("success").toVariant();
                QString strV = v.toString();
                if(strV == u"true")
                {
                    ret = true;
                }
            }
            if(reply.contains("msg"))
            {
                //msg = QString::fromUtf8(reply.value("msg").toString().toUtf8());
                msg = reply.value("msg").toString();
            }
            if(reply.contains("item"))
            {
                QJsonObject obj_item = reply.value("item").toObject();
                if(obj_item.contains("item_uid"))
                {
                    plan.uuid = obj_item.value("item_uid").toString();
                }
            }
        }
 
        return ret;
    }
    
    //---------------------------------------------------------------------------

    bool update_plan(QString &msg, BlueskyPlan& plan)
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
        QByteArray msg_arr = gen_send_mesg2("queue_item_update", params);
        zmq::message_t zmsg(msg_arr.data(), msg_arr.length());
        zmq::send_result_t s_res = _zmq_comm_socket->send(zmsg);
logI<<QString::fromUtf8(msg_arr).toStdString()<<"\n";
        zmq::recv_result_t r_res = _zmq_comm_socket->recv(message);
        if(r_res.has_value())
        {
            QJsonObject reply = QJsonDocument::fromJson(QByteArray::fromRawData((char*)message.data(), message.size())).object();
            if(reply.contains("success"))
            {
                QVariant v = reply.value("success").toVariant();
                QString strV = v.toString().toLower();
                if(strV == u"true" || strV == "true")
                {
                    ret = true;
                }
            }
            if(reply.contains("msg"))
            {
                msg = reply["msg"].toString();
            }
            if(reply.contains("item"))
            {
                QJsonObject obj_item = reply["item"].toObject();
                if(obj_item.contains("item_uid"))
                {
                    plan.uuid = obj_item.value("item_uid").toString();
                }
            }
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
        zmq::message_t zmsg(msg_arr.data(), msg_arr.length());
        zmq::send_result_t s_res = _zmq_comm_socket->send(zmsg);

        zmq::recv_result_t r_res = _zmq_comm_socket->recv(message);
        if(r_res.has_value())
        {
            QJsonObject reply = QJsonDocument::fromJson(QByteArray::fromRawData((char*)message.data(), message.size())).object();
            if(reply.contains("success"))
            {
                QVariant v = reply.value("success").toVariant();
                QString strV = v.toString().toLower();
                if(strV == u"true" || strV == "true")
                {
                    ret = true;
                }
            }
            if(reply.contains("msg"))
            {
                msg = reply["msg"].toString();
            }
        }
        return ret;
    }

   //---------------------------------------------------------------------------
    
    bool removePlan(QString &msg, int row, BlueskyPlan& out_plan)
    {
        bool ret = false;
        if(_zmq_comm_socket == nullptr)
        {
            return ret;
        }
        zmq::message_t message;
        
        QJsonObject params;
        params["pos"] = row;
        QByteArray msg_arr = gen_send_mesg2("queue_item_remove", params);
        zmq::message_t zmsg(msg_arr.data(), msg_arr.length());
        zmq::send_result_t s_res = _zmq_comm_socket->send(zmsg);

        zmq::recv_result_t r_res = _zmq_comm_socket->recv(message);
        if(r_res.has_value())
        {
            QJsonObject reply = QJsonDocument::fromJson(QByteArray::fromRawData((char*)message.data(), message.size())).object();
            if(reply.contains("success"))
            {
                QVariant v = reply.value("success").toVariant();
                QString strV = v.toString().toLower();
                if(strV == u"true" || strV == "true")
                {
                    ret = true;
                }
            }
            if(reply.contains("msg"))
            {
                msg = reply["msg"].toString();
            }
            if(reply.contains("item"))
            {
                QJsonObject obj_item = reply["item"].toObject();
                if(obj_item.contains("item_uid"))
                {
                    out_plan.uuid = obj_item.value("item_uid").toString();
                }
            }
        }
        return ret;
    }

   //---------------------------------------------------------------------------
    
    bool removePlan(QString &msg, QString uuid)
    {
        bool ret = false;
        if(_zmq_comm_socket == nullptr)
        {
            return ret;
        }
        zmq::message_t message;
        
        QJsonObject params;
        params["uid"] = uuid;
        QByteArray msg_arr = gen_send_mesg2("queue_item_remove", params);
        zmq::message_t zmsg(msg_arr.data(), msg_arr.length());
        zmq::send_result_t s_res = _zmq_comm_socket->send(zmsg);

        zmq::recv_result_t r_res = _zmq_comm_socket->recv(message);
        if(r_res.has_value())
        {
            QJsonObject reply = QJsonDocument::fromJson(QByteArray::fromRawData((char*)message.data(), message.size())).object();
            if(reply.contains("success"))
            {
                QVariant v = reply.value("success").toVariant();
                QString strV = v.toString().toLower();
                if(strV == u"true" || strV == "true")
                {
                    ret = true;
                }
            }
            if(reply.contains("msg"))
            {
                msg = reply["msg"].toString();
            }
        }
        return ret;
    }

    //---------------------------------------------------------------------------

    void parse_args(const QJsonObject& kwargs, BlueskyPlan &plan)
    {
        for(auto pitr : kwargs.keys())
        {
            BlueskyParam bsp;
            bsp.name = pitr;
            bsp.setValue( kwargs.value(pitr).toString());
            /*
            if(kwargs.value(pitr).isDouble())
            {
                double p = kwargs.value(pitr).toDouble();
                bsp.setValue( QString::number(p) );
            }
            
            else if (kwargs.value(pitr).isBool())
            {
                bool p = kwargs.value(pitr).toBool();
                if(p)
                {
                    bsp.default_val = "True";
                }
                else
                {
                    bsp.default_val = "False";
                }
                bsp.kind = BlueskyParamType::Bool;
            }
            
            else
            {
                QString strval = kwargs.value(pitr).toString();
                QString lstrval = strval.toLower();
                if(lstrval == "true" || lstrval == "false" || lstrval == u"true" || lstrval == u"false")
                {
                    bsp.default_val = strval;
                    bsp.kind = BlueskyParamType::Bool;
                }
                else
                {
                    bsp.setValue(strval);
                }
            }
            
            else if( kwargs.value(pitr).isString() )
            {
                bsp.setValue(kwargs.value(pitr).toString());
            }
            */
            
            plan.parameters.push_back(bsp);
        }
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
        zmq::message_t zmsg(msg_arr.data(), msg_arr.length());
        zmq::send_result_t s_res = _zmq_comm_socket->send(zmsg);;

        zmq::recv_result_t r_res = _zmq_comm_socket->recv(message);
        if(r_res.has_value())
        {
logI<<message.to_string()<<"\n";
            QJsonObject reply = QJsonDocument::fromJson(QByteArray::fromRawData((char*)message.data(), message.size())).object();
            if(reply.contains("success"))
            {
                QVariant v = reply.value("success").toVariant();
                QString strV = v.toString().toLower();
                if(strV == u"true" || strV == "true")
                {
                    ret = true;
                }
            }
            if(reply.contains("plans_allowed"))
            {
                QJsonObject iojb = reply["plans_allowed"].toObject();
                QStringList lili = iojb.keys();
                for(auto aaa : lili)
                {
                    logI<<aaa.toStdString()<<"\n";
                }
                for( auto itr : iojb)
                {
                    QJsonObject pobj = itr.toObject();
                    if(pobj.contains("name"))
                    {
                        logI<<pobj.value("name").toString().toStdString()<<"\n";
                        plans[pobj.value("name").toString()].name = pobj.value("name").toString();
                        if(pobj.contains("description"))
                        {
                            plans[pobj.value("name").toString()].description = pobj.value("description").toString();
                        }
                        if(pobj.contains("module"))
                        {
                            plans[pobj.value("name").toString()].module = pobj.value("module").toString();
                        }
                        if(pobj.contains("parameters"))
                        {
                            
                            QJsonArray params_obj = pobj["parameters"].toArray();

                            for( auto itr2 : params_obj)
                            {
                                BlueskyParam bsparam;
                                QJsonObject param = itr2.toObject();
                                if(param.contains("name"))
                                {
                                    bsparam.name = param.value("name").toString();
                                    if(param.contains("default"))
                                    {
                                        bsparam.kind = BlueskyParamType::String;
                                        bsparam.setValue(param.value("default").toString());
                                    }
                                    if(param.contains("description"))
                                    {
                                        bsparam.description = param.value("description").toString();
                                    }
                                }
                                /* only have seen 1 and 3 and not sure what they mean
                                if(param.contains("kind"))
                                {
                                    QJsonObject kind = param["kind"].toObject();
                                    if(kind.contains("value"))
                                    {
                                        int tval = kind["value"].toInt();
                                        switch (tval)
                                        {
                                            case 1:
                                                bsparam.kind = BlueskyParamType::Numeral;
                                                break;
                                            case 3:
                                                bsparam.kind = BlueskyParamType::String;
                                                break;
                                            default:
                                                break;
                                        }
                                    }
                                }
                                */
                                plans[pobj.value("name").toString()].parameters.push_back(bsparam);
                            }
                        }
                    }
                    
                }
            }
            if(reply.contains("msg"))
            {
                msg = reply.value("msg").toString();
            }
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
        running_plan.type = "";
        running_plan.uuid = "";
        running_plan.parameters.clear();
        zmq::message_t message;
        QByteArray msg_arr = gen_send_mesg("queue_get", nullptr); 
        zmq::message_t zmsg(msg_arr.data(), msg_arr.length());
        zmq::send_result_t s_res = _zmq_comm_socket->send(zmsg);

        zmq::recv_result_t r_res = _zmq_comm_socket->recv(message);
        if(r_res.has_value())
        {
logI<<message.to_string()<<"\n";
            QJsonObject reply = QJsonDocument::fromJson(QByteArray::fromRawData((char*)message.data(), message.size())).object();
            if(reply.contains("success"))
            {
                QVariant v = reply.value("success").toVariant();
                QString strV = v.toString().toLower();
                if(strV == u"true" || strV == "true")
                {
                    ret = true;
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
                        plan.type = param.value("name").toString();
                    }
                    if(param.contains("args"))
                    {
                        parse_args(param["args"].toObject(), plan);
                    }
                    if(param.contains("kwargs"))
                    {
                        parse_args(param["kwargs"].toObject(), plan);
                    }
                    if(param.contains("user"))
                    {
                        plan.user = param.value("user").toString();
                    }
                    if(param.contains("meta"))
                    {
                        QJsonObject meta = param["meta"].toObject();
                        if(meta.contains("name"))
                        {
                            plan.name = meta.value("name").toString();
                        }
                    }
                    if(param.contains("item_uid"))
                    {
                        plan.uuid = param.value("item_uid").toString();
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
                    running_plan.type =  running_item.value("name").toString();
                }
                if(running_item.contains("args"))
                {
                    parse_args(running_item["args"].toObject(), running_plan);
                }
                if(running_item.contains("kwargs"))
                {
                    parse_args(running_item["kwargs"].toObject(), running_plan);
                }
                if(running_item.contains("user"))
                {
                    running_plan.user =  running_item.value("user").toString();
                }
                if(running_item.contains("item_uid"))
                {
                    running_plan.uuid =  running_item.value("item_uid").toString();
                }
                if(running_item.contains("properties"))
                {
                    QJsonObject props = running_item["properties"].toObject();
                    if(props.contains("time_start"))
                    {
                        running_plan.result.time_start =  props["time_start"].toDouble();
                    }
                }
            }
            /*
            if(reply.contains("plan_queue_uid"))
            {

            }
            */
        }
        return ret;
    }
    
    //---------------------------------------------------------------------------
    
    bool clear_history(QString &msg)
    {
        bool ret = false;
        if(_zmq_comm_socket == nullptr)
        {
            return ret;
        }
        zmq::message_t message;
        QByteArray msg_arr = gen_send_mesg("history_clear", nullptr); 
        zmq::message_t zmsg(msg_arr.data(), msg_arr.length());
        zmq::send_result_t s_res = _zmq_comm_socket->send(zmsg);

        zmq::recv_result_t r_res = _zmq_comm_socket->recv(message);
        if(r_res.has_value())
        {
            QJsonObject reply = QJsonDocument::fromJson(QByteArray::fromRawData((char*)message.data(), message.size())).object();
            msg = QString::fromUtf8((char*)message.data(), message.size());
            if(reply.contains("success"))
            {
                QVariant v = reply.value("success").toVariant();
                QString strV = v.toString().toLower();
                if(strV == u"true" || strV == "true")
                {
                    ret = true;
                }
            }
            if(reply.contains("msg"))
            {
                msg = reply.value("msg").toString();
            }
        }
        return ret;
    }

    //---------------------------------------------------------------------------

    bool get_scan_history(QString &msg, std::vector<BlueskyPlan> &finished_plans, bool raw_mesg=false)
    {
        bool ret = false;
        if(_zmq_comm_socket == nullptr)
        {
            return ret;
        }
        zmq::message_t message;
        QByteArray msg_arr = gen_send_mesg("history_get", nullptr); 
        zmq::message_t zmsg(msg_arr.data(), msg_arr.length());
        zmq::send_result_t s_res = _zmq_comm_socket->send(zmsg);

        zmq::recv_result_t r_res = _zmq_comm_socket->recv(message);
        if(r_res.has_value())
        {
            if(raw_mesg)
            {
                msg = QString::fromUtf8((char*)message.data(), message.size());
                return true;
            }

            QJsonObject reply = QJsonDocument::fromJson(QByteArray::fromRawData((char*)message.data(), message.size())).object();
            if(reply.contains("success"))
            {
                QVariant v = reply.value("success").toVariant();
                QString strV = v.toString().toLower();
                if(strV == u"true" || strV == "true")
                {
                    ret = true;
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
            finished_plans.clear();
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
                        parse_args(param["args"].toObject(), plan);
                    }
                    if(param.contains("kwargs"))
                    {
                        parse_args(param["kwargs"].toObject(), plan);
                    }
                    if(param.contains("result"))
                    {
                            /*
                            {'exit_status': 'completed',
                        'run_uids': ['ab6e2ebc-effb-4385-8988-fff128e592c1'],
                        'scan_ids': [5],
                        'time_start': 1729088124.896845,
                        'time_stop': 1729088129.916682,
                        'msg': '',
                        'traceback': ''}}],
                        */
                        QJsonObject result = param["result"].toObject();
                        if(result.contains("exit_status"))
                        {
                            plan.result.exit_status = result["exit_status"].toString();
                        }
                        if(result.contains("msg"))
                        {
                            plan.result.msg = result["msg"].toString();
                        }
                        if(result.contains("traceback"))
                        {
                            plan.result.traceback = result["traceback"].toString();
                        }
                        if(result.contains("time_start"))
                        {
                            plan.result.time_start = result["time_start"].toDouble();
                        }
                        if(result.contains("time_stop"))
                        {
                            plan.result.time_stop = result["time_stop"].toDouble();
                        }
                        if(result.contains("run_uids"))
                        {
                            QJsonArray runs = result["run_uids"].toArray();
                            if(runs.size() > 0)
                            {
                                plan.result.run_uids = runs.at(0).toString();
                            }
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
                    finished_plans.push_back(plan);
                }
            }
        }
        return ret;
    }

    //---------------------------------------------------------------------------

protected:

    zmq::socket_t *_zmq_comm_socket;

};


//---------------------------------------------------------------------------

#endif /*  */

//---------------------------------------------------------------------------
