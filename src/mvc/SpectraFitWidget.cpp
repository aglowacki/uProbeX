/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/SpectraFitWidget.h>
#include <QApplication>
#include <QGroupBox>
#include <data_struct/analysis_job.h>
#include <core/process_whole.h>

/*---------------------------------------------------------------------------*/

SpectraFitWidget::SpectraFitWidget(QWidget *parent) : QWidget(parent)
{
    _accepted = false;
    _elements_to_fit = nullptr;
    _int_spec = nullptr;
    _fit_routine.set_update_coherent_amplitude_on_fit(false);
    createLayout();
}

/*---------------------------------------------------------------------------*/

SpectraFitWidget::~SpectraFitWidget()
{

 
}

/*---------------------------------------------------------------------------*/

void SpectraFitWidget::createLayout()
{
    _progressBarBlocks = new QProgressBar();
    _progressBarBlocks->setRange(0,100);
    _progressBarFiles = new QProgressBar();
    _progressBarFiles->setRange(0, 100);

    _btn_run = new QPushButton("Run");
    connect(_btn_run, &QPushButton::released, this, &SpectraFitWidget::runProcessing);

    _btn_accept = new QPushButton("Accept");
    connect(_btn_accept, &QPushButton::released, this, &SpectraFitWidget::runProcessing);
    _btn_accept->setEnabled(false);

    _btn_cancel = new QPushButton("Cancel");
    connect(_btn_cancel, &QPushButton::released, this, &SpectraFitWidget::close);





    QHBoxLayout* buttonlayout = new QHBoxLayout();
    buttonlayout->addWidget(_btn_run);
    buttonlayout->addWidget(_btn_accept);
    buttonlayout->addWidget(_btn_cancel);

    QHBoxLayout* hbox_progresss_blocks = new QHBoxLayout();
    hbox_progresss_blocks->addWidget(new QLabel("Current Iteration:"));
    hbox_progresss_blocks->addWidget(_progressBarBlocks);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addItem(buttonlayout);
    layout->addItem(hbox_progresss_blocks);

    setLayout(layout);

}

/*---------------------------------------------------------------------------*/

void SpectraFitWidget::updateFitParams(data_struct::Fit_Parameters out_fit_params, data_struct::Fit_Parameters element_fit_params)
{
    _out_fit_params = out_fit_params;
    _element_fit_params = element_fit_params;
}

/*---------------------------------------------------------------------------*/

void SpectraFitWidget::setOptimizer(QString opt)
{
    if (opt == STR_LM_FIT)
    {
        _fit_routine.set_optimizer(&_lmfit_optimizer);
    }
    else if (opt == STR_MP_FIT)
    {
        _fit_routine.set_optimizer(&_mpfit_optimizer);
    }
}

/*---------------------------------------------------------------------------*/

void SpectraFitWidget::setSpectra(data_struct::Spectra* spectra)
{
    _int_spec = spectra;
}

/*---------------------------------------------------------------------------*/

void SpectraFitWidget::setElementsToFit(data_struct::Fit_Element_Map_Dict* elements_to_fit)
{
    _elements_to_fit = elements_to_fit;
}
/*---------------------------------------------------------------------------*/

void SpectraFitWidget::runProcessing()
{
    if (_elements_to_fit != nullptr && _int_spec != nullptr)
    {

        _btn_run->setEnabled(false);

        fitting::models::Gaussian_Model model;

        //Range of energy in spectra to fit
        fitting::models::Range energy_range;
        energy_range.min = 0;
        energy_range.max = _int_spec->rows() - 1;

        //data_struct::Spectra s1 = _integrated_spectra.sub_spectra(energy_range);

        //Fitting routines

        //reset model fit parameters to defaults
        model.reset_to_default_fit_params();
        //Update fit parameters by override values
        model.update_fit_params_values(&_out_fit_params);
        model.update_and_add_fit_params_values_gt_zero(&_element_fit_params);

        //model.set_fit_params_preset(fitting::models::Fit_Params_Preset::BATCH_FIT_WITH_TAILS);

        //Initialize the fit routine
        _fit_routine.initialize(&model, _elements_to_fit, energy_range);
        //Fit the spectra saving the element counts in element_fit_count_dict
        // single threaded
        //out_fit_params = fit_routine.fit_spectra_parameters(&model, int_spectra, _elements_to_fit);
        // use background thread to not freeze up the gui
        Callback_Func_Status_Def cb_func = std::bind(&SpectraFitWidget::status_callback, this, std::placeholders::_1, std::placeholders::_2);

        std::future<data_struct::Fit_Parameters> ret = global_threadpool.enqueue(&fitting::routines::Param_Optimized_Fit_Routine::fit_spectra_parameters, _fit_routine, &model, (Spectra*)_int_spec, _elements_to_fit, &cb_func);

        while (ret._Is_ready() == false)
        {
            QCoreApplication::processEvents();
        }
        _new_out_fit_params = ret.get();


        _btn_run->setEnabled(true);
    }
}

void SpectraFitWidget::status_callback(size_t cur_itr, size_t total_itr)
{
    if (_total_itr != total_itr)
    {
        _total_itr = total_itr;
        _progressBarBlocks->setRange(0, _total_itr);
    }

    _progressBarBlocks->setValue(cur_itr);
    
    QCoreApplication::processEvents();
}