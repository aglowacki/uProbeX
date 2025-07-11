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
#include <QInputDialog>
#include <QDesktopServices>
#include <QFileDialog>
#include <math.h>

#include "data_struct/element_info.h"
#include "io/file/hl_file_io.h"
#include <mvc/NumericPrecDelegate.h>
#include <preferences/Preferences.h>
#include "io/file/aps/aps_roi.h"

using namespace data_struct;

//---------------------------------------------------------------------------

FitSpectraWidget::FitSpectraWidget(QWidget* parent) : QWidget(parent)
{

    _cb_add_elements = new QComboBox();
    _cb_add_shell = new QComboBox();
    _cb_pileup_elements = new QComboBox();
    _cb_detector_element = new QComboBox();
    _chk_is_pileup = new QCheckBox("Pile Up");
    _detector_element = "Si"; // default to Si detector if not found in param override
    _int_spec = nullptr;
    _displayROIs = true;
    _elements_to_fit = nullptr;
    _fitting_dialog = nullptr;
	_param_override = nullptr;
    _fit_spec.setZero(2048);
    _showDetailedFitSpec = Preferences::inst()->getValue(STR_PFR_DETAILED_FIT_SPEC).toBool();
	_showFitIntMatrix = Preferences::inst()->getValue(STR_PFR_SHOW_FIT_INT_MATRIX).toBool();
    _showFitIntNNLS = Preferences::inst()->getValue(STR_PFR_SHOW_FIT_INT_NNLS).toBool();
    _showMaxChanSpec = Preferences::inst()->getValue(STR_PFR_SHOW_MAX_CHAN_SPEC).toBool();
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

    connect(_chk_is_pileup, &QCheckBox::stateChanged, this, &FitSpectraWidget::pileup_chk_changed);
    connect(_chk_is_pileup, &QCheckBox::stateChanged, this, &FitSpectraWidget::element_selection_changed);

    connect(_cb_add_shell, &QComboBox::currentIndexChanged, this, &FitSpectraWidget::element_selection_changed);
    connect(_cb_add_elements, &QComboBox::currentIndexChanged, this, &FitSpectraWidget::element_selection_changed);
    connect(_cb_pileup_elements, &QComboBox::currentIndexChanged, this, &FitSpectraWidget::element_selection_changed);

    createLayout();

    _spectra_background.resize(10);
    _spectra_background.Zero(10);
    connect(this, &FitSpectraWidget::signal_finished_fit, this, &FitSpectraWidget::finished_fitting);


    _set_fit_params_bounds_menu = new QMenu("Set Bounds");
    QAction* action_bounds = _set_fit_params_bounds_menu->addAction("FIXED");
    connect(action_bounds, &QAction::triggered, this, &FitSpectraWidget::set_fit_params_bounds_fixed);

    action_bounds = _set_fit_params_bounds_menu->addAction("FIT");
    connect(action_bounds, &QAction::triggered, this, &FitSpectraWidget::set_fit_params_bounds_fit);

    action_bounds = _set_fit_params_bounds_menu->addAction("LIMITED_LO_HI");
    connect(action_bounds, &QAction::triggered, this, &FitSpectraWidget::set_fit_params_bounds_limited_lo_hi);

    action_bounds = _set_fit_params_bounds_menu->addAction("LIMITED_LO");
    connect(action_bounds, &QAction::triggered, this, &FitSpectraWidget::set_fit_params_bounds_limited_lo);

    action_bounds = _set_fit_params_bounds_menu->addAction("LIMITED_HI");
    connect(action_bounds, &QAction::triggered, this, &FitSpectraWidget::set_fit_params_bounds_limited_hi);
 //FIXED=1, LIMITED_LO_HI=2, LIMITED_LO=3, LIMITED_HI=4, FIT=5};
    _fit_param_contextMenu = new QMenu(("Context menu"), this);
    _fit_param_contextMenu->addMenu(_set_fit_params_bounds_menu);
    action_bounds = _fit_param_contextMenu->addAction("Load from CSV");
    connect(action_bounds, &QAction::triggered, this, &FitSpectraWidget::show_load_fit_params_dialog);

}

//---------------------------------------------------------------------------

FitSpectraWidget::~FitSpectraWidget()
{
    if(_periodic_table_widget != nullptr)
    {
        delete _periodic_table_widget;
    }

    if (_fitting_dialog != nullptr)
    {
        delete _fitting_dialog;
        _fitting_dialog = nullptr;
    }
}

//---------------------------------------------------------------------------

void FitSpectraWidget::createLayout()
{    
    std::vector<std::string> bound_types {"Not Initialized", "Fixed", "Limited Low High", "Limited Low", "Limited High", "Fit"};

    _spectra_widget = new SpectraWidget();

    _spectra_dock = new QDockWidget("Spectra", this);
    _spectra_dock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	_spectra_dock->setWidget(_spectra_widget);

    _fit_params_tab_widget = new QTabWidget();

    fitting::models::Gaussian_Model<double> g_model;

    _fit_params_table_model = new FitParamsTableModel();
    _fit_params_table_model->setFitParams(g_model.fit_parameters());
    _fit_params_table_model->setOptimizerSupportsMinMax(true);
    connect(_fit_params_table_model, &FitParamsTableModel::onEnergyChange, this, &FitSpectraWidget::replot_integrated_spectra_with_background);
    ComboBoxDelegate *cbDelegate = new ComboBoxDelegate(bound_types);
    NumericPrecDelegate* npDelegate = new NumericPrecDelegate();

    _periodic_table_widget = new PeriodicTableWidget();
    connect(_periodic_table_widget, &PeriodicTableWidget::onSelect, this, &FitSpectraWidget::update_selected_element_to_add);

    _fit_params_table = new QTableView();
    _fit_params_table->setModel(_fit_params_table_model);
    _fit_params_table->sortByColumn(0, Qt::AscendingOrder);
    _fit_params_table->setItemDelegateForColumn(1, npDelegate);
    _fit_params_table->setItemDelegateForColumn(2, cbDelegate);
    _fit_params_table->setItemDelegateForColumn(3, npDelegate);
    _fit_params_table->setItemDelegateForColumn(4, npDelegate);
    _fit_params_table->setItemDelegateForColumn(5, npDelegate);
    _fit_params_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    _fit_params_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_fit_params_table, &QTableView::customContextMenuRequested,this,&FitSpectraWidget::fit_params_customMenuRequested);

    _fit_elements_table_model = new FitElementsTableModel(_detector_element);
    //_fit_elements_table_model->setDisplayHeaderMinMax(true);

    connect(_spectra_widget, &SpectraWidget::y_axis_changed, _fit_elements_table_model, &FitElementsTableModel::update_counts_log10);
    connect(_fit_elements_table_model, &FitElementsTableModel::braching_ratio_changed, this, &FitSpectraWidget::on_braching_ratio_update);

    _fit_elements_table = new QTreeView();
    _fit_elements_table->setModel(_fit_elements_table_model);
    _fit_elements_table->setItemDelegateForColumn(1, npDelegate);
    _fit_elements_table->sortByColumn(0, Qt::AscendingOrder);
    //_fit_elements_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    QItemSelectionModel* mod = _fit_elements_table->selectionModel();
    connect(mod,&QItemSelectionModel::currentChanged,this,&FitSpectraWidget::element_selection_changed_from);

    connect(_spectra_widget, &SpectraWidget::onSettingsDialog, this, &FitSpectraWidget::onSettingsDialog);

    _btn_fit_spectra = new QPushButton("Fit Spectra");
    connect(_btn_fit_spectra, &QPushButton::released, this, &FitSpectraWidget::Fit_Spectra_Click);

    _btn_fit_roi_spectra = new QPushButton("Fit ROI Spectra");
    connect(_btn_fit_roi_spectra, &QPushButton::released, this, &FitSpectraWidget::Fit_ROI_Spectra_Click);
    _btn_fit_roi_spectra->setEnabled(false);

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

    _btn_periodic_table = new QPushButton();
    _btn_periodic_table->setIcon(QIcon(":/images/grid.png"));
    _btn_periodic_table->setFixedSize(32,32);
    connect(_btn_periodic_table, &QPushButton::released, this, &FitSpectraWidget::display_periodic_table);

    QGridLayout* add_element_grid_layout = new QGridLayout();
    add_element_grid_layout->setAlignment(Qt::AlignTop);
    add_element_grid_layout->addWidget(new QLabel("Element"), 0, 0);
    add_element_grid_layout->addWidget(_cb_add_elements, 0, 1);
    add_element_grid_layout->addWidget(_btn_periodic_table, 0, 2);
    add_element_grid_layout->addWidget(new QLabel("Shell"), 1, 0);
    add_element_grid_layout->addWidget(_cb_add_shell, 1, 1);
    //add_element_grid_layout->addWidget(new QLabel("Detector Element"), 2, 0);
    //add_element_grid_layout->addWidget(_cb_detector_element, 2, 1);
    add_element_grid_layout->addWidget(_chk_is_pileup, 2, 0);
    add_element_grid_layout->addWidget(_cb_pileup_elements, 2, 1);
    add_element_grid_layout->addWidget(_btn_add_element, 3, 0);
    add_element_grid_layout->addWidget(_btn_del_element, 3, 1);

    QLayout* elements_layout;
    if(Preferences::inst()->getValue(STR_PREF_SPRECTRA_CONTROLS_HORIZONTAL_OPTION).toInt() == 1)
    {
        elements_layout = new QVBoxLayout();
    }
    else
    {
        elements_layout = new QHBoxLayout();
    }
    elements_layout->addWidget(_fit_elements_table);
    elements_layout->addItem(add_element_grid_layout);

    QWidget* element_widget = new QWidget();
    element_widget->setLayout(elements_layout);


    _fit_params_tab_widget->addTab(_fit_params_table, "Fit Parameters");
    _fit_params_tab_widget->addTab(element_widget, "Fit Elements");

    _cb_fitting_preset = new QComboBox();
    _cb_fitting_preset->addItem("User Defined");
    _cb_fitting_preset->addItem("Fit Amplitutes Only");
    _cb_fitting_preset->addItem("Fit No Tails");
    _cb_fitting_preset->addItem("Fit With Tails");
    _cb_fitting_preset->addItem("Fit With Free Energy");
    _cb_fitting_preset->addItem("Fit No Tails Energy Quadratic");
    connect(_cb_fitting_preset, &QComboBox::currentIndexChanged, this, &FitSpectraWidget::optimizer_preset_changed);

    _chk_auto_model = new QCheckBox("Auto Update Model");
    _chk_auto_model->setChecked(false);
    connect(_chk_auto_model,&QCheckBox::stateChanged,this,&FitSpectraWidget::check_auto_model);

    QGridLayout *grid_layout = new QGridLayout();

    QLayout * orient_layout;
	//QSplitter* splitter = new QSplitter();
    if(Preferences::inst()->getValue(STR_PREF_SPRECTRA_CONTROLS_HORIZONTAL_OPTION).toInt() == 1)
    {
        orient_layout = new QHBoxLayout();
        //splitter->setOrientation(Qt::Horizontal);
        grid_layout->addWidget(_cb_fitting_preset, 0, 0);

        grid_layout->addWidget(_btn_fit_spectra, 1, 0);
        grid_layout->addWidget(_btn_fit_roi_spectra, 1, 1);

        grid_layout->addWidget(_chk_auto_model, 2, 0);
        grid_layout->addWidget(_btn_model_spectra, 2, 1);

        grid_layout->addWidget(_btn_export_csv, 3, 0);
        grid_layout->addWidget(_btn_export_parameters, 3, 1);
    }
    else
    {
        orient_layout = new QVBoxLayout();
	    //splitter->setOrientation(Qt::Vertical);
        grid_layout->addWidget(_cb_fitting_preset, 0, 0);
        grid_layout->addWidget(_chk_auto_model, 0, 2);
        grid_layout->addWidget(_btn_export_csv, 0, 3);

        grid_layout->addWidget(_btn_fit_spectra, 1, 0);
        grid_layout->addWidget(_btn_fit_roi_spectra, 1, 1);
        grid_layout->addWidget(_btn_model_spectra, 1, 2);
        grid_layout->addWidget(_btn_export_parameters, 1, 3);

        grid_layout->addItem(new QSpacerItem(9999, 10, QSizePolicy::Maximum), 0, 77);
    
    }
	QVBoxLayout* vlayout_tab = new QVBoxLayout();
	vlayout_tab->addWidget(_fit_params_tab_widget);
	vlayout_tab->addItem(grid_layout);
    vlayout_tab->setSpacing(0);
	vlayout_tab->setContentsMargins(0, 0, 0, 0);
	QWidget* tab_and_buttons_widget = new QWidget();
	tab_and_buttons_widget->setLayout(vlayout_tab);

    orient_layout->addWidget(_spectra_dock);
    orient_layout->addWidget(tab_and_buttons_widget);
//	splitter->addWidget(_spectra_dock);
//	splitter->setStretchFactor(0, 1);
//	splitter->addWidget(tab_and_buttons_widget);

    _cb_fitting_preset->setCurrentIndex(3);
    //optimizer_preset_changed(3); // batch with tails

    QLayout* layout = new QVBoxLayout();
//	layout->addWidget(splitter);
	layout->addItem(orient_layout);
    layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

//---------------------------------------------------------------------------

void FitSpectraWidget::displayROIs(bool val)
{
    _btn_fit_roi_spectra->setEnabled(val);
    _displayROIs = val;
    if (val == false)
    {
        for (auto& itr : _roi_spec_map)
        {
            _spectra_widget->remove_spectra(QString(itr.first.c_str()));
        }
    }

    replot_integrated_spectra(false);
}

//---------------------------------------------------------------------------

void FitSpectraWidget::update_selected_element_to_add(QString val)
{
    _cb_add_elements->setCurrentText(val);
}

//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------

void FitSpectraWidget::on_braching_ratio_update(data_struct::Fit_Element_Map<double>* element)
{
    if (element != nullptr)
    {
        _spectra_widget->set_element_lines(element);
    }
}

//---------------------------------------------------------------------------

void FitSpectraWidget::onSettingsDialog()
{

    SpectraWidgetSettingsDialog *settings_dialog = new SpectraWidgetSettingsDialog();
    settings_dialog->exec();
    if (settings_dialog->isAccepted())
    {
        _spectra_widget->set_log10(Preferences::inst()->getValue(STR_PFR_LOG_10).toBool());
        _spectra_widget->setBackgroundBlack(Preferences::inst()->getValue(STR_PFR_SPECTRA_BLACK_BG).toBool());

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
		_showFitIntMatrix = Preferences::inst()->getValue(STR_PFR_SHOW_FIT_INT_MATRIX).toBool();
		if (_showFitIntMatrix)
		{
            if (_fit_int_spec_map.count(STR_FIT_GAUSS_MATRIX) > 0)
            {
                QString name = "Fitted_Int_" + QString(STR_FIT_GAUSS_MATRIX.c_str());
                _spectra_widget->append_spectra(name, _fit_int_spec_map.at(STR_FIT_GAUSS_MATRIX), (data_struct::Spectra<double>*) & _ev);
            }

            if (_fit_int_spec_map.count("Background") > 0)
            {
                QString name = "Fitted_Int_" + QString(STR_FIT_INT_BACKGROUND.c_str());
                _spectra_widget->append_spectra(name, _fit_int_spec_map.at("Background"), (data_struct::Spectra<double>*) & _ev);
            }
		}
		else
		{
            QString name = "Fitted_Int_" + QString(STR_FIT_GAUSS_MATRIX.c_str());
            _spectra_widget->remove_spectra(name);
            name = "Fitted_Int_" + QString(STR_FIT_INT_BACKGROUND.c_str());
            _spectra_widget->remove_spectra(name);
		}

        _showFitIntNNLS = Preferences::inst()->getValue(STR_PFR_SHOW_FIT_INT_NNLS).toBool();
        if (_showFitIntNNLS)
        {
            if (_fit_int_spec_map.count(STR_FIT_NNLS) > 0)
            {
                QString name = "Fitted_Int_" + QString(STR_FIT_NNLS.c_str());
                _spectra_widget->append_spectra(name, _fit_int_spec_map.at(STR_FIT_NNLS), (data_struct::Spectra<double>*) & _ev);
            }
        }
        else
        {
            QString name = "Fitted_Int_" + QString(STR_FIT_NNLS.c_str());
            _spectra_widget->remove_spectra(name);
        }

        _showMaxChanSpec = Preferences::inst()->getValue(STR_PFR_SHOW_MAX_CHAN_SPEC).toBool();
        if(_showMaxChanSpec)
        {
            for (auto& itr : _max_chan_spec_map)
            {
                _spectra_widget->append_spectra(QString(itr.first.c_str()), itr.second, (data_struct::Spectra<double>*) & _ev);
            }
        }
        else
        {
            for (auto& itr : _max_chan_spec_map)
            {
                _spectra_widget->remove_spectra(QString(itr.first.c_str()));
            }
        }
    }
}

//---------------------------------------------------------------------------

void FitSpectraWidget::show_load_fit_params_dialog(bool val)
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Open Fit Parameters", _dataset_dir.absolutePath(),
        "Fit Parameters (*.csv);;All Files (*.*)");

    // Dialog returns a nullptr string if user press cancel.
    if (fileName.isNull() || fileName.isEmpty()) return;

    QString filePath = QFileInfo(fileName).canonicalFilePath();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) 
    {
        logE<< file.errorString().toStdString();
        return;
    }
    bool conv_log10 = Preferences::inst()->getValue(STR_PFR_LOG_10).toBool();
    data_struct::Fit_Parameters<double> fit_params = _fit_params_table_model->getFitParams();
    data_struct::Fit_Parameters<double> fit_elements = _fit_elements_table_model->getAsFitParams();
    QStringList wordList;
    while (!file.atEnd()) 
    {
        QByteArray line = file.readLine();
        QString sline = QString(line).trimmed();
        wordList = sline.split(',');
        if( wordList.size() == 2 )
        {
            std::string sfirst = wordList[0].toStdString();
            if( fit_params.contains(sfirst) )
            {
                bool ok = false;
                double val = wordList[1].toDouble(&ok);
                if(ok)
                {
                    if( sfirst == STR_COMPTON_AMPLITUDE || sfirst == STR_COHERENT_SCT_AMPLITUDE)
                    {
                        if(conv_log10)
                        {
                            val = log10(val);
                        }
                    }
                    fit_params[sfirst].value = val;
                }
            }
            else if( fit_elements.contains(sfirst) ) 
            {
                bool ok = false;
                double val = wordList[1].toDouble(&ok);
                if(ok)
                {
                    if(conv_log10)
                    {
                        val = log10(val);
                    }
                    fit_elements[sfirst].value = val;
                }
            }
        }
        else if( wordList.size() > 2 )
        {
            QByteArray line2 = file.readLine();
            QString sline2 = QString(line2).trimmed();
            QStringList wordList2 = sline2.split(',');
            /*
            if(wordList.size() != wordList2.size())
            {
                QString msg = "CSV key size does not match values size. Key size = " + QString::number(wordList.size()) + " , Value size  ="+ QString::number(wordList2.size());
                QMessageBox::warning(this, "Failed to load", msg);
                return;
            }
            */
            int i = 0;
            for( auto &itr : wordList)
            {
                if (i >= wordList2.size())
                {
                    break;
                }
                std::string sfirst = itr.toStdString();
                if( io::file::aps::BASE_FILE_TAGS_TRANSLATION.count(sfirst) > 0)
                {
                    sfirst = io::file::aps::BASE_FILE_TAGS_TRANSLATION.at(sfirst);
                }
                if( fit_params.contains(sfirst) )
                {
                    bool ok = false;
                    double val = wordList2[i].toDouble(&ok);
                    if(ok)
                    {
                        if( sfirst == STR_COMPTON_AMPLITUDE || sfirst == STR_COHERENT_SCT_AMPLITUDE)
                        {
                            if(conv_log10)
                            {
                                val = log10(val);
                            }
                        }
                        fit_params[sfirst].value = val;
                    }
                }
                else if( fit_elements.contains(sfirst) ) 
                {
                    bool ok = false;
                    double val = wordList2[i].toDouble(&ok);
                    if(ok)
                    {
                        if(conv_log10)
                        {
                            val = log10(val);
                        }
                        fit_elements[sfirst].value = val;
                    }
                }
                i++;
            }
        }
    }
    _fit_params_table_model->setFitParams(fit_params);
    _fit_elements_table_model->updateElementValues(&fit_elements);
}

//---------------------------------------------------------------------------

void FitSpectraWidget::on_export_fit_paramters()
{
    data_struct::Fit_Parameters<double> fit_params;
    data_struct::Fit_Parameters<double> model_fit_params = _fit_params_table_model->getFitParams();
	data_struct::Fit_Element_Map_Dict<double> element_to_fit = _fit_elements_table_model->getElementsToFit();
	
    fit_params.append_and_update(model_fit_params);
    //fit_params.append_and_update(&element_fit_params);

    emit export_fit_paramters(fit_params, element_to_fit);
}

//---------------------------------------------------------------------------

void FitSpectraWidget::on_export_csv()
{
    emit export_csv_and_png(_spectra_widget->getPngofChart(), &_ev, _int_spec, &_spectra_background, &_fit_spec, &_labeled_spectras);
}

//---------------------------------------------------------------------------

void FitSpectraWidget::replot_integrated_spectra(bool snipback)
{
    if (_int_spec != nullptr)
    {        

        data_struct::Fit_Parameters<double> fit_params = _fit_params_table_model->getFitParams();

        fitting::models::Range energy_range;
        energy_range.min = 0;
        energy_range.max = _int_spec->size() - 1;

        ArrayDr energy = ArrayDr::LinSpaced(energy_range.count(), energy_range.min, energy_range.max);

        _ev = fit_params[STR_ENERGY_OFFSET].value + energy * fit_params[STR_ENERGY_SLOPE].value + pow(energy, 2.0) * fit_params[STR_ENERGY_QUADRATIC].value;
        
        /*
        ArrayDr weights = *_int_spec;
        weights = weights.unaryExpr([](double v) { return v == 0.0 ? v : 1.0 / v; });
        weights = convolve1d(weights, 5);
        weights = Eigen::abs(weights);
        weights /= weights.maxCoeff();
        _spectra_widget->append_spectra("weights", &weights, (data_struct::Spectra<double>*) & _ev);
        */
        /*
        ArrayDr weights = *_int_spec;
        weights = weights.log10();
        weights = weights.unaryExpr([](double v) { return std::isfinite(v) ? v : 0.0; });
        weights = convolve1d(weights, 5);
        weights = Eigen::abs(weights);
        weights /= weights.maxCoeff();
        _spectra_widget->append_spectra("weights", &weights, (data_struct::Spectra<double>*) & _ev);
        */
        _spectra_widget->append_spectra(DEF_STR_INT_SPECTRA, _int_spec, (data_struct::Spectra<double>*) & _ev);
        if (snipback)
        {
            Range range = get_energy_range(_int_spec->size(), &fit_params);
            _spectra_background = snip_background<double>((Spectra<double>*)_int_spec,
                fit_params[STR_ENERGY_OFFSET].value,
                fit_params[STR_ENERGY_SLOPE].value,
                fit_params[STR_ENERGY_QUADRATIC].value,
                fit_params[STR_SNIP_WIDTH].value,
                range.min,
                range.max);

            _spectra_widget->append_spectra(DEF_STR_BACK_SPECTRA, &_spectra_background, (data_struct::Spectra<double>*) & _ev);
            //TESTING
            //_spectra_background = snip_background2<double>((Spectra<double>*)_int_spec, fit_params[STR_SNIP_WIDTH].value);
            //_spectra_widget->append_spectra("SNIP", &_spectra_background, (data_struct::Spectra<double>*) & _ev);


        }
        _spectra_widget->setXLabel("Energy (keV)");

        /*
        _showFitIntSpec = Preferences::inst()->getValue(STR_PFR_SHOW_FIT_INT_SPEC).toBool();
        if (_showFitIntSpec)
        {
            for (auto &itr : _fit_int_spec_map)
            {
                QString name = "Fitted_Int_" + QString(itr.first.c_str());
                _spectra_widget->append_spectra(name, itr.second, (data_struct::Spectra<double>*)&_ev);
            }
        }
        */

        _showFitIntMatrix = Preferences::inst()->getValue(STR_PFR_SHOW_FIT_INT_MATRIX).toBool();
        if (_showFitIntMatrix)
        {
            if (_fit_int_spec_map.count(STR_FIT_GAUSS_MATRIX) > 0)
            {
                QString name = "Fitted_Int_" + QString(STR_FIT_GAUSS_MATRIX.c_str());
                _spectra_widget->append_spectra(name, _fit_int_spec_map.at(STR_FIT_GAUSS_MATRIX), (data_struct::Spectra<double>*) & _ev);
            }
        }
        else
        {
            QString name = "Fitted_Int_" + QString(STR_FIT_GAUSS_MATRIX.c_str());
            _spectra_widget->remove_spectra(name);
        }

        _showFitIntNNLS = Preferences::inst()->getValue(STR_PFR_SHOW_FIT_INT_NNLS).toBool();
        if (_showFitIntNNLS)
        {
            if (_fit_int_spec_map.count(STR_FIT_NNLS) > 0)
            {
                QString name = "Fitted_Int_" + QString(STR_FIT_NNLS.c_str());
                _spectra_widget->append_spectra(name, _fit_int_spec_map.at(STR_FIT_NNLS), (data_struct::Spectra<double>*) & _ev);
            }
        }
        else
        {
            QString name = "Fitted_Int_" + QString(STR_FIT_NNLS.c_str());
            _spectra_widget->remove_spectra(name);
        }

        _showMaxChanSpec = Preferences::inst()->getValue(STR_PFR_SHOW_MAX_CHAN_SPEC).toBool();
        if (_showMaxChanSpec)
        {
            for (auto& itr : _max_chan_spec_map)
            {
                _spectra_widget->append_spectra(QString(itr.first.c_str()), itr.second, (data_struct::Spectra<double>*) & _ev);
            }
        }

        if (_displayROIs)
        {
            for (auto& itr : _roi_spec_map)
            {
                QColor* color = &(_roi_spec_colors.at(itr.first));
                _spectra_widget->append_spectra(QString(itr.first.c_str()), itr.second, (data_struct::Spectra<double>*) & _ev, color);
            }
        }
    }
}

//---------------------------------------------------------------------------

void FitSpectraWidget::appendFitIntSpectra(std::string name, ArrayDr* spec)
{
    _fit_int_spec_map[name] = spec;
}

//---------------------------------------------------------------------------

void FitSpectraWidget::appendMaxChanSpectra(std::string name, const ArrayDr* spec)
{
    _max_chan_spec_map[name] = spec;
}

//---------------------------------------------------------------------------

void FitSpectraWidget::appendROISpectra(std::string name, ArrayDr* spec, QColor color)
{
    _roi_spec_map[name] = spec;
    _roi_spec_colors[name] = color;
}

//---------------------------------------------------------------------------

void FitSpectraWidget::deleteROISpectra(std::string name)
{
    if (_roi_spec_map.count(name) > 0)
    {
        ArrayDr* spec = _roi_spec_map.at(name);
        if (spec != nullptr)
        {
            spec->resize(1);
            // delete spec; throws exception , need to investigate TODO
        }
        _roi_spec_map.erase(name);
        _spectra_widget->remove_spectra(QString(name.c_str()));
    }
    replot_integrated_spectra(false);
}

//---------------------------------------------------------------------------

void FitSpectraWidget::deleteAllROISpectra()
{
    for (auto &itr : _roi_spec_map)
    {
        ArrayDr* spec = itr.second;
        _spectra_widget->remove_spectra(QString(itr.first.c_str()));
        if (spec != nullptr)
        {
            spec->resize(1);
            // delete spec; throws exception , need to investigate TODO
        }
    }
    _roi_spec_map.clear();
    replot_integrated_spectra(false);
}

//---------------------------------------------------------------------------

void FitSpectraWidget::clearFitIntSpectra()
{
    for (auto& itr : _fit_int_spec_map)
    {
        QString name = "Fitted_Int_" + QString(itr.first.c_str());
        _spectra_widget->remove_spectra(name);
    }
    _fit_int_spec_map.clear();

}

//---------------------------------------------------------------------------

void FitSpectraWidget::clearMaxChanSpectra()
{
    for (auto& itr : _max_chan_spec_map)
    {
        _spectra_widget->remove_spectra(QString(itr.first.c_str()));
    }
    _max_chan_spec_map.clear();

}
//---------------------------------------------------------------------------

void FitSpectraWidget::clearROISpectra()
{
    for (auto& itr : _roi_spec_map)
    {
        _spectra_widget->remove_spectra(QString(itr.first.c_str()));
    }
    _roi_spec_map.clear();
}

//---------------------------------------------------------------------------

void FitSpectraWidget::display_periodic_table()
{
    _periodic_table_widget->show();
}

//---------------------------------------------------------------------------

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
            std::map<int, double> ratios = _param_override->get_custom_factor(el_name.toStdString());
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

    update_spectra_top_axis();
}

//---------------------------------------------------------------------------

void FitSpectraWidget::del_element()
{
    //get selected elements to remove
    QModelIndexList selected_rows = _fit_elements_table->selectionModel()->selectedRows();
    for( auto i : selected_rows)
    {
        if(i.isValid())
        {
            data_struct::Fit_Element_Map<double>* fit_element = nullptr;
            QString out_name;
            bool is_parent = false;
            _fit_elements_table_model->getElementByIndex(i, &fit_element, out_name, is_parent);

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
    update_spectra_top_axis();
    if(_chk_auto_model->isChecked())
    {
        Model_Spectra_Click();
    }
}

//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------

void FitSpectraWidget::Fit_Spectra_Click()
{
//    _btn_fit_spectra->setEnabled(false);
//    _btn_model_spectra->setEnabled(false);

    

    if(_elements_to_fit != nullptr && _int_spec != nullptr)
    {
        data_struct::Fit_Parameters<double> out_fit_params = _fit_params_table_model->getFitParams();
        data_struct::Fit_Parameters<double> element_fit_params = _fit_elements_table_model->getAsFitParams();


        if (_fitting_dialog == nullptr)
        {
            _fitting_dialog = new FittingDialog();
        }
        _fitting_dialog->updateFitParams(out_fit_params, element_fit_params);
       // _fitting_dialog->setOptimizer(_cb_opttimizer->currentText());
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

        if (_fitting_dialog->accepted_fit())
        {

            disconnect(_fit_params_table_model,&FitParamsTableModel::dataChanged,this,&FitSpectraWidget::Model_Spectra_Val_Change);

            disconnect(_fit_elements_table_model,&FitElementsTableModel::dataChanged,this,&FitSpectraWidget::Model_Spectra_Val_Change);

            //_fit_params_table_model->updateFitParams(&out_fit_params);

            //_fit_elements_table_model->updateElementValues(&out_fit_params);

			_fit_params_table_model->updateFitParams(_fitting_dialog->get_new_fit_params());

			_fit_elements_table_model->updateElementValues(_fitting_dialog->get_new_fit_params());


            if (_chk_auto_model->checkState() == Qt::Checked)
            {
                connect(_fit_params_table_model,&FitParamsTableModel::dataChanged,this,&FitSpectraWidget::Model_Spectra_Val_Change);
                connect(_fit_elements_table_model,&FitElementsTableModel::dataChanged,this,&FitSpectraWidget::Model_Spectra_Val_Change);

                Model_Spectra_Click();
            }

            replot_integrated_spectra(true);
			
			_fit_spec = _fitting_dialog->get_fit_spectra(&_labeled_spectras);
            _spectra_widget->append_spectra(DEF_STR_FIT_INT_SPECTRA, &_fit_spec, (data_struct::Spectra<double>*) & _ev);
            if (_showDetailedFitSpec)
            {
                for (auto& itr : _labeled_spectras)
                {
                    _spectra_widget->append_spectra(QString(itr.first.c_str()), (Spectra<double>*)&itr.second, (data_struct::Spectra<double>*) & _ev);
                }
            }

            // update background 
            data_struct::Fit_Parameters<double> *fit_params = _fitting_dialog->get_new_fit_params();
            if (fit_params != nullptr && fit_params->contains(STR_SNIP_WIDTH))
            {
                if (fit_params->at(STR_SNIP_WIDTH).bound_type != E_Bound_Type::FIXED)
                {
                    Range range = get_energy_range(_fit_spec.size(), fit_params);
                    _spectra_background = snip_background<double>((Spectra<double>*) & _fit_spec,
                        fit_params->at(STR_ENERGY_OFFSET).value,
                        fit_params->at(STR_ENERGY_SLOPE).value,
                        fit_params->at(STR_ENERGY_QUADRATIC).value,
                        fit_params->at(STR_SNIP_WIDTH).value,
                        range.min,
                        range.max);

                    _spectra_widget->append_spectra(DEF_STR_BACK_SPECTRA, &_spectra_background, (data_struct::Spectra<double>*) & _ev);

                    //TESTING
                    //_spectra_background = snip_background2<double>((Spectra<double>*)_int_spec, fit_params->at(STR_SNIP_WIDTH).value);
                    //_spectra_widget->append_spectra("SNIP", &_spectra_background, (data_struct::Spectra<double>*) & _ev);
                }
            }
            emit signal_finished_fit();		
        }

        _fitting_dialog->waitToFinishRunning();
        delete _fitting_dialog;
        _fitting_dialog = nullptr;
    }

}

//---------------------------------------------------------------------------

void FitSpectraWidget::Fit_ROI_Spectra_Click()
{
    // bring up dialog to select roi 
    QString roi_name;
    ArrayDr* roi_spec = nullptr;

    if (_roi_spec_map.size() > 1)
    {
        QInputDialog qDialog;

        QStringList items;
        for (auto itr : _roi_spec_map)
        {
            items << QString(itr.first.c_str());
        }

        qDialog.setOptions(QInputDialog::UseListViewForComboBoxItems);
        qDialog.setComboBoxItems(items);
        qDialog.setWindowTitle("Select ROI to fit");

        qDialog.exec();

        roi_name = qDialog.textValue();

        if (_roi_spec_map.count(roi_name.toStdString()) > 0)
        {
            roi_spec = _roi_spec_map.at(roi_name.toStdString());
        }
        else
        {
            logE << "Could not find roi spectra " << roi_name.toStdString() << "\n";
            return;
        }
    }
    else
    {
        for (auto itr : _roi_spec_map)
        {
            roi_name = QString(itr.first.c_str());
            roi_spec = itr.second;
            break;
        }
    }
   

    if (_elements_to_fit != nullptr && roi_spec != nullptr)
    {
        data_struct::Fit_Parameters<double> out_fit_params = _fit_params_table_model->getFitParams();
        data_struct::Fit_Parameters<double> element_fit_params = _fit_elements_table_model->getAsFitParams();


        if (_fitting_dialog == nullptr)
        {
            _fitting_dialog = new FittingDialog();
        }
        _fitting_dialog->updateFitParams(out_fit_params, element_fit_params);
        //_fitting_dialog->setOptimizer(_cb_opttimizer->currentText());
        _fitting_dialog->setSpectra((Spectra<double>*)roi_spec, _ev);
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
        // save results to file

        if (_fitting_dialog->accepted_fit())
        {
            _fit_spec = _fitting_dialog->get_fit_spectra(&_labeled_spectras);
            if (_fit_spec.size() == _spectra_background.size())
            {
                _fit_spec += _spectra_background;
            }

            fitting::optimizers::OPTIMIZER_OUTCOME outcome = _fitting_dialog->getOutcome();
            std::string result = fitting::optimizers::optimizer_outcome_to_str(outcome);
            data_struct::Fit_Element_Map_Dict<double> elements_to_fit = _fitting_dialog->get_elements_to_fit();
            data_struct::Fit_Parameters<double>*  new_fit_params = _fitting_dialog->get_new_fit_params();

            //int detector_num = -1;
            QDir tmp_dir = _dataset_dir;
            tmp_dir.cdUp();
            tmp_dir.cdUp();
            QFileInfo finfo(_dataset_dir.absolutePath());
            
            int detector_num = finfo.completeSuffix().toInt();

            QString roi_file_name = finfo.fileName() + "_roi_" + roi_name;
            QString roi_override_name = tmp_dir.absolutePath() + QDir::separator() +"maps_fit_parameters_override_"+roi_file_name+".txt"+QString::number(detector_num);
            io::file::save_optimized_fit_params(tmp_dir.absolutePath().toStdString(), roi_file_name.toStdString(), detector_num, result, new_fit_params, (Spectra<double>*)roi_spec, &elements_to_fit);
            //io::file::aps::create_detector_fit_params_from_avg(roi_override_name.toStdString(), *new_fit_params, detector_num);
            _param_override->fit_params.update_and_add_values(new_fit_params);
            io::file::aps::save_parameters_override(roi_override_name.toStdString(), _param_override);
            // open file location
            tmp_dir.cd("output");
            if (false == QDesktopServices::openUrl(QUrl::fromLocalFile(tmp_dir.absolutePath())))
            {
                logE << "Failed to open dir " << tmp_dir.absolutePath().toStdString() << "\n";
            }
        }

        delete _fitting_dialog;
        _fitting_dialog = nullptr;
    }
}

//---------------------------------------------------------------------------

void FitSpectraWidget::Model_Spectra_Val_Change(QModelIndex,QModelIndex,QVector<int>)
{
    Model_Spectra_Click();
}

//---------------------------------------------------------------------------

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

        std::unordered_map<std::string, ArrayTr<double>> labeled_spectras;
        data_struct::Spectra<double> fit_spec = model.model_spectrum(&fit_params, _elements_to_fit, &labeled_spectras, energy_range);

        replot_integrated_spectra(true);

        if(fit_spec.size() == _spectra_background.size())
        {
            fit_spec += _spectra_background;
        }

        _spectra_widget->append_spectra(DEF_STR_MODEL_SPECTRA, &fit_spec, (data_struct::Spectra<double>*)&_ev);
    }
    emit signal_finished_fit();

}

//---------------------------------------------------------------------------

void FitSpectraWidget::finished_fitting()
{
    _btn_fit_spectra->setEnabled(true);
    if(_chk_auto_model->checkState() == Qt::Checked)
        _btn_model_spectra->setEnabled(false);
    else
        _btn_model_spectra->setEnabled(true);

	replot_integrated_spectra(true);
}

//---------------------------------------------------------------------------

void FitSpectraWidget::fit_params_customMenuRequested(QPoint pos)
{
    _fit_param_contextMenu->exec(_fit_params_table->mapToGlobal(pos));
}

//---------------------------------------------------------------------------

void FitSpectraWidget::set_fit_params_bounds(data_struct::E_Bound_Type e_type)
{
    QModelIndexList selected_rows = _fit_params_table->selectionModel()->selectedIndexes();
    for(QModelIndex index : selected_rows)
    {
        _fit_params_table_model->setDataFitBounds(index, e_type);
    }
}

//---------------------------------------------------------------------------

void FitSpectraWidget::check_auto_model(int state)
{
    if(state == Qt::Checked)
    {
        _btn_model_spectra->setEnabled(false);
        Model_Spectra_Click();
        connect(_fit_params_table_model,&FitParamsTableModel::dataChanged,this,&FitSpectraWidget::Model_Spectra_Val_Change);
        connect(_fit_elements_table_model,&FitElementsTableModel::dataChanged,this,&FitSpectraWidget::Model_Spectra_Val_Change);
    }
    else
    {
        _btn_model_spectra->setEnabled(true);
        disconnect(_fit_params_table_model,&FitParamsTableModel::dataChanged,this,&FitSpectraWidget::Model_Spectra_Val_Change);
        disconnect(_fit_elements_table_model,&FitElementsTableModel::dataChanged,this,&FitSpectraWidget::Model_Spectra_Val_Change);
    }
}

//---------------------------------------------------------------------------

void FitSpectraWidget::element_selection_changed_from(QModelIndex current, QModelIndex previous)
{

    data_struct::Fit_Element_Map<double>* out_element = nullptr;
    QString out_name;
    bool is_parent = false;
    if(_fit_elements_table_model->getElementByIndex(current, &out_element, out_name, is_parent))
    {
        if(is_parent)
        {
            _spectra_widget->set_element_lines(out_element);
        }
        else // is element line, show ROI width instead 
        {
            if(data_struct::Str_Element_Param_Map.count(out_name.toStdString()))
            {
                _spectra_widget->set_element_width(out_element, data_struct::Str_Element_Param_Map.at(out_name.toStdString()));
            }
        }
    }
}

//---------------------------------------------------------------------------

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
        std::map<int, double> ratios = _param_override->get_custom_factor(full_name.toStdString());
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

//---------------------------------------------------------------------------

void FitSpectraWidget::optimizer_preset_changed(int val)
{
    switch (val)
    {
        case 0:
            //_fit_params_table_model->setOptimizerPreset(fitting::models::Fit_Params_Preset::NOT_SET);
            break;
        case 1:
            _fit_params_table_model->setOptimizerPreset(fitting::models::Fit_Params_Preset::MATRIX_BATCH_FIT);
            break;
        case 2:
            _fit_params_table_model->setOptimizerPreset(fitting::models::Fit_Params_Preset::BATCH_FIT_NO_TAILS);
            break;
        case 3:
            _fit_params_table_model->setOptimizerPreset(fitting::models::Fit_Params_Preset::BATCH_FIT_WITH_TAILS);
            break;
        case 4:
            _fit_params_table_model->setOptimizerPreset(fitting::models::Fit_Params_Preset::BATCH_FIT_WITH_FREE_ENERGY);
            break;
        case 5:
            _fit_params_table_model->setOptimizerPreset(fitting::models::Fit_Params_Preset::BATCH_FIT_NO_TAILS_E_QUAD);
            break;
        default:
            break;
    }
    _fit_params_table->resizeColumnToContents(0);
}

//---------------------------------------------------------------------------

void FitSpectraWidget::setIntegratedSpectra(ArrayDr* int_spec)
{
	_int_spec = int_spec;
    replot_integrated_spectra(true);
    _spectra_widget->onResetChartViewOnlyY();
}

//---------------------------------------------------------------------------

void FitSpectraWidget::setFitParams(data_struct::Fit_Parameters<double>* fit_params)
{
    _fit_params_table_model->updateFitParams(fit_params);
}

//---------------------------------------------------------------------------

void FitSpectraWidget::setElementsToFit(data_struct::Fit_Element_Map_Dict<double>* elements_to_fit)
{
    _fit_elements_table_model->updateFitElements(elements_to_fit);
	_elements_to_fit = elements_to_fit;
    update_spectra_top_axis();
}

//---------------------------------------------------------------------------

void FitSpectraWidget::update_spectra_top_axis()
{
    if (_elements_to_fit != nullptr)
    {
        _spectra_widget->clear_top_axis();

        std::map < float, std::string> labels;
        for (const auto& itr : *_elements_to_fit)
        {
            Fit_Element_Map<double> em(itr.first, Element_Info_Map<double>::inst()->get_element(itr.first));
            if (_param_override != nullptr)
            {
                std::map<int, double> ratios = _param_override->get_custom_factor(itr.first);
                for (const auto& itr2 : ratios)
                {
                    em.multiply_custom_multiply_ratio(itr2.first, itr2.second);
                }
            }

            em.init_energy_ratio_for_detector_element(data_struct::Element_Info_Map<double>::inst()->get_element(_detector_element));
            if (em.center() > 0.)
            {
                labels[em.center()] = itr.first;
            }
        }
        _spectra_widget->set_top_axis(labels);
    }
}

//---------------------------------------------------------------------------