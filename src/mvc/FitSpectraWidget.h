/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef FIT_SPECTRA_WIDGET_H
#define FIT_SPECTRA_WIDGET_H

//---------------------------------------------------------------------------


#include <QWidget>
#include <QTableView>
#include <QComboBox>
#include <QTreeView>
#include <QPushButton>
#include <QDockWidget>
#include <QCheckBox>
#include <algorithm>
#include <mvc/MapsH5Model.h>
#include <mvc/SpectraWidget.h>
#include "mvc/FitParamsTableModel.h"
#include "mvc/FitElementsTableModel.h"
#include "mvc/FittingDialog.h"
#include "data_struct/spectra.h"
#include "data_struct/fit_element_map.h"
#include "data_struct/params_override.h"
#include "mvc/SpectraWidgetSettingsDialog.h"
#include "mvc/PeriodicTableWidget.h"
#include "mvc/ElementInfoDialog.h"
#include "mvc/CustomPeakDialog.h"

//---------------------------------------------------------------------------

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

   void setIntegratedSpectra(ArrayDr* int_spec);

   void setParamOverride(data_struct::Params_Override<double>* po);

   void setElementDetector(std::string e) {_detector_element = e;}

   void setFitParams(data_struct::Fit_Parameters<double>* fit_params);

   void setElementsToFit(data_struct::Fit_Element_Map_Dict<double>* elements_to_fit);

   void clearAllSpectra() { _spectra_widget->clearAllSpectra(); }

   void appendFitIntSpectra(std::string, ArrayDr*);

   void appendMaxChanSpectra(std::string name, const ArrayDr* spec);

   void appendROISpectra(std::string name, ArrayDr* spec, QColor color);

   void deleteROISpectra(std::string name);

   void deleteAllROISpectra();

   void clearFitIntSpectra();
   
   void clearMaxChanSpectra();

   void clearROISpectra();

   void displayROIs(bool val);

   void setFitRoiEnabled(bool val) { _btn_fit_roi_spectra->setEnabled(val); }

   void setDatasetDir(QDir dir) { _dataset_dir = dir; }
signals:

   void signal_finished_fit();

   void vertical_element_line_changed(qreal, QString);

   void export_fit_paramters(data_struct::Fit_Parameters<double> fit_params, data_struct::Fit_Element_Map_Dict<double> elements_to_fit);
                                                    //ev                           int spec                  back                 fit              detailed fit
   void export_csv_and_png(QPixmap, ArrayDr *, ArrayDr*, ArrayDr*, ArrayDr*, std::unordered_map<std::string, ArrayDr> *);

public slots:

   void Fit_Spectra_Click();

   void Fit_ROI_Spectra_Click();

   void Model_Spectra_Click();

   void finished_fitting();

   void check_auto_model(int state);

   void optimizer_preset_changed(int idx);

//   void element_clicked(QModelIndex index);

   void Model_Spectra_Val_Change(QModelIndex,QModelIndex,QVector<int>);

   void element_selection_changed_from(QModelIndex,QModelIndex);

   void element_selection_changed(int index);

   void replot_integrated_spectra(bool snipback);

   void replot_integrated_spectra_with_background() { replot_integrated_spectra(true); }

   void add_element();

   void del_element();

   void add_custom_peak_pressed();

   void onSettingsDialog();

   void on_export_csv();

   void on_braching_ratio_update(data_struct::Fit_Element_Map<double>* element);
   
   void on_width_multi_changed(data_struct::Fit_Element_Map<double>* element, const QString& shell_name);

protected:

   SpectraWidget* _spectra_widget;

   QDockWidget* _spectra_dock;
   
   FitParamsTableModel* _fit_params_table_model;

   FitElementsTableModel* _fit_elements_table_model;

   data_struct::Fit_Element_Map_Dict<double>* _elements_to_fit;

   //MapsH5Model* _h5_model;

   QTableView* _fit_params_table;

   QTreeView* _fit_elements_table;

   FittingDialog* _fitting_dialog;

   /**
    * @brief Create layout
    */
   void createLayout();

   void update_spectra_top_axis();

private slots:

   void fit_params_customMenuRequested(QPoint);

   void set_fit_params_bounds_fixed(bool v) {set_fit_params_bounds(data_struct::E_Bound_Type::FIXED);}

   void set_fit_params_bounds_fit(bool v) {set_fit_params_bounds(data_struct::E_Bound_Type::FIT);}

   void set_fit_params_bounds_limited_lo_hi(bool v) { set_fit_params_bounds(data_struct::E_Bound_Type::LIMITED_LO_HI); }

   void set_fit_params_bounds_limited_lo(bool v) { set_fit_params_bounds(data_struct::E_Bound_Type::LIMITED_LO); }

   void set_fit_params_bounds_limited_hi(bool v) { set_fit_params_bounds(data_struct::E_Bound_Type::LIMITED_HI); }

   void show_load_fit_params_dialog(bool);

   void set_fit_params_bounds(data_struct::E_Bound_Type e_type);

   void pileup_chk_changed(int state);

   void on_export_fit_paramters();

   void display_periodic_table();

   void display_element_info();

   void update_selected_element_to_add(QString);

private:

   QCheckBox *_chk_auto_model;

   QComboBox *_cb_fitting_preset;

   QTabWidget *_fit_params_tab_widget;

   QPushButton* _btn_fit_spectra;

   QPushButton* _btn_fit_roi_spectra;

   QPushButton* _btn_model_spectra;

   QPushButton* _btn_export_parameters;

   QPushButton* _btn_export_csv;

   QPushButton* _btn_add_element;

   QPushButton* _btn_del_element;

   QPushButton* _btn_add_custom_peak;

   ArrayDr* _int_spec;

   ArrayDr _spectra_background;

   ArrayDr _ev;

   data_struct::Spectra<double> _fit_spec;

   data_struct::Params_Override<double>* _param_override;

   std::unordered_map<std::string, ArrayDr*> _fit_int_spec_map;

   std::unordered_map<std::string, const ArrayDr*> _max_chan_spec_map;

   std::map<std::string, ArrayDr*> _roi_spec_map;

   std::unordered_map<std::string, QColor> _roi_spec_colors;

   std::string _detector_element;

   QMenu *_fit_param_contextMenu;

   QMenu *_set_fit_params_bounds_menu;

   QComboBox* _cb_add_elements;

   QComboBox* _cb_add_shell;

   QComboBox* _cb_pileup_elements;

   QCheckBox* _chk_is_pileup;

   QComboBox* _cb_detector_element;

   bool _showDetailedFitSpec;

	bool _showFitIntMatrix;

   bool _showFitIntNNLS;

   bool _showMaxChanSpec;

   bool _displayROIs;

   std::unordered_map<std::string, ArrayDr> _labeled_spectras;

   QDir _dataset_dir;

   QPushButton* _btn_periodic_table;

   QPushButton* _btn_element_info;

   PeriodicTableWidget* _periodic_table_widget;

   ElementInfoDialog _element_info_dialog;

   CustomPeakDialog _custon_peak_dialog;
};


//---------------------------------------------------------------------------

#endif /* FitSpectraWidget_H_ */

//---------------------------------------------------------------------------

