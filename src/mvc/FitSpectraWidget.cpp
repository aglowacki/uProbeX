/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/FitSpectraWidget.h>
#include <future>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QGridLayout>
#include <QItemSelectionModel>
#include <QDebug>

#include <math.h>

using namespace data_struct;

QString STR_LM_FIT = "Levenberg-Marquardt Fit";
QString STR_MP_FIT = "MP Fit";

/*---------------------------------------------------------------------------*/

FitSpectraWidget::FitSpectraWidget(QWidget* parent) : QWidget(parent)
{

    _fit_thread = nullptr;
    createLayout();

    _spectra_background.resize(10);
    _spectra_background.Zero(10);
    connect(this, SIGNAL(signal_finished_fit()), this, SLOT(finished_fitting()));


    _set_fit_params_bounds_menu = new QMenu("Set Bounds");
    QAction* action_bounds = _set_fit_params_bounds_menu->addAction("FIXED");
    connect(action_bounds, SIGNAL(triggered(bool)), this, SLOT(set_fit_params_bounds_fixed(bool)));

    action_bounds = _set_fit_params_bounds_menu->addAction("FIT");
    connect(action_bounds, SIGNAL(triggered(bool)), this, SLOT(set_fit_params_bounds_fit(bool)));

 //FIXED=1, LIMITED_LO_HI=2, LIMITED_LO=3, LIMITED_HI=4, FIT=5};
    _fit_param_contextMenu = new QMenu(("Context menu"), this);
    _fit_param_contextMenu->addMenu(_set_fit_params_bounds_menu);

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
    _fit_params_table->setItemDelegateForColumn(2, cbDelegate);
    _fit_params_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    _fit_params_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_fit_params_table,
            SIGNAL(customContextMenuRequested(QPoint)),
            this,
            SLOT(fit_params_customMenuRequested(QPoint)));

    _fit_elements_table_model = new FitElementsTableModel();

    _fit_elements_table = new QTreeView();
    _fit_elements_table->setModel(_fit_elements_table_model);
    _fit_elements_table->sortByColumn(0);
    //_fit_elements_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QItemSelectionModel* mod = _fit_elements_table->selectionModel();
    connect(mod,
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(element_selection_changed(QModelIndex,QModelIndex)));

    _btn_fit_spectra = new QPushButton("Fit Spectra");
    connect(_btn_fit_spectra, &QPushButton::released, this, &FitSpectraWidget::Fit_Spectra_Click);

    _btn_model_spectra = new QPushButton("Model Spectra");
    connect(_btn_model_spectra, &QPushButton::released, this, &FitSpectraWidget::Model_Spectra_Click);

    _fit_params_tab_widget->addTab(_fit_params_table, "Fit Parameters");
    _fit_params_tab_widget->addTab(_fit_elements_table, "Fit Elements");

    _cb_opttimizer = new QComboBox();
    _cb_opttimizer->addItem(STR_LM_FIT);
    _cb_opttimizer->addItem(STR_MP_FIT);
    connect(_cb_opttimizer,
            SIGNAL(currentIndexChanged(QString)),
            this,
            SLOT(optimizer_changed(QString)));

    _chk_auto_model = new QCheckBox("Auto Update Model Spectra on Data Change");
    _chk_auto_model->setChecked(false);
    connect(_chk_auto_model,
            SIGNAL(stateChanged(int)),
            this,
            SLOT(check_auto_model(int)));

    QGridLayout *grid_layout = new QGridLayout();
    grid_layout->addWidget(_cb_opttimizer, 0, 0);
    grid_layout->addWidget(_btn_fit_spectra, 0, 1);

    grid_layout->addWidget(_chk_auto_model, 1, 0);
    grid_layout->addWidget(_btn_model_spectra, 1, 1);

    QLayout* layout = new QVBoxLayout();
    layout->addWidget(_spectra_widget);
    layout->addWidget(_fit_params_tab_widget);
    layout->addItem(grid_layout);
    setLayout(layout);
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::Fit_Spectra_Click()
{
    _btn_fit_spectra->setEnabled(false);
    _btn_model_spectra->setEnabled(false);
    if(_fit_thread != nullptr)
    {
        _fit_thread->join();
        delete _fit_thread;
        _fit_thread = nullptr;
    }
    //_fit_thread = new std::thread( [this]()
    std::async( [this]()
    {

        if(_elements_to_fit != nullptr)
        {
            fitting::optimizers::LMFit_Optimizer lmfit_optimizer;
            fitting::optimizers::MPFit_Optimizer mpfit_optimizer;
            fitting::models::Gaussian_Model model;


            data_struct::Fit_Parameters out_fit_params = _fit_params_table_model->getFitParams();

            const data_struct::Spectra *int_spectra = _h5_model->getIntegratedSpectra();

            //Range of energy in spectra to fit
            fitting::models::Range energy_range;
            energy_range.min = 0;
            energy_range.max = int_spectra->rows() -1;

            //data_struct::Spectra s1 = _integrated_spectra.sub_spectra(energy_range);

            //Fitting routines
            fitting::routines::Param_Optimized_Fit_Routine fit_routine;
            fit_routine.set_update_coherent_amplitude_on_fit(false);

            if(_cb_opttimizer->currentText() == STR_LM_FIT)
            {
                fit_routine.set_optimizer(&lmfit_optimizer);
            }
            else if(_cb_opttimizer->currentText() == STR_MP_FIT)
            {
                fit_routine.set_optimizer(&mpfit_optimizer);
            }

            //reset model fit parameters to defaults
            model.reset_to_default_fit_params();
            //Update fit parameters by override values
            model.update_fit_params_values(&out_fit_params);

            //model.set_fit_params_preset(fitting::models::Fit_Params_Preset::BATCH_FIT_WITH_TAILS);

            //Initialize the fit routine
            fit_routine.initialize(&model, _elements_to_fit, energy_range);
            //Fit the spectra saving the element counts in element_fit_count_dict
            out_fit_params = fit_routine.fit_spectra_parameters(&model, int_spectra, _elements_to_fit);
            //out_fit_params = fit_routine.fit_spectra_parameters(&model, &s1, _elements_to_fit);

            disconnect(_fit_params_table_model,
                       SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                       this,
                       SLOT(Model_Spectra_Val_Change(QModelIndex,QModelIndex,QVector<int>)));

            disconnect(_fit_elements_table_model,
                       SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                       this,
                       SLOT(Model_Spectra_Val_Change(QModelIndex,QModelIndex,QVector<int>)));

            _fit_params_table_model->updateFitParams(&out_fit_params);

            _fit_elements_table_model->updateElementValues(&out_fit_params);

            if(_chk_auto_model->checkState() == Qt::Checked)
            {
                connect(_fit_params_table_model,
                        SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                        this,
                        SLOT(Model_Spectra_Val_Change(QModelIndex,QModelIndex,QVector<int>)));
                connect(_fit_elements_table_model,
                        SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                        this,
                        SLOT(Model_Spectra_Val_Change(QModelIndex,QModelIndex,QVector<int>)));

                Model_Spectra_Click();
            }

            _spectra_background = snip_background(_h5_model->getIntegratedSpectra(),
                                                 out_fit_params.at(STR_ENERGY_OFFSET).value,
                                                 out_fit_params.at(STR_ENERGY_SLOPE).value,
                                                 out_fit_params.at(STR_ENERGY_QUADRATIC).value,
                                                 0, //spectral binning
                                                 out_fit_params.at(STR_SNIP_WIDTH).value,
                                                 0, //spectra energy start range
                                                 _h5_model->getIntegratedSpectra()->rows());

            data_struct::ArrayXr energy = data_struct::ArrayXr::LinSpaced(energy_range.count(), energy_range.min, energy_range.max);
            data_struct::ArrayXr ev = out_fit_params.at(STR_ENERGY_OFFSET).value + energy * out_fit_params.at(STR_ENERGY_SLOPE).value + pow(energy, (real_t)2.0) * out_fit_params.at(STR_ENERGY_QUADRATIC).value;

            _spectra_widget->append_spectra("Background", &_spectra_background, (data_struct::Spectra*)&ev);

            data_struct::Spectra fit_spec = model.model_spectrum(&out_fit_params, _elements_to_fit, energy_range);

            if(fit_spec.size() == _spectra_background.size())
            {
                fit_spec += _spectra_background;
            }
            for(int i=0; i<fit_spec.size(); i++)
            {
             if(fit_spec[i] <= 0.0)
                 fit_spec[i] = 0.1;
            }

            _spectra_widget->append_spectra("Fit Spectra", &fit_spec, (data_struct::Spectra*)&ev);
        }
        emit signal_finished_fit();
    }
    );
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::Model_Spectra_Val_Change(QModelIndex,QModelIndex,QVector<int>)
{
    Model_Spectra_Click();
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::Model_Spectra_Click()
{
    _btn_fit_spectra->setEnabled(false);
    _btn_model_spectra->setEnabled(false);
    if(_fit_thread != nullptr)
    {
        _fit_thread->join();
        delete _fit_thread;
        _fit_thread = nullptr;
    }
//    _fit_thread = new std::thread( [this]()
    {

        if(_elements_to_fit != nullptr)
        {
            fitting::models::Gaussian_Model model;
            //Range of energy in spectra to fit

            data_struct::Fit_Parameters fit_params;
            data_struct::Fit_Parameters model_fit_params = _fit_params_table_model->getFitParams();
            data_struct::Fit_Parameters element_fit_params = _fit_elements_table_model->getAsFitParams();
            fit_params.append_and_update(&model_fit_params);
            fit_params.append_and_update(&element_fit_params);
/*
            fitting::models::Range energy_range = data_struct::get_energy_range(sub_struct->fit_params_override_dict.min_energy,
                                                                                     sub_struct->fit_params_override_dict.max_energy,
                                                                                     _spectra_background.size(),
                                                                                     model_fit_params[STR_ENERGY_OFFSET].value,
                                                                                     model_fit_params[STR_ENERGY_SLOPE].value);
*/
            fitting::models::Range energy_range;
            energy_range.min = 0;
            energy_range.max = _spectra_background.size() -1;


            data_struct::Spectra fit_spec = model.model_spectrum(&fit_params, _elements_to_fit, energy_range);


            _spectra_background = snip_background(_h5_model->getIntegratedSpectra(),
                                                 fit_params.at(STR_ENERGY_OFFSET).value,
                                                 fit_params.at(STR_ENERGY_SLOPE).value,
                                                 fit_params.at(STR_ENERGY_QUADRATIC).value,
                                                 0, //spectral binning
                                                 fit_params.at(STR_SNIP_WIDTH).value,
                                                 0, //spectra energy start range
                                                 _h5_model->getIntegratedSpectra()->size());

            data_struct::ArrayXr energy = data_struct::ArrayXr::LinSpaced(energy_range.count(), energy_range.min, energy_range.max);
            data_struct::ArrayXr ev = fit_params.at(STR_ENERGY_OFFSET).value + energy * fit_params.at(STR_ENERGY_SLOPE).value + pow(energy, (real_t)2.0) * fit_params.at(STR_ENERGY_QUADRATIC).value;

            //_spectra_widget->append_spectra("Background", &_spectra_background, (data_struct::Spectra*)&ev);


//            _fit_params_table_model->updateFitParams(&fit_params);
//            _fit_elements_table_model->updateElementValues(&fit_params);

            if(fit_spec.size() == _spectra_background.size())
            {
                fit_spec += _spectra_background;
            }
            for(int i=0; i<fit_spec.size(); i++)
            {
             if(fit_spec[i] <= 0.0)
                 fit_spec[i] = 0.1;
            }
            _spectra_widget->append_spectra("Model Spectra", &fit_spec, (data_struct::Spectra*)&ev);
        }
        emit signal_finished_fit();
    }
    //);
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::finished_fitting()
{
    _btn_fit_spectra->setEnabled(true);
    if(_chk_auto_model->checkState() == Qt::Checked)
        _btn_model_spectra->setEnabled(false);
    else
        _btn_model_spectra->setEnabled(true);
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::fit_params_customMenuRequested(QPoint pos)
{
    _fit_param_contextMenu->exec(_fit_params_table->mapToGlobal(pos));
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::set_fit_params_bounds(data_struct::E_Bound_Type e_type)
{
    QModelIndexList selected_rows = _fit_params_table->selectionModel()->selectedIndexes();
    for(QModelIndex index : selected_rows)
    {
        _fit_params_table_model->setDataFitBounds(index, e_type);
    }
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::check_auto_model(int state)
{
    if(state == Qt::Checked)
    {
        _btn_model_spectra->setEnabled(false);
        connect(_fit_params_table_model,
                SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                this,
                SLOT(Model_Spectra_Val_Change(QModelIndex,QModelIndex,QVector<int>)));

        connect(_fit_elements_table_model,
                SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                this,
                SLOT(Model_Spectra_Val_Change(QModelIndex,QModelIndex,QVector<int>)));
    }
    else
    {
        _btn_model_spectra->setEnabled(true);
        disconnect(_fit_params_table_model,
                   SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                   this,
                   SLOT(Model_Spectra_Val_Change(QModelIndex,QModelIndex,QVector<int>)));

        disconnect(_fit_elements_table_model,
                   SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                   this,
                   SLOT(Model_Spectra_Val_Change(QModelIndex,QModelIndex,QVector<int>)));
    }
}

/*---------------------------------------------------------------------------*/

//void FitSpectraWidget::element_clicked(QModelIndex index)
//{
//    data_struct::Fit_Element_Map *element = _fit_elements_table_model->getElementByIndex(index);
//    if(element != nullptr)
//    {
//        QString name = QString(element->full_name().c_str());
//        float energy_offset = _fit_params_table_model->getFitParamValue(STR_ENERGY_OFFSET);
//        float energy_slope = _fit_params_table_model->getFitParamValue(STR_ENERGY_SLOPE);
//        //int x_val = int(((element->center() / 2.0 / 1000.0) - energy_offset) / energy_slope);
//        int left_roi = int(((element->center() - element->width() / 2.0 / 1000.0) - energy_offset) / energy_slope);
//        int right_roi = int(((element->center() + element->width() / 2.0 / 1000.0) - energy_offset) / energy_slope);
//        int x_val = ( (right_roi - left_roi) / 2.0 ) + left_roi;

//        //emit(vertical_element_line_changed(x_val, name));
//        _spectra_widget->set_vertical_line(x_val, name);
//    }
//}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::element_selection_changed(QModelIndex current, QModelIndex previous)
{
    _spectra_widget->set_element_lines(_fit_elements_table_model->getElementByIndex(current));

//    data_struct::Fit_Element_Map *element = _fit_elements_table_model->getElementByIndex(current);
//    if(element != nullptr)
//    {
//        QString name = QString(element->full_name().c_str());
//        float energy_offset = _fit_params_table_model->getFitParamValue(STR_ENERGY_OFFSET);
//        float energy_slope = _fit_params_table_model->getFitParamValue(STR_ENERGY_SLOPE);
//        //int x_val = int(((element->center() / 2.0 / 1000.0) - energy_offset) / energy_slope);
//        int left_roi = int(((element->center() - element->width() / 2.0 / 1000.0) - energy_offset) / energy_slope);
//        int right_roi = int(((element->center() + element->width() / 2.0 / 1000.0) - energy_offset) / energy_slope);
//        int x_val = ( (right_roi - left_roi) / 2.0 ) + left_roi;

//        _spectra_widget->set_vertical_line(x_val, name);
//    }
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::optimizer_changed(QString val)
{
    if(val == STR_LM_FIT)
        _fit_params_table_model->setOptimizerSupportsMinMax(false);
    else if(val == STR_MP_FIT)
        _fit_params_table_model->setOptimizerSupportsMinMax(true);

    _fit_params_table->resizeColumnToContents(0);

}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::setModels(MapsH5Model* h5_model,
                                 data_struct::Fit_Parameters* fit_params,
                                 data_struct::Fit_Element_Map_Dict *elements_to_fit)
{
    _h5_model = h5_model;
    _elements_to_fit = elements_to_fit;
    data_struct::Fit_Parameters element_fit_params;

    if(_elements_to_fit != nullptr)
    {
        for(auto &itr : *_elements_to_fit)
        {
            element_fit_params.add_parameter(data_struct::Fit_Param(itr.first, 0.00001, data_struct::E_Bound_Type::FIT));
        }
    }
    else if(_h5_model != nullptr)
    {
        _elements_to_fit = new data_struct::Fit_Element_Map_Dict();
        std::vector<std::string> count_names = _h5_model->count_names();
        for(std::string &itr : count_names)
        {
            data_struct::Fit_Element_Map* fit_element = gen_element_map(itr);
            if(fit_element != nullptr)
            {
                (*_elements_to_fit)[itr] = fit_element;
                element_fit_params.add_parameter(data_struct::Fit_Param(itr, 0.00001, data_struct::E_Bound_Type::FIT));
            }
        }
    }

    _fit_elements_table_model->updateFitElements(elements_to_fit);

    _fit_params_table_model->updateFitParams(fit_params);

    if(_h5_model != nullptr && _h5_model->is_integrated_spectra_loaded())
    {
        const data_struct::Spectra* int_spec = _h5_model->getIntegratedSpectra();

        fitting::models::Range energy_range;
        energy_range.min = 0;
        energy_range.max = int_spec->size() -1;

        data_struct::ArrayXr energy = data_struct::ArrayXr::LinSpaced(energy_range.count(), energy_range.min, energy_range.max);
        if(fit_params != nullptr)
        {
            data_struct::ArrayXr ev = fit_params->at(STR_ENERGY_OFFSET).value + energy * fit_params->at(STR_ENERGY_SLOPE).value + pow(energy, (real_t)2.0) * fit_params->at(STR_ENERGY_QUADRATIC).value;

            _spectra_widget->append_spectra("Integrated Spectra", int_spec, (data_struct::Spectra*)&ev);

            _spectra_background = snip_background(int_spec,
                                                 fit_params->at(STR_ENERGY_OFFSET).value,
                                                 fit_params->at(STR_ENERGY_SLOPE).value,
                                                 fit_params->at(STR_ENERGY_QUADRATIC).value,
                                                 0, //spectral binning
                                                 fit_params->at(STR_SNIP_WIDTH).value,
                                                 0, //spectra energy start range
                                                 int_spec->size()-1);

            _spectra_widget->append_spectra("Background", &_spectra_background, (data_struct::Spectra*)&ev);
            _spectra_widget->setXLabel("Energy (kEv)");
        }
        else
        {
            _spectra_widget->append_spectra("Integrated Spectra", int_spec, (data_struct::Spectra*)&energy);
            _spectra_widget->setXLabel("Channels");
        }
    }
}
