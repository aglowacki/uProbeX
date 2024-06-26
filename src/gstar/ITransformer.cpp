/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/ITransformer.h>

using namespace gstar;

//---------------------------------------------------------------------------

ITransformer::ITransformer()
{

}

//---------------------------------------------------------------------------

ITransformer::~ITransformer()
{

}

//---------------------------------------------------------------------------

void ITransformer::setDecimalPrecision(int number)
{

   m_decimalPreci = number;

}

//---------------------------------------------------------------------------

void ITransformer::getResultWithPrecision(double* outX, double* outY, double* outZ)
{

    *outX = QString::number(*outX, 'f', m_decimalPreci).toDouble();
    *outY = QString::number(*outY, 'f', m_decimalPreci).toDouble();
    *outZ = QString::number(*outZ, 'f', m_decimalPreci).toDouble();

}

//---------------------------------------------------------------------------
