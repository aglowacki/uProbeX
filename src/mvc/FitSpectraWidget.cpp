/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/FitSpectraWidget.h>
#include <future>
#include <QCoreApplication>
#include <QVBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QItemSelectionModel>
#include <QSplitter>
#include <QMessageBox>
#include <QSpacerItem>
#include <math.h>

#include "data_struct/element_info.h"
#include "fitting//optimizers/lmfit_optimizer.h"
#include <mvc/NumericPrecDelegate.h>
#include <preferences/Preferences.h>

using namespace data_struct;

/*---------------------------------------------------------------------------*/

FitSpectraWidget::FitSpectraWidget(QWidget* parent) : QWidget(parent)
{

    _cb_add_elements = new QComboBox();
    _cb_add_shell = new QComboBox();
    _cb_pileup_elements = new QComboBox();
    _cb_detector_element = new QComboBox();
    _chk_is_pileup = new QCheckBox("Pile Up");
    _detector_element = "Si"; // default to Si detector if not found in param override
    _int_spec = nullptr;
    _elements_to_fit = nullptr;
    _fitting_dialog = nullptr;
	_param_override = nullptr;
    _fit_spec.setZero(2048);
    _showDetailedFitSpec = Preferences::inst()->getValue(STR_PFR_DETAILED_FIT_SPEC).toBool();
	_showFitIntSpec = Preferences::inst()->getValue(STR_PFR_SHOW_FIT_INT_SPEC).toBool();
    for(const std::string& e : data_struct::Element_Symbols)
    {
        _cb_add_elements->addItem(QString::fromStdString(e));
        _cb_pileup_elements->addItem(QString::fromStdString(e));
    }

    _cb_detector_element->addItem("Si");
    _cb_detector_element->addItem("Ge");

    _cb_pileup_elements->setEnabled(false);

    _cb_add_elements->setCurrentIndex(1);
    _cb_pileup_elements->setCurrentIndex(1);

    _cb_add_shell->addItem("K");
    _cb_add_shell->addItem("L");
    _cb_add_shell->addItem("M");

    connect(_chk_is_pileup, SIGNAL(stateChanged(int)), this, SLOT(pileup_chk_changed(int)) );
    connect(_chk_is_pileup, SIGNAL(stateChanged(int)), this, SLOT(element_selection_changed(int)));

    connect(_cb_add_shell, SIGNAL(currentIndexChanged(int)), this, SLOT(element_selection_changed(int)));
    connect(_cb_add_elements, SIGNAL(currentIndexChanged(int)), this, SLOT(element_selection_changed(int)));
    connect(_cb_pileup_elements, SIGNAL(currentIndexChanged(int)), this, SLOT(element_selection_changed(int)));

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
    if (_fitting_dialog != nullptr)
    {
        delete _fitting_dialog;
        _fitting_dialog = nullptr;
    }
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::createLayout()
{    
    std::vector<std::string> bound_types {"Not Initialized", "Fixed", "Limited Low High", "Limited Low", "Limited High", "Fit"};

    _spectra_widget = new SpectraWidget();

    _fit_params_tab_widget = new QTabWidget();

    fitting::models::Gaussian_Model<double> g_model;

    _fit_params_table_model = new FitParamsTableModel();
    _fit_params_table_model->setFitParams(g_model.fit_parameters());
    connect(_fit_params_table_model, &FitParamsTableModel::onEnergyChange, this, &FitSpectraWidget::replot_integrated_spectra_with_background);
    ComboBoxDelegate *cbDelegate = new ComboBoxDelegate(bound_types);
    NumericPrecDelegate* npDelegate = new NumericPrecDelegate();


    _fit_params_table = new QTableView();
    _fit_params_table->setModel(_fit_params_table_model);
    _fit_params_table->sortByColumn(0, Qt::AscendingOrder);
    _fit_params_table->setItemDelegateForColumn(1, npDelegate);
    _fit_params_table->setItemDelegateForColumn(2, cbDelegate);
    _fit_params_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    _fit_params_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_fit_params_table,
            SIGNAL(customContextMenuRequested(QPoint)),
            this,
            SLOT(fit_params_customMenuRequested(QPoint)));

    _fit_elements_table_model = new FitElementsTableModel(_detector_element);

    connect(_spectra_widget, SIGNAL(y_axis_changed(bool)), _fit_elements_table_model, SLOT(update_counts_log10(bool)));
    connect(_fit_elements_table_model, &FitElementsTableModel::braching_ratio_changed, this, &FitSpectraWidget::on_braching_ratio_update);

    _fit_elements_table = new QTreeView();
    _fit_elements_table->setModel(_fit_elements_table_model);
    _fit_elements_table->sortByColumn(0);
    //_fit_elements_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QItemSelectionModel* mod = _fit_elements_table->selectionModel();
    connect(mod,
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(element_selection_changed(QModelIndex,QModelIndex)));

    _btnSsettings = new QPushButton(QIcon(":/images/gear.png"), "", this);
    _btnSsettings->setMaximumWidth(64);
    connect(_btnSsettings, &QPushButton::released, this, &FitSpectraWidget::onSettingsDialog);

    _btn_fit_spectra = new QPushButton("Fit Spectra");
    connect(_btn_fit_spectra, &QPushButton::released, this, &FitSpectraWidget::Fit_Spectra_Click);

    _btn_model_spectra = new QPushButton("Model Spectra");
    connect(_btn_model_spectra, &QPushButton::released, this, &FitSpectraWidget::Model_Spectra_Click);

    _btn_export_parameters = new QPushButton("Export Fit Parameters");
    connect(_btn_export_parameters, &QPushButton::released, this, &FitSpectraWidget::on_export_fit_paramters);

    _btn_export_csv = new QPushButton("Export CSV");
    connect(_btn_export_csv, &QPushButton::released, this, &FitSpectraWidget::on_export_csv);

    _btn_add_element = new QPushButton("Add Element");
    connect(_btn_add_element, &QPushButton::released, this, &FitSpectraWidget::add_element);

    _btn_del_element = new QPushButton("Delete Element");
    connect(_btn_del_element, &QPushButton::released, this, &FitSpectraWidget::del_element);

    QGridLayout* add_element_grid_layout = new QGridLayout();
    add_element_grid_layout->setAlignment(Qt::AlignTop);
    add_element_grid_layout->addWidget(new QLabel("Element"), 0, 0);
    add_element_grid_layout->addWidget(_cb_add_elements, 0, 1);
    add_element_grid_layout->addWidget(new QLabel("Shell"), 1, 0);
    add_element_grid_layout->addWidget(_cb_add_shell, 1, 1);
    //add_element_grid_layout->addWidget(new QLabel("Detector Element"), 2, 0);
    //add_element_grid_layout->addWidget(_cb_detector_element, 2, 1);
    add_element_grid_layout->addWidget(_chk_is_pileup, 2, 0);
    add_element_grid_layout->addWidget(_cb_pileup_elements, 2, 1);
    add_element_grid_layout->addWidget(_btn_add_element, 3, 0);
    add_element_grid_layout->addWidget(_btn_del_element, 3, 1);

    QHBoxLayout* elements_layout = new QHBoxLayout();
    elements_layout->addWidget(_fit_elements_table);
    elements_layout->addLayout(add_element_grid_layout);

    QWidget* element_widget = new QWidget();
    element_widget->setLayout(elements_layout);


    _fit_params_tab_widget->addTab(_fit_params_table, "Fit Parameters");
    _fit_params_tab_widget->addTab(element_widget, "Fit Elements");

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
    grid_layout->addWidget(_btnSsettings, 0, 0);
    grid_layout->addWidget(_cb_opttimizer, 0, 1);
    grid_layout->addWidget(_btn_fit_spectra, 0, 2);
    grid_layout->addWidget(_chk_auto_model, 0, 3);
    grid_layout->addWidget(_btn_model_spectra, 0, 4);
    grid_layout->addWidget(_btn_export_parameters, 0, 5);
    grid_layout->addWidget(_btn_export_csv, 0, 6);
    grid_layout->addItem(new QSpacerItem(999, 10, QSizePolicy::Maximum), 0, 77);

	QVBoxLayout* vlayout_tab = new QVBoxLayout();
	vlayout_tab->addWidget(_fit_params_tab_widget);
	vlayout_tab->addItem(grid_layout);
	QWidget* tab_and_buttons_widget = new QWidget();
	tab_and_buttons_widget->setLayout(vlayout_tab);

	QSplitter* splitter = new QSplitter();
	splitter->setOrientation(Qt::Vertical);
	splitter->addWidget(_spectra_widget);
	splitter->setStretchFactor(0, 1);
	splitter->addWidget(tab_and_buttons_widget);

    update_spectra_top_axis({ "Ca", "Fe", "Cu"});


    QLayout* layout = new QVBoxLayout();
	layout->addWidget(splitter);
    setLayout(layout);
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::setParamOverride(data_struct::Params_Override<double>* po)
{
	_param_override = po; 
	if (_param_override != nullptr)
	{
		setElementDetector(_param_override->detector_element);
		setElementsToFit(&(_param_override->elements_to_fit));
		setFitParams(&(_param_override->fit_params));
	}
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::on_braching_ratio_update(data_struct::Fit_Element_Map<double>* element)
{
    if (element != nullptr)
    {
        _spectra_widget->set_element_lines(element);
    }
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::onSettingsDialog()
{

    SpectraWidgetSettingsDialog *settings_dialog = new SpectraWidgetSettingsDialog();
    settings_dialog->exec();
    if (settings_dialog->isAccepted())
    {
        _spectra_widget->set_log10(Preferences::inst()->getValue(STR_PFR_LOG_10).toBool());

        _showDetailedFitSpec = Preferences::inst()->getValue(STR_PFR_DETAILED_FIT_SPEC).toBool(); 
        if (_showDetailedFitSpec)
        {

            for (auto& itr : _labeled_spectras)
            {
                _spectra_widget->append_spectra(QString(itr.first.c_str()), (Spectra<double>*)&itr.second, (data_struct::Spectra<double>*) & _ev);
            }
        }
        else
        {
            for (auto& itr : _labeled_spectras)
            {
                _spectra_widget->remove_spectra(QString(itr.first.c_str()));
            }
        }
		_showFitIntSpec = Preferences::inst()->getValue(STR_PFR_SHOW_FIT_INT_SPEC).toBool();
		if (_showFitIntSpec)
		{
			for (auto &itr : _fit_int_spec_map)
			{
				QString name = "Fitted_Int_" + QString(itr.first.c_str());
				_spectra_widget->append_spectra(name, itr.second, (data_struct::Spectra<double>*)&_ev);
			}
		}
		else
		{
			for (auto& itr : _fit_int_spec_map)
			{
				QString name = "Fitted_Int_" + QString(itr.first.c_str());
				_spectra_widget->remove_spectra(name);
			}
		}
    }
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::on_export_fit_paramters()
{
    data_struct::Fit_Parameters<double> fit_params;
    data_struct::Fit_Parameters<double> model_fit_params = _fit_params_table_model->getFitParams();
	data_struct::Fit_Element_Map_Dict<double> element_to_fit = _fit_elements_table_model->getElementsToFit();
	
    fit_params.append_and_update(model_fit_params);
    //fit_params.append_and_update(&element_fit_params);

    emit export_fit_paramters(fit_params, element_to_fit);
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::on_export_csv()
{
    emit export_csv_and_png(_spectra_widget->getPngofChart(), &_ev, _int_spec, &_spectra_background, &_fit_spec, &_labeled_spectras);
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::replot_integrated_spectra(bool snipback)
{
    if (_int_spec != nullptr)
    {
		//std::lock_guard<std::mutex> lock(_mutex);

        data_struct::Fit_Parameters<double> fit_params = _fit_params_table_model->getFitParams();

        fitting::models::Range energy_range;
        energy_range.min = 0;
        energy_range.max = _int_spec->size() - 1;

        ArrayDr energy = ArrayDr::LinSpaced(energy_range.count(), energy_range.min, energy_range.max);

        _ev = fit_params[STR_ENERGY_OFFSET].value + energy * fit_params[STR_ENERGY_SLOPE].value + pow(energy, 2.0) * fit_params[STR_ENERGY_QUADRATIC].value;

        _spectra_widget->append_spectra(DEF_STR_INT_SPECTRA, _int_spec, (data_struct::Spectra<double>*) & _ev);
        if (snipback)
        {
			// TODO: Look into why memory access violation happens when streaming
				_spectra_background = snip_background<double>((Spectra<double>*)_int_spec,
					fit_params[STR_ENERGY_OFFSET].value,
					fit_params[STR_ENERGY_SLOPE].value,
					fit_params[STR_ENERGY_QUADRATIC].value,
					fit_params[STR_SNIP_WIDTH].value,
					0, //spectra energy start range
					_int_spec->size() - 1);

				_spectra_widget->append_spectra(DEF_STR_BACK_SPECTRA, &_spectra_background, (data_struct::Spectra<double>*) & _ev);
			
        }
        _spectra_widget->setXLabel("Energy (kEv)");
        
		_showFitIntSpec = Preferences::inst()->getValue(STR_PFR_SHOW_FIT_INT_SPEC).toBool();
		if (_showFitIntSpec)
		{
			for (auto &itr : _fit_int_spec_map)
			{
				QString name = "Fitted_Int_" + QString(itr.first.c_str());
				_spectra_widget->append_spectra(name, itr.second, (data_struct::Spectra<double>*)&_ev);
			}
		}
		
        for (auto& itr : _roi_spec_map)
        {
            _spectra_widget->append_spectra(QString(itr.first.c_str()), itr.second, (data_struct::Spectra<double>*) & _ev);
        }

    }
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::appendFitIntSpectra(string name, ArrayDr* spec)
{
    _fit_int_spec_map[name] = spec;
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::appendROISpectra(string name, ArrayDr* spec)
{
    _roi_spec_map[name] = spec;
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::clearFitIntSpectra()
{
    for (auto& itr : _fit_int_spec_map)
    {
        QString name = "Fitted_Int_" + QString(itr.first.c_str());
        _spectra_widget->remove_spectra(name);
    }
    _fit_int_spec_map.clear();

}
/*---------------------------------------------------------------------------*/

void FitSpectraWidget::clearROISpectra()
{
    for (auto& itr : _roi_spec_map)
    {
        _spectra_widget->remove_spectra(QString(itr.first.c_str()));
    }
    _roi_spec_map.clear();
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::add_element()
{

    if(_elements_to_fit == nullptr)
    {
        _elements_to_fit = new data_struct::Fit_Element_Map_Dict<double>();
    }

    QString el_name = _cb_add_elements->currentText();
    int shell = _cb_add_shell->currentIndex();
    if(_chk_is_pileup->checkState() == Qt::CheckState::Unchecked)
    {
        if(shell == 1)
        {
            el_name += "_L";
        }
        else if(shell == 2)
        {
            el_name += "_M";
        }
    }
	
    data_struct::Fit_Element_Map<double>* fit_element = gen_element_map<double>(el_name.toStdString());
    if(fit_element != nullptr)
    {
		if (_param_override != nullptr)
		{
			map<int, double> ratios = _param_override->get_custom_factor(el_name.toStdString());
			for (const auto &itr : ratios)
			{
				fit_element->multiply_custom_multiply_ratio(itr.first, itr.second);
			}
		}

        if(_chk_is_pileup->checkState() == Qt::CheckState::Checked)
        {
            QString pileup_name = _cb_pileup_elements->currentText();
            data_struct::Element_Info<double>* pileup_element = data_struct::Element_Info_Map<double>::inst()->get_element(pileup_name.toStdString());
            if(pileup_element != nullptr)
            {
                el_name += "_" + pileup_name;
                fit_element->set_as_pileup(pileup_name.toStdString(), pileup_element);
            }
        }

        fit_element->init_energy_ratio_for_detector_element(data_struct::Element_Info_Map<double>::inst()->get_element(_detector_element));
        //check if it exists in list
        if(_elements_to_fit->count(el_name.toStdString()) == 0)
        {
            (*_elements_to_fit)[el_name.toStdString()] = fit_element;
        }
    }


    _fit_elements_table_model->appendElement(fit_element);

}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::del_element()
{
    //get selected elements to remove
    QModelIndexList selected_rows = _fit_elements_table->selectionModel()->selectedRows();
    for( auto i : selected_rows)
    {
        if(i.isValid())
        {
            data_struct::Fit_Element_Map<double>* fit_element = _fit_elements_table_model->getElementByIndex(i);

            if( fit_element != nullptr && _elements_to_fit->find(fit_element->full_name()) != _elements_to_fit->end() )
            {
                Fit_Element_Map<double>* el = (*_elements_to_fit)[fit_element->full_name()];
				_elements_to_fit->erase(fit_element->full_name());
                if(el != nullptr)
                {
                    delete el;
                }
            }
			_spectra_widget->set_element_lines(nullptr);
            auto p = i.parent();
            if(false == p.isValid())
            {	
                _fit_elements_table_model->removeRows(i.row(), 1);
            }
            else
            {
                _fit_elements_table_model->removeRows(i.row(), 1, p);
            }
        }
        break;
    }
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::pileup_chk_changed(int state)
{
    switch(state)
    {
    case Qt::CheckState::Checked:
        _cb_pileup_elements->setEnabled(true);
        break;
    case Qt::CheckState::Unchecked:
        _cb_pileup_elements->setEnabled(false);
        break;
    default:
        _cb_pileup_elements->setEnabled(false);
        break;
    }
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::Fit_Spectra_Click()
{
//    _btn_fit_spectra->setEnabled(false);
//    _btn_model_spectra->setEnabled(false);

    

    if(_elements_to_fit != nullptr && _int_spec != nullptr)
    {
		////std::lock_guard<std::mutex> lock(_mutex);

        //fitting::optimizers::LMFit_Optimizer lmfit_optimizer;
        //fitting::optimizers::MPFit_Optimizer mpfit_optimizer;
        //fitting::models::Gaussian_Model model;


        data_struct::Fit_Parameters<double> out_fit_params = _fit_params_table_model->getFitParams();
        data_struct::Fit_Parameters<double> element_fit_params = _fit_elements_table_model->getAsFitParams();


        if (_fitting_dialog == nullptr)
        {
            _fitting_dialog = new FittingDialog();
        }
        _fitting_dialog->updateFitParams(out_fit_params, element_fit_params);
        _fitting_dialog->setOptimizer(_cb_opttimizer->currentText());
        _fitting_dialog->setSpectra((Spectra<double>*)_int_spec, _ev);
        _fitting_dialog->setElementsToFit(_elements_to_fit);
        if (_fit_spec.size() > 0)
        {
            _fitting_dialog->setFitSpectra(&_fit_spec);
        }
        _fitting_dialog->setDisplayRange(_spectra_widget->getDisplayEnergyMin(),
            _spectra_widget->getDisplayEnergyMax(),
            _spectra_widget->getDisplayHeightMin(),
            _spectra_widget->getDisplayHeightMax());
        _fitting_dialog->exec();

        //Range of energy in spectra to fit
        //fitting::models::Range energy_range;
        //energy_range.min = 0;
        //energy_range.max = _int_spec->rows() -1;

        //data_struct::Spectra s1 = _integrated_spectra.sub_spectra(energy_range);

        //Fitting routines
        //fitting::routines::Param_Optimized_Fit_Routine fit_routine;
        //fit_routine.set_update_coherent_amplitude_on_fit(false);

        //if(_cb_opttimizer->currentText() == STR_LM_FIT)
        //{
        //    fit_routine.set_optimizer(&lmfit_optimizer);
        //}
        //else if(_cb_opttimizer->currentText() == STR_MP_FIT)
        //{
        //    fit_routine.set_optimizer(&mpfit_optimizer);
        //}

        //reset model fit parameters to defaults
        //model.reset_to_default_fit_params();
        //Update fit parameters by override values
        //model.update_fit_params_values(&out_fit_params);
        //model.update_and_add_fit_params_values_gt_zero(&element_fit_params);

        //model.set_fit_params_preset(fitting::models::Fit_Params_Preset::BATCH_FIT_WITH_TAILS);

        //Initialize the fit routine
        //fit_routine.initialize(&model, _elements_to_fit, energy_range);
        
        //Fit the spectra saving the element counts in element_fit_count_dict
        // single threaded
        //out_fit_params = fit_routine.fit_spectra_parameters(&model, int_spectra, _elements_to_fit);
        // use background thread to not freeze up the gui
        //std::future<data_struct::Fit_Parameters> ret = global_threadpool.enqueue(&fitting::routines::Param_Optimized_Fit_Routine::fit_spectra_parameters, fit_routine, &model, (Spectra*)_int_spec, _elements_to_fit);

        //while (ret._Is_ready() == false)
        //{
        //    QCoreApplication::processEvents();
        //}
        //out_fit_params = ret.get();

        //out_fit_params = fit_routine.fit_spectra_parameters(&model, &s1, _elements_to_fit);

        if (_fitting_dialog->accepted_fit())
        {

            disconnect(_fit_params_table_model,
                SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)),
                this,
                SLOT(Model_Spectra_Val_Change(QModelIndex, QModelIndex, QVector<int>)));

            disconnect(_fit_elements_table_model,
                SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)),
                this,
                SLOT(Model_Spectra_Val_Change(QModelIndex, QModelIndex, QVector<int>)));

            //_fit_params_table_model->updateFitParams(&out_fit_params);

            //_fit_elements_table_model->updateElementValues(&out_fit_params);

			_fit_params_table_model->updateFitParams(_fitting_dialog->get_new_fit_params());

			_fit_elements_table_model->updateElementValues(_fitting_dialog->get_new_fit_params());


            if (_chk_auto_model->checkState() == Qt::Checked)
            {
                connect(_fit_params_table_model,
                    SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)),
                    this,
                    SLOT(Model_Spectra_Val_Change(QModelIndex, QModelIndex, QVector<int>)));
                connect(_fit_elements_table_model,
                    SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)),
                    this,
                    SLOT(Model_Spectra_Val_Change(QModelIndex, QModelIndex, QVector<int>)));

                Model_Spectra_Click();
            }

            replot_integrated_spectra(true);
			
            //data_struct::Spectra fit_spec = model.model_spectrum(&out_fit_params, _elements_to_fit, energy_range);
			_fit_spec = _fitting_dialog->get_fit_spectra(&_labeled_spectras);
            if (_fit_spec.size() == _spectra_background.size())
            {
                _fit_spec += _spectra_background;
            }
            for (int i = 0; i < _fit_spec.size(); i++)
            {
                if (_fit_spec[i] <= 0.0)
                {
                    _fit_spec[i] = 0.1;
                }
            }

            _spectra_widget->append_spectra(DEF_STR_FIT_INT_SPECTRA, &_fit_spec, (data_struct::Spectra<double>*) & _ev);
            if (_showDetailedFitSpec)
            {
                for (auto& itr : _labeled_spectras)
                {
                    _spectra_widget->append_spectra(QString(itr.first.c_str()), (Spectra<double>*)&itr.second, (data_struct::Spectra<double>*) & _ev);
                }
            }
            emit signal_finished_fit();		
        }

        _fitting_dialog->waitToFinishRunning();
        delete _fitting_dialog;
        _fitting_dialog = nullptr;
    }

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


    if(_elements_to_fit != nullptr && _int_spec != nullptr)
    {
        fitting::models::Gaussian_Model<double> model;
        //Range of energy in spectra to fit

        data_struct::Fit_Parameters<double> fit_params;
        data_struct::Fit_Parameters<double> model_fit_params = _fit_params_table_model->getFitParams();
        data_struct::Fit_Parameters<double> element_fit_params = _fit_elements_table_model->getAsFitParams();
        fit_params.append_and_update(model_fit_params);
        fit_params.append_and_update(element_fit_params);

        fitting::models::Range energy_range;
        energy_range.min = 0;
        energy_range.max = _int_spec->size()-1;

        unordered_map<string, ArrayTr<double>> labeled_spectras;
        data_struct::Spectra<double> fit_spec = model.model_spectrum(&fit_params, _elements_to_fit, &labeled_spectras, energy_range);

        replot_integrated_spectra(true);

        if(fit_spec.size() == _spectra_background.size())
        {
            fit_spec += _spectra_background;
        }
        for(int i=0; i<fit_spec.size(); i++)
        {
            if(fit_spec[i] <= 0.0)
                fit_spec[i] = 0.1;
        }
        _spectra_widget->append_spectra(DEF_STR_MODEL_SPECTRA, &fit_spec, (data_struct::Spectra<double>*)&_ev);
    }
    emit signal_finished_fit();

}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::finished_fitting()
{
    _btn_fit_spectra->setEnabled(true);
    if(_chk_auto_model->checkState() == Qt::Checked)
        _btn_model_spectra->setEnabled(false);
    else
        _btn_model_spectra->setEnabled(true);

	replot_integrated_spectra(true);
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
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::element_selection_changed(int index)
{

    QString element_name = _cb_add_elements->currentText();
    QString full_name = element_name;
    if(_chk_is_pileup->checkState() == Qt::CheckState::Unchecked)
    {
        if(_cb_add_shell->currentText() != "K")
        {
            full_name = element_name + "_" +_cb_add_shell->currentText();
        }
    }

    Fit_Element_Map<double> em(full_name.toStdString(), Element_Info_Map<double>::inst()->get_element(element_name.toStdString()));

	if (_param_override != nullptr)
	{
		map<int, double> ratios = _param_override->get_custom_factor(full_name.toStdString());
		for (const auto &itr : ratios)
		{
			em.multiply_custom_multiply_ratio(itr.first, itr.second);
		}
	}

    if(_chk_is_pileup->checkState() == Qt::CheckState::Checked)
    {
        em.set_as_pileup(_cb_pileup_elements->currentText().toStdString(), Element_Info_Map<double>::inst()->get_element(_cb_pileup_elements->currentText().toStdString()));
    }

    em.init_energy_ratio_for_detector_element(data_struct::Element_Info_Map<double>::inst()->get_element(_detector_element));
    _spectra_widget->set_element_lines(&em);
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

void FitSpectraWidget::setIntegratedSpectra(ArrayDr* int_spec)
{
	{
		///std::lock_guard<std::mutex> lock(_mutex);
		_int_spec = int_spec;
	}
    replot_integrated_spectra(true);
    _spectra_widget->onResetChartView();
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::setFitParams(data_struct::Fit_Parameters<double>* fit_params)
{
    _fit_params_table_model->updateFitParams(fit_params);
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::setElementsToFit(data_struct::Fit_Element_Map_Dict<double>* elements_to_fit)
{
    _fit_elements_table_model->updateFitElements(elements_to_fit);
	_elements_to_fit = elements_to_fit;
}

/*---------------------------------------------------------------------------*/

void FitSpectraWidget::update_spectra_top_axis(std::vector<std::string> element_names)
{

    std::map < std::string, float> labels;
    for (const auto& itr : element_names)
    {
        Fit_Element_Map<double> em(itr, Element_Info_Map<double>::inst()->get_element(itr));
		if (_param_override != nullptr)
		{
			map<int, double> ratios = _param_override->get_custom_factor(itr);
			for (const auto &itr : ratios)
			{
				em.multiply_custom_multiply_ratio(itr.first, itr.second);
			}
		}

        em.init_energy_ratio_for_detector_element(data_struct::Element_Info_Map<double>::inst()->get_element(_detector_element));
        labels[itr] = em.center();
    }
    _spectra_widget->set_top_axis(labels);
}

/*---------------------------------------------------------------------------*/