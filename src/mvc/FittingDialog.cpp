/*-----------------------------------------------------------------------------
 * Copyright (c) 2020, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/FittingDialog.h>
#include <QApplication>
#include <QGroupBox>

/*---------------------------------------------------------------------------*/

FittingDialog::FittingDialog(QWidget *parent) : QDialog(parent)
{
    _accepted = false;
    _canceled = false;
    _running = false;
    _elements_to_fit = nullptr;
    _int_spec = nullptr;
    _fit_routine.set_update_coherent_amplitude_on_fit(false);
    createLayout();
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

void FittingDialog::createLayout()
{
    std::vector<std::string> bound_types{ "Not Initialized", "Fixed", "Limited Low High", "Limited Low", "Limited High", "Fit" };

    _progressBarBlocks = new QProgressBar();
    _progressBarBlocks->setRange(0,100);
    _progressBarFiles = new QProgressBar();
    _progressBarFiles->setRange(0, 100);

    ComboBoxDelegate* cbDelegate = new ComboBoxDelegate(bound_types);

    _fit_params_table_model = new FitParamsTableModel();

    _new_fit_params_table_model = new FitParamsTableModel();
    _new_fit_params_table_model->setEditable(false);

    _fit_params_table = new QTableView();
    _fit_params_table->setModel(_fit_params_table_model);
    _fit_params_table->sortByColumn(0, Qt::AscendingOrder);
    _fit_params_table->setItemDelegateForColumn(2, cbDelegate);
    _fit_params_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        
    _new_fit_params_table = new QTableView();
    _new_fit_params_table->setModel(_new_fit_params_table_model);
    _new_fit_params_table->sortByColumn(0, Qt::AscendingOrder);
    _new_fit_params_table->setItemDelegateForColumn(2, cbDelegate);
    _new_fit_params_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    

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

    QHBoxLayout* hbox_tables = new QHBoxLayout();
    hbox_tables->addWidget(_fit_params_table);
    hbox_tables->addWidget(_new_fit_params_table);

    QHBoxLayout* hbox_progresss_blocks = new QHBoxLayout();
    hbox_progresss_blocks->addWidget(new QLabel("Current Iteration:"));
    hbox_progresss_blocks->addWidget(_progressBarBlocks);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addItem(hbox_tables);
    layout->addItem(buttonlayout);
    layout->addItem(hbox_progresss_blocks);

    setLayout(layout);

}

/*---------------------------------------------------------------------------*/

void FittingDialog::updateFitParams(data_struct::Fit_Parameters out_fit_params, data_struct::Fit_Parameters element_fit_params)
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
        _fit_routine.set_optimizer(&_lmfit_optimizer);
        _fit_params_table_model->setOptimizerSupportsMinMax(false);
    }
    else if (opt == STR_MP_FIT)
    {
        _fit_routine.set_optimizer(&_mpfit_optimizer);
        _fit_params_table_model->setOptimizerSupportsMinMax(true);
    }
}

/*---------------------------------------------------------------------------*/

void FittingDialog::setSpectra(data_struct::Spectra* spectra)
{
    _int_spec = spectra;
}

/*---------------------------------------------------------------------------*/

void FittingDialog::setElementsToFit(data_struct::Fit_Element_Map_Dict* elements_to_fit)
{
    _elements_to_fit = elements_to_fit;
}

/*---------------------------------------------------------------------------*/

data_struct::Spectra FittingDialog::get_fit_spectra()
{
	return _model.model_spectrum(&_new_out_fit_params, _elements_to_fit, _energy_range);
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
    if (_elements_to_fit != nullptr && _int_spec != nullptr)
    {
        _running = true;
        _btn_run->setEnabled(false);

        //Range of energy in spectra to fit
        _energy_range;
		_energy_range.min = 0;
		_energy_range.max = _int_spec->rows() - 1;

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
        _fit_routine.initialize(&_model, _elements_to_fit, _energy_range);
        //Fit the spectra saving the element counts in element_fit_count_dict
        // single threaded
        //out_fit_params = fit_routine.fit_spectra_parameters(&_model, int_spectra, _elements_to_fit);

        // use background thread to not freeze up the gui
        Callback_Func_Status_Def cb_func = std::bind(&FittingDialog::status_callback, this, std::placeholders::_1, std::placeholders::_2);
        try
        {
            _new_out_fit_params = _fit_routine.fit_spectra_parameters(&_model, _int_spec, _elements_to_fit, &cb_func);
        }
        catch (int e)
        {
            logI << "Exception Thrown" << std::endl;
            _running = false;
            close();
            return;
        }
        /*
        //std::future<data_struct::Fit_Parameters> ret = global_threadpool.enqueue(&fitting::routines::Param_Optimized_Fit_Routine::fit_spectra_parameters, _fit_routine, &_model, _int_spec, _elements_to_fit, &_cb_func);
        std::future<data_struct::Fit_Parameters> ret = global_threadpool.enqueue([&](){
            try
            {
                return _fit_routine.fit_spectra_parameters(&_model, _int_spec, _elements_to_fit, &cb_func);
            }
            catch (int e)
            {
                data_struct::Fit_Parameters f;
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

        _running = false;
		_btn_accept->setEnabled(true);
        //_btn_run->setEnabled(true);
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