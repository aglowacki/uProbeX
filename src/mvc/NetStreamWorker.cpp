/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/NetStreamWorker.h>

#include <QDebug>

/*---------------------------------------------------------------------------*/

NetStreamWorker::NetStreamWorker(QObject* parent) : QThread(parent)
{

    _subscriber = new io::net::Zmq_Subscriber("tcp://127.0.0.1:43434");

}

/*---------------------------------------------------------------------------*/

NetStreamWorker::~NetStreamWorker()
{
    if(_subscriber != nullptr)
    {
        delete _subscriber;
        _subscriber = nullptr;
    }
}

/*---------------------------------------------------------------------------*/
