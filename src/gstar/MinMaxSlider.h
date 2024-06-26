/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MinMaxSlider_H_
#define MinMaxSlider_H_

//---------------------------------------------------------------------------

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QLineEdit>
#include <QSlider>

//---------------------------------------------------------------------------

namespace gstar
{

   class MinMaxSlider: public QWidget
   {

   Q_OBJECT

public:

    MinMaxSlider(bool is_glob=true, QWidget* parent = 0);

   ~MinMaxSlider();

   void setMinMax(float min, float max);

   float getUserMin();

   float getUserMax();

signals:
    void min_max_val_changed();

public slots:

   void min_slider_changed(int);

   void max_slider_changed(int);

   void min_lineedit_changed();

   void max_lineedit_changed();

private:
    QLabel* _min_label;

    QLabel* _max_label;

    QLineEdit* _min_lineedit;

    QLineEdit* _max_lineedit;
   
    QSlider* _min_slider;

    QSlider* _max_slider;

    qreal _min_val;

    qreal _max_val;

   };

}

//---------------------------------------------------------------------------


#endif /* RANGEWIDGET_H_ */

