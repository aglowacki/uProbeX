/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "mvc/BatchRoiFitWidget.h"
#include <QApplication>
#include <core/process_whole.h>
#include <QScrollArea>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDir>

//---------------------------------------------------------------------------

BatchRoiFitWidget::BatchRoiFitWidget(std::string directory, QWidget *parent) : QWidget(parent)
{

    _directory = directory;
    _total_itr = 0;
    _canceled = false;
    if (_directory[directory.length() - 1] != DIR_END_CHAR)
    {
        _directory += DIR_END_CHAR;
    }
    createLayout();

}

//---------------------------------------------------------------------------

BatchRoiFitWidget::~BatchRoiFitWidget()
{

 
}

//---------------------------------------------------------------------------

void BatchRoiFitWidget::createLayout()
{

    _progressBarBlocks = new QProgressBar();
    _progressBarBlocks->setRange(0,100);
    _progressBarFiles = new QProgressBar();
    _progressBarFiles->setRange(0, 100);

    _btn_run = new QPushButton("Run");
    connect(_btn_run, &QPushButton::released, this, &BatchRoiFitWidget::runProcessing);
    _btn_cancel = new QPushButton("Cancel");
    connect(_btn_cancel, &QPushButton::released, this, &BatchRoiFitWidget::onClose);

    _le_detectors = new QLineEdit("0,1,2,3,4,5,6");

    _file_list_model = new QStandardItemModel();
    _file_list_view = new QListView();
    _file_list_view->setModel(_file_list_model);
    _file_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    QHBoxLayout* buttonlayout = new QHBoxLayout();
    buttonlayout->addWidget(_btn_run);
    buttonlayout->addWidget(_btn_cancel);
   
    QHBoxLayout* hbox_progresss_blocks = new QHBoxLayout();
    hbox_progresss_blocks->addWidget(new QLabel("Current Fitting:"));
    hbox_progresss_blocks->addWidget(_progressBarBlocks);

    QHBoxLayout* hbox_progresss_files = new QHBoxLayout();
    hbox_progresss_files->addWidget(new QLabel("All Dataset Files:"));
    hbox_progresss_files->addWidget(_progressBarFiles);

    QHBoxLayout* detector_hbox = new QHBoxLayout();
    detector_hbox->addWidget(new QLabel("Detectors  "));
    detector_hbox->addWidget(_le_detectors);

    _optimizer_widget = new OptimizerOptionsWidget();
    fitting::optimizers::NLOPT_Optimizer<double> optimizer;
    _optimizer_widget->setOptimizer(optimizer);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addItem(detector_hbox);
    layout->addWidget(_optimizer_widget);
    layout->addWidget(_file_list_view);
    layout->addItem(buttonlayout);
    layout->addItem(hbox_progresss_blocks);
    layout->addItem(hbox_progresss_files);
    
    this->setLayout(layout);
}

//---------------------------------------------------------------------------

void BatchRoiFitWidget::onClose()
{
    _canceled = true;
    if (_btn_run->isEnabled())
    {
        close();
    }
}

//---------------------------------------------------------------------------

void BatchRoiFitWidget::updateFileList(std::unordered_map<QString, QFileInfo> roi_map)
{
    _progressBarFiles->setValue(0);
    _progressBarBlocks->setValue(0);

    _file_list_model->clear();

    _roi_map = roi_map;

    if (_roi_map.size() == 0)
    {
        _btn_run->setEnabled(false);
    }
    else
    {
        _btn_run->setEnabled(true);
    }

    _file_list.clear();
    for (auto& itr : _roi_map)
    {
        _file_list.push_back(itr.first);
        _file_list_model->appendRow(new QStandardItem(QIcon(":/images/circle_gray.png"), itr.first));
    }

}

//---------------------------------------------------------------------------

void BatchRoiFitWidget::status_callback(size_t cur_itr, size_t total_itr)
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

    /*
    if (cur_itr == _total_itr)
    {
        int val = _progressBarFiles->value() + 1;
        _progressBarFiles->setValue(val);
        
    }
    */

    _progressBarBlocks->setValue(cur_itr);
    QCoreApplication::processEvents();
}


//---------------------------------------------------------------------------

void BatchRoiFitWidget::runProcessing()
{
    _btn_run->setEnabled(false);
    _le_detectors->setEnabled(false);
    _file_list_view->setEnabled(false);
    _canceled = false;
    QCoreApplication::processEvents();
    data_struct::Analysis_Job<double> analysis_job;

    //analysis_job.set_optimizer_algorithm();

    //run in thread
    analysis_job.dataset_directory = _directory;
    if (_optimizer_widget->isHybrid())
    {
        analysis_job.optimize_fit_routine = OPTIMIZE_FIT_ROUTINE::HYBRID;
    }
    else
    {
        analysis_job.optimize_fit_routine = OPTIMIZE_FIT_ROUTINE::ALL_PARAMS;
    }

    _optimizer_widget->updateOptimizerOptions(*(analysis_job.optimizer()));

    analysis_job.use_weights = _optimizer_widget->useWeights();
    analysis_job.detector_num_arr.clear();

    QString dstring = _le_detectors->text();
    if (dstring.length() > 0)
    {
        if (dstring.count(",") > 0)
        {
            QStringList dlist = dstring.split(",");
            foreach(QString str, dlist)
            {
                analysis_job.detector_num_arr.push_back(str.toInt());
            }
        }
        else if (dstring.count(":") > 0)
        {
            QStringList dlist = dstring.split(":");
            try
            {
                int low = dlist[0].toInt();
                int high = dlist[1].toInt();
                for (int i = low; i < high + 1; i++)
                {
                    analysis_job.detector_num_arr.push_back(i);
                }
            }
            catch (...)
            {
                QMessageBox::warning(this, "Bad detectors", "Could not parse detectors. For range use 'num:max' ");
                return;
            }
        }
        else
        {
            try
            {
                analysis_job.detector_num_arr.push_back(dstring.toInt());
            }
            catch (...)
            {
                QMessageBox::warning(this, "Bad detectors", "Could not parse detectors. Please enter number for single detector, or use ',' for list of detectors. ");
                return;
            }
        }
    }
    else
    {
        QMessageBox::warning(this, "No detectors listed", "Please enter detctors ',' separated");
        return;
    }
    
    Callback_Func_Status_Def cb_func = std::bind(&BatchRoiFitWidget::status_callback, this, std::placeholders::_1, std::placeholders::_2);
    // TODO: will need to update to fitting routes instead of getting fit parameters

    analysis_job.fitting_routines.push_back(Fitting_Routines::NNLS);

    if (io::file::init_analysis_job_detectors(&analysis_job))
    {
        // lock all fit parameters except elastic/inelastic amp and elements
        
        analysis_job.optimize_fit_params_preset = fitting::models::Fit_Params_Preset::BATCH_FIT_NO_TAILS;
        
        int proc_total = _roi_map.size() * analysis_job.detector_num_arr.size();
        _progressBarFiles->setRange(0, proc_total);
        _progressBarBlocks->setRange(0, _total_itr);

        _progressBarBlocks->setValue(0);
        _progressBarFiles->setValue(0);

        analysis_job.quantification_standard_filename = "maps_standardinfo.txt";
        io::file::File_Scan::inst()->populate_netcdf_hdf5_files(analysis_job.dataset_directory);

        if (analysis_job.quantification_standard_filename.length() > 0)
        {
            perform_quantification(&analysis_job, false);
        }

        std::map<int, std::map<std::string, data_struct::Fit_Parameters<double>>> roi_fit_params;
        int i = 0;
        for (auto& itr : _roi_map)
        {
            i++;
            optimize_single_roi(analysis_job, itr.second.fileName().toStdString(), roi_fit_params, &cb_func);
            _progressBarFiles->setValue(i);
            QCoreApplication::processEvents();
        }

        _progressBarBlocks->setValue(_total_itr);
        _progressBarFiles->setValue(proc_total);

        // save all to csv
        for (auto detector_num : analysis_job.detector_num_arr)
        {
            std::string save_path = analysis_job.dataset_directory + "output/specfit_results" + std::to_string(detector_num) + ".csv";
            std::string quant_save_path = analysis_job.dataset_directory + "output/specfit_results" + std::to_string(detector_num) + "_quantified.csv";
            io::file::csv::save_v9_specfit(save_path, roi_fit_params.at(detector_num));
            if (analysis_job.get_detector(detector_num)->quantification_standards.size() > 0)
            {
                io::file::csv::save_v9_specfit_quantified(quant_save_path, analysis_job.get_detector(detector_num), analysis_job.fitting_routines, roi_fit_params.at(detector_num));
            }
        }
    }
    


    _btn_run->setEnabled(true);
    _le_detectors->setEnabled(true);
    _file_list_view->setEnabled(true);
    _btn_cancel->setText("Close");

    // open file location
    QDir tmp_dir(QString(_directory.c_str()));
    tmp_dir.cd("output");
    if (false == QDesktopServices::openUrl(QUrl::fromLocalFile(tmp_dir.absolutePath())))
    {
        logE << "Failed to open dir " << tmp_dir.absolutePath().toStdString() << "\n";
    }

}

//---------------------------------------------------------------------------