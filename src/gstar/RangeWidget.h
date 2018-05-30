/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef RANGEWIDGET_H_
#define RANGEWIDGET_H_

/*---------------------------------------------------------------------------*/

#include <QHBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

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

   class RangeWidget: public QWidget
   {

   Q_OBJECT

  public:

   /**
    * Constructs a range widget with 0 as minimum value and 1000 as maximum
    * value. It is parented to parent.
    *
    * @param the widget to which this widget is parented.
    */
   RangeWidget(QWidget* parent = 0);

   /**
    * Default destructor.
    */
   ~RangeWidget();

   /**
    * Set an arbitrary integer id value for the range widget. This value is
    * passed as id by the valueChanged() signal. The default value is 0.
    *
    * @param id integer id value for the range widget.
    */
   void setId(int id);

   /**
    * Set the maximum end of the range.
    *
    * @param max the new maximum end of the range.
    */
   void setMaximum(int max);

   /**
    * Set the minimum end of the range.
    *
    * @param min the new minimum end of the range.
    */
   void setMinimum(int min);

   /**
    * Set the current value of the range.
    *
    * @param v the new value.
    */
   void setValue(int v);

   /**
    * Return the current value.
    *
    * @return the current value.
    */
   int value();

   /**
    * Change the Rangewidget enable status.
    *
    * @return the current value.
   */
   void setRangewidgetStatus(bool m_rangeEnable = false);

   /**
    * Change the Rangewidget visible status.
    *
    * @return the current value.
   */
   void setRangeWidgetVisible(bool m_rangeVisible = false);

   signals:

   /**
    * This signal is emitted whenever the range widget's value is changed.
    * The new value is passed in value. The id associated with the widget is
    * passed in id.
    *
    * @param value the widget's new value.
    * @param id the widget's assigned id.
    */
   void valueChanged(int value, int id);

   private slots:

   /**
    * Slot to handle the first button's clicked() signal.
    */
   void clickFirst();

   /**
    * Slot to handle the previous button's clicked() signal.
    */
   void clickPrevious();

   /**
    * Slot to handle the next button's clicked() signal.
    */
   void clickNext();

   /**
    * Slot to handle the last button's clicked() signal.
    */
   void clickLast();

   /**
    * Slot to handle the spin box's valueChanged() signal.
    */
   void spinChanged(int value);

   private:

   /**
    * The first button. This button sets the value to the minimum value.
    */
   QPushButton* m_btnFirst;

   /**
    * The previous button. This button decrements the value by 1.
    */
   QPushButton* m_btnPrevious;

   /**
    * The spin box used for display of the current value and manual input.
    */
   QSpinBox* m_spinValue;

   /**
    * The next button. This button increments the value by 1.
    */
   QPushButton* m_btnNext;

   /**
    * The last button. This button sets the value to the maximum value.
    */
   QPushButton* m_btnLast;

   /**
    * The id of this widget.
    */
   int m_id;

   };

}

/*---------------------------------------------------------------------------*/


#endif /* RANGEWIDGET_H_ */
