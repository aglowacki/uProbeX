/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_COLOR_TRANSFER_FUNCTION_H
#define GSTAR_COLOR_TRANSFER_FUNCTION_H

/*---------------------------------------------------------------------------*/

#include <gstar/Array.h>

#include <vector>

#include <mutex>

/*---------------------------------------------------------------------------*/

using namespace std;

/*---------------------------------------------------------------------------*/

namespace gstar
{

class ColorTransferFunction
{

public:
   /**
    * Constructor
    */
   ColorTransferFunction();

   /**
    * Copy constructor
    */
   ColorTransferFunction(const ColorTransferFunction& s);

   /**
    * Assignment constructor
    */
   ColorTransferFunction& operator=(const ColorTransferFunction& s);

   /**
    * Destructor
    */
   ~ColorTransferFunction();

   /**
    * Get datatype
    */
   int getDataType();

   /**
    * Get read index
    */
   vector<double> getRedIndex();

   /**
    * Get green index
    */
   vector<double> getGreenIndex();

   /**
    * Get blue index
    */
   vector<double> getBlueIndex();

   /**
    * Get red value
    */
   vector<double> getRedValue();

   /**
    * Get green value
    */
   vector<double> getGreenValue();

   /**
    * Get blue value
    */
   vector<double> getBlueValue();

   // TODO : Revisit these maps. exposing internal maps is not a good idea.
   /**
    * Get red map
    */
   double* getRedMap();

   /**
    * Get blue map
    */
   double* getBlueMap();

   /**
    * Get green map
    */
   double* getGreenMap();

   /**
    * Get red sub range
    */
   void getRedSubRange(double* min, double* max);

   /**
    * Get green sub range
    */
   void getGreenSubRange(double* min, double* max);

   /**
    * Get blue sub range
    */
   void getBlueSubRange(double* min, double* max);

   /**
    * Get total range
    */
   void getTotalRange(double* min, double* max);

   /**
    * Init total range
    */
   void initRange(double min, double max);

   /**
    * Set datatype
    */
   void setDataType(int dType);

   /**
    * Set red
    */
   void setRed(vector<double> i, vector<double> v);

   /**
    * Set green
    */
   void setGreen(vector<double> i, vector<double> v);

   /**
    * Set blue
    */
   void setBlue(vector<double> i, vector<double> v);

   /**
    * Set red sub range
    */
   void setRedSubRange(double min, double max);

   /**
    * Set green sub range
    */
   void setGreenSubRange(double min, double max);

   /**
    * Set blue sub range
    */
   void setBlueSubRange(double min, double max);

   /**
    * Set total range
    */
   void setTotalRange(double min, double max);

   /**
    * Update
    */
   void update();

private:
   /**
    * Compute map
    */
   void computeMap(double* map,
                   vector<double> index,
                   vector<double> values);

private:

   /**
    * DataType : Char (8bit), Short (16 bit), or Short12 (12 bit)
    */
   int m_dataType;

   /**
    * Red sub range
    */
   double m_redSubRange[2];

   /**
    * Green sub range
    */
   double m_greenSubRange[2];

   /**
    * Blue sub range
    */
   double m_blueSubRange[2];

   /**
    * Total range
    */
   double m_totalRange[2];

   /**
    * Ired
    */
   vector<double> m_iRed;

   /**
    * Vred
    */
   vector<double> m_vRed;

   /**
    * IGred
    */
   vector<double> m_iGreen;
   
   /**
    * Vgreen
    */
   vector<double> m_vGreen;

   /**
    * Iblue
    */
   vector<double> m_iBlue;

   /**
    * Vblue
    */
   vector<double> m_vBlue;

   /**
    * Mutex color map
    */
   std::mutex m_mutexColorMap;

   //TODO: remove the hard-coded numbers
   /**
    * Red map
    */
   double m_redMap[65537];

   /**
    * Green map
    */
   double m_greenMap[65537];

   /**
    * Blue map
    */
   double m_blueMap[65537];

};

}

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
