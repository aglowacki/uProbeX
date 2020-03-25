﻿/*-----------------------------------------------------------------------------
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
#include <QComboBox>
#include <QTreeView>
#include <QPushButton>
#include <mvc/MapsH5Model.h>
#include <mvc/SpectraWidget.h>
#include "mvc/FitParamsTableModel.h"
#include "mvc/FitElementsTableModel.h"
#include "mvc/ComboBoxDelegate.h"
#include <QComboBox>
#include <QCheckBox>
#include "data_struct/spectra.h"
#include "data_struct/fit_element_map.h"
#include "data_struct/fit_parameters.h"

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
   FitSpectraWidget(QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~FitSpectraWidget();

   void setH5Model(MapsH5Model* h5_model);

   void setFitParams(data_struct::Fit_Parameters* fit_params);

   void setElementsToFit(data_struct::Fit_Element_Map_Dict *elements_to_fit);

signals:

   void signal_finished_fit();

   void vertical_element_line_changed(qreal, QString);

public slots:

   void Fit_Spectra_Click();

   void Model_Spectra_Click();

   void finished_fitting();

   void check_auto_model(int state);

   void optimizer_changed(QString val);

//   void element_clicked(QModelIndex index);

   void Model_Spectra_Val_Change(QModelIndex,QModelIndex,QVector<int>);

   void element_selection_changed(QModelIndex,QModelIndex);

   void element_selection_changed(int index);

   void export_fit_paramters();

   void replot_integrated_spectra();

   void add_element();

   void del_element();

protected:

   SpectraWidget* _spectra_widget;

   FitParamsTableModel* _fit_params_table_model;

   FitElementsTableModel* _fit_elements_table_model;

   data_struct::Fit_Element_Map_Dict *_elements_to_fit;

   MapsH5Model* _h5_model;

   QTableView* _fit_params_table;

   QTreeView* _fit_elements_table;

   /**
    * @brief Create layout
    */
   void createLayout();

   void update_spectra_top_axis(std::vector<std::string> element_names);

private slots:

   void fit_params_customMenuRequested(QPoint);

   void set_fit_params_bounds_fixed(bool v) {set_fit_params_bounds(data_struct::E_Bound_Type::FIXED);}

   void set_fit_params_bounds_fit(bool v) {set_fit_params_bounds(data_struct::E_Bound_Type::FIT);}

   void set_fit_params_bounds(data_struct::E_Bound_Type e_type);

   void h5_int_spec_updated(bool b_snip_background);

   void pileup_chk_changed(int state);

private:

    QCheckBox *_chk_auto_model;

    QComboBox *_cb_opttimizer;

   QTabWidget *_fit_params_tab_widget;

   std::thread* _fit_thread;

   QPushButton* _btn_fit_spectra;

   QPushButton* _btn_model_spectra;

   QPushButton* _btn_export_parameters;

   QPushButton* _btn_replot_integrated;

   QPushButton* _btn_add_element;

   QPushButton* _btn_del_element;

   data_struct::ArrayXr _spectra_background;

   QMenu *_fit_param_contextMenu;

   QMenu *_set_fit_params_bounds_menu;

   QComboBox* _cb_add_elements;

   QComboBox* _cb_add_shell;

   QComboBox* _cb_pileup_elements;

   QCheckBox* _chk_is_pileup;

   QComboBox* _cb_detector_element;
};


/*---------------------------------------------------------------------------*/

#endif /* FitSpectraWidget_H_ */

/*---------------------------------------------------------------------------*/

