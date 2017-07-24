/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <proc/Histogram.h>

#include <QtCore/qmath.h>

#include <QDebug>

/*---------------------------------------------------------------------------*/

Histogram::Histogram(int bins, int max)
: m_nbins(bins),m_max(max)
{

   //m_nbins = bins;
   m_min = 0.0;
   //m_max = 255.0;
   m_binsize = m_max / (qreal) m_nbins;
   m_bins = new int[m_nbins];

   reset();

}

/*---------------------------------------------------------------------------*/

Histogram::~Histogram()
{

   if (m_bins != NULL)
      delete m_bins;

}

/*---------------------------------------------------------------------------*/

void Histogram::addIntensity(qreal val)
{

   int bin = qCeil(val / m_binsize) - 1;
   if (bin < 0) bin = 0;

   if (bin >= 0 && bin < m_nbins)
   {
      m_bins[bin]++;
   }

}

/*---------------------------------------------------------------------------*/

int Histogram::getBins()
{

   return m_nbins;

}

/*---------------------------------------------------------------------------*/


QList<int> Histogram::getCounts()
{

   QList<int> counts;

   for (int i=0; i<m_nbins; i++)
   {
      counts.push_back(m_bins[i]);
   }

   return counts;

}

/*---------------------------------------------------------------------------*/

int Histogram::getMaxBinNumber()
{

   int i;
   for(i = m_nbins-1; i >= 0; i--)
   {
      if(m_bins[i] > 0)
         break;
   }
   return ((m_max * i) / m_nbins);

}

/*---------------------------------------------------------------------------*/

int Histogram::getMinBinNumber()
{

   int i;
   for(i = 0; i < m_nbins; i++)
   {
      if(m_bins[i] > 0)
         break;
   }
   return ((m_max * i) / m_nbins);

}

/*---------------------------------------------------------------------------*/


void Histogram::reset()
{

   for (int i=0; i<m_nbins; i++) m_bins[i] = 0;

}

/*---------------------------------------------------------------------------*/

void Histogram::setBins(int bins)
{

   m_nbins = bins;
   if (m_bins != NULL)
   {
      delete m_bins;
      m_bins = new int[m_nbins];
   }

}

/*---------------------------------------------------------------------------*/
