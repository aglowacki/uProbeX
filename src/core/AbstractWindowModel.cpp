/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <core/AbstractWindowModel.h>

/*---------------------------------------------------------------------------*/

AbstractWindowModel::AbstractWindowModel()
{


}

/*---------------------------------------------------------------------------*/

AbstractWindowModel::~AbstractWindowModel()
{


}


/*---------------------------------------------------------------------------*/

QList<int> AbstractWindowModel::getHistogram()
{

   return m_histogram;

}

/*---------------------------------------------------------------------------*/

void AbstractWindowModel::setHistogram(QList<int> hist)
{

   m_histogram = hist;

}

/*---------------------------------------------------------------------------*/
