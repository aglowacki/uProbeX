/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef FIT_SPECTRA_WIDGET_H
#define FIT_SPECTRA_WIDGET_H

/*---------------------------------------------------------------------------*/


//#include <QAction>
//#include <QMenu>
#include <thread>
#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <mvc/MapsH5Model.h>
#include <mvc/SpectraWidget.h>
#include "mvc/FitParamsTableModel.h"
#include "mvc/ComboBoxDelegate.h"
#include "data_struct/xrf/spectra.h"

/*---------------------------------------------------------------------------*/

/**
 * @brief When open the acquisition window, the widget is showing for capturing
 * the image from the area detector writer, the window will also be updated to
 * show the image.
 */
class FitSpectraWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   FitSpectraWidget(QWidget* parent = NULL);

   /**
    * Destructor.
    */
   ~FitSpectraWidget();

   void setModels(data_struct::xrf::Fit_Parameters* fit_params,
                  data_struct::xrf::Fit_Element_Map_Dict *elements_to_fit,
                  MapsH5Model* h5_model);

signals:

   void signal_finished_fit();

public slots:

   void Fit_Spectra_Click();

   void finished_fitting();

protected:

   SpectraWidget* _spectra_widget;

   FitParamsTableModel* _fit_params_table_model;

   data_struct::xrf::Fit_Parameters* _fit_params;

   data_struct::xrf::Fit_Element_Map_Dict *_elements_to_fit;

   MapsH5Model* _h5_model;

   QTableView* _fit_params_table;

   /**
    * @brief Create layout
    */
   void createLayout();

private slots:

private:

   std::thread* _fit_thread;

   QPushButton* _btn_fit_spectra;
   //QAction *_action_check_log10;

   data_struct::xrf::Spectra _spectra_background;

   //QMenu *_contextMenu;
};


/*---------------------------------------------------------------------------*/

#endif /* FitSpectraWidget_H_ */

/*---------------------------------------------------------------------------*/

