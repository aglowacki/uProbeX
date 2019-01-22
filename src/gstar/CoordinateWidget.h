/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef COORDINATEWIDGET_H_
#define COORDINATEWIDGET_H_

/*---------------------------------------------------------------------------*/

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include "gstar/CoordinateModel.h"

/*---------------------------------------------------------------------------*/

/**
 * @brief The RangeWidget class provides a Qt range widget, similar to a
 * spin box.
 *
 * RangeWidget is designed to handle integer values.
 *
 * RangeWidget allows the user to choose a value by clicking the previous/next
 * buttons, or the first/last buttons. The user can also type the value in
 * manually.
 *
 * Every time the value changes RangeWidget emits the valueChanged() signal.
 */

namespace gstar
{

   class CoordinateWidget: public QWidget
   {

   Q_OBJECT

public:

   /**
    * Constructs a range widget with 0 as minimum value and 1000 as maximum
    * value. It is parented to parent.
    *
    * @param the widget to which this widget is parented.
    */
   CoordinateWidget(QWidget* parent = 0);

   /**
    * Default destructor.
    */
   ~CoordinateWidget();

   /**
    * Set the current value of the range.
    *
    * @param x - x coordinate.
    * @param y - y coordinate.
    */
   void setCoordinate(int x, int y);

   /**
    * @brief setCoordinate
    * @param x
    * @param y
    * @param z
    */
   void setCoordinate(double x, double y, double z);

   /**
    * @brief setLabel
    * @param xLabel
    * @param yLabel
    */
   void setLabel(QString xLabel, QString yLabel);

   /**
    * @brief setLabel
    * @param xLabel
    * @param yLabel
    * @param zLabel
    */
   void setLabel(QString xLabel, QString yLabel, QString zLabel);

   /**
    * @brief setUnitsLabel
    * @param units
    */
   void setUnitsLabel(QString units);

   /**
    * sets the model
    */
   void setModel(CoordinateModel* model);

   /**
    * @brief setnullptr
    */
   void setnullptr();

private:

   /**
    * coordinate model
    */
   CoordinateModel* m_model;

   /**
    * @brief m_lblPixelXLabel
    */
   QLabel* m_lblPixelXLabel;

   /**
    * @brief m_lblPixelYLabel
    */
   QLabel* m_lblPixelYLabel;

   /**
    * @brief m_lblPixelYLabel
    */
   QLabel* m_lblPixelZLabel;

   /**
    * Label for position X coordinate value
    */
   QLabel* m_lblPixelXCoordinate;

   /**
    * Label for position Y coordinate value
    */
   QLabel* m_lblPixelYCoordinate;

   /**
    * Label for position Z coordinate value
    */
   QLabel* m_lblPixelZCoordinate;

   /**
    * @brief m_lblXUnits
    */
   QLabel* m_lblXUnits;

   /**
    * @brief m_lblYUnits
    */
   QLabel* m_lblYUnits;

   /**
    * @brief m_lblZUnits
    */
   QLabel* m_lblZUnits;

   /**
    * @brief m_decimalPreci
    */
   //int m_decimalPreci;

   };

}

/*---------------------------------------------------------------------------*/


#endif /* RANGEWIDGET_H_ */
