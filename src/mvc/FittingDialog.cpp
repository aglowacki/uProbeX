/*-----------------------------------------------------------------------------
 * Copyright (c) 2020, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/FittingDialog.h>
#include <QApplication>
#include <QGroupBox>
#include <QSplitter>
#include <QScrollBar>
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

FittingDialog::FittingDialog(QWidget *parent) : QDialog(parent)
{
    _accepted = false;
    _canceled = false;
    _running = false;
    _is_hybrid_fit = false;
    _elements_to_fit = nullptr;
    _int_spec = nullptr;
    _param_fit_routine.set_update_coherent_amplitude_on_fit(false);
    _hybrid_fit_routine.set_update_coherent_amplitude_on_fit(false);
    _createLayout();
}

/*---------------------------------------------------------------------------*/

FittingDialog::~FittingDialog()
{
    if (_fit_params_table_model != nullptr)
    {
        delete _fit_params_table_model;
    }
 
    if (_new_fit_params_table_model != nullptr)
    {
        delete _new_fit_params_table_model;
    }
}

/*---------------------------------------------------------------------------*/

void FittingDialog::_createLayout()
{
    std::vector<std::string> bound_types{ "Not Initialized", "Fixed", "Limited Low High", "Limited Low", "Limited High", "Fit" };

    _progressBarBlocks = new QProgressBar();
    _progressBarBlocks->setRange(0,100);
    _progressBarFiles = new QProgressBar();
    _progressBarFiles->setRange(0, 100);

    ComboBoxDelegate* cbDelegate = new ComboBoxDelegate(bound_types);
    NumericPrecDelegate* npDelegate = new NumericPrecDelegate();

    _fit_params_table_model = new FitParamsTableModel();

    _new_fit_params_table_model = new FitParamsTableModel();
    _new_fit_params_table_model->setEditable(false);

    _fit_params_table = new QTableView();
    _fit_params_table->setModel(_fit_params_table_model);
    _fit_params_table->sortByColumn(0, Qt::AscendingOrder);
    _fit_params_table->setItemDelegateForColumn(1, npDelegate);
    _fit_params_table->setItemDelegateForColumn(2, cbDelegate);
    _fit_params_table->setItemDelegateForColumn(3, npDelegate);
    _fit_params_table->setItemDelegateForColumn(4, npDelegate);
    _fit_params_table->setItemDelegateForColumn(5, npDelegate);
    _fit_params_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        
    _new_fit_params_table = new QTableView();
    _new_fit_params_table->setModel(_new_fit_params_table_model);
    _new_fit_params_table->sortByColumn(0, Qt::AscendingOrder);
    _new_fit_params_table->setItemDelegateForColumn(2, cbDelegate);
    _new_fit_params_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    connect(_new_fit_params_table->verticalScrollBar(), &QAbstractSlider::valueChanged, _fit_params_table->verticalScrollBar(), &QAbstractSlider::setValue);

    _btn_run = new QPushButton("Run");
    connect(_btn_run, &QPushButton::released, this, &FittingDialog::runProcessing);

    _btn_accept = new QPushButton("Accept");
    connect(_btn_accept, &QPushButton::released, this, &FittingDialog::onAccepted);
    _btn_accept->setEnabled(false);

    _btn_cancel = new QPushButton("Cancel");
    connect(_btn_cancel, &QPushButton::released, this, &FittingDialog::onCancel);

    QHBoxLayout* buttonlayout = new QHBoxLayout();
    buttonlayout->addWidget(_btn_run);
    buttonlayout->addWidget(_btn_accept);
    buttonlayout->addWidget(_btn_cancel);

    _spectra_widget = new SpectraWidget();
    
    QHBoxLayout* hbox_tables = new QHBoxLayout();
    hbox_tables->addWidget(_fit_params_table);
    hbox_tables->addWidget(_new_fit_params_table);

    QHBoxLayout* hbox_progresss_blocks = new QHBoxLayout();
    hbox_progresss_blocks->addWidget(new QLabel("Current Iteration:"));
    hbox_progresss_blocks->addWidget(_progressBarBlocks);

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

    QHBoxLayout* hbox_lm = new QHBoxLayout();
    hbox_lm->addWidget(lbl_scale);
    hbox_lm->addWidget(_opt_lm_scale_diag);
    _lm_fit_ctrl_grp = new QGroupBox();
    _lm_fit_ctrl_grp->setLayout(hbox_lm);
    optimizerLayout->addWidget(_lm_fit_ctrl_grp, 0, 6, Qt::AlignLeft);

    QHBoxLayout* hbox_mp = new QHBoxLayout();
    hbox_mp->addWidget(lbl_covtol);
    hbox_mp->addWidget(_opt_mp_covtol);
    _mp_fit_ctrl_grp = new QGroupBox();
    _mp_fit_ctrl_grp->setLayout(hbox_mp);
    optimizerLayout->addWidget(_mp_fit_ctrl_grp, 1, 6, Qt::AlignLeft);
    
    QVBoxLayout* layout = new QVBoxLayout();

    QWidget* bottomWidget = new QWidget();
    bottomWidget->setLayout(hbox_tables);

    QSplitter* splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(_spectra_widget);
    splitter->setStretchFactor(0, 1);
    splitter->addWidget(bottomWidget);
    layout->addWidget(splitter);
    layout->addWidget(new QLabel("Hover over optimization options for descriptions."));
    layout->addItem(optimizerLayout);
    layout->addItem(buttonlayout);
    layout->addItem(hbox_progresss_blocks);

    //setLayout(layout);

    setGeometry(100, 100, 1024, 768);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidgetResizable(true);
    scrollArea->setLayout(layout);

    QLayout* l = new QVBoxLayout();
    l->addWidget(scrollArea);

    setLayout(l);

    

}

/*---------------------------------------------------------------------------*/

void FittingDialog::updateFitParams(data_struct::Fit_Parameters<double> out_fit_params, data_struct::Fit_Parameters<double> element_fit_params)
{
    _out_fit_params = out_fit_params;
    _element_fit_params = element_fit_params;

    _fit_params_table_model->setFitParams(out_fit_params);
    //_fit_params_table_model->updateFitParams(&out_fit_params);

	_progressBarBlocks->setValue(0);
	_accepted = false;
	_btn_accept->setEnabled(false);
}

/*---------------------------------------------------------------------------*/

void FittingDialog::setOptimizer(QString opt)
{
    if (opt == STR_LM_FIT)
    {
        _mp_fit_ctrl_grp->setVisible(false);
        _lm_fit_ctrl_grp->setVisible(true);
        _optimizer = &_lmfit_optimizer;
        _param_fit_routine.set_optimizer(_optimizer);
        _is_hybrid_fit = false;
        _fit_params_table_model->setOptimizerSupportsMinMax(false);
    }
    else if (opt == STR_MP_FIT)
    {
        _lm_fit_ctrl_grp->setVisible(false);
        _mp_fit_ctrl_grp->setVisible(true);
        _optimizer = &_mpfit_optimizer;
        _param_fit_routine.set_optimizer(_optimizer);
        _is_hybrid_fit = false;
        _fit_params_table_model->setOptimizerSupportsMinMax(true);
    }
    else if (opt == STR_HYBRID_MP_FIT)
    {
        _lm_fit_ctrl_grp->setVisible(false);
        _mp_fit_ctrl_grp->setVisible(true);
        _optimizer = &_mpfit_optimizer;
        _is_hybrid_fit = true;
        _hybrid_fit_routine.set_optimizer(_optimizer);
        _fit_params_table_model->setOptimizerSupportsMinMax(true);
    }
    _updateGUIOptimizerOptions();
}

/*---------------------------------------------------------------------------*/

void  FittingDialog::_updateGUIOptimizerOptions()
{
    if (_optimizer != nullptr)
    {
        unordered_map<string, double> opt = _optimizer->get_options();
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
}

/*---------------------------------------------------------------------------*/

void  FittingDialog::_updateOptimizerOptions()
{
    if (_optimizer != nullptr)
    {
        unordered_map<string, double> opt;
        opt[STR_OPT_FTOL] = _opt_ftol->value();
        opt[STR_OPT_XTOL] = _opt_xtol->value();
        opt[STR_OPT_GTOL] = _opt_gtol->value();
        opt[STR_OPT_EPSILON] = _opt_epsilon->value();
        opt[STR_OPT_STEP] = _opt_stepbound->value();
        opt[STR_OPT_MAXITER] = _opt_maxiter->value();
        opt[STR_OPT_SCALE_DIAG] = _opt_lm_scale_diag->value();
        opt[STR_OPT_COVTOL] = _opt_mp_covtol->value();
        _optimizer->set_options(opt);
    }
}

/*---------------------------------------------------------------------------*/

void FittingDialog::setSpectra(data_struct::Spectra<double>* spectra, ArrayDr energy)
{
    _int_spec = spectra;
    _ev = energy;
    if (spectra != nullptr)
    {
        _energy_range;
        _energy_range.min = 0;
        _energy_range.max = _int_spec->rows() - 1;
        _spectra_widget->append_spectra(DEF_STR_INT_SPECTRA, _int_spec, &_ev);

        _spectra_background = snip_background<double>((Spectra<double>*)_int_spec,
                _out_fit_params[STR_ENERGY_OFFSET].value,
                _out_fit_params[STR_ENERGY_SLOPE].value,
                _out_fit_params[STR_ENERGY_QUADRATIC].value,
                _out_fit_params[STR_SNIP_WIDTH].value,
                0, //spectra energy start range
                _int_spec->size() - 1);
        
        _spectra_widget->append_spectra(DEF_STR_BACK_SPECTRA, &_spectra_background, (data_struct::Spectra<double>*) & _ev);
    }
}

/*---------------------------------------------------------------------------*/

void FittingDialog::setFitSpectra(data_struct::Spectra<double>* spectra)
{
    _spectra_widget->append_spectra(DEF_STR_FIT_INT_SPECTRA, spectra, &_ev);
}

/*---------------------------------------------------------------------------*/

void FittingDialog::setElementsToFit(data_struct::Fit_Element_Map_Dict<double>* elements_to_fit)
{
    _elements_to_fit = elements_to_fit;
    if (_elements_to_fit != nullptr)
    {
        unordered_map<string, ArrayDr> labeled_spectras;
        data_struct::Spectra<double> fit_spec = _model.model_spectrum(&_out_fit_params, _elements_to_fit, &labeled_spectras, _energy_range);
        /*
        if (fit_spec.size() == _spectra_background.size())
        {
            fit_spec += _spectra_background;
        }
        */
        for (int i = 0; i < fit_spec.size(); i++)
        {
            if (fit_spec[i] <= 0.0)
            {
                fit_spec[i] = 0.1;
            }
        }
        
        _spectra_widget->append_spectra(DEF_STR_FIT_INT_SPECTRA, &fit_spec, &_ev);
    }
}

/*---------------------------------------------------------------------------*/

data_struct::Spectra<double> FittingDialog::get_fit_spectra(unordered_map<string, ArrayDr>* labeled_spectras)
{
	return _model.model_spectrum(&_new_out_fit_params, _elements_to_fit, labeled_spectras, _energy_range);
}

/*---------------------------------------------------------------------------*/

void FittingDialog::setDisplayRange(QString wmin, QString wmax, QString hmin, QString hmax)
{
    _spectra_widget->setDisplayRange(wmin, wmax, hmin, hmax);
}

/*---------------------------------------------------------------------------*/

void FittingDialog::onAccepted()
{
	_accepted = true;
	_btn_accept->setEnabled(false);
	close();
}

/*---------------------------------------------------------------------------*/

void FittingDialog::onCancel()
{
    _canceled = true;
    if (_running == false)
    {
        close();
    }
}

/*---------------------------------------------------------------------------*/

void FittingDialog::runProcessing()
{
    fitting::optimizers::OPTIMIZER_OUTCOME outcome;

    if (_elements_to_fit != nullptr && _int_spec != nullptr)
    {
        _running = true;
        _btn_run->setEnabled(false);

        //Range of energy in spectra to fit
        _energy_range;
		_energy_range.min = 0;
		_energy_range.max = _int_spec->rows() - 1;

        _updateOptimizerOptions();

        //data_struct::Spectra s1 = _integrated_spectra.sub_spectra(energy_range);

        //Fitting routines
        _out_fit_params = _fit_params_table_model->getFitParams();

        //reset model fit parameters to defaults
        _model.reset_to_default_fit_params();
        //Update fit parameters by override values
        _model.update_fit_params_values(&_out_fit_params);
        _model.update_and_add_fit_params_values_gt_zero(&_element_fit_params);

        //_model.set_fit_params_preset(fitting::models::Fit_Params_Preset::BATCH_FIT_WITH_TAILS);

        //Initialize the fit routine
        if (_is_hybrid_fit)
        {
            _hybrid_fit_routine.initialize(&_model, _elements_to_fit, _energy_range);
        }
        else
        {
            _param_fit_routine.initialize(&_model, _elements_to_fit, _energy_range);
        }
        //Fit the spectra saving the element counts in element_fit_count_dict
        // single threaded
        //out_fit_params = fit_routine.fit_spectra_parameters(&_model, int_spectra, _elements_to_fit);

        // use background thread to not freeze up the gui
        Callback_Func_Status_Def cb_func = std::bind(&FittingDialog::status_callback, this, std::placeholders::_1, std::placeholders::_2);
        try
        {
            if (_is_hybrid_fit)
            {
                outcome = _hybrid_fit_routine.fit_spectra_parameters(&_model, _int_spec, _elements_to_fit, _new_out_fit_params, &cb_func);
            }
            else
            {
                outcome = _param_fit_routine.fit_spectra_parameters(&_model, _int_spec, _elements_to_fit, _new_out_fit_params, &cb_func);
            }
        }
        catch (int e)
        {
            logI << "Exception Thrown" << std::endl;
            _running = false;
            close();
            return;
        }
        /*
        //std::future<data_struct::Fit_Parameters<double>> ret = global_threadpool.enqueue(&fitting::routines::Param_Optimized_Fit_Routine::fit_spectra_parameters, _fit_routine, &_model, _int_spec, _elements_to_fit, &_cb_func);
        std::future<data_struct::Fit_Parameters<double>> ret = global_threadpool.enqueue([&](){
            try
            {
                return _fit_routine.fit_spectra_parameters(&_model, _int_spec, _elements_to_fit, &cb_func);
            }
            catch (int e)
            {
                data_struct::Fit_Parameters<double> f;
                return f;
            }
        });

        while (ret._Is_ready() == false)
        {
            if (_canceled)
            {
                _running = false;
                return;
            }
            else
            {
                std::this_thread::sleep_for(20ms);
            }
            //QCoreApplication::processEvents();
        }
        
        _new_out_fit_params = ret.get();
        */
        _new_fit_params_table_model->setFitParams(_new_out_fit_params);

        //don't want to display element counts, so removing them
        _new_fit_params_table_model->only_keep_these_keys(_out_fit_params);

		_progressBarBlocks->setValue(_total_itr);

        unordered_map<string, ArrayDr> labeled_spectras;
        data_struct::Spectra<double> fit_spec = _model.model_spectrum(&_new_out_fit_params, _elements_to_fit, &labeled_spectras, _energy_range);
        
        if (fit_spec.size() == _spectra_background.size())
        {
            fit_spec += _spectra_background;
        }
        
        for (int i = 0; i < fit_spec.size(); i++)
        {
            if (fit_spec[i] <= 0.0)
            {
                fit_spec[i] = 0.1;
            }
        }

        _spectra_widget->append_spectra(DEF_STR_NEW_FIT_INT_SPECTRA, &fit_spec, &_ev);

        _running = false;
		_btn_accept->setEnabled(true);
        _btn_run->setEnabled(true);
    }
}

/*---------------------------------------------------------------------------*/

void FittingDialog::status_callback(size_t cur_itr, size_t total_itr)
{
    if (_canceled)
    {
        throw 20;
    }

    if (_total_itr != total_itr)
    {
        _total_itr = total_itr;
        _progressBarBlocks->setRange(0, _total_itr);
    }

    _progressBarBlocks->setValue(cur_itr);
    QCoreApplication::processEvents();
}

void FittingDialog::waitToFinishRunning()
{
    while (_running)
    {
        std::this_thread::sleep_for(20ms);
    }
}

/*---------------------------------------------------------------------------*/