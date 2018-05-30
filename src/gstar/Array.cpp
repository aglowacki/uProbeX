/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/Array.h>

using namespace gstar;

/*---------------------------------------------------------------------------*/

Array::Array(unsigned long long bytes)
    : m_bytes(bytes)
{

    m_buffer = new uchar[m_bytes];
    m_rank = 0;
    m_type = UINT8;
    //m_type = UINT16;
    m_dataDims = nullptr;

}

/*---------------------------------------------------------------------------*/

Array::Array(uchar* buffer, unsigned long long bytes)
{
    if (buffer != NULL && bytes > 0)
    {
        m_buffer = buffer;
        m_bytes = bytes;
    }
}

/*---------------------------------------------------------------------------*/

Array::~Array()
{

    if (m_dataDims != NULL)
    {
        delete [] m_dataDims;
    }
    m_dataDims = nullptr;

    if (m_buffer != NULL)
    {
        delete [] m_buffer;
    }
    m_buffer = nullptr;

}

/*---------------------------------------------------------------------------*/

uchar* Array::getBuffer() const
{

    return m_buffer;

}

/*---------------------------------------------------------------------------*/

unsigned long long Array::getBytes() const
{

    return m_bytes;

}

/*---------------------------------------------------------------------------*/

void Array::getDims(unsigned long long* dims, unsigned long sz) const
{

    for (unsigned int d = 0 ; (d < m_rank) && (d < sz) ; d++) {
        dims[d] = (unsigned long long) m_dataDims[d];
    }

}

/*---------------------------------------------------------------------------*/

unsigned long Array::getRank() const
{

    return m_rank;

}

/*---------------------------------------------------------------------------*/

Array::DataType Array::getType() const
{

    return m_type;

}

/*---------------------------------------------------------------------------*/

void Array::setDimensions(unsigned long long* dims, unsigned long sz)
{

    for (unsigned long d = 0 ; (d < m_rank) && (d < sz) ; d++) {
        m_dataDims[d] = dims[d];
    }

}

/*---------------------------------------------------------------------------*/

void Array::setRank(unsigned long rank)
{

    m_rank = rank;
    m_dataDims = new unsigned long long[m_rank];

}

/*---------------------------------------------------------------------------*/

void Array::setType(Array::DataType type)
{

    m_type = type;

}

/*---------------------------------------------------------------------------*/
