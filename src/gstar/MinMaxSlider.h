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

   bool getUserMinMax(float &minCoef, float &maxCoef, float &minPerc, float &maxPerc);

signals:
    void min_max_val_changed(bool);

public slots:

   void min_slider_changed();

   void max_slider_changed();

   void min_slider_value_changed(int);

   void max_slider_value_changed(int);

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

    float _diff_over_128;

    bool _round_val;
   };

}

//---------------------------------------------------------------------------


#endif /* RANGEWIDGET_H_ */

