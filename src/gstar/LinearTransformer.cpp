/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/LinearTransformer.h>
#include <math.h>

using namespace gstar;

/*---------------------------------------------------------------------------*/

#define STR_SCALE_X "ScaleX"
#define STR_SCALE_Y "ScaleY"
#define STR_SCALE_Z "ScaleZ"
#define STR_TOPLEFT_X "TopLeftX"
#define STR_TOPLEFT_Y "TopLeftY"
#define STR_TOPLEFT_Z "TopLeftZ"
#define STR_DIVIDER_X "DividerX"
#define STR_DIVIDER_Y "DividerY"
#define STR_DIVIDER_Z "DividerZ"

/*---------------------------------------------------------------------------*/

LinearTransformer::LinearTransformer() : ITransformer()
{

   m_divider_x = 1.0;
   m_divider_y = 1.0;
   m_divider_z = 1.0;
   m_scaleX = 0.0;
   m_scaleY = 0.0;
   m_scaleZ = 0.0;
   m_topLeftX = 0.0;
   m_topLeftY = 0.0;
   m_topLeftZ = 0.0;

}

/*---------------------------------------------------------------------------*/

LinearTransformer::~LinearTransformer()
{

}

/*---------------------------------------------------------------------------*/

bool LinearTransformer::Init(QMap<QString, double> globalVars) 
{ 

   //TODO fill in vars  
   return true; 

} 

/*---------------------------------------------------------------------------*/

QMap<QString, double> LinearTransformer::getAllCoef()
{

   QMap<QString, double> coefs;

   coefs.insert("topLeftX", m_topLeftX);
   coefs.insert("topLeftY", m_topLeftY);
   coefs.insert("topLeftZ", m_topLeftZ);
   coefs.insert("dividerX", m_divider_x);
   coefs.insert("dividerY", m_divider_y);
   coefs.insert("dividerZ", m_divider_z);
   coefs.insert("scaleX", m_scaleX);
   coefs.insert("scaleY", m_scaleY);
   coefs.insert("scaleZ", m_scaleZ);

   return coefs;

}

/*---------------------------------------------------------------------------*/

double LinearTransformer::getScaleX()
{

   return m_scaleX;

}

/*---------------------------------------------------------------------------*/

double LinearTransformer::getScaleY()
{

   return m_scaleY;

}

/*---------------------------------------------------------------------------*/

double LinearTransformer::getScaleZ()
{

   return m_scaleZ;

}

/*---------------------------------------------------------------------------*/

double LinearTransformer::getTopLeftX()
{

   return m_topLeftX;

}

/*---------------------------------------------------------------------------*/

double LinearTransformer::getTopLeftY()
{

   return m_topLeftY;

}

/*---------------------------------------------------------------------------*/

bool LinearTransformer::getVariable(QString name, double *val)
{

   if(name == STR_SCALE_X)
   {
      *val = m_scaleX;
      return true;
   }
   else if(name == STR_SCALE_Y)
   {
      *val = m_scaleY;
      return true;
   }
   else if(name == STR_SCALE_Z)
   {
      *val = m_scaleZ;
      return true;
   }
   else if(name == STR_TOPLEFT_X)
   {
      *val = m_topLeftX;
      return true;
   }
   else if(name == STR_TOPLEFT_Y)
   {
      *val = m_topLeftY;
      return true;
   }
   else if(name == STR_TOPLEFT_Z)
   {
      *val = m_topLeftZ;
      return true;
   }
   else if(name == STR_DIVIDER_X)
   {
      *val = m_divider_x;
      return true;
   }
   else if(name == STR_DIVIDER_Y)
   {
      *val = m_divider_y;
      return true;
   }
   else if(name == STR_DIVIDER_Z)
   {
      *val = m_divider_z;
      return true;
   }

   return false;

}

/*---------------------------------------------------------------------------*/

void LinearTransformer::setDivider(double dx, double dy, double dz)
{

   m_divider_x = dx;
   m_divider_y = dy;
   m_divider_z = dz;

}

/*---------------------------------------------------------------------------*/

void LinearTransformer::setScale(double x, double y, double z)
{

   m_scaleX =  x;
   m_scaleY =  y;
   m_scaleZ =  z;

}

/*---------------------------------------------------------------------------*/

void LinearTransformer::setScaleX(double x)
{

   m_scaleX =  x;

}

/*---------------------------------------------------------------------------*/

void LinearTransformer::setScaleY(double y)
{

   m_scaleY = y;

}

/*---------------------------------------------------------------------------*/

void LinearTransformer::setScaleZ(double z)
{

   m_scaleZ = z;

}


/*---------------------------------------------------------------------------*/

void LinearTransformer::setTopLeft(double x, double y)
{

   m_topLeftX = x;
   m_topLeftY = y;

}

/*---------------------------------------------------------------------------*/

bool LinearTransformer::setVariable(QString name, double val)
{

   if(name == STR_SCALE_X)
   {
      m_scaleX = val;
      return true;
   }
   else if(name == STR_SCALE_Y)
   {
      m_scaleY = val;
      return true;
   }
   else if(name == STR_SCALE_Z)
   {
      m_scaleZ = val;
      return true;
   }
   else if(name == STR_TOPLEFT_X)
   {
      m_topLeftX = val;
      return true;
   }
   else if(name == STR_TOPLEFT_Y)
   {
      m_topLeftY = val;
      return true;
   }
   else if(name == STR_TOPLEFT_Z)
   {
      m_topLeftZ = val;
      return true;
   }
   else if(name == STR_DIVIDER_X)
   {
      m_divider_x = val;
      return true;
   }
   else if(name == STR_DIVIDER_Y)
   {
      m_divider_y = val;
      return true;
   }
   else if(name == STR_DIVIDER_Z)
   {
      m_divider_z = val;
      return true;
   }

   return false;

}

/*---------------------------------------------------------------------------*/

void LinearTransformer::transformCommand(double inX,
                                         double inY,
                                         double inZ,
                                         double *outX,
                                         double *outY,
                                         double *outZ)
{

   *outX = ( (m_scaleX * (double)inX) + m_topLeftX ) / m_divider_x;
   *outY = ( (m_scaleY * (double)inY) + m_topLeftY ) / m_divider_y;
   *outZ = ( (m_scaleZ * (double)inZ) + m_topLeftZ ) / m_divider_z;

}

/*---------------------------------------------------------------------------*/
