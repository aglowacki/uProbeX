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
#include <QMessageBox>
#include <preferences/Preferences.h>
#include "core/GlobalThreadPool.h"

const static QString STR_PROCESS("Per Pixel Process");
const static QString STR_PROCESS_ANALYZED("Per Pixel ReProcess");
const static QString STR_PROCESS_ANALYZED_ALL("Per Pixel ReProcess All");
const static QString STR_BATCH_ROI("Process ROI's");
const static QString STR_H5_EXPORT("hdf5_export");
const static QString STR_GEN_SCAN_AREA("Generate Scan Area");

/*---------------------------------------------------------------------------*/

MapsWorkspaceFilesWidget::MapsWorkspaceFilesWidget(QWidget* parent) : QWidget(parent)
{

    _per_pixel_fit_widget = nullptr;
    _batch_roi_fit_widget = nullptr;
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

    _h5_tab_widget->addCustomContext(STR_PROCESS_ANALYZED);
    _h5_tab_widget->addCustomContext(STR_BATCH_ROI);
    _h5_tab_widget->addCustomButtonRow(STR_PROCESS_ANALYZED, STR_BATCH_ROI);    
    // TODO: need to implement
    //_h5_tab_widget->addCustomContext(STR_H5_EXPORT, "Export Images"); 

    connect(_h5_tab_widget, &FileTabWidget::loadList, [this](const QStringList& sl) { this->onOpenModel(sl, MODEL_TYPE::MAPS_H5); });
    connect(_h5_tab_widget, &FileTabWidget::unloadList, [this](const QStringList& sl) { this->onCloseModel(sl, MODEL_TYPE::MAPS_H5); });
    connect(_h5_tab_widget, &FileTabWidget::customContext, this, &MapsWorkspaceFilesWidget::onCustomContext);
    connect(_h5_tab_widget, &FileTabWidget::customButton, this, &MapsWorkspaceFilesWidget::onCustomButton);
    connect(_h5_tab_widget, &FileTabWidget::selectNewRow, this, &MapsWorkspaceFilesWidget::onDatasetSelected);

    _mda_tab_widget = new FileTabWidget();
    connect(_mda_tab_widget, &FileTabWidget::loadList, [this](const QStringList& sl) { this->onOpenModel(sl, MODEL_TYPE::RAW); });
    connect(_mda_tab_widget, &FileTabWidget::unloadList, [this](const QStringList& sl) { this->onCloseModel(sl, MODEL_TYPE::RAW); });
    connect(_mda_tab_widget, &FileTabWidget::customContext, this, &MapsWorkspaceFilesWidget::onCustomContext);
    connect(_mda_tab_widget, &FileTabWidget::customButton, this, &MapsWorkspaceFilesWidget::onCustomButton);
    connect(_mda_tab_widget, &FileTabWidget::selectNewRow, this, &MapsWorkspaceFilesWidget::onDatasetSelected);
    _mda_tab_widget->addCustomContext(STR_PROCESS);
    _mda_tab_widget->addCustomContext(STR_BATCH_ROI);
    _mda_tab_widget->addCustomButtonRow(STR_PROCESS, STR_BATCH_ROI);

    _vlm_tab_widget = new FileTabWidget();
    QAction* sws_file = new QAction("SWS | *.sws", this);
    connect(sws_file, &QAction::triggered, [this](bool val) { _vlm_tab_widget->filterTextChanged("*.sws"); });
    QAction* tiff_file = new QAction("TIFF | *.tiff", this);
    connect(tiff_file, &QAction::triggered, [this](bool val) { _vlm_tab_widget->filterTextChanged("*.tiff"); });
    QAction* tif_file = new QAction("TIF | *.tif", this);
    connect(tif_file, &QAction::triggered, [this](bool val) { _vlm_tab_widget->filterTextChanged("*.tif"); });
    connect(_vlm_tab_widget, &FileTabWidget::loadList, [this](const QStringList& sl) { this->onOpenModel(sl, MODEL_TYPE::VLM); });
    connect(_vlm_tab_widget, &FileTabWidget::unloadList, [this](const QStringList& sl) { this->onCloseModel(sl, MODEL_TYPE::VLM); });
    connect(_vlm_tab_widget, &FileTabWidget::customContext, this, &MapsWorkspaceFilesWidget::onCustomContext);
    connect(_vlm_tab_widget, &FileTabWidget::customButton, this, &MapsWorkspaceFilesWidget::onCustomButton);
    connect(_vlm_tab_widget, &FileTabWidget::selectNewRow, this, &MapsWorkspaceFilesWidget::onDatasetSelected);
    _vlm_tab_widget->appendFilterHelpAction(sws_file);
    _vlm_tab_widget->appendFilterHelpAction(tiff_file);
    _vlm_tab_widget->appendFilterHelpAction(tif_file);
    _vlm_tab_widget->addCustomContext(STR_GEN_SCAN_AREA);
    _vlm_tab_widget->addCustomButtonRow(STR_GEN_SCAN_AREA);


    connect(&_gen_scan_vlm_widget, &GenScanVlmWidget::new_scan_area, this, &MapsWorkspaceFilesWidget::newScanArea);

    QLayout* vlayout = new QVBoxLayout();

    _tab_widget->insertTab(0, _h5_tab_widget, "Analyized Data");
    _tab_widget->insertTab(1, _mda_tab_widget, "Raw Data");
    _tab_widget->insertTab(2, _vlm_tab_widget, "Light Microscope");
    
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
        //disconnect(_model, &MapsWorkspaceModel::newFitParamsFileLoaded, this, &MapsWorkspaceFilesWidget::loadedFitParams);
	}
	
	_model = model;
	if (_model != nullptr)
	{
         //create per pixel process widget and pass workspace
        if(_per_pixel_fit_widget == nullptr)
        {
            _per_pixel_fit_widget = new PerPixelFitWidget();
            connect(_per_pixel_fit_widget, &PerPixelFitWidget::processed_list_update, this, &MapsWorkspaceFilesWidget::onProcessed_list_update);
        }

		QString path = _model->get_directory_name();
		if (path.length() > 0)
		{
			_lbl_workspace->setText(path);
		}

        connect(_model, &MapsWorkspaceModel::doneLoadingMDA, this, &MapsWorkspaceFilesWidget::updatedMDA);
        connect(_model, &MapsWorkspaceModel::doneLoadingVLM, this, &MapsWorkspaceFilesWidget::updatedVLM);
        connect(_model, &MapsWorkspaceModel::doneLoadingImgDat, this, &MapsWorkspaceFilesWidget::updatedHDF);
        connect(_model, &MapsWorkspaceModel::doneLoadingROIS, this, &MapsWorkspaceFilesWidget::updateROIS);
        connect(_model, &MapsWorkspaceModel::doneUnloading, this, &MapsWorkspaceFilesWidget::clearLists);
        //connect(_model, &MapsWorkspaceModel::newFitParamsFileLoaded, this, &MapsWorkspaceFilesWidget::loadedFitParams);
        connect(_h5_tab_widget, &FileTabWidget::onRefresh, _model, &MapsWorkspaceModel::reload_analyzed);
        connect(_vlm_tab_widget, &FileTabWidget::onRefresh, _model, &MapsWorkspaceModel::reload_vlm);
        connect(_mda_tab_widget, &FileTabWidget::onRefresh, _model, &MapsWorkspaceModel::reload_raw);
	}

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::updatedMDA()
{
    _mda_tab_widget->set_file_list(_model->get_raw_file_list());
    std::vector<QString> loaded_names = _model->get_loaded_raw_names();
    for (const auto& itr : loaded_names)
    {
        _mda_tab_widget->loaded_file_status_changed(File_Loaded_Status::LOADED, itr);
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::updatedVLM()
{
    _vlm_tab_widget->set_file_list(_model->get_vlm_file_list());
    std::vector<QString> loaded_names = _model->get_loaded_vlm_names();
    for (const auto& itr : loaded_names)
    {
        _vlm_tab_widget->loaded_file_status_changed(File_Loaded_Status::LOADED, itr);
    }
    //_vlm_tab_widget->set_roi_num_list(_model->get_region_num_list());
}
/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::updatedHDF()
{
    _h5_tab_widget->set_file_list(_model->get_hdf5_file_list());
    std::vector<QString> loaded_names = _model->get_loaded_h5_names();
    for (const auto& itr : loaded_names)
    {
        _h5_tab_widget->loaded_file_status_changed(File_Loaded_Status::LOADED, itr);
    }
    _h5_tab_widget->set_roi_num_list(_model->get_roi_num_list());
}

//---------------------------------------------------------------------------

void MapsWorkspaceFilesWidget::updateROIS()
{
    _h5_tab_widget->set_roi_num_list(_model->get_roi_num_list());
    _mda_tab_widget->set_roi_num_list(_model->get_roi_num_list());
}

//---------------------------------------------------------------------------

void MapsWorkspaceFilesWidget::update_roi_num(QString name, int val)
{
    _h5_tab_widget->set_roi_num(name, val);
    _mda_tab_widget->set_roi_num(name, val);
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::setFileTabActionsEnabled(bool val)
{
    
    _h5_tab_widget->setActionsAndButtonsEnabled(val);
    _mda_tab_widget->setActionsAndButtonsEnabled(val);
    _vlm_tab_widget->setActionsAndButtonsEnabled(val);
    
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::onOpenModel(const QStringList& names_list, MODEL_TYPE mt)
{
    setFileTabActionsEnabled(false);
    QCoreApplication::processEvents();
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
                std::future_status status;
                do
                {
                    status = ret.wait_for(std::chrono::milliseconds(100));
                    QCoreApplication::processEvents();
                } while (status != std::future_status::ready);

                MapsH5Model* h5Model = ret.get();
                if (h5Model != nullptr)
                {
                    // have to call from main thread because ifstream.open locks up in threadpool
                    //_model->load_v9_rois(name, h5Model);

                    int idx = -1;
                    for (int b = 0; b <MAX_DETECTORS; b++)
                    {
                        QString bs = QString::number(b);
                        if (name.endsWith(bs))
                        {
                            idx = b;
                            break;
                        }
                    }
                    data_struct::Params_Override<double>* param_override = _model->getParamOverride(idx);
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
                
				if (name.endsWith(".mda") || name.contains(".mca"))
				{
					QCoreApplication::processEvents();
					// loading through background thread on windows seems to lock up on fopen
					model = _model->get_RAW_Model(name);
				}
				if (name.endsWith(".h5") || name.endsWith(".hdf5") || name.endsWith(".emd"))
				{
					QCoreApplication::processEvents();
					std::future< RAW_Model*> ret = Global_Thread_Pool::inst()->enqueue([this, name] { return _model->get_RAW_Model(name); });
                    std::future_status status;
                    do
                    {
                        status = ret.wait_for(std::chrono::milliseconds(100));
                        QCoreApplication::processEvents();
                    } while (status != std::future_status::ready);

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
            else if (mt == MODEL_TYPE::VLM)
            {
                VLM_Model* Model = _model->get_VLM_Model(name);
                
                if (Model != nullptr)
                {
                    load_status = LOADED;
                    Model->getImage();
                }
                else
                {
                    load_status = FAILED_LOADING;
                }
                emit loaded_model(name, mt);
                QCoreApplication::processEvents();
                /*
                std::future< VLM_Model*> ret = Global_Thread_Pool::inst()->enqueue([this, name] { return _model->get_VLM_Model(name); });
                std::future_status status;
                int cntr = 0;
                do
                {
                    status = ret.wait_for(std::chrono::milliseconds(100));
                    QCoreApplication::processEvents();
                    cntr++;
                    if (cntr > 1200)
                    {
                        load_status = FAILED_LOADING;
                        break;
                    }
                } while (status != std::future_status::ready);
                if (load_status == UNLOADED)
                {
                    VLM_Model* Model = ret.get();
                    if (Model != nullptr)
                    {
                        load_status = LOADED;
                        Model->getImage();
                        emit loaded_model(name, mt);
                    }
                    else
                    {
                        load_status = FAILED_LOADING;
                    }
                }*/
                _vlm_tab_widget->loaded_file_status_changed(load_status, name);
            }

            cur++;
            emit loaded_perc(cur, amt);
            QCoreApplication::processEvents();
        }
    }
    setFileTabActionsEnabled(true);
}

//---------------------------------------------------------------------------

void MapsWorkspaceFilesWidget::closeAllModels(MODEL_TYPE mt)
{
    setFileTabActionsEnabled(false);
    File_Loaded_Status load_status = UNLOADED;
    QStringList names_list;
    if (_model != nullptr)
    {
        switch (mt)
        {
        case MODEL_TYPE::MAPS_H5:
            _model->unload_all_H5_Model();
            _h5_tab_widget->loaded_file_status_changed_all(load_status);
            names_list = _model->get_h5_names_as_qstringlist();
            break;
        case MODEL_TYPE::RAW:
            _model->unload_all_RAW_Model();
            _mda_tab_widget->loaded_file_status_changed_all(load_status);
            break;
        case MODEL_TYPE::VLM:
            _model->unload_all_VLM_Model();
            _vlm_tab_widget->loaded_file_status_changed_all(load_status);
            break;
        }
    
    }

    setFileTabActionsEnabled(true);
    emit unloadList_model(names_list, mt);
}

//---------------------------------------------------------------------------

void MapsWorkspaceFilesWidget::onCloseModel(const QStringList& names_list, MODEL_TYPE mt)
{
    setFileTabActionsEnabled(false);
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
            case MODEL_TYPE::VLM:
                _model->unload_VLM_Model(name);
                _vlm_tab_widget->loaded_file_status_changed(load_status, name);
                break;
            }
        }
    }

    setFileTabActionsEnabled(true);
    emit unloadList_model(names_list, mt);
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::clearLists()
{
	_h5_tab_widget->unload_all();
	_mda_tab_widget->unload_all();
	_vlm_tab_widget->unload_all();
}

//---------------------------------------------------------------------------

void MapsWorkspaceFilesWidget::onCustomContext(const QString& context_label, const QStringList& file_list)
{
    if (context_label == STR_PROCESS)
    {
        onPerPixelProcessList(file_list);
    }
    if (context_label == STR_PROCESS_ANALYZED)
    {
        onPerPixelProcessListAnalyzed(file_list);
    }
    else if (context_label == STR_BATCH_ROI)
    {
        onBatchRoiList(file_list);
    }
    else if (context_label == STR_H5_EXPORT)
    {
        //onExportImages(file_list);
    }
    else if (context_label == STR_GEN_SCAN_AREA)
    {
        onGenScanArea(file_list);
    }
}

//---------------------------------------------------------------------------

void MapsWorkspaceFilesWidget::onCustomButton(const QString& context_label, const QStringList& file_list)
{
    if (context_label == STR_PROCESS)
    {
        onPerPixelProcessList(file_list);
    }
    if (context_label == STR_PROCESS_ANALYZED)
    {
        onPerPixelProcessListAnalyzed(file_list);
    }
    else if (context_label == STR_BATCH_ROI)
    {
        onBatchRoiList(file_list);
    }
    else if (context_label == STR_GEN_SCAN_AREA)
    {
        onGenScanArea(file_list);
    }
}

//---------------------------------------------------------------------------

void MapsWorkspaceFilesWidget::onPerPixelProcessList(const QStringList& file_list)
{
    if (_model != nullptr)
    {
        _per_pixel_fit_widget->setDir(_model->get_directory_name().toStdString());
    }
    onCloseModel(file_list, MODEL_TYPE::MAPS_H5);
    _per_pixel_fit_widget->updateFileList(file_list);
    _per_pixel_fit_widget->show();
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::onPerPixelProcessListAnalyzed(const QStringList& file_list)
{
    if (_model != nullptr)
    {
        _per_pixel_fit_widget->setDir(_model->get_directory_name().toStdString());
    }
    onCloseModel(file_list, MODEL_TYPE::MAPS_H5);
    QStringList nfile_list = file_list;
    // for each files replace .mda.h5[0-n] to .mda and remove duplicates
    for(QString &str : nfile_list)
    {
        if(str.endsWith(".h5"))
        {
            str = str.replace(".h5", ".mda");
        }
        else
        {
            for(int i =0; i<30; i++)
            {
                QString cmp = ".h5"+QString::number(i);
                if(str.endsWith(cmp))
                {
                    str = str.replace(cmp, "");
                }
            }
        }
    }
    nfile_list.removeDuplicates();
    _per_pixel_fit_widget->updateFileList(nfile_list);
    _per_pixel_fit_widget->show();
}

//---------------------------------------------------------------------------

void MapsWorkspaceFilesWidget::onBatchRoiList(const QStringList& file_list)
{
    std::map<QString, std::string> map_set;
    std::unordered_map<QString, QFileInfo> roi_map;
    for (auto& itr : file_list)
    {
        QFileInfo finfo(itr);
        map_set[finfo.baseName()] = finfo.baseName().toStdString();
    }

    logI << "file cnt = " << file_list.size() << "\n";
    logI << "map_set cnt = " << map_set.size() << "\n";

    QDir directory = _model->get_directory();
    if (false == directory.cd(STR_MAPS_ROIS_DIR_NAME.c_str()))
    {
        QMessageBox::warning(nullptr, "ROI Warning", "Did not find folder 'rois' in this workspace. Please create ROI's and try again");
        return;
    }
    QStringList roi_list = directory.entryList(QStringList() << "*.roi" << "*.r0i", QDir::Files);
    logI << "roi_list = " << roi_list.size();

    foreach(QString filename, roi_list)
    {
        QFileInfo finfo(filename);
        if (map_set.count(finfo.baseName()) > 0)
        {
            roi_map[finfo.baseName()] = finfo;
        }
    }

    
    //create batch roi process widget and pass workspace
    if (_batch_roi_fit_widget == nullptr)
    {
        _batch_roi_fit_widget = new BatchRoiFitWidget(_model->get_directory_name().toStdString());
    }
    _batch_roi_fit_widget->updateFileList(roi_map);
    _batch_roi_fit_widget->show();
    
}

//---------------------------------------------------------------------------

void MapsWorkspaceFilesWidget::onGenScanArea(const QStringList& file_list)
{
    if(_model!= nullptr)
    {
        _gen_scan_vlm_widget.setDir(_model->get_directory_name());
        _gen_scan_vlm_widget.updateFileList(_model->get_hdf5_file_list());
    }
    _gen_scan_vlm_widget.show();
}

//---------------------------------------------------------------------------

void MapsWorkspaceFilesWidget::newScanArea(const QString &name)
{
    QStringList vlm_list;
    vlm_list.append(name);
    if(_model != nullptr)
    {
        _model->reload_vlm();
    }
    onOpenModel(vlm_list, MODEL_TYPE::VLM);
    
}

//---------------------------------------------------------------------------

void MapsWorkspaceFilesWidget::onProcessed_list_update(QStringList file_list)
{
    if (_model != nullptr)
    {
        _model->reload_analyzed();

        onOpenModel(file_list, MODEL_TYPE::MAPS_H5);
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceFilesWidget::onDatasetSelected(const QString name)
{
    if (Preferences::inst()->getValue(STR_PRF_SHOW_DATASET_ON_FILE_SELECT).toBool())
    {
        emit datasetSelected(name);
    }
}

/*---------------------------------------------------------------------------*/