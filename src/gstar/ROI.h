/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_ROI_H
#define GSTAR_ROI_H

/*---------------------------------------------------------------------------*/

#include <QString>

/*---------------------------------------------------------------------------*/

namespace gstar
{
/**
 * @brief ROI properties
 *
 * The ROI class is an informational class with ROI properties.
 */
class ROI
{

public:

   /**
    * Constructor
    */
   ROI();

   /**
    * Constructor
    *
    * @param x - x position
    * @param y - y position
    * @param w - width
    * @param h - height
    * @param color - color name
    * @param text - description
    */
   ROI(double x, double y, double w, double h, QString color, QString text);

   /**
    * Get color name
    *
    * @return color name
    */
   QString getColor();

   /**
    * Get height
    *
    * @return height
    */
   double getHeight();

   /**
    * Get text
    *
    * @return descriptive text
    */
   QString getText();

   /**
    * Get width
    *
    * @return width
    */
   double getWidth();

   /**
    * Get x position
    *
    * @return x position
    */
   double getX();

   /**
    * Get y position
    *
    * @return y position
    */
   double getY();

   /**
    * Set color
    *
    * @param color - color name
    */
   void setColor(QString color);

   /**
    * Set height
    *
    * @param h - height
    */
   void setHeight(double h);

   /**
    * Set descriptive text
    *
    * @param text - descriptive text
    */
   void setText(QString text);

   /**
    * Set width
    *
    * @param w - width
    */
   void setWidth(double w);

   /**
    * Set x position
    *
    * @param x - x position
    */
   void setX(double x);

   /**
    * Set y position
    *
    * @param y - y position
    */
   void setY(double y);

private:

   /**
    * X position
    */
   double m_x;

   /**
    * Y position
    */
   double m_y;

   /**
    * Width
    */
   double m_w;

   /**
    * Height
    */
   double m_h;

   /**
    * Color
    */
   QString m_color;

   /**
    * Descriptive text
    */
   QString m_text;

};

}

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
