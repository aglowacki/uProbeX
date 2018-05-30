/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_COLOR_GRADIENT_DISPLAY_H
#define GSTAR_COLOR_GRADIENT_DISPLAY_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QPainter>

#include <gstar/ColorTransferFunction.h>

/*---------------------------------------------------------------------------*/

namespace gstar
{

class ColorGradientDisplay
: public QWidget
{

   Q_OBJECT

public:
   /**
    * Constructor
    */
   ColorGradientDisplay(QWidget* widget = 0);

   /**
    * Paint the event
    */
   void paintEvent(QPaintEvent* event);

   /**
    * Set the color transfer function
    */
   void setColorTransferFunction(ColorTransferFunction ctf);

private:
   /**
    * Color transfer function
    */
   ColorTransferFunction m_ctf;

};

}

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
