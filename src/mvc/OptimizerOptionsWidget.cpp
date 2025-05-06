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

 //---------------------------------------------------------------------------

const QString tip_xtol = "Relative error between last two approximations. Termination occurs when the relative error between two consecutive iterates is at most xtol.";
const QString tip_maxitr = "Used to set the maximum number of function evaluations to maxiter*(number_of_parameters+1).";
const QString tip_general = "General passes fit parameters and element amplitudes to optimzier, Hybrid passes only fit parameters to optimizer and calls NNLS for element amplitudes.";

//---------------------------------------------------------------------------

OptimizerOptionsWidget::OptimizerOptionsWidget(QWidget *parent) : QWidget(parent)
{
    _createLayout();
}

//---------------------------------------------------------------------------

OptimizerOptionsWidget::~OptimizerOptionsWidget()
{

}

//---------------------------------------------------------------------------

void OptimizerOptionsWidget::_createLayout()
{

    QGridLayout* optimizerLayout = new QGridLayout();

    _cb_optimization_type = new QComboBox();
    _cb_optimization_type->addItem(STR_GENERAL_FIT);
    _cb_optimization_type->addItem(STR_HYBRID_FIT);
    _cb_optimization_type->setCurrentIndex(0);
    QLabel* lbl_opt_type = new QLabel("Optimization Type: ");
    lbl_opt_type->setToolTip(tip_general);


    _cb_algorithm = new QComboBox();
    QLabel* lbl_algo = new QLabel("Algorithm: ");
   
    _opt_xtol = new QLineEdit();
    _opt_xtol->setToolTip(tip_xtol);
    QLabel* lbl_xtol = new QLabel("XTol: ");
    lbl_xtol->setToolTip(tip_xtol);

    _opt_maxiter = new QSpinBox();
    _opt_maxiter->setToolTip(tip_maxitr);
    _opt_maxiter->setRange(0, 1000000);
    _opt_maxiter->setSingleStep(100.0);
    QLabel* lbl_maxitr = new QLabel("MaxIter: ");
    lbl_maxitr->setToolTip(tip_maxitr);

    _ck_use_weights = new QCheckBox("Use Weights");
    _ck_use_weights->setChecked(true);

    optimizerLayout->addWidget(lbl_opt_type, 0, 0, Qt::AlignRight);
    optimizerLayout->addWidget(_cb_optimization_type, 0, 1, Qt::AlignLeft);
    
    optimizerLayout->addWidget(lbl_xtol, 0, 2, Qt::AlignRight);
    optimizerLayout->addWidget(_opt_xtol, 0, 3, Qt::AlignLeft);

    optimizerLayout->addWidget(lbl_algo, 1, 0, Qt::AlignRight);
    optimizerLayout->addWidget(_cb_algorithm, 1, 1, Qt::AlignLeft);

    optimizerLayout->addWidget(lbl_maxitr, 0, 4, Qt::AlignRight);
    optimizerLayout->addWidget(_opt_maxiter, 0, 5, Qt::AlignLeft);

    optimizerLayout->addWidget(_ck_use_weights, 0, 6, Qt::AlignLeft);
    
    QVBoxLayout* layout = new QVBoxLayout();

    layout->addWidget(new QLabel("Hover over optimization options for descriptions."));
    layout->addItem(optimizerLayout);

    setGeometry(100, 100, 1024, 768);

    QScrollArea* scrollArea = new QScrollArea(this);
    //scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumWidth(800);
    scrollArea->setLayout(layout);

    QLayout* l = new QVBoxLayout();
    l->addWidget(scrollArea);

    setLayout(l);

}

//---------------------------------------------------------------------------

void OptimizerOptionsWidget::setOptimizer(fitting::optimizers::Optimizer<double>& optimizer)
{
    updateGUIOptimizerOptions(optimizer);
}

//---------------------------------------------------------------------------

bool OptimizerOptionsWidget::isHybrid()
{
    if(_cb_optimization_type->currentText() == STR_GENERAL_FIT)
    {
        return false;
    }
    return true;
}

//---------------------------------------------------------------------------

void  OptimizerOptionsWidget::updateGUIOptimizerOptions(fitting::optimizers::Optimizer<double>& optimizer)
{
    
    _cb_algorithm->clear();
    for(auto itr : optimizer.get_algorithm_list())
    {
        _cb_algorithm->addItem(QString(itr.c_str()));
    }
    _cb_algorithm->setCurrentText("LN_SBPLX");

    std::unordered_map<std::string, double> opt = optimizer.get_options();
    if (opt.count(STR_OPT_XTOL) > 0)
    {
        _opt_xtol->setText(QString::number(opt.at(STR_OPT_XTOL), 'e', 0));
    }
    if (opt.count(STR_OPT_MAXITER) > 0)
    {
        _opt_maxiter->setValue(opt.at(STR_OPT_MAXITER));
    }

}

//---------------------------------------------------------------------------

void  OptimizerOptionsWidget::updateOptimizerOptions(fitting::optimizers::Optimizer<double>& optimizer)
{
    optimizer.set_algorithm( _cb_algorithm->currentText().toStdString());

    std::unordered_map<std::string, double> opt;
    opt[STR_OPT_XTOL] = _opt_xtol->text().toDouble();
    opt[STR_OPT_MAXITER] = _opt_maxiter->value();
    optimizer.set_options(opt);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------