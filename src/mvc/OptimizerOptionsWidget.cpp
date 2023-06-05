/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/OptimizerOptionsWidget.h>
#include <QGroupBox>
#include <QSplitter>
#include <QScrollBar>
#include <QScrollArea>
#include <mvc/NumericPrecDelegate.h>

 /*---------------------------------------------------------------------------*/

const QString tip_ftol = "Relative error desired in the sum of squares. Termination occurs when both the actual and predicted relative reductions in the sum of squares are at most ftol.";
const QString tip_xtol = "Relative error between last two approximations. Termination occurs when the relative error between two consecutive iterates is at most xtol.";
const QString tip_gtol = "Orthogonality desired between fvec and its derivs. Termination occurs when the cosine of the angle between fvec and any column of the Jacobian is at most gtol in absolute value.";
const QString tip_epsilon = "Step used to calculate the Jacobian, should be slightly larger than the relative error in the user-supplied functions.";
const QString tip_step = "Used in determining the initial step bound. This bound is set to the product of stepbound and the Euclidean norm of diag*x if nonzero, or else to stepbound itself. In most cases stepbound should lie in the interval (0.1,100.0). Generally, the value 100.0 is recommended.";
const QString tip_maxitr = "Used to set the maximum number of function evaluations to maxiter*(number_of_parameters+1).";
const QString tip_scale = "If 1, the variables will be rescaled internally. Recommended value is 1.";
const QString tip_covtol = "Range tolerance for covariance calculation Default: 1e-14";


/*---------------------------------------------------------------------------*/

OptimizerOptionsWidget::OptimizerOptionsWidget(QWidget *parent) : QWidget(parent)
{
    _createLayout();
}

/*---------------------------------------------------------------------------*/

OptimizerOptionsWidget::~OptimizerOptionsWidget()
{

}

/*---------------------------------------------------------------------------*/

void OptimizerOptionsWidget::_createLayout()
{

    QGridLayout* optimizerLayout = new QGridLayout();

    _opt_ftol = new QDoubleSpinBox();
    _opt_ftol->setToolTip(tip_ftol);
    _opt_ftol->setDecimals(30);
    _opt_ftol->setRange(0.0, 1.0);
    _opt_ftol->setSingleStep(1.0e-12);
    QLabel* lbl_ftol = new QLabel("FTol: ");
    lbl_ftol->setToolTip(tip_ftol);
    
    _opt_xtol = new QDoubleSpinBox();
    _opt_xtol->setToolTip(tip_xtol);
    _opt_xtol->setDecimals(30);
    _opt_xtol->setRange(0.0, 1.0);
    _opt_xtol->setSingleStep(1.0e-12);
    QLabel* lbl_xtol = new QLabel("XTol: ");
    lbl_xtol->setToolTip(tip_xtol);

    _opt_gtol = new QDoubleSpinBox();
    _opt_gtol->setToolTip(tip_gtol);
    _opt_gtol->setDecimals(30);
    _opt_gtol->setRange(0.0, 1.0);
    _opt_gtol->setSingleStep(1.0e-12);
    QLabel* lbl_gtol = new QLabel("GTol: ");
    lbl_gtol->setToolTip(tip_gtol);
    
    _opt_epsilon = new QDoubleSpinBox();
    _opt_epsilon->setToolTip(tip_epsilon);
    _opt_epsilon->setDecimals(30);
    _opt_epsilon->setRange(0.0, 1.0);
    _opt_epsilon->setSingleStep(1.0e-12);
    QLabel* lbl_epsilon = new QLabel("Epsilon: ");
    lbl_epsilon->setToolTip(tip_epsilon);

    _opt_stepbound = new QDoubleSpinBox();
    _opt_stepbound->setToolTip(tip_step);
    _opt_stepbound->setDecimals(3);
    _opt_stepbound->setRange(0.0, 1000.0);
    _opt_stepbound->setSingleStep(0.1);
    QLabel* lbl_step = new QLabel("Step Bound: ");
    lbl_step->setToolTip(tip_step);

    _opt_maxiter = new QSpinBox();
    _opt_maxiter->setToolTip(tip_maxitr);
    //_opt_maxiter->setDecimals(1);
    _opt_maxiter->setRange(0, 100000);
    _opt_maxiter->setSingleStep(100.0);
    QLabel* lbl_maxitr = new QLabel("MaxIter: ");
    lbl_maxitr->setToolTip(tip_maxitr);

    _opt_lm_scale_diag = new QSpinBox();
    _opt_lm_scale_diag->setToolTip(tip_scale);
    //_opt_lm_scale_diag->setDecimals(1);
    _opt_lm_scale_diag->setRange(0, 1);
    _opt_lm_scale_diag->setSingleStep(1.0);
    QLabel* lbl_scale = new QLabel("Scale Diag: ");
    lbl_scale->setToolTip(tip_scale);

    _opt_mp_covtol = new QDoubleSpinBox();
    _opt_mp_covtol->setToolTip(tip_covtol);
    _opt_mp_covtol->setDecimals(30);
    _opt_mp_covtol->setRange(0.0, 1.0);
    _opt_mp_covtol->setSingleStep(1.0e-15);
    QLabel* lbl_covtol = new QLabel("CovTol: ");
    lbl_covtol->setToolTip(tip_covtol);

    _ck_use_weights = new QCheckBox("Use Weights");
    _ck_use_weights->setChecked(false);

    optimizerLayout->addWidget(lbl_ftol, 0, 0, Qt::AlignRight);
    optimizerLayout->addWidget(_opt_ftol, 0, 1, Qt::AlignLeft);
    optimizerLayout->addWidget(lbl_xtol, 0, 2, Qt::AlignRight);
    optimizerLayout->addWidget(_opt_xtol, 0, 3, Qt::AlignLeft);
    optimizerLayout->addWidget(lbl_gtol, 0, 4, Qt::AlignRight);
    optimizerLayout->addWidget(_opt_gtol, 0, 5, Qt::AlignLeft);

    optimizerLayout->addWidget(lbl_epsilon, 1, 0, Qt::AlignRight);
    optimizerLayout->addWidget(_opt_epsilon, 1, 1, Qt::AlignLeft);
    optimizerLayout->addWidget(lbl_step, 1, 2, Qt::AlignRight);
    optimizerLayout->addWidget(_opt_stepbound, 1, 3, Qt::AlignLeft);
    optimizerLayout->addWidget(lbl_maxitr, 1, 4, Qt::AlignRight);
    optimizerLayout->addWidget(_opt_maxiter, 1, 5, Qt::AlignLeft);

    optimizerLayout->addWidget(_ck_use_weights, 0, 6, Qt::AlignLeft);

    QHBoxLayout* hbox_lm = new QHBoxLayout();
    hbox_lm->addWidget(lbl_scale);
    hbox_lm->addWidget(_opt_lm_scale_diag);
    _lm_fit_ctrl_grp = new QGroupBox();
    _lm_fit_ctrl_grp->setLayout(hbox_lm);
    optimizerLayout->addWidget(_lm_fit_ctrl_grp, 0, 7, Qt::AlignLeft);

    QHBoxLayout* hbox_mp = new QHBoxLayout();
    hbox_mp->addWidget(lbl_covtol);
    hbox_mp->addWidget(_opt_mp_covtol);
    _mp_fit_ctrl_grp = new QGroupBox();
    _mp_fit_ctrl_grp->setLayout(hbox_mp);
    optimizerLayout->addWidget(_mp_fit_ctrl_grp, 1, 7, Qt::AlignLeft);
    
    QVBoxLayout* layout = new QVBoxLayout();

    layout->addWidget(new QLabel("Hover over optimization options for descriptions."));
    layout->addItem(optimizerLayout);

    setGeometry(100, 100, 1024, 768);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumWidth(800);
    scrollArea->setLayout(layout);

    QLayout* l = new QVBoxLayout();
    l->addWidget(scrollArea);

    setLayout(l);


}

/*---------------------------------------------------------------------------*/

void OptimizerOptionsWidget::setOptimizer(QString opt, fitting::optimizers::Optimizer<double>& optimizer)
{
    if (opt == STR_LM_FIT)
    {
        _mp_fit_ctrl_grp->setVisible(false);
        _lm_fit_ctrl_grp->setVisible(true);
        _ck_use_weights->setChecked(false);
    }
    else if (opt == STR_MP_FIT)
    {
        _lm_fit_ctrl_grp->setVisible(false);
        _mp_fit_ctrl_grp->setVisible(true);
        _ck_use_weights->setChecked(true);
    }
    else if (opt == STR_HYBRID_MP_FIT)
    {
        _lm_fit_ctrl_grp->setVisible(false);
        _mp_fit_ctrl_grp->setVisible(true);
        _ck_use_weights->setChecked(false);
    }
    updateGUIOptimizerOptions(optimizer);
}

/*---------------------------------------------------------------------------*/

void  OptimizerOptionsWidget::updateGUIOptimizerOptions(fitting::optimizers::Optimizer<double>& optimizer)
{
    
    std::unordered_map<std::string, double> opt = optimizer.get_options();
    if (opt.count(STR_OPT_FTOL) > 0)
    {
        _opt_ftol->setValue(opt.at(STR_OPT_FTOL));
    }
    if (opt.count(STR_OPT_XTOL) > 0)
    {
        _opt_xtol->setValue(opt.at(STR_OPT_XTOL));
    }
    if (opt.count(STR_OPT_GTOL) > 0)
    {
        _opt_gtol->setValue(opt.at(STR_OPT_GTOL));
    }
    if (opt.count(STR_OPT_EPSILON) > 0)
    {
        _opt_epsilon->setValue(opt.at(STR_OPT_EPSILON));
    }
    if (opt.count(STR_OPT_STEP) > 0)
    {
        _opt_stepbound->setValue(opt.at(STR_OPT_STEP));
    }
    if (opt.count(STR_OPT_MAXITER) > 0)
    {
        _opt_maxiter->setValue(opt.at(STR_OPT_MAXITER));
    }
    if (opt.count(STR_OPT_SCALE_DIAG) > 0)
    {
        _opt_lm_scale_diag->setValue((int)opt.at(STR_OPT_SCALE_DIAG));
    }
    if (opt.count(STR_OPT_COVTOL) > 0)
    {
        _opt_mp_covtol->setValue(opt.at(STR_OPT_COVTOL));
    }
}

/*---------------------------------------------------------------------------*/

void  OptimizerOptionsWidget::updateOptimizerOptions(fitting::optimizers::Optimizer<double>& optimizer)
{
    std::unordered_map<std::string, double> opt;
    opt[STR_OPT_FTOL] = _opt_ftol->value();
    opt[STR_OPT_XTOL] = _opt_xtol->value();
    opt[STR_OPT_GTOL] = _opt_gtol->value();
    opt[STR_OPT_EPSILON] = _opt_epsilon->value();
    opt[STR_OPT_STEP] = _opt_stepbound->value();
    opt[STR_OPT_MAXITER] = _opt_maxiter->value();
    opt[STR_OPT_SCALE_DIAG] = _opt_lm_scale_diag->value();
    opt[STR_OPT_COVTOL] = _opt_mp_covtol->value();
    optimizer.set_options(opt);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/