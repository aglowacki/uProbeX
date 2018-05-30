/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/ColorTransferFunction.h>
#include <QDebug>

using namespace gstar;

/*---------------------------------------------------------------------------*/

ColorTransferFunction::ColorTransferFunction()
{

   m_dataType = 0;

}

/*---------------------------------------------------------------------------*/

ColorTransferFunction::ColorTransferFunction(const ColorTransferFunction& s)
{

   m_redSubRange[0] = s. m_redSubRange[0];
   m_redSubRange[1] = s. m_redSubRange[1];
   m_greenSubRange[0] = s.m_greenSubRange[0];
   m_greenSubRange[1] = s.m_greenSubRange[1];
   m_blueSubRange[0] = s.m_blueSubRange[0];
   m_blueSubRange[1] = s.m_blueSubRange[1];

   m_totalRange[0] = s.m_totalRange[0];
   m_totalRange[1] = s.m_totalRange[1];

   m_iRed = s.m_iRed;
   m_vRed = s.m_vRed;

   m_iGreen = s.m_iGreen;
   m_vGreen = s.m_vGreen;

   m_iBlue = s.m_iBlue;
   m_vBlue = s.m_vBlue;

   m_dataType = s.m_dataType;

   for (int i=0; i<65536; i++)
   {
      m_redMap[i] = s.m_redMap[i];
      m_greenMap[i] = s.m_greenMap[i];
      m_blueMap[i] = s.m_blueMap[i];
   }

}

/*---------------------------------------------------------------------------*/

ColorTransferFunction::~ColorTransferFunction()
{

}

/*---------------------------------------------------------------------------*/


void ColorTransferFunction::computeMap(double* map,
                                       vector<double> index,
                                       vector<double> values)
{

   if (map == NULL) return;

   int range = m_totalRange[1] - m_totalRange[0] + 1;

   if (index.size() <= 1 || index.size() != values.size() )
   {
      for (int i=0; i<range; i++) map[i] = 0;
   }
   else
   {
      int prevIndex = 0;

      if (index[0] > 0)
      {
         for (int j=0; j<index[0]; j++) map[j] = values[0];
         prevIndex = index[0];
      }

      for (unsigned int i=1; i<index.size(); i++)
      {
         double slope = (values[i] - values[i-1]) / (index[i] - index[i-1]);

         for (int j=prevIndex; j<=(int)index[i]; j++)
         {
            map[j] = values[i-1] + (j - index[i-1]) * slope;
         }

         prevIndex = index[i] + 1;

      }

      if (index[index.size() - 1] < (range-1))
      {
         for (int j=prevIndex; j<range; j++) map[j] = values[values.size()-1];
      }

   }

}

/*---------------------------------------------------------------------------*/

ColorTransferFunction&
ColorTransferFunction::operator=(const ColorTransferFunction& s)
{


   m_redSubRange[0] = s. m_redSubRange[0];
   m_redSubRange[1] = s. m_redSubRange[1];
   m_greenSubRange[0] = s.m_greenSubRange[0];
   m_greenSubRange[1] = s.m_greenSubRange[1];
   m_blueSubRange[0] = s.m_blueSubRange[0];
   m_blueSubRange[1] = s.m_blueSubRange[1];

   m_totalRange[0] = s.m_totalRange[0];
   m_totalRange[1] = s.m_totalRange[1];

   m_iRed = s.m_iRed;
   m_vRed = s.m_vRed;

   m_iGreen = s.m_iGreen;
   m_vGreen = s.m_vGreen;

   m_iBlue = s.m_iBlue;
   m_vBlue = s.m_vBlue;

   m_dataType = s.m_dataType;

   for (int i=0; i<65536; i++)
   {
      m_redMap[i] = s.m_redMap[i];
      m_greenMap[i] = s.m_greenMap[i];
      m_blueMap[i] = s.m_blueMap[i];
   }

   return *this;

}

/*---------------------------------------------------------------------------*/

int ColorTransferFunction::getDataType()
{

   return m_dataType;

}

/*---------------------------------------------------------------------------*/

double* ColorTransferFunction::getRedMap()
{

   return m_redMap;

}

/*---------------------------------------------------------------------------*/

double* ColorTransferFunction::getGreenMap()
{

   return m_greenMap;

}

/*---------------------------------------------------------------------------*/

double* ColorTransferFunction::getBlueMap()
{

   return m_blueMap;

}

/*---------------------------------------------------------------------------*/

vector<double> ColorTransferFunction::getRedIndex()
{

   return m_iRed;

}

/*---------------------------------------------------------------------------*/

vector<double> ColorTransferFunction::getGreenIndex()
{

   return m_iGreen;

}

/*---------------------------------------------------------------------------*/

vector<double> ColorTransferFunction::getBlueIndex()
{

   return m_iBlue;

}

/*---------------------------------------------------------------------------*/

vector<double> ColorTransferFunction::getRedValue()
{

   return m_vRed;

}

/*---------------------------------------------------------------------------*/

vector<double> ColorTransferFunction::getGreenValue()
{

   return m_vGreen;

}

/*---------------------------------------------------------------------------*/

vector<double> ColorTransferFunction::getBlueValue()
{

   return m_vBlue;

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunction::getRedSubRange(double* min, double* max)
{

   *min = m_redSubRange[0];
   *max = m_redSubRange[1];

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunction::getGreenSubRange(double* min, double* max)
{

   *min = m_greenSubRange[0];
   *max = m_greenSubRange[1];

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunction::getBlueSubRange(double* min, double* max)
{

   *min = m_blueSubRange[0];
   *max = m_blueSubRange[1];

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunction::getTotalRange(double* min, double* max)
{

   *min = m_totalRange[0];
   *max = m_totalRange[1];

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunction::initRange(double min, double max)
{

   m_redSubRange[0] = min;
   m_redSubRange[1] = max;
   m_greenSubRange[0] = min;
   m_greenSubRange[1] = max;
   m_blueSubRange[0] = min;
   m_blueSubRange[1] = max;

   m_totalRange[0] = min;
   m_totalRange[1] = max;


   update();
}

/*---------------------------------------------------------------------------*/

void ColorTransferFunction::setDataType(int dType)
{

   m_dataType = dType;

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunction::setRed(vector<double> i, vector<double> v)
{

   m_iRed = i;
   m_vRed = v;

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunction::setGreen(vector<double> i, vector<double> v)
{

   m_iGreen = i;
   m_vGreen = v;

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunction::setBlue(vector<double> i, vector<double> v)
{

   m_iBlue = i;
   m_vBlue = v;

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunction::setRedSubRange(double min, double max)
{

   m_redSubRange[0] = min;
   m_redSubRange[1] = max;

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunction::setGreenSubRange(double min, double max)
{

   m_greenSubRange[0] = min;
   m_greenSubRange[1] = max;

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunction::setBlueSubRange(double min, double max)
{

   m_blueSubRange[0] = min;
   m_blueSubRange[1] = max;

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunction::setTotalRange(double min, double max)
{

   m_totalRange[0] = min;
   m_totalRange[1] = max;

}

/*---------------------------------------------------------------------------*/

void ColorTransferFunction::update()
{

   computeMap(m_redMap, m_iRed, m_vRed);
   computeMap(m_blueMap, m_iBlue, m_vBlue);
   computeMap(m_greenMap, m_iGreen, m_vGreen);

}

/*---------------------------------------------------------------------------*/
