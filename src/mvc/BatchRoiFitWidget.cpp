/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "mvc/BatchRoiFitWidget.h"
#include <QApplication>
#include <core/process_whole.h>
#include <QScrollArea>
#include <QDesktopServices>
#include <QDir>

/*---------------------------------------------------------------------------*/

BatchRoiFitWidget::BatchRoiFitWidget(std::string directory, QWidget *parent) : QWidget(parent)
{

    _directory = directory;
    _canceled = false;
    if (_directory[directory.length() - 1] != DIR_END_CHAR)
    {
        _directory += DIR_END_CHAR;
    }
    createLayout();

}

/*---------------------------------------------------------------------------*/

BatchRoiFitWidget::~BatchRoiFitWidget()
{

 
}

/*---------------------------------------------------------------------------*/

void BatchRoiFitWidget::createLayout()
{

    _optionsWidget = new PerPixelOptionsWidget();
    _optionsWidget->setExportOptionsVisible(false);

    connect(_optionsWidget, &PerPixelOptionsWidget::onRunning, this, &BatchRoiFitWidget::runProcessing);
    connect(_optionsWidget, &PerPixelOptionsWidget::onStop, this, &BatchRoiFitWidget::close);
   
    //_optimizer_widget = new OptimizerOptionsWidget();
    //_optimizer_widget->setOptimizer(STR_LM_FIT, *(_analysis_job.optimizer()));
 
    QVBoxLayout* layout = new QVBoxLayout();
    //layout->addWidget(_cb_opt_method);
    //layout->addWidget(_optimizer_widget);
    layout->addWidget(_optionsWidget);
    this->setLayout(layout);
}

/*---------------------------------------------------------------------------*/

void BatchRoiFitWidget::updateFileList(std::unordered_map<QString, QFileInfo> roi_map)
{
    QStringList filelist;
    _roi_map = roi_map;
    for (auto& itr : _roi_map)
    {
        filelist.append(itr.first);
        //_file_list.push_back(itr.first);
        //_file_list_model->appendRow(new QStandardItem(QIcon(":/images/circle_gray.png"), itr.first));
    }

    _optionsWidget->updateFileList(filelist);

}

/*---------------------------------------------------------------------------*/

void BatchRoiFitWidget::runProcessing()
{
    _optionsWidget->setRunning();

    //run in thread
    _analysis_job.dataset_directory = _directory;
    /*
    if (_cb_opt_method->currentText() == STR_HYBRID_MP_FIT)
    {
        _analysis_job.set_optimizer("mpfit");
        _analysis_job.optimize_fit_routine = OPTIMIZE_FIT_ROUTINE::HYBRID;
    }
    else
    {
        if (_cb_opt_method->currentText() == STR_MP_FIT)
        {
            _analysis_job.set_optimizer("mpfit");
        }
        else
        {
            _analysis_job.set_optimizer("lmfit");
        }
    }
    
    _optimizer_widget->updateOptimizerOptions(*(_analysis_job.optimizer()));
    */
    
    _optionsWidget->updateAnalysisJob<double>(_analysis_job);


    if (io::file::init_analysis_job_detectors(&_analysis_job))
    {
        // lock all fit parameters except elastic/inelastic amp and elements
        
        _analysis_job.optimize_fit_params_preset = fitting::models::Fit_Params_Preset::NOT_SET;
        std::vector<std::string> except_list = { STR_COMPTON_AMPLITUDE, STR_COHERENT_SCT_AMPLITUDE };
        for (auto detector_num : _analysis_job.detector_num_arr)
        {
            _analysis_job.get_detector(detector_num)->fit_params_override_dict.fit_params.set_all_except(data_struct::E_Bound_Type::FIXED, except_list);
        }

        _analysis_job.quantification_standard_filename = "maps_standardinfo.txt";
        io::file::File_Scan::inst()->populate_netcdf_hdf5_files(_analysis_job.dataset_directory);

        if (_analysis_job.quantification_standard_filename.length() > 0)
        {
            perform_quantification(&_analysis_job, false);
        }

        try
        {
            std::map<int, std::map<std::string, data_struct::Fit_Parameters<double>>> roi_fit_params;
            int i = 0;
            for (auto& itr : _roi_map)
            {
                i++;
                optimize_single_roi(_analysis_job, itr.second.fileName().toStdString(), roi_fit_params, &_optionsWidget->status_callback_func);
                _optionsWidget->incrCurFile();

            }

            _optionsWidget->setStatusDone();

            // save all to csv
            for (auto detector_num : _analysis_job.detector_num_arr)
            {
                std::string save_path = _analysis_job.dataset_directory + "output/specfit_results" + std::to_string(detector_num) + ".csv";
                std::string quant_save_path = _analysis_job.dataset_directory + "output/specfit_results" + std::to_string(detector_num) + "_quantified.csv";
                io::file::csv::save_v9_specfit(save_path, roi_fit_params.at(detector_num));
                if (_analysis_job.get_detector(detector_num)->quantification_standards.size() > 0)
                {
                    io::file::csv::save_v9_specfit_quantified(quant_save_path, _analysis_job.get_detector(detector_num), _analysis_job.fitting_routines, roi_fit_params.at(detector_num));
                }
            }
        }
        catch (...)
        {
            logI << "Batch ROI fitting canceled\n";
        }
    }
    
    _optionsWidget->setStoped();

    // open file location
    QDir tmp_dir(QString(_directory.c_str()));
    tmp_dir.cd("output");
    if (false == QDesktopServices::openUrl(QUrl::fromLocalFile(tmp_dir.absolutePath())))
    {
        logE << "Failed to open dir " << tmp_dir.absolutePath().toStdString() << "\n";
    }

}

/*---------------------------------------------------------------------------*/