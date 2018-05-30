/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_COLOR_TRANSFER_FUNCTION_EDITOR_H
#define GSTAR_COLOR_TRANSFER_FUNCTION_EDITOR_H

/*---------------------------------------------------------------------------*/

#include <QGridLayout>
#include <QWidget>

#include "gstar/ColorGradientDisplay.h"
#include "gstar/ControlPointWidget.h"

#include <QPushButton>

/*---------------------------------------------------------------------------*/

namespace gstar
{

class ColorTransferFunctionEditor
: public QWidget
{

   Q_OBJECT

public:
   /**
    * Constructor
    */
   ColorTransferFunctionEditor(QWidget* widget = 0);

   /**
    * Get the color transfer function
    */
   ColorTransferFunction getColorTransferFunction();

   /**
    * Set color transfer function
    */
   void setColorFunction(ColorTransferFunction ctf);

   /**
    * Set sub range
    */
   void setSubRange(double min, double max);

   /**
    * Set total range
    */
   void setTotalRange(double min, double max);

   void setCTFTotalRange(double min, double max);
   /**
    * Update histogram
    */
   void updateHistogram(QList<int> points);

signals:
   /**
    * Request auto levels
    */
   void requestAutoLevels();

   /**
    * Request predefined color
    */
   void requestPredefinedColor(QString);

   /**
    * Update transfer function
    */
   void transferFunctionUpdated(gstar::ColorTransferFunction ctf);

public slots:
   /**
    * Red range changed
    */
   void redRangeChanged(double min, double max);

   /**
    * Green range changed
    */
   void greenRangeChanged(double min, double max);

   /**
    * Blue range changed
    */
   void blueRangeChanged(double min, double max);


   /**
    * Predefined color clicked
    */
   void predefinedColorClicked();

   /**
    * Points changed
    */
   void pointsChanged();

private:
   /**
    * App path
    */
   QString m_appPath;

   /**
    * Red
    */
   ControlPointWidget* m_red;

   /**
    * Green
    */
   ControlPointWidget* m_green;

   /**
    * Blue
    */
   ControlPointWidget* m_blue;

   /**
    * Display
    */
   ColorGradientDisplay* m_display;

   /**
    * Btn lock
    */
   QPushButton* m_btnLock;

   /**
    * Btn auto
    */
   QPushButton* m_btnAuto;

   /**
    * Btn color red
    */
   QPushButton* m_btnColorRed;

   /**
    * Btn color green
    */
   QPushButton* m_btnColorGreen;

   /**
    * Btn color blue
    */
   QPushButton* m_btnColorBlue;

   /**
    * Btn color hot
    */
   QPushButton* m_btnColorHot;

   /**
    * Btn color gray
    */
   QPushButton* m_btnColorGray;

   /**
    * Btn color ice
    */
   QPushButton* m_btnColorIce;

   /**
    * CTF
    */

   ColorTransferFunction m_ctf;

   /**
    * Total range
    */
   //double m_totalRange[2];

};

}

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
