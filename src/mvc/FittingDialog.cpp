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
#include <chrono>

using namespace std::chrono_literals;

#define FP_INDEX 0
#define OPT_INDEX 1
#define OUTPUT_INDEX 2

 //---------------------------------------------------------------------------
//---------------------------------------------------------------------------

FittingDialog::FittingDialog(QWidget *parent) : QDialog(parent)
{
    _accepted = false;
    _canceled = false;
    _running = false;
    _is_hybrid_fit = false;
    _elements_to_fit = nullptr;
    _param_fit_routine.set_update_coherent_amplitude_on_fit(false);
    _hybrid_fit_routine.set_update_coherent_amplitude_on_fit(false);
    ////_model.set_num_threads(std::thread::hardware_concurrency());
    _createLayout();
}

//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------

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
    
    QVBoxLayout* vbox_fitp = new QVBoxLayout();

    QHBoxLayout* hbox_btn_update = new QHBoxLayout();
    _btn_update_fitp = new QPushButton();
    _btn_update_fitp->setText("<- Update Selected Values");
    _btn_update_fitp->setMaximumWidth(200);
    connect(_btn_update_fitp, &QPushButton::released, this, &FittingDialog::onUpdateSelected);

    QHBoxLayout* hbox_tables = new QHBoxLayout();
    hbox_tables->addWidget(_fit_params_table);
    hbox_tables->addWidget(_new_fit_params_table);


    hbox_btn_update->addStretch();
    hbox_btn_update->addWidget(_btn_update_fitp);
    hbox_btn_update->addStretch();

    vbox_fitp->addLayout(hbox_btn_update);
    vbox_fitp->addLayout(hbox_tables);

    QHBoxLayout* hbox_progresss_blocks = new QHBoxLayout();
    hbox_progresss_blocks->addWidget(new QLabel("Current Iteration:"));
    hbox_progresss_blocks->addWidget(_progressBarBlocks);

    _optimizer_widget = new OptimizerOptionsWidget();
    
    QVBoxLayout* layout = new QVBoxLayout();

    QWidget* bottomWidget = new QWidget();
    bottomWidget->setLayout(vbox_fitp);


    _le_outcome = new QLineEdit();
    _le_detailed_outcome = new QLineEdit();
    _le_residual = new QLineEdit();
    _le_num_itr = new QLineEdit();

    QGridLayout* glayout = new QGridLayout();
    glayout->setAlignment(Qt::AlignTop);
    glayout->addWidget(new QLabel("Outcome: "), 0, 0, Qt::AlignRight);
    glayout->addWidget(_le_outcome, 0, 1);
    glayout->addWidget(new QLabel("Detailed Outcome: "), 1, 0, Qt::AlignRight);
    glayout->addWidget(_le_detailed_outcome, 1, 1);
    glayout->addWidget(new QLabel("Residual Error: "), 2, 0, Qt::AlignRight);
    glayout->addWidget(_le_residual, 2, 1);
    glayout->addWidget(new QLabel("Number of Iterations: "), 3, 0, Qt::AlignRight);
    glayout->addWidget(_le_num_itr, 3, 1);

    QWidget* ouput_widget = new QWidget();
    ouput_widget->setLayout(glayout);

    _tabWidget = new QTabWidget(this);
    _tabWidget->addTab(bottomWidget, QIcon(), "Fit Parameters");
    _tabWidget->addTab(_optimizer_widget, QIcon(), "Optimizer Options");
    _tabWidget->addTab(ouput_widget, QIcon(), "Output");
  
    QSplitter* splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(_spectra_widget);
    splitter->setStretchFactor(0, 1);
    splitter->addWidget(_tabWidget);
    
    layout->addWidget(splitter);
    layout->addItem(buttonlayout);
    layout->addItem(hbox_progresss_blocks);

    setGeometry(100, 100, 1920, 1024);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setLayout(layout);

    QLayout* l = new QVBoxLayout();
    l->addWidget(scrollArea);
    
    setLayout(l);
}

//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------

void FittingDialog::setOptimizer(QString opt)
{
    if (opt == STR_LM_FIT)
    {
        _optimizer = &_lmfit_optimizer;
        _param_fit_routine.set_optimizer(_optimizer);
        _is_hybrid_fit = false;
        _fit_params_table_model->setOptimizerSupportsMinMax(false);
    }
    else if (opt == STR_MP_FIT)
    {
        _optimizer = &_mpfit_optimizer;
        _param_fit_routine.set_optimizer(_optimizer);
        _is_hybrid_fit = false;
        _fit_params_table_model->setOptimizerSupportsMinMax(true);
    }
    else if (opt == STR_HYBRID_MP_FIT)
    {
        _optimizer = &_mpfit_optimizer;
        _is_hybrid_fit = true;
        _hybrid_fit_routine.set_optimizer(_optimizer);
        _fit_params_table_model->setOptimizerSupportsMinMax(true);
    }
    _optimizer_widget->setOptimizer(opt, *_optimizer);
}


//---------------------------------------------------------------------------

void FittingDialog::setSpectra(data_struct::Spectra<double>* spectra, ArrayDr energy)
{
    _ev = energy;
    if (spectra != nullptr)
    {
        _int_spec = *spectra;
        data_struct::Range energy_range = data_struct::get_energy_range(_int_spec.rows(), &(_out_fit_params));
        _energy_range.min = 0;
        _energy_range.max = _int_spec.size() - 1;

        _spectra_background = snip_background<double>((Spectra<double>*)&_int_spec,
                _out_fit_params[STR_ENERGY_OFFSET].value,
                _out_fit_params[STR_ENERGY_SLOPE].value,
                _out_fit_params[STR_ENERGY_QUADRATIC].value,
                _out_fit_params[STR_SNIP_WIDTH].value,
                energy_range.min,
                energy_range.max);
        
        
        for (int i = 0; i < energy_range.min; i++)
        {
            _int_spec[i] = 0.0;
            _spectra_background[i] = 0.0;
        }
        for (int i = energy_range.max; i < _int_spec.rows(); i++)
        {
            _int_spec[i] = 0.0;
            _spectra_background[i] = 0.0;
        }

        _spectra_widget->append_spectra(DEF_STR_INT_SPECTRA, &_int_spec, &_ev);
        _spectra_widget->append_spectra(DEF_STR_BACK_SPECTRA, &_spectra_background, (data_struct::Spectra<double>*) & _ev);

        //get original back
        _int_spec = *spectra;
    }
}

//---------------------------------------------------------------------------

void FittingDialog::setFitSpectra(data_struct::Spectra<double>* spectra)
{
    _spectra_widget->append_spectra(DEF_STR_FIT_INT_SPECTRA, spectra, &_ev);
}

//---------------------------------------------------------------------------

void FittingDialog::setElementsToFit(data_struct::Fit_Element_Map_Dict<double>* elements_to_fit)
{
    _elements_to_fit = elements_to_fit;
    if (_elements_to_fit != nullptr)
    {
       
        data_struct::Spectra<double> fit_spec = _model.model_spectrum(&_out_fit_params, _elements_to_fit, &_labeled_spectras, _energy_range);
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

//---------------------------------------------------------------------------

data_struct::Spectra<double> FittingDialog::get_fit_spectra(std::unordered_map<std::string, ArrayDr>* labeled_spectras)
{
    (*labeled_spectras) = _labeled_spectras;
    return _new_fit_spec;
}

//---------------------------------------------------------------------------

void FittingDialog::setDisplayRange(QString wmin, QString wmax, QString hmin, QString hmax)
{
    _spectra_widget->setDisplayRange(wmin, wmax, hmin, hmax);
}

//---------------------------------------------------------------------------

void FittingDialog::onAccepted()
{
	_accepted = true;
	_btn_accept->setEnabled(false);
	close();
}

//---------------------------------------------------------------------------

void FittingDialog::onCancel()
{
    if (_running)
    {
        _canceled = true;
    }
    else
    {
        close();
    }
}


//---------------------------------------------------------------------------

void FittingDialog::onUpdateSelected()
{
    QModelIndexList selectedRows = _new_fit_params_table->selectionModel()->selectedIndexes();
    
    for (int i = selectedRows.count() - 1; i >= 0; i--)
    {
        _fit_params_table_model->setDataValueForRow(selectedRows[i].row(), _new_fit_params_table_model->getDataValueForRow(selectedRows[i].row()));
    }
    _fit_params_table_model->updateAll();
}

//---------------------------------------------------------------------------

void FittingDialog::runProcessing()
{
    if (_elements_to_fit != nullptr && _int_spec.size() > 0)
    {
        _running = true;
        _btn_run->setEnabled(false);

        _tabWidget->setCurrentIndex(OUTPUT_INDEX);
        QCoreApplication::processEvents();
        if (_new_fit_spec.size() > 0)
        {
            setFitSpectra(&_new_fit_spec);
        }

        bool use_weights = _optimizer_widget->useWeights();

        _optimizer_widget->updateOptimizerOptions(*_optimizer);

        //data_struct::Spectra s1 = _integrated_spectra.sub_spectra(energy_range);

        //Fitting routines
        _out_fit_params = _fit_params_table_model->getFitParams();

        //reset model fit parameters to defaults
        _model.reset_to_default_fit_params();
        //Update fit parameters by override values
        _model.update_fit_params_values(&_out_fit_params);
        _model.update_and_add_fit_params_values_gt_zero(&_element_fit_params);

        _energy_range = data_struct::get_energy_range(_int_spec.rows(), &(_out_fit_params));

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
                _outcome = _hybrid_fit_routine.fit_spectra_parameters(&_model, &_int_spec, _elements_to_fit, use_weights, _new_out_fit_params, &cb_func);
            }
            else
            {
                _outcome = _param_fit_routine.fit_spectra_parameters(&_model, &_int_spec, _elements_to_fit, use_weights, _new_out_fit_params, &cb_func);
            }
            
            std::string out_str = optimizer_outcome_to_str(_outcome);
            _le_outcome->setText(QString(out_str.c_str()));

            std::string det_out = _optimizer->get_last_detailed_outcome();
            _le_detailed_outcome->setText(QString(det_out.c_str()));

            if (_new_out_fit_params.contains(STR_RESIDUAL))
            {
                auto res_val = _new_out_fit_params.value(STR_RESIDUAL);
                _le_residual->setText(QString::number(res_val));
            }
            if (_new_out_fit_params.contains(STR_NUM_ITR))
            {
                auto itr_val = _new_out_fit_params.value(STR_NUM_ITR);
                _le_num_itr->setText(QString::number(itr_val));
            }
        }
        catch (int e)
        {
            logI << "Exception Thrown" << std::endl;
            _running = false;
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

        if (false == _canceled)
        {

            _new_fit_params_table_model->setFitParams(_new_out_fit_params);

            //don't want to display element counts, so removing them
            _new_fit_params_table_model->only_keep_these_keys(_out_fit_params);

            _progressBarBlocks->setValue(_total_itr);

            data_struct::Range energy_range_whole;
            energy_range_whole.min = 0;
            energy_range_whole.max = _int_spec.size() - 1;

            _new_fit_spec = _model.model_spectrum(&_new_out_fit_params, _elements_to_fit, &_labeled_spectras, energy_range_whole);

            if (_new_fit_spec.size() == _spectra_background.size())
            {
                _new_fit_spec += _spectra_background;
            }
            
            data_struct::Range energy_range = data_struct::get_energy_range(_new_fit_spec.rows(), &(_out_fit_params));
            for (int i = 0; i < energy_range.min; i++)
            {
                _new_fit_spec[i] = 0.0;
                _spectra_background[i] = 0.0;
            }
            for (int i = energy_range.max; i < _new_fit_spec.rows(); i++)
            {
                _new_fit_spec[i] = 0.0;
                _spectra_background[i] = 0.0;
            }

            for (int i = 0; i < _new_fit_spec.size(); i++)
            {
                if (_new_fit_spec[i] <= 0.0)
                {
                    _new_fit_spec[i] = 0.01;
                }
            }

            _spectra_widget->append_spectra(DEF_STR_NEW_FIT_INT_SPECTRA, &_new_fit_spec, &_ev);
        }
        else
        {
            _le_outcome->setText("Canceled");
        }
        _canceled = false;
        _running = false;
		_btn_accept->setEnabled(true);
        _btn_run->setEnabled(true);
    }
}

//---------------------------------------------------------------------------

void FittingDialog::status_callback(size_t cur_itr, size_t total_itr)
{
    if (_canceled)
    {
        _progressBarBlocks->setValue(_total_itr);
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

//---------------------------------------------------------------------------