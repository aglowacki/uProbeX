/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

/*---------------------------------------------------------------------------*/

#include <QList>

/*---------------------------------------------------------------------------*/

/**
 * @brief Computes histogram from image intensities. The addIntensity() method 
 *        updates the count in appropriate bin number. The bin number is 
 *        computed based on the total number of bins provided at the time of
 *        construction. The Histogram class can be re-used with same bin and
 *        max value after calling reset() method.
 */
class Histogram
{
public:

   /**
    * Compute image histogram with give number of bins.
    *
    * @param bins - Total number of bins
    * @param max - Maximum pixel intensity in the image.
    */
   Histogram(int bins, int max = 255);

   /**
    * Destructor
    */
   ~Histogram();

   /**
    * Compute bin number for each pixel intensity value.
    */
   void addIntensity(qreal val);

   /**
    * Get bin number.
    */
   int getBins();

   /**
    * Get the histogram, the index is the bin number and the value is the count.
    */
   QList<int> getCounts();

   /**
    * @brief return max
    * @return
    */
   int getMaxBinNumber();

   /**
    * @brief return min
    * @return
    */
   int getMinBinNumber();

   /**
    * Re-initialize the count, set all of them to zero.
    */
   void reset();

   /**
    * Set bin number.
    */
   void setBins(int bins);

private:

   /**
    * Size (range) of each bin
    */
   qreal m_binsize;

   /**
    * Total number of bins.
    */
   int m_nbins;

   /**
    * Hold count for each bin
    */
   int* m_bins;

   /**
    * Minimum pixel intensity value
    */
   qreal m_min;

   /**
    * Maximum pixel intensity value.
    */
   qreal m_max;

};


#endif
