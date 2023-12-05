/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef Optimizer_Options_Widget_H
#define Optimizer_Options_Widget_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include "mvc/ComboBoxDelegate.h"
#include "fitting//optimizers/lmfit_optimizer.h"
#include "fitting//optimizers/mpfit_optimizer.h"
#include "fitting/routines/param_optimized_fit_routine.h"
#include "fitting/routines/hybrid_param_nnls_fit_routine.h"

/*---------------------------------------------------------------------------*/

const static QString STR_LM_FIT = "Levenberg-Marquardt Fit";
const static QString STR_MP_FIT = "MP Fit";
const static QString STR_HYBRID_MP_FIT = "Hybrid MP Fit";

/*---------------------------------------------------------------------------*/

class OptimizerOptionsWidget : public QWidget
{

   Q_OBJECT

public:

    /**
     * Constructor.
     */
    OptimizerOptionsWidget(QWidget *parent = nullptr);

    /**
     * Destructor.
     */
    ~OptimizerOptionsWidget();

    void setOptimizer(QString opt, fitting::optimizers::Optimizer<double>& optimizer);

    void updateOptimizerOptions(fitting::optimizers::Optimizer<double>& optimizer); // get gui settings and send to optimizer

    void updateGUIOptimizerOptions(fitting::optimizers::Optimizer<double>& optimizer); // get optimizer settings and update GUI

    bool useWeights() {  return _ck_use_weights->isChecked();  }

protected:

   /**
    * @brief Create layout
    */
   void _createLayout();

   QTextEdit *_textEdit;

   QGroupBox* _lm_fit_ctrl_grp;

   QGroupBox* _mp_fit_ctrl_grp;

   QCheckBox* _ck_use_weights;

   QLineEdit* _opt_ftol;

   QLineEdit* _opt_xtol;

   QLineEdit* _opt_gtol;

   QLineEdit* _opt_epsilon;

   QDoubleSpinBox* _opt_stepbound;

   QSpinBox* _opt_maxiter;

   QSpinBox* _opt_lm_scale_diag;

   QSpinBox* _opt_lm_verbose;

   QLineEdit* _opt_mp_covtol;

};


/*---------------------------------------------------------------------------*/

#endif 

/*---------------------------------------------------------------------------*/
