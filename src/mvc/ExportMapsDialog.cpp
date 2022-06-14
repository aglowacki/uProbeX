/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ExportMapsDialog.h>

/*---------------------------------------------------------------------------*/

ExportMapsDialog::ExportMapsDialog(std::string directory, QWidget *parent) : QWidget(parent)
{
    /*
    _directory = directory;
    if (_directory[directory.length() - 1] != DIR_END_CHAR)
    {
        _directory += DIR_END_CHAR;
    }*/
    createLayout();

}

/*---------------------------------------------------------------------------*/

ExportMapsDialog::~ExportMapsDialog()
{

 
}

/*---------------------------------------------------------------------------*/

void ExportMapsDialog::createLayout()
{

    _progressBarBlocks = new QProgressBar();
    _progressBarBlocks->setRange(0,100);
    _progressBarFiles = new QProgressBar();
    _progressBarFiles->setRange(0, 100);

    _btn_run = new QPushButton("Export");
    connect(_btn_run, &QPushButton::released, this, &ExportMapsDialog::runProcessing);
    _btn_cancel = new QPushButton("Cancel");
    connect(_btn_cancel, &QPushButton::released, this, &ExportMapsDialog::close);

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

void ExportMapsDialog::updateFileList(QStringList file_list)
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

void ExportMapsDialog::runProcessing()
{
    _btn_run->setEnabled(false);
    //run in thread
    

    _btn_run->setEnabled(true);
    emit processed_list_update();
}

void ExportMapsDialog::status_callback(size_t cur_block, size_t total_blocks)
{
    /*
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
    */
    QCoreApplication::processEvents();
}