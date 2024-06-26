/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/CoordinateModel.h>
#include <gstar/ITransformer.h>

using namespace gstar;

//---------------------------------------------------------------------------

CoordinateModel::CoordinateModel() : QObject(0)
{

}

//---------------------------------------------------------------------------

CoordinateModel::CoordinateModel(ITransformer *transformer) : QObject(0)
{

   m_transformer = transformer;

}

//---------------------------------------------------------------------------

CoordinateModel::~CoordinateModel()
{

   if(m_transformer != nullptr)
   {
      delete m_transformer;
      m_transformer = nullptr;
   }

}

//---------------------------------------------------------------------------

ITransformer* CoordinateModel::getTransformer()
{

   return m_transformer;

}

//---------------------------------------------------------------------------

bool CoordinateModel::getTransformerVariable(QString name, double *val)
{

   if(m_transformer != nullptr)
   {
      return m_transformer->getVariable(name, val);
   }

   return false;

}

//---------------------------------------------------------------------------

bool CoordinateModel::setTransformerVariable(QString name, double val)
{

   if(m_transformer != nullptr)
   {
      if(m_transformer->setVariable(name, val))
      {
         emit modelUpdated();
         return true;
      }
   }

   return false;

}

//---------------------------------------------------------------------------

void CoordinateModel::setTransformer(ITransformer *transformer)
{

   m_transformer = transformer;

}

//---------------------------------------------------------------------------

void CoordinateModel::setTransformerPrecision(int number)
{

   m_transformer->setDecimalPrecision(number);

}

//---------------------------------------------------------------------------

void CoordinateModel::runTransformer(double inX,
                                     double inY,
                                     double inZ,
                                     double *outX,
                                     double *outY,
                                     double *outZ)
{

   if(m_transformer != nullptr)
   {
      m_transformer->transformCommand(inX, inY, inZ, outX, outY, outZ);
      ///TODO: find out why getResultWithPrec cause major performance issues
//      m_transformer->getResultWithPrecision(outX, outY, outZ);
      emit transformOutput(*outX, *outY, *outZ);
   }

}


//---------------------------------------------------------------------------
