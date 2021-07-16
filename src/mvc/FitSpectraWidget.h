/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef FIT_SPECTRA_WIDGET_H
#define FIT_SPECTRA_WIDGET_H

/*---------------------------------------------------------------------------*/


#include <QWidget>
#include <QTableView>
#include <QComboBox>
#include <QTreeView>
#include <QPushButton>
#include <mutex>
#include <mvc/MapsH5Model.h>
#include <mvc/SpectraWidget.h>
#include "mvc/FitParamsTableModel.h"
#include "mvc/FitElementsTableModel.h"
#include "mvc/FittingDialog.h"
#include <QCheckBox>
#include "data_struct/spectra.h"
#include "data_struct/fit_element_map.h"
#include "data_struct/params_override.h"
#include "mvc/SpectraWidgetSettingsDialog.h"

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

   void setIntegratedSpectra(data_struct::ArrayXr* int_spec);

   void setParamOverride(data_struct::Params_Override* po);

   void setElementDetector(std::string e) {_detector_element = e;}

   void setFitParams(data_struct::Fit_Parameters* fit_params);

   void setElementsToFit(data_struct::Fit_Element_Map_Dict *elements_to_fit);

   void clearAllSpectra() { _spectra_widget->clearAllSpectra(); }

   void appendFitIntSpectra(string, data_struct::ArrayXr*);

   void appendROISpectra(string, data_struct::ArrayXr*);

   void clearFitIntSpectra();

   void clearROISpectra();

signals:

   void signal_finished_fit();

   void vertical_element_line_changed(qreal, QString);

   void export_fit_paramters(data_struct::Fit_Parameters fit_params, data_struct::Fit_Element_Map_Dict elements_to_fit);
                                                    //ev                           int spec                  back                 fit              detailed fit
   void export_csv_and_png(QPixmap, data_struct::ArrayXr *, data_struct::ArrayXr*, data_struct::ArrayXr*, data_struct::ArrayXr*, unordered_map<string, data_struct::ArrayXr> *);

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

   void replot_integrated_spectra(bool snipback);

   void replot_integrated_spectra_with_background() { replot_integrated_spectra(true); }

   void add_element();

   void del_element();

   void onSettingsDialog();

   void on_export_csv();

   void on_braching_ratio_update(data_struct::Fit_Element_Map* element);

protected:

   SpectraWidget* _spectra_widget;

   FitParamsTableModel* _fit_params_table_model;

   FitElementsTableModel* _fit_elements_table_model;

   data_struct::Fit_Element_Map_Dict *_elements_to_fit;

   //MapsH5Model* _h5_model;

   QTableView* _fit_params_table;

   QTreeView* _fit_elements_table;

   FittingDialog* _fitting_dialog;

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

   void pileup_chk_changed(int state);

   void on_export_fit_paramters();

private:

	std::mutex _mutex;

    QCheckBox *_chk_auto_model;

    QComboBox *_cb_opttimizer;

    QTabWidget *_fit_params_tab_widget;

    QPushButton* _btn_fit_spectra;

    QPushButton* _btn_model_spectra;

    QPushButton* _btn_export_parameters;

    QPushButton* _btn_export_csv;

    QPushButton* _btn_add_element;

    QPushButton* _btn_del_element;

    data_struct::ArrayXr* _int_spec;

    data_struct::ArrayXr _spectra_background;

    data_struct::ArrayXr _ev;

    data_struct::Spectra _fit_spec;

    data_struct::Params_Override* _param_override;

    unordered_map<string, data_struct::ArrayXr*> _fit_int_spec_map;

    unordered_map<string, data_struct::ArrayXr*> _roi_spec_map;

    std::string _detector_element;

    QMenu *_fit_param_contextMenu;

    QMenu *_set_fit_params_bounds_menu;

    QComboBox* _cb_add_elements;

    QComboBox* _cb_add_shell;

    QComboBox* _cb_pileup_elements;

    QCheckBox* _chk_is_pileup;

    QComboBox* _cb_detector_element;

    QPushButton* _btnSsettings;

    bool _showDetailedFitSpec;

	bool _showFitIntSpec;

    unordered_map<string, data_struct::ArrayXr> _labeled_spectras;
};


/*---------------------------------------------------------------------------*/

#endif /* FitSpectraWidget_H_ */

/*---------------------------------------------------------------------------*/

