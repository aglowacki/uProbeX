/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ExportMapsDialog.h>

/*---------------------------------------------------------------------------*/

ExportMapsDialog::ExportMapsDialog(std::string directory, QWidget *parent) : QWidget(parent)
{
    
    _directory = directory;
    if (_directory[directory.length() - 1] != DIR_END_CHAR)
    {
        _directory += DIR_END_CHAR;
    }
    createLayout();

}

/*---------------------------------------------------------------------------*/

ExportMapsDialog::~ExportMapsDialog()
{

 
}

/*---------------------------------------------------------------------------*/

void ExportMapsDialog::createLayout()
{

    _progressBarFiles = new QProgressBar();
    _progressBarFiles->setRange(0, 100);

    _btn_run = new QPushButton("Export");
    connect(_btn_run, &QPushButton::released, this, &ExportMapsDialog::runProcessing);
    _btn_cancel = new QPushButton("Close");
    connect(_btn_cancel, &QPushButton::released, this, &ExportMapsDialog::close);

    QGroupBox* processing_grp = new QGroupBox();
    QVBoxLayout* v_proc_layout = new QVBoxLayout();
    _export_tiff = new QCheckBox("Save Tiff");
    _export_png = new QCheckBox("Save PNG");
    _export_ascii = new QCheckBox("Save ASCII");

    v_proc_layout->addWidget(_export_tiff);
    v_proc_layout->addWidget(_export_png);
    v_proc_layout->addWidget(_export_ascii);

    processing_grp->setLayout(v_proc_layout);
    processing_grp->setTitle("Export Image Options");
    /*
    QGroupBox* saving_grp = new QGroupBox();
    QVBoxLayout* v_save_layout = new QVBoxLayout();
    _save_avg = new QCheckBox("Generate Avg H5");
    _save_v9 = new QCheckBox("Add v9 soft links");
    _save_exchange = new QCheckBox("Add Exchange format");
    _save_csv = new QCheckBox("Save CSV of integrated fits");
    _perform_quantification = new QCheckBox("Perform Quantification (maps_standardinfo.txt)");

    v_save_layout->addWidget(_save_avg);
    v_save_layout->addWidget(_save_v9);
    v_save_layout->addWidget(_save_exchange);
    v_save_layout->addWidget(_save_csv);
    v_save_layout->addWidget(_perform_quantification);

    saving_grp->setLayout(v_save_layout);
    saving_grp->setTitle("Export Options");
    */
    QHBoxLayout* buttonlayout = new QHBoxLayout();
    buttonlayout->addWidget(_btn_run);
    buttonlayout->addWidget(_btn_cancel);

    QHBoxLayout* proc_save_layout = new QHBoxLayout();
    proc_save_layout->addWidget(processing_grp);
    //proc_save_layout->addWidget(saving_grp);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addItem(proc_save_layout);
    layout->addItem(buttonlayout);
    layout->addWidget(_progressBarFiles);
    
    setLayout(layout);
}

/*---------------------------------------------------------------------------*/

void ExportMapsDialog::updateFileList(QStringList file_list)
{
    _progressBarFiles->setValue(0);
    
    /*
    _file_list_model->clear();
    for(auto & itr : file_list)
    {
        _file_list_model->appendRow(new QStandardItem(QIcon(":/images/circle_gray.png"), itr));
    }
    */
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

    _progressBarBlocks->setValue(_cur_block);
    */
    QCoreApplication::processEvents();
}