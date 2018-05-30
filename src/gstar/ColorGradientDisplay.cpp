/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/ColorGradientDisplay.h>

using namespace gstar;

/*---------------------------------------------------------------------------*/

ColorGradientDisplay::ColorGradientDisplay(QWidget* widget)
   : QWidget(widget)
{

   setMaximumHeight(25);

}

/*---------------------------------------------------------------------------*/

void ColorGradientDisplay::paintEvent(QPaintEvent* event)
{

   Q_UNUSED(event)

   double r, g, b;
   double min, max;

   QPainter p(this);

   QImage sim(size(), QImage::Format_RGB32);
   QLinearGradient sg(0, 0, width(), 0);

   m_ctf.getTotalRange(&min, &max);
   double* redMap = m_ctf.getRedMap();
   double* blueMap = m_ctf.getBlueMap();
   double* greenMap = m_ctf.getGreenMap();

   int colorDist= max/255;

   for (int i = min ; i < max ; i += colorDist) {
      r = redMap[i];
      g = greenMap[i];
      b = blueMap[i];
      sg.setColorAt((i - min) / (max - min), QColor(r, g, b));
   }

   QPainter pim(&sim);
   pim.fillRect(rect(), sg);

   p.drawImage(rect(), sim, sim.rect());

   p.setPen(QColor(128, 128, 128));
   p.drawRect(0, 0, width() - 1, height() - 1);

}

/*---------------------------------------------------------------------------*/

void ColorGradientDisplay::setColorTransferFunction(ColorTransferFunction ctf)
{

   m_ctf = ctf;

   update();

}

/*---------------------------------------------------------------------------*/
