/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ContrastDialog_H_
#define ContrastDialog_H_

//---------------------------------------------------------------------------

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialog>
#include "gstar/HistogramPlot.h"
#include "gstar/MinMaxSlider.h"
#include "data_struct/fit_parameters.h"
//---------------------------------------------------------------------------

namespace gstar
{

   class ContrastDialog: public QDialog
   {

   Q_OBJECT

public:

    ContrastDialog(QWidget* parent = 0);

    ~ContrastDialog();
    
    void set_array(const data_struct::ArrayXXr<float>* arr);

signals:
	void on_min_max_update(float min, float max, bool send_draw_event);

public slots:
	void min_max_updated();

	void on_accepted();

	void on_rejected();

private:
    HistogramPlot* _historgram;

    QWidget* _historgram_widget;

    MinMaxSlider* _min_max_slider;
    
	QPushButton* _btn_accept;

	QPushButton* _btn_cancel;

    const data_struct::ArrayXXr<float>* _arr;

   };

}

//---------------------------------------------------------------------------


#endif /* _H_ */

