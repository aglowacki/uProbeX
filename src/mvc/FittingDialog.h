/*-----------------------------------------------------------------------------
 * Copyright (c) 2020, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef FITTING_DIALOG_H
#define FITTING_DIALOG_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QThread>
#include <QTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QCheckBox>
#include <QEventLoop>
#include <QDialog>
#include <QTableView>
#include <thread>
#include <QHeaderView>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include "mvc/ComboBoxDelegate.h"
#include "mvc/FitParamsTableModel.h"
#include "mvc/SpectraWidget.h"
#include "fitting//optimizers/lmfit_optimizer.h"
#include "fitting//optimizers/mpfit_optimizer.h"
#include "fitting/routines/param_optimized_fit_routine.h"
#include "fitting/routines/hybrid_param_nnls_fit_routine.h"
#include "fitting/models/gaussian_model.h"
#include <mvc/MapsH5Model.h>
#include "core/GlobalThreadPool.h"

/*---------------------------------------------------------------------------*/

const static QString STR_LM_FIT = "Levenberg-Marquardt Fit";
const static QString STR_MP_FIT = "MP Fit";
const static QString STR_HYBRID_MP_FIT = "Hybrid MP Fit";

/*---------------------------------------------------------------------------*/

class FittingDialog : public QDialog
{

   Q_OBJECT

public:

    /**
     * Constructor.
     */
	FittingDialog(QWidget *parent = nullptr);

    /**
     * Destructor.
     */
    ~FittingDialog();

    void updateFitParams(data_struct::Fit_Parameters<double> out_fit_params, data_struct::Fit_Parameters<double> element_fit_params);

    void status_callback(size_t cur_block, size_t total_blocks);

    void setOptimizer(QString opt);

    bool accepted_fit() { return _accepted; }

    void setSpectra(data_struct::Spectra<double>* spectra, ArrayDr energy);

    void setFitSpectra(data_struct::Spectra<double>* spectra);

    void setElementsToFit(data_struct::Fit_Element_Map_Dict<double>* elements_to_fit);

	data_struct::Spectra<double> get_fit_spectra(std::unordered_map<std::string, ArrayDr>* labeled_spectras);

	data_struct::Fit_Parameters<double>* get_new_fit_params() { return &_new_out_fit_params; }

    data_struct::Fit_Element_Map_Dict<double> get_elements_to_fit() { return *_elements_to_fit; }

    void waitToFinishRunning();

    void setDisplayRange(QString wmin, QString wmax, QString hmin, QString hmax);

    fitting::optimizers::OPTIMIZER_OUTCOME getOutcome() { return _outcome; }

signals:

    void processed_list_update();

public slots:
    void runProcessing();

	void onAccepted();

    void onCancel();
   
protected:

   /**
    * @brief Create layout
    */
   void _createLayout();

   void _updateGUIOptimizerOptions(); // get optimizer settings and update GUI

   void _updateOptimizerOptions(); // get gui settings and send to optimizer

   QTableView* _fit_params_table;

   QTableView* _new_fit_params_table;

   FitParamsTableModel* _fit_params_table_model;

   FitParamsTableModel* _new_fit_params_table_model;

   SpectraWidget* _spectra_widget;

   fitting::models::Range _energy_range;
   
   fitting::models::Gaussian_Model<double> _model;

   QTextEdit *_textEdit;

   QProgressBar *_progressBarFiles;

   QProgressBar* _progressBarBlocks;

   QPushButton *_btn_run;

   QPushButton* _btn_accept;

   QPushButton *_btn_cancel;

   QGroupBox* _lm_fit_ctrl_grp;

   QGroupBox* _mp_fit_ctrl_grp;

   fitting::optimizers::LMFit_Optimizer<double> _lmfit_optimizer;

   fitting::optimizers::MPFit_Optimizer<double> _mpfit_optimizer;

   fitting::optimizers::Optimizer<double>*_optimizer;

   //fitting::routines::Base_Fit_Routine<double> *_fit_routine;

   fitting::routines::Param_Optimized_Fit_Routine<double> _param_fit_routine;

   fitting::routines::Hybrid_Param_NNLS_Fit_Routine<double> _hybrid_fit_routine;

   size_t _total_itr;

   bool _accepted;

   bool _canceled;

   bool _running;

   data_struct::Spectra<double>* _int_spec;

   data_struct::Fit_Element_Map_Dict<double>* _elements_to_fit;

   data_struct::Fit_Parameters<double> _out_fit_params;

   data_struct::Fit_Parameters<double> _element_fit_params;

   data_struct::Fit_Parameters<double> _new_out_fit_params;

   data_struct::Spectra<double> _new_fit_spec;

   QDoubleSpinBox* _opt_ftol;

   QDoubleSpinBox* _opt_xtol;

   QDoubleSpinBox* _opt_gtol;

   QDoubleSpinBox* _opt_epsilon;

   QDoubleSpinBox* _opt_stepbound;

   QSpinBox* _opt_maxiter;

   QSpinBox* _opt_lm_scale_diag;

   QDoubleSpinBox* _opt_mp_covtol;

   ArrayDr _spectra_background;

   bool _is_hybrid_fit;

   // energy vector
   ArrayDr _ev;

   fitting::optimizers::OPTIMIZER_OUTCOME _outcome;

};


/*---------------------------------------------------------------------------*/

#endif 

/*---------------------------------------------------------------------------*/
