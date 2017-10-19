/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/NetStreamWorker.h>

#include <QDebug>

/*---------------------------------------------------------------------------*/

NetStreamWorker::NetStreamWorker(QObject* parent) : QThread(parent)
{

    _context = nullptr;
    _zmq_socket = nullptr;

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
