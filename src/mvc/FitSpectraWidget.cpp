/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/FitSpectraWidget.h>


#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QDebug>

#include <math.h>

using namespace data_struct::xrf;

/*---------------------------------------------------------------------------*/

FitSpectraWidget::FitSpectraWidget(QWidget* parent) : QWidget(parent)
{

    _fit_thread = nullptr;
    createLayout();

    connect(this, SIGNAL(signal_finished_fit()), this, SLOT(finished_fitting()));
/*
    _action_check_log10 = new QAction("Toggle Log10", this);
    _action_check_log10->setCheckable(true);
    _action_check_log10->setChecked(_display_log10);
    connect(_action_check_log10, SIGNAL(triggered()), this, SLOT(_check_log10()));

    _contextMenu = new QMenu(("Context menu"), this);
    _contextMenu->addAction(_action_check_log10);

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));
            */
}

/*---------------------------------------------------------------------------*/

FitSpectraWidget::~FitSpectraWidget()
{
    if(_fit_thread != nullptr)
    {
        _fit_thread->join();
        delete _fit_thread;
    }
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::createLayout()
{    
    std::vector<std::string> bound_types {"Not Initialized", "Fixed", "Limited Low High", "Limited Low", "Limited High", "Fit"};

    _spectra_widget = new SpectraWidget();

    _fit_params_tab_widget = new QTabWidget();

    fitting::models::Gaussian_Model g_model;

    _fit_params_table_model = new FitParamsTableModel();
    _fit_params_table_model->setFitParams(g_model.fit_parameters());
    ComboBoxDelegate *cbDelegate = new ComboBoxDelegate(bound_types);

    _fit_params_table = new QTableView();
    _fit_params_table->setModel(_fit_params_table_model);
    _fit_params_table->sortByColumn(0, Qt::AscendingOrder);
    _fit_params_table->setItemDelegateForColumn(5, cbDelegate);

    _fit_elements_table_model = new FitParamsTableModel();

    _fit_elements_table = new QTableView();
    _fit_elements_table->setModel(_fit_elements_table_model);
 //   _fit_elements_table->sortByColumn(0, Qt::AscendingOrder);
    _fit_elements_table->setItemDelegateForColumn(5, cbDelegate);

    _btn_fit_spectra = new QPushButton("Fit Spectra");
    connect(_btn_fit_spectra, &QPushButton::released, this, &FitSpectraWidget::Fit_Spectra_Click);

    _btn_model_spectra = new QPushButton("Model Spectra");
    connect(_btn_model_spectra, &QPushButton::released, this, &FitSpectraWidget::Model_Spectra_Click);

    _fit_params_tab_widget->addTab(_fit_params_table, "Fit Parameters");
    _fit_params_tab_widget->addTab(_fit_elements_table, "Fit Elements");

    QLayout* button_layout = new QHBoxLayout();
    button_layout->addWidget(_btn_fit_spectra);
    button_layout->addWidget(_btn_model_spectra);

    QLayout* layout = new QVBoxLayout();
    layout->addWidget(_spectra_widget);
    layout->addWidget(_fit_params_tab_widget);
    layout->addItem(button_layout);
    setLayout(layout);
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::Fit_Spectra_Click()
{
    _btn_fit_spectra->setEnabled(false);
    _btn_model_spectra->setEnabled(false);
    _spectra_widget->remove_spectra("Fit Spectra");
    if(_fit_thread != nullptr)
    {
        _fit_thread->join();
        delete _fit_thread;
    }
    _fit_thread = new std::thread( [this]()
    {

        if(_elements_to_fit != nullptr)
        {
            data_struct::xrf::Fit_Parameters out_fit_params = _fit_params_table_model->getFitParams();

            data_struct::xrf::Spectra fit_spec = _h5_model->fit_integrated_spectra(&out_fit_params, _elements_to_fit);
            _fit_params_table_model->updateFitParams(&out_fit_params);
            _fit_elements_table_model->updateFitParams(&out_fit_params);
            if(fit_spec.size() == _spectra_background.size())
            {
                fit_spec += _spectra_background;
            }
            for(int i=0; i<fit_spec.size(); i++)
            {
             if(fit_spec[i] <= 0.0)
                 fit_spec[i] = 0.1;
            }
            _spectra_widget->append_spectra("Fit Spectra", &fit_spec);
        }
        emit signal_finished_fit();
    });
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::Model_Spectra_Click()
{
    _btn_fit_spectra->setEnabled(false);
    _btn_model_spectra->setEnabled(false);
    _spectra_widget->remove_spectra("Model Spectra");
    if(_fit_thread != nullptr)
    {
        _fit_thread->join();
        delete _fit_thread;
    }
    _fit_thread = new std::thread( [this]()
    {

        if(_elements_to_fit != nullptr)
        {
            fitting::models::Gaussian_Model model;
            //Range of energy in spectra to fit

            data_struct::xrf::Fit_Parameters fit_params;
            data_struct::xrf::Fit_Parameters model_fit_params = _fit_params_table_model->getFitParams();
            data_struct::xrf::Fit_Parameters element_fit_params = _fit_elements_table_model->getFitParams();
            fit_params.append_and_update(&model_fit_params);
            fit_params.append_and_update(&element_fit_params);
/*
            fitting::models::Range energy_range = data_struct::xrf::get_energy_range(sub_struct->fit_params_override_dict.min_energy,
                                                                                     sub_struct->fit_params_override_dict.max_energy,
                                                                                     _spectra_background.size(),
                                                                                     model_fit_params[fitting::models::STR_ENERGY_OFFSET].value,
                                                                                     model_fit_params[fitting::models::STR_ENERGY_SLOPE].value);
*/
            fitting::models::Range energy_range;
            energy_range.min = 0;
            energy_range.max = _spectra_background.size() -1;


            data_struct::xrf::Spectra fit_spec = model.model_spectrum(&fit_params, _elements_to_fit, energy_range);

            _fit_params_table_model->updateFitParams(&fit_params);
            _fit_elements_table_model->updateFitParams(&fit_params);
            if(fit_spec.size() == _spectra_background.size())
            {
                fit_spec += _spectra_background;
            }
            for(int i=0; i<fit_spec.size(); i++)
            {
             if(fit_spec[i] <= 0.0)
                 fit_spec[i] = 0.1;
            }
            _spectra_widget->append_spectra("Model Spectra", &fit_spec);
        }
        emit signal_finished_fit();
    });
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::finished_fitting()
{
    _btn_fit_spectra->setEnabled(true);
    _btn_model_spectra->setEnabled(true);
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::setModels(data_struct::xrf::Fit_Parameters* fit_params,
                                    data_struct::xrf::Fit_Element_Map_Dict *elements_to_fit,
                                    MapsH5Model* h5_model)
{
    _h5_model = h5_model;
    _elements_to_fit = elements_to_fit;

    data_struct::xrf::Fit_Parameters element_fit_params;
    for(auto &itr : *_elements_to_fit)
    {
        element_fit_params.add_parameter(itr.first, data_struct::xrf::Fit_Param(itr.first, 0.00001));
    }

    _fit_elements_table_model->setFitParams(element_fit_params);

    _fit_params_table_model->updateFitParams(fit_params);

    _spectra_widget->append_spectra("Integrated Spectra", _h5_model->getIntegratedSpectra());

    _spectra_background = snip_background(_h5_model->getIntegratedSpectra(),
                                         fit_params->at(fitting::models::STR_ENERGY_OFFSET).value,
                                         fit_params->at(fitting::models::STR_ENERGY_SLOPE).value,
                                         fit_params->at(fitting::models::STR_ENERGY_QUADRATIC).value,
                                         0, //spectral binning
                                         fit_params->at(STR_SNIP_WIDTH).value,
                                         0, //spectra energy start range
                                         _h5_model->getIntegratedSpectra()->size());

    _spectra_widget->append_spectra("Background", &_spectra_background);

}
