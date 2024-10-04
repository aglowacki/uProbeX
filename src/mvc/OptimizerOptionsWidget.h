/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef Optimizer_Options_Widget_H
#define Optimizer_Options_Widget_H

//---------------------------------------------------------------------------

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QGroupBox>
#include "mvc/ComboBoxDelegate.h"
#include "fitting//optimizers/optimizer.h"
#include "fitting/routines/param_optimized_fit_routine.h"
#include "fitting/routines/hybrid_param_nnls_fit_routine.h"

//---------------------------------------------------------------------------

const static QString STR_GENERAL_FIT = "General";
const static QString STR_HYBRID_FIT = "Hybrid";

//---------------------------------------------------------------------------

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

    void setOptimizer(fitting::optimizers::Optimizer<double>& optimizer);

    void updateOptimizerOptions(fitting::optimizers::Optimizer<double>& optimizer); // get gui settings and send to optimizer

    void updateGUIOptimizerOptions(fitting::optimizers::Optimizer<double>& optimizer); // get optimizer settings and update GUI

    bool useWeights() {  return _ck_use_weights->isChecked();  }

   bool isHybrid();

protected:

   /**
    * @brief Create layout
    */
   void _createLayout();

   QTextEdit *_textEdit;

   QComboBox *_cb_optimization_type;

   QComboBox *_cb_algorithm;

   QCheckBox* _ck_use_weights;

   //QLineEdit* _opt_ftol;

   QLineEdit* _opt_xtol;

   //QLineEdit* _opt_gtol;

   //QLineEdit* _opt_epsilon;

   //QDoubleSpinBox* _opt_stepbound;

   QSpinBox* _opt_maxiter;

   //QSpinBox* _opt_lm_scale_diag;

   //QSpinBox* _opt_lm_verbose;

   //QLineEdit* _opt_mp_covtol;

};


//---------------------------------------------------------------------------

#endif 

//---------------------------------------------------------------------------
