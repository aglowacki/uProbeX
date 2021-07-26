/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ContrastDialog_H_
#define ContrastDialog_H_

/*---------------------------------------------------------------------------*/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialog>
#include "gstar/HistogramPlot.h"
#include "gstar/MinMaxSlider.h"
#include "data_struct/fit_parameters.h"
/*---------------------------------------------------------------------------*/

namespace gstar
{

   class ContrastDialog: public QDialog
   {

   Q_OBJECT

public:

    ContrastDialog(const data_struct::ArrayXXr* arr , QWidget* parent = 0);

    ~ContrastDialog();

//signals:


private:
    HistogramPlot* _historgram;

    QWidget* _historgram_widget;

    MinMaxSlider* _min_max_slider;
    
    const data_struct::ArrayXXr* _arr;

   };

}

/*---------------------------------------------------------------------------*/


#endif /* RANGEWIDGET_H_ */

