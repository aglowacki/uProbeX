/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsWorkspaceFilesWidget.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QRegExp>
#include <QDebug>

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

    connect(_h5_tab_widget, SIGNAL(loadList(const QStringList&)),
            this, SLOT(onOpenHDF5(const QStringList&)));

    connect(_h5_tab_widget, SIGNAL(unloadList(const QStringList&)),
            this, SLOT(onCloseHDF5(const QStringList&)));

    connect(this, SIGNAL(status_loaded_hdf5(File_Loaded_Status, const QString&)),
            _h5_tab_widget, SLOT(loaded_file_status_changed(File_Loaded_Status, const QString&)));

    _mda_tab_widget = new FileTabWidget();
    connect(_mda_tab_widget, SIGNAL(onOpenItem(QString)),
            this, SLOT(onOpenMDA(QString)));

    connect(_mda_tab_widget, SIGNAL(onCloseItem(QString)),
            this, SLOT(onCloseMDA(QString)));

    connect(this, SIGNAL(status_loaded_mda(File_Loaded_Status, const QString&)),
            _mda_tab_widget, SLOT(loaded_file_status_changed(File_Loaded_Status, const QString&)));

    connect(_mda_tab_widget, &FileTabWidget::customContext, this, &MapsWorkspaceFilesWidget::onPerPixelProcess);

    _mda_tab_widget->addCustomContext("mda", "Per Pixel Process");

    _sws_tab_widget = new FileTabWidget();
    connect(_sws_tab_widget, SIGNAL(onOpenItem(QString)),
            this, SLOT(onOpenSWS(QString)));

    connect(_sws_tab_widget, SIGNAL(onCloseItem(QString)),
            this, SLOT(onCloseSWS(QString)));

    connect(this, SIGNAL(status_loaded_sws(File_Loaded_Status, const QString&)),
            _sws_tab_widget, SLOT(loaded_file_status_changed(File_Loaded_Status, const QString&)));

    _fit_params_table_model = new FitParamsTableModel();
    ComboBoxDelegate *cbDelegate = new ComboBoxDelegate(bound_types);

    _fit_params_table = new QTableView();
    _fit_params_table->setModel(_fit_params_table_model);
    //_fit_params_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //_fit_params_table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    _fit_params_table->sortByColumn(0, Qt::AscendingOrder);
    _fit_params_table->setItemDelegateForColumn(2, cbDelegate);

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

        disconnect(_model,
            SIGNAL(doneLoadingMDA()),
            this,
            SLOT(updateMDA()));

        disconnect(_model,
            SIGNAL(doneLoadingVLM()),
            this,
            SLOT(updateSWS()));

		disconnect(_model,
            SIGNAL(doneLoadingImgDat()),
			this,
            SLOT(updateH5()));
/*
        disconnect(_model,
            SIGNAL(doneLoading()),
            this,
            SLOT(model_done_loading()));
  */
		disconnect(_model,
			SIGNAL(doneUnloading()),
			this,
			SLOT(model_done_unloading()));

		disconnect(_model,
			SIGNAL(newFitParamsFileLoaded(int)),
			this,
			SLOT(loadedFitParams(int)));
	}
	
	_model = model;
	if (_model != nullptr)
	{
		QString path = _model->get_directory_name();
		if (path.length() > 0)
		{
			_lbl_workspace->setText(path);
		}

        connect(_model,
            SIGNAL(doneLoadingMDA()),
            this,
            SLOT(updateMDA()));

        connect(_model,
            SIGNAL(doneLoadingVLM()),
            this,
            SLOT(updateSWS()));

        connect(_model,
            SIGNAL(doneLoadingImgDat()),
            this,
            SLOT(updateH5()));

        /*
		connect(_model,
			SIGNAL(doneLoading()),
			this,
			SLOT(model_done_loading()));
*/
		connect(_model,
			SIGNAL(doneUnloading()),
			this,
			SLOT(model_done_unloading()));

		connect(_model,
			SIGNAL(newFitParamsFileLoaded(int)),
			this,
			SLOT(loadedFitParams(int)));

	}

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::updateMDA()
{

    _mda_tab_widget->set_file_list(_model->get_raw_file_list());

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::updateH5()
{

    _h5_tab_widget->set_file_list(_model->get_hdf5_file_list());

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::updateSWS()
{

    _sws_tab_widget->set_file_list(_model->get_sws_file_list());

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::model_done_loading()
{

    _h5_tab_widget->set_file_list(_model->get_hdf5_file_list());
    _mda_tab_widget->set_file_list(_model->get_raw_file_list());
    _sws_tab_widget->set_file_list(_model->get_sws_file_list());

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::model_done_unloading()
{
    _h5_tab_widget->unload_all();
    _mda_tab_widget->unload_all();
    _sws_tab_widget->unload_all();
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

void MapsWorkspaceFilesWidget::onOpenHDF5(const QStringList& names_list)
{
    if(_model != nullptr)
    {

        int amt = names_list.count();
        int cur = 0;
        
        foreach (QString name , names_list)
        {
            QStringList opened_list;
            File_Loaded_Status load_status = UNLOADED;
            
            std::future< MapsH5Model*> ret = global_threadpool.enqueue([this, name] { return _model->getMapsH5Model(name); });
            while (ret._Is_ready() == false)
            {
                QCoreApplication::processEvents();
            }

            MapsH5Model* h5Model = ret.get();
            if(h5Model != nullptr)
            {
                int idx = -1;
                if(name.endsWith('0'))
                {
                    idx = 0;
                }
                else if(name.endsWith('1'))
                {
                    idx = 1;
                }
                else if(name.endsWith('2'))
                {
                    idx = 2;
                }
                else if(name.endsWith('3'))
                {
                    idx = 3;
                }
				data_struct::Params_Override *param_override = _model->getParamOverride(idx);
				//if we can't find override for specific detector, try to use the avg one.
				if (param_override == nullptr)
				{
					param_override = _model->getParamOverride(-1);
				}
                h5Model->set_fit_parameters_override( param_override );
                opened_list.append(name);
                emit loadList_H5(opened_list);
                load_status = LOADED;
                //emit showFitSpecWindow(h5Model, _model->getFitParameters(-1), _model->getElementToFit(-1));
            }
            else
            {
                load_status = FAILED_LOADING;
            }
            emit status_loaded_hdf5(load_status, name);
            cur++;
            emit loaded_perc(cur, amt);
            QCoreApplication::processEvents();
        }
        //emit loadList_H5(opened_list);
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::onCloseHDF5(const QStringList& names_list)
{
    if(_model != nullptr)
    {
        foreach (QString name , names_list)
        {
            File_Loaded_Status load_status = UNLOADED;
            _model->unload_H5_Model(name);
            emit status_loaded_hdf5(load_status, name);
        }
    }
    emit unloadList_H5(names_list);
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::onOpenMDA(QString name)
{
    if(_model != nullptr)
    {
        File_Loaded_Status load_status = UNLOADED;
        MDA_Model* mda_model = _model->get_MDA_Model(name);
        if(mda_model != nullptr)
        {
            load_status = LOADED;
            emit show_MDA_Window(mda_model);
        }
        else
        {
            load_status = FAILED_LOADING;
        }
        emit status_loaded_mda(load_status, name);
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::onCloseMDA(QString name)
{
    if(_model != nullptr)
    {
        File_Loaded_Status load_status = UNLOADED;
        _model->unload_MDA_Model(name);
        emit status_loaded_mda(load_status, name);
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::onOpenSWS(QString name)
{
    if(_model != nullptr)
    {
        File_Loaded_Status load_status = UNLOADED;
        SWSModel* sws_model = _model->get_SWS_Model(name);
        if(sws_model != nullptr)
        {
            load_status = LOADED;
            emit show_SWS_Window(sws_model);
        }
        else
        {
            load_status = FAILED_LOADING;
        }
        emit status_loaded_sws(load_status, name);
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::onCloseSWS(QString name)
{
    if(_model != nullptr)
    {
        File_Loaded_Status load_status = UNLOADED;
        _model->unload_SWS_Model(name);
        emit status_loaded_sws(load_status, name);
    }
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
    _model->reload_analyzed();

}