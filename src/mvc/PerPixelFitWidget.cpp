/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/PerPixelFitWidget.h>
#include <QApplication>
#include <QGroupBox>
#include <data_struct/analysis_job.h>
#include <core/process_whole.h>
#include <QScrollArea>
#include <QMessageBox>

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
    processing_grp->setTitle("Processing Options");

    QGroupBox* saving_grp = new QGroupBox();
    QVBoxLayout* v_save_layout = new QVBoxLayout();
    _save_avg = new QCheckBox("Generate Avg H5");
    _save_v9 = new QCheckBox("Add v9 soft links");
    _save_exchange = new QCheckBox("Add Exchange format");
    _save_csv = new QCheckBox("Save CSV of integrated fits");
    _perform_quantification = new QCheckBox("Perform Quantification (maps_standardinfo.txt)");

    _le_detectors = new QLineEdit("0,1,2,3,4,5,6");

    v_save_layout->addWidget(_save_avg);
    v_save_layout->addWidget(_save_v9);
    v_save_layout->addWidget(_save_exchange);
    v_save_layout->addWidget(_save_csv);
    v_save_layout->addWidget(_perform_quantification);

    saving_grp->setLayout(v_save_layout);
    saving_grp->setTitle("Export Options");

    _file_list_model = new QStandardItemModel();
    _file_list_view = new QListView();
    _file_list_view->setModel(_file_list_model);
    _file_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QHBoxLayout* buttonlayout = new QHBoxLayout();
    buttonlayout->addWidget(_btn_run);
    buttonlayout->addWidget(_btn_cancel);

    QHBoxLayout* proc_save_layout = new QHBoxLayout();
    proc_save_layout->addWidget(processing_grp);
    proc_save_layout->addWidget(saving_grp);

    QHBoxLayout* hbox_progresss_blocks = new QHBoxLayout();
    hbox_progresss_blocks->addWidget(new QLabel("Current Fitting:"));
    hbox_progresss_blocks->addWidget(_progressBarBlocks);

    QHBoxLayout* hbox_progresss_files = new QHBoxLayout();
    hbox_progresss_files->addWidget(new QLabel("All Dataset Files:"));
    hbox_progresss_files->addWidget(_progressBarFiles);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(_le_detectors);
    layout->addItem(proc_save_layout);
    layout->addWidget(_file_list_view);
    layout->addItem(buttonlayout);
    layout->addItem(hbox_progresss_blocks);
    layout->addItem(hbox_progresss_files);
    
    setLayout(layout);
}

/*---------------------------------------------------------------------------*/

void PerPixelFitWidget::updateFileList(QStringList file_list)
{
    _progressBarFiles->setValue(0);
    _progressBarBlocks->setValue(0);

    _file_list_model->clear();
    for(auto & itr : file_list)
    {
        _file_list_model->appendRow(new QStandardItem(QIcon(":/images/circle_gray.png"), itr));
    }
}

/*---------------------------------------------------------------------------*/

void PerPixelFitWidget::runProcessing()
{
    _btn_run->setEnabled(false);
    //run in thread
    data_struct::Analysis_Job<double> analysis_job;
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

    if (io::file::init_analysis_job_detectors(&analysis_job))
    {
        io::file::File_Scan::inst()->populate_netcdf_hdf5_files(_directory);
        Callback_Func_Status_Def cb_func = std::bind(&PerPixelFitWidget::status_callback, this, std::placeholders::_1, std::placeholders::_2);
        //std::function<void(const Fit_Parameters* const, const  Range* const, Spectra*)> cb_func = std::bind(&PerPixelFitWidget::model_spectrum, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        if (_perform_quantification->isChecked())
        {
            analysis_job.quantification_standard_filename = "maps_standardinfo.txt";
            perform_quantification(&analysis_job);
        }
        process_dataset_files(&analysis_job, &cb_func);
        //QCoreApplication::processEvents();
        _progressBarFiles->setValue(total_file_range);
        _progressBarBlocks->setValue(_total_blocks);
        QCoreApplication::processEvents();
    }
    
    if (_save_avg->isChecked())
    {
        analysis_job.generate_average_h5 = true;
    }

    //add v9 layout soft links
    if (_save_v9->isChecked())
    {
        analysis_job.add_v9_layout = true;
    }


    //add exchange
    if (_save_exchange->isChecked())
    {
        analysis_job.add_exchange_layout = true;
    }
    
    //export csv
    if (_save_csv->isChecked())
    {
        analysis_job.export_int_fitted_to_csv = true;
    }
   

    iterate_datasets_and_update(analysis_job);

    _btn_run->setEnabled(true);
    emit processed_list_update();
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