/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/NetStreamWorker.h>

#include <QDebug>

/*---------------------------------------------------------------------------*/

NetStreamWorker::NetStreamWorker(QObject* parent) : QThread(parent)
{

    std::string conn_str = "tcp://127.0.0.1:43434";
    //std::string conn_str = "tcp://192.168.0.254:43434";
    _context = new zmq::context_t(1);
    _zmq_socket = new zmq::socket_t(*_context, ZMQ_SUB);
    _zmq_socket->connect(conn_str);
    _zmq_socket->setsockopt(ZMQ_SUBSCRIBE, "XRF-Counts", 10);

}

/*---------------------------------------------------------------------------*/

NetStreamWorker::~NetStreamWorker()
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

/*---------------------------------------------------------------------------*/
