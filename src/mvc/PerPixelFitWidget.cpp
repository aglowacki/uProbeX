/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/PerPixelFitWidget.h>
#include <core/process_whole.h>

//---------------------------------------------------------------------------

PerPixelFitWidget::PerPixelFitWidget(QWidget *parent) : QWidget(parent)
{

    createLayout();

}

//---------------------------------------------------------------------------

PerPixelFitWidget::~PerPixelFitWidget()
{

 
}

//---------------------------------------------------------------------------

void PerPixelFitWidget::createLayout()
{

    _optionsWidget = new PerPixelOptionsWidget();

    connect(_optionsWidget, &PerPixelOptionsWidget::onRunning, this, &PerPixelFitWidget::runProcessing);
    connect(_optionsWidget, &PerPixelOptionsWidget::onStop, this, &PerPixelFitWidget::close);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(_optionsWidget);
    
    setLayout(layout);
}

//---------------------------------------------------------------------------

void PerPixelFitWidget::setDir(std::string directory)
{
    _directory = directory;
    if (_directory[directory.length() - 1] != DIR_END_CHAR)
    {
        _directory += DIR_END_CHAR;
    }
}

//---------------------------------------------------------------------------

void PerPixelFitWidget::runProcessing()
{
    // TODO: fix so we can do float
    data_struct::Analysis_Job<double> analysis_job;
    analysis_job.dataset_directory = _directory;
 
    _optionsWidget->setRunning();
    _optionsWidget->updateAnalysisJob<double>(analysis_job);

    try
    {
        if (io::file::init_analysis_job_detectors(&analysis_job))
        {
            io::file::File_Scan::inst()->populate_netcdf_hdf5_files(_directory);

            process_dataset_files(&analysis_job, &_optionsWidget->status_callback_func);
            if (_optionsWidget->performQuantificationChecked())
            {
                perform_quantification(&analysis_job, true);
            }
        }
        iterate_datasets_and_update(analysis_job);
        emit processed_list_update(_optionsWidget->fileList());
    }
    catch (...)
    {
        logI << "Batch ROI fitting canceled\n";
    }
    _optionsWidget->setStoped();
}

//---------------------------------------------------------------------------