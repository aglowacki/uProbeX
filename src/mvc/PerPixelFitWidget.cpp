/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/PerPixelFitWidget.h>
#include <QGroupBox>
#include <data_struct/analysis_job.h>
#include <core/process_whole.h>

/*---------------------------------------------------------------------------*/

PerPixelFitWidget::PerPixelFitWidget(QWidget *parent) : QWidget(parent)
{

    createLayout();

}

/*---------------------------------------------------------------------------*/

PerPixelFitWidget::~PerPixelFitWidget()
{

 
}

/*---------------------------------------------------------------------------*/

void PerPixelFitWidget::createLayout()
{

    _progressBar = new QProgressBar();
    _progressBar->setRange(0,100);

    _btn_run = new QPushButton("Run");
    connect(_btn_run, &QPushButton::released, this, &PerPixelFitWidget::runProcessing);
    _btn_cancel = new QPushButton("Cancel");
    connect(_btn_cancel, &QPushButton::released, this, &PerPixelFitWidget::close);

    QGroupBox* processing_grp = new QGroupBox();
    QVBoxLayout* v_proc_layout = new QVBoxLayout();
    _proc_roi = new QCheckBox("Region of Interest Analysis");
    _proc_nnls = new QCheckBox("Non-Negative Least Squares Analysis");
    _proc_matrix = new QCheckBox("Per Pixel Matrix Analysis");

    v_proc_layout->addWidget(_proc_roi);
    v_proc_layout->addWidget(_proc_nnls);
    v_proc_layout->addWidget(_proc_matrix);

    processing_grp->setLayout(v_proc_layout);

    _file_list_model = new QStandardItemModel();
    _file_list_view = new QListView();
    _file_list_view->setModel(_file_list_model);
    _file_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QHBoxLayout* buttonlayout = new QHBoxLayout();
    buttonlayout->addWidget(_btn_run);
    buttonlayout->addWidget(_btn_cancel);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(processing_grp);
    layout->addWidget(_file_list_view);
    layout->addItem(buttonlayout);
    layout->addWidget(_progressBar);

    setLayout(layout);

}

/*---------------------------------------------------------------------------*/

void PerPixelFitWidget::updateFileList(QStringList file_list)
{
    _file_list_model->clear();
    for(auto & itr : file_list)
    {
        _file_list_model->appendRow(new QStandardItem(QIcon(":/images/circle_gray.png"), itr));
    }
}

/*---------------------------------------------------------------------------*/

void PerPixelFitWidget::runProcessing()
{
    //run in thread
    data_struct::Analysis_Job analysis_job;

    if (_proc_roi->isChecked())
    {
        analysis_job.fitting_routines.push_back(data_struct::Fitting_Routines::ROI);
    }
    if (_proc_nnls->isChecked())
    {
        analysis_job.fitting_routines.push_back(data_struct::Fitting_Routines::NNLS);
    }
    if (_proc_matrix->isChecked())
    {
        analysis_job.fitting_routines.push_back(data_struct::Fitting_Routines::GAUSS_MATRIX);
    }

    //analysis_job.generate_average_h5 = true;
    //analysis_job.add_v9_layout = true;
    //analysis_job.add_exchange_layout = true;
    //analysis_job.detector_num_arr.push_back(std::stoi(item));

    //for _file_list_model
    //analysis_job.dataset_files.push_back(itr);

    //if(perform quantificaiton
    //perform_quantification(&analysis_job);

    /*
    process_dataset_files(&analysis_job);


    if(analysis_job.generate_average_h5)
    {
        for(const auto& dataset_file : analysis_job.dataset_files)
        {
            io::generate_h5_averages(analysis_job.dataset_directory, dataset_file, analysis_job.detector_num_arr);
        }
    }

    //add v9 layout soft links
    if(analysis_job.add_v9_layout)
    {
        for(const auto& dataset_file : analysis_job.dataset_files)
        {
            io::file::HDF5_IO::inst()->add_v9_layout(analysis_job.dataset_directory, dataset_file, analysis_job.detector_num_arr);
        }
    }


    //add exchange
    if(analysis_job.add_exchange_layout)
    {
        for(const auto& dataset_file : analysis_job.dataset_files)
        {
            io::file::HDF5_IO::inst()->add_exchange_layout(analysis_job.dataset_directory, dataset_file, analysis_job.detector_num_arr);
        }
    }
    */
}
