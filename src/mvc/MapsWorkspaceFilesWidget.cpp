/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsWorkspaceFilesWidget.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QRegExp>
#include "core/GlobalThreadPool.h"

/*---------------------------------------------------------------------------*/

MapsWorkspaceFilesWidget::MapsWorkspaceFilesWidget(QWidget* parent) : QWidget(parent)
{

    _per_pixel_fit_widget = nullptr;
	_model = nullptr;
    createLayout();

}

/*---------------------------------------------------------------------------*/

MapsWorkspaceFilesWidget::~MapsWorkspaceFilesWidget()
{
    
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::createLayout()
{
    std::vector<std::string> bound_types {"Not Initialized", "Fixed", "Limited Low High", "Limited Low", "Limited High", "Fit"};
    _lbl_workspace = new QLabel();
    _tab_widget = new QTabWidget();

    _h5_tab_widget = new FileTabWidget();
    QAction *h5avg_file = new QAction("All Avg files  | *.h5", this);
    connect(h5avg_file, &QAction::triggered, [this](bool val){ _h5_tab_widget->filterTextChanged("*.h5"); });
    QAction *h5det_file = new QAction("All Detector 0 | *.h50", this);
    connect(h5det_file, &QAction::triggered, [this](bool val){ _h5_tab_widget->filterTextChanged("*.h50"); });
    _h5_tab_widget->appendFilterHelpAction(h5avg_file);
    _h5_tab_widget->appendFilterHelpAction(h5det_file);

    connect(_h5_tab_widget, &FileTabWidget::customContext, this, &MapsWorkspaceFilesWidget::onPerPixelProcess);

    _h5_tab_widget->addCustomContext("hdf5", "Per Pixel Process");
    connect(_h5_tab_widget, &FileTabWidget::loadList, [this](const QStringList& sl) { this->onOpenModel(sl, MODEL_TYPE::MAPS_H5); });
    connect(_h5_tab_widget, &FileTabWidget::unloadList, [this](const QStringList& sl) { this->onCloseModel(sl, MODEL_TYPE::MAPS_H5); });
    connect(_h5_tab_widget, &FileTabWidget::processList, this, &MapsWorkspaceFilesWidget::onPerPixelProcessList);

    _mda_tab_widget = new FileTabWidget();
    connect(_mda_tab_widget, &FileTabWidget::loadList, [this](const QStringList& sl) { this->onOpenModel(sl, MODEL_TYPE::RAW); });
    connect(_mda_tab_widget, &FileTabWidget::unloadList, [this](const QStringList& sl) { this->onCloseModel(sl, MODEL_TYPE::RAW); });
    connect(_mda_tab_widget, &FileTabWidget::processList, this, &MapsWorkspaceFilesWidget::onPerPixelProcessList);
    connect(_mda_tab_widget, &FileTabWidget::customContext, this, &MapsWorkspaceFilesWidget::onPerPixelProcess);
    _mda_tab_widget->addCustomContext("mda", "Per Pixel Process");

    _sws_tab_widget = new FileTabWidget();
    _sws_tab_widget->setProcessButtonVisible(false);
    connect(_sws_tab_widget, &FileTabWidget::loadList, [this](const QStringList& sl) { this->onOpenModel(sl, MODEL_TYPE::SWS); });
    connect(_sws_tab_widget, &FileTabWidget::unloadList, [this](const QStringList& sl) { this->onCloseModel(sl, MODEL_TYPE::SWS); });

    _fit_params_table_model = new FitParamsTableModel();
    ComboBoxDelegate *cbDelegate = new ComboBoxDelegate(bound_types);

    /*
    _fit_params_table = new QTableView();
    _fit_params_table->setModel(_fit_params_table_model);
    //_fit_params_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //_fit_params_table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    _fit_params_table->sortByColumn(0, Qt::AscendingOrder);
    _fit_params_table->setItemDelegateForColumn(2, cbDelegate);
    */
    QLayout* vlayout = new QVBoxLayout();

    _tab_widget->insertTab(0, _h5_tab_widget, "Analyized Data");
    _tab_widget->insertTab(1, _mda_tab_widget, "Raw Data");
    _tab_widget->insertTab(2, _sws_tab_widget, "Light Microscope");

    vlayout->addWidget(_tab_widget);
    setLayout(vlayout);
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::setModel(MapsWorkspaceModel *model)
{
	if (_model != nullptr)
	{
        disconnect(_model, &MapsWorkspaceModel::doneLoadingMDA, this, &MapsWorkspaceFilesWidget::updatedMDA);
        disconnect(_model, &MapsWorkspaceModel::doneLoadingVLM, this, &MapsWorkspaceFilesWidget::updatedVLM);
        disconnect(_model, &MapsWorkspaceModel::doneLoadingImgDat, this, &MapsWorkspaceFilesWidget::updatedHDF);
        disconnect(_model, &MapsWorkspaceModel::doneUnloading, this, &MapsWorkspaceFilesWidget::clearLists);
        disconnect(_model, &MapsWorkspaceModel::newFitParamsFileLoaded, this, &MapsWorkspaceFilesWidget::loadedFitParams);
	}
	
	_model = model;
	if (_model != nullptr)
	{
		QString path = _model->get_directory_name();
		if (path.length() > 0)
		{
			_lbl_workspace->setText(path);
		}

        connect(_model, &MapsWorkspaceModel::doneLoadingMDA, this, &MapsWorkspaceFilesWidget::updatedMDA);
        connect(_model, &MapsWorkspaceModel::doneLoadingVLM, this, &MapsWorkspaceFilesWidget::updatedVLM);
        connect(_model, &MapsWorkspaceModel::doneLoadingImgDat, this, &MapsWorkspaceFilesWidget::updatedHDF);
        connect(_model, &MapsWorkspaceModel::doneUnloading, this, &MapsWorkspaceFilesWidget::clearLists);
        connect(_model, &MapsWorkspaceModel::newFitParamsFileLoaded, this, &MapsWorkspaceFilesWidget::loadedFitParams);
        connect(_h5_tab_widget, &FileTabWidget::onRefresh, _model, &MapsWorkspaceModel::reload_analyzed);
        connect(_sws_tab_widget, &FileTabWidget::onRefresh, _model, &MapsWorkspaceModel::reload_vlm);
        connect(_mda_tab_widget, &FileTabWidget::onRefresh, _model, &MapsWorkspaceModel::reload_raw);
	}

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::updatedMDA()
{
    _mda_tab_widget->set_file_list(_model->get_raw_file_list());
    vector<QString> loaded_names = _model->get_loaded_raw_names();
    for (const auto& itr : loaded_names)
    {
        _mda_tab_widget->loaded_file_status_changed(File_Loaded_Status::LOADED, itr);
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::updatedVLM()
{
    _sws_tab_widget->set_file_list(_model->get_sws_file_list());
    vector<QString> loaded_names = _model->get_loaded_vlm_names();
    for (const auto& itr : loaded_names)
    {
        _sws_tab_widget->loaded_file_status_changed(File_Loaded_Status::LOADED, itr);
    }
}
/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::updatedHDF()
{
    _h5_tab_widget->set_file_list(_model->get_hdf5_file_list());
    vector<QString> loaded_names = _model->get_loaded_h5_names();
    for (const auto& itr : loaded_names)
    {
        _h5_tab_widget->loaded_file_status_changed(File_Loaded_Status::LOADED, itr);
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::loadedFitParams(int idx)
{
    if(idx == -1) //avg fit params
    {
        data_struct::Fit_Parameters* fit_params = _model->getFitParameters(idx);
        if(fit_params != nullptr)
        {
            _fit_params_table_model->setFitParams(*fit_params);
        }
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::onOpenModel(const QStringList& names_list, MODEL_TYPE mt)
{

    if (_model != nullptr)
    {

        int amt = names_list.count();
        int cur = 0;

        foreach(QString name, names_list)
        {
            File_Loaded_Status load_status = UNLOADED;

            if (mt == MODEL_TYPE::MAPS_H5)
            {
                std::future< MapsH5Model*> ret = Global_Thread_Pool::inst()->enqueue([this, name] { return _model->get_MapsH5_Model(name); });
                while (ret._Is_ready() == false)
                {
                    QCoreApplication::processEvents();
                }

                MapsH5Model* h5Model = ret.get();
                if (h5Model != nullptr)
                {
                    int idx = -1;
                    if (name.endsWith('0'))
                    {
                        idx = 0;
                    }
                    else if (name.endsWith('1'))
                    {
                        idx = 1;
                    }
                    else if (name.endsWith('2'))
                    {
                        idx = 2;
                    }
                    else if (name.endsWith('3'))
                    {
                        idx = 3;
                    }
                    data_struct::Params_Override* param_override = _model->getParamOverride(idx);
                    //if we can't find override for specific detector, try to use the avg one.
                    if (param_override == nullptr)
                    {
                        param_override = _model->getParamOverride(-1);
                    }
                    h5Model->set_fit_parameters_override(param_override);
                    emit loaded_model(name, mt);
                    load_status = LOADED;
                }
                else
                {
                    load_status = FAILED_LOADING;
                }
                _h5_tab_widget->loaded_file_status_changed(load_status, name);
            }
            else if (mt == MODEL_TYPE::RAW)
            {
				RAW_Model* model = nullptr;
                
                QApplication::setOverrideCursor(Qt::WaitCursor);
                
				if (name.endsWith(".mda"))
				{
					QCoreApplication::processEvents();
					// loading through background thread on windows seems to lock up on fopen
					model = _model->get_RAW_Model(name);
				}
				if (name.endsWith(".h5") || name.endsWith(".hdf5") || name.endsWith(".emd"))
				{
					QCoreApplication::processEvents();
					std::future< RAW_Model*> ret = Global_Thread_Pool::inst()->enqueue([this, name] { return _model->get_RAW_Model(name); });
					while (ret._Is_ready() == false)
					{
						QCoreApplication::processEvents();
					}

					model = ret.get();
				}
                QApplication::restoreOverrideCursor();
                /*
                
                QCoreApplication::processEvents();
                std::future< RAW_Model*> ret = Global_Thread_Pool::inst()->enqueue([this, name] { return _model->get_RAW_Model(name); });
                while (ret._Is_ready() == false)
                {
                    QCoreApplication::processEvents();
                }

                model = ret.get();
                */
                if (model != nullptr)
                {
                    emit loaded_model(name, mt);
                    load_status = LOADED;
                }
                else
                {
                    load_status = FAILED_LOADING;
                }
                emit _mda_tab_widget->loaded_file_status_changed(load_status, name);
            }
            else if (mt == MODEL_TYPE::SWS)
            {
                std::future< SWSModel*> ret = Global_Thread_Pool::inst()->enqueue([this, name] { return _model->get_SWS_Model(name); });
                while (ret._Is_ready() == false)
                {
                    QCoreApplication::processEvents();
                }

                SWSModel* Model = ret.get();
                if (Model != nullptr)
                {
                    load_status = LOADED;
                    emit loaded_model(name, mt);
                }
                else
                {
                    load_status = FAILED_LOADING;
                }
                _sws_tab_widget->loaded_file_status_changed(load_status, name);
            }

            cur++;
            emit loaded_perc(cur, amt);
            QCoreApplication::processEvents();
        }
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::onCloseModel(const QStringList& names_list, MODEL_TYPE mt)
{
    File_Loaded_Status load_status = UNLOADED;
    if (_model != nullptr)
    {
        foreach(QString name, names_list)
        {
            switch (mt)
            {
            case MODEL_TYPE::MAPS_H5:
                _model->unload_H5_Model(name);
                _h5_tab_widget->loaded_file_status_changed(load_status, name);
                break;
            case MODEL_TYPE::RAW:
                _model->unload_RAW_Model(name);
                _mda_tab_widget->loaded_file_status_changed(load_status, name);
                break;
            case MODEL_TYPE::SWS:
                _model->unload_SWS_Model(name);
                _sws_tab_widget->loaded_file_status_changed(load_status, name);
                break;
            }
        }
    }
    emit unloadList_model(names_list, mt);
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::clearLists()
{
	_h5_tab_widget->unload_all();
	_mda_tab_widget->unload_all();
	_sws_tab_widget->unload_all();
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::onPerPixelProcess(const QString& context_label, const QStringList& file_list)
{
    onPerPixelProcessList(file_list);
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::onPerPixelProcessList(const QStringList& file_list)
{

    //create per pixel process widget and pass workspace
    if(_per_pixel_fit_widget == nullptr)
    {
        _per_pixel_fit_widget = new PerPixelFitWidget(_model->get_directory_name().toStdString());
        connect(_per_pixel_fit_widget, &PerPixelFitWidget::processed_list_update, this, &MapsWorkspaceFilesWidget::onProcessed_list_update);
    }
    _per_pixel_fit_widget->updateFileList(file_list);
    _per_pixel_fit_widget->show();
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::onProcessed_list_update()
{
    if (_model != nullptr)
    {
        _model->reload_analyzed();
    }
}
