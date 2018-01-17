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
#include <QTreeView>
#include <QPushButton>
#include <mvc/MapsH5Model.h>
#include <mvc/SpectraWidget.h>
#include "mvc/FitParamsTableModel.h"
#include "mvc/FitElementsTableModel.h"
#include "mvc/ComboBoxDelegate.h"
#include <QComboBox>
#include <QCheckBox>
#include "data_struct/xrf/spectra.h"
#include "data_struct/xrf/fit_element_map.h"

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

   void setModels(MapsH5Model* h5_model,
                  data_struct::xrf::Fit_Parameters* fit_params,
                  data_struct::xrf::Fit_Element_Map_Dict *elements_to_fit);

signals:

   void signal_finished_fit();

   void vertical_element_line_changed(qreal, QString);

public slots:

   void Fit_Spectra_Click();

   void Model_Spectra_Click();

   void finished_fitting();

   void check_auto_model(int state);

   void optimizer_changed(QString val);

   void element_clicked(QModelIndex index);

   void Model_Spectra_Val_Change(QModelIndex,QModelIndex,QVector<int>);

protected:

   SpectraWidget* _spectra_widget;

   FitParamsTableModel* _fit_params_table_model;

   FitElementsTableModel* _fit_elements_table_model;

   data_struct::xrf::Fit_Element_Map_Dict *_elements_to_fit;

   MapsH5Model* _h5_model;

   QTableView* _fit_params_table;

   QTreeView* _fit_elements_table;

   /**
    * @brief Create layout
    */
   void createLayout();

private slots:

private:

    QCheckBox *_chk_auto_model;

    QComboBox *_cb_opttimizer;

   QTabWidget *_fit_params_tab_widget;

   std::thread* _fit_thread;

   QPushButton* _btn_fit_spectra;

   QPushButton* _btn_model_spectra;

   //QAction *_action_check_log10;

   data_struct::xrf::Spectra _spectra_background;

   //QMenu *_contextMenu;
};


/*---------------------------------------------------------------------------*/

#endif /* FitSpectraWidget_H_ */

/*---------------------------------------------------------------------------*/

