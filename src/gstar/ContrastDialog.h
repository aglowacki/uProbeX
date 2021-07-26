/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ContrastDialog_H_
#define ContrastDialog_H_

/*---------------------------------------------------------------------------*/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include "gstar/HistogramPlot.h"
#include "gstar/MinMaxSlider.h"
/*---------------------------------------------------------------------------*/

namespace gstar
{

   class ContrastDialog: public QDialog
   {

   Q_OBJECT

public:

    ContrastDialog(QWidget* parent = 0);

   ~ContrastDialog();

private:
    HistogramPlot* _historgram;

    QWidget* _historgram_widget;

    MinMaxSlider* _min_max_slider;
    

   };

}

/*---------------------------------------------------------------------------*/


#endif /* RANGEWIDGET_H_ */

