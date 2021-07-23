/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MinMaxSlider_H_
#define MinMaxSlider_H_

/*---------------------------------------------------------------------------*/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QLineEdit>
#include <QSlider>

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

   class MinMaxSlider: public QWidget
   {

   Q_OBJECT

public:

    MinMaxSlider(QWidget* parent = 0);

   ~MinMaxSlider();

   void setMin(float min);

   void setMax(float max);

   void setVal(float val);

signals:
    void min_val_changed(int);

    void max_val_changed(int);

public slots:

   void min_slider_changed(int);

   void max_slider_changed(int);

   void min_lineedit_changed(QString);

   void max_lineedit_changed(QString);

private:
    QLineEdit* _min_lineedit;

    QLineEdit* _max_lineedit;
   
    QSlider* _min_slider;

    QSlider* _max_slider;

    int _min_val;

    int _max_val;

   };

}

/*---------------------------------------------------------------------------*/


#endif /* RANGEWIDGET_H_ */

