/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/NetStreamWorker.h>

#include <QDebug>

/*---------------------------------------------------------------------------*/

NetStreamWorker::NetStreamWorker(QObject* parent) : QObject(parent)
{

    _subscriber = new io::net::Zmq_Subscriber("tcp://127.0.0.1:5556");

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

void NetStreamWorker::doWork()
{
    data_struct::xrf::Stream_Block *new_packet;
    while(_running)
    {
        _subscriber->get_counts(new_packet);
        emit newData(new_packet);
    }
}

/*---------------------------------------------------------------------------*/
