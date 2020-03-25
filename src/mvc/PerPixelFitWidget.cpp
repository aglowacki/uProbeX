/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/PerPixelFitWidget.h>
#include <QApplication>
#include <QGroupBox>
#include <data_struct/analysis_job.h>
#include <core/process_whole.h>

/*---------------------------------------------------------------------------*/

PerPixelFitWidget::PerPixelFitWidget(std::string directory, QWidget *parent) : QWidget(parent)
{

    _cur_file = 0;
    _cur_block = 0;
    _total_blocks = 0;
    _directory = directory;
    if (_directory[directory.length() - 1] != DIR_END_CHAR)
    {
        _directory += DIR_END_CHAR;
    }
    createLayout();

}

/*---------------------------------------------------------------------------*/

PerPixelFitWidget::~PerPixelFitWidget()
{

 
}

/*---------------------------------------------------------------------------*/

void PerPixelFitWidget::createLayout()
{

    _progressBarBlocks = new QProgressBar();
    _progressBarBlocks->setRange(0,100);
    _progressBarFiles = new QProgressBar();
    _progressBarFiles->setRange(0, 100);

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
    layout->addWidget(_progressBarBlocks);
    layout->addWidget(_progressBarFiles);

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
    analysis_job.dataset_directory = _directory;
    
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
    for ( unsigned int i = 0; i < 4; i++)
    {
        analysis_job.detector_num_arr.push_back(i);
    }
    QModelIndex parent = QModelIndex();
    for (int r = 0; r < _file_list_model->rowCount(parent); ++r)
    {
        QModelIndex index = _file_list_model->index(r, 0, parent);
        QVariant name = _file_list_model->data(index);
        analysis_job.dataset_files.push_back(name.toString().toStdString());
    }
   

    size_t total_file_range = 0;
    if (analysis_job.quick_and_dirty == true)
    {
        total_file_range = (analysis_job.fitting_routines.size() * analysis_job.dataset_files.size());
    }
    else
    {
        total_file_range = (analysis_job.fitting_routines.size() * analysis_job.dataset_files.size() * analysis_job.detector_num_arr.size());
    }
    _progressBarFiles->setRange(0, total_file_range);

    //if(perform quantificaiton
    //perform_quantification(&analysis_job);

    if (io::init_analysis_job_detectors(&analysis_job))
    {
        io::populate_netcdf_hdf5_files(_directory);
        Callback_Func_Status_Def cb_func = std::bind(&PerPixelFitWidget::status_callback, this, std::placeholders::_1, std::placeholders::_2);
        //std::function<void(const Fit_Parameters* const, const  Range* const, Spectra*)> cb_func = std::bind(&PerPixelFitWidget::model_spectrum, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        process_dataset_files(&analysis_job, &cb_func);
        //QCoreApplication::processEvents();
        emit processed_list_update();
        _progressBarFiles->setValue(total_file_range);
        _progressBarBlocks->setValue(_total_blocks);
        QCoreApplication::processEvents();
    }
    /*

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

void PerPixelFitWidget::status_callback(size_t cur_block, size_t total_blocks)
{
    if (_total_blocks != total_blocks)
    {
        _total_blocks = total_blocks;
        _progressBarBlocks->setRange(0, _total_blocks);
    }

    _cur_block = cur_block;
    if (_cur_block == 0)
    {
        _cur_file++;
        _progressBarFiles->setValue(_cur_file-1);
    }
    _progressBarBlocks->setValue(_cur_block);
    
    QCoreApplication::processEvents();
}