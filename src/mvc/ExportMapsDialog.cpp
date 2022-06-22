/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ExportMapsDialog.h>
#include <QFileDialog>
#include <QDesktopServices>

/*---------------------------------------------------------------------------*/

ExportMapsDialog::ExportMapsDialog(QDir directory, QWidget *parent) : QWidget(parent)
{
    
    _directory = directory;
    createLayout();
    _total_blocks = 100;
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
    connect(_btn_run, &QPushButton::released, this, &ExportMapsDialog::export_released);
    _btn_cancel = new QPushButton("Close");
    connect(_btn_cancel, &QPushButton::released, this, &ExportMapsDialog::close);

    QGroupBox* processing_grp = new QGroupBox();
    QVBoxLayout* v_proc_layout = new QVBoxLayout();
    QHBoxLayout* h_layout = new QHBoxLayout();
    _le_dir = new QLabel();
    _le_dir->setText(_directory.absolutePath());
    QPushButton *btn_browse = new QPushButton("Browse");
    connect(btn_browse, &QPushButton::released, this, &ExportMapsDialog::on_browse);
    QPushButton *btn_open = new QPushButton("Open");
    connect(btn_open, &QPushButton::released, this, &ExportMapsDialog::on_open);
    h_layout->addWidget(_le_dir);
    h_layout->addWidget(btn_browse);
    h_layout->addWidget(btn_open);
    _export_tiff = new QCheckBox("Save Tiff");
    _export_tiff->setChecked(true);
    _export_png = new QCheckBox("Save PNG");
    _export_png->setChecked(true);
    _export_ascii = new QCheckBox("Save ASCII");
    _export_ascii->setChecked(true);

    v_proc_layout->addItem(h_layout);
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

void ExportMapsDialog::on_browse()
{
    _directory = QFileDialog::getExistingDirectory(this, "Export Directory", _directory.absolutePath());
}

/*---------------------------------------------------------------------------*/

void ExportMapsDialog::on_open()
{
    if (false == QDesktopServices::openUrl(QUrl::fromLocalFile(_directory.absolutePath())))
    {
        logE << "Failed to open dir " << _directory.absolutePath().toStdString() << "\n";
    }
}

/*---------------------------------------------------------------------------*/

void ExportMapsDialog::status_callback(size_t cur_block, size_t total_blocks)
{
    
    if (_total_blocks != total_blocks)
    {
        _total_blocks = total_blocks;
        _progressBarFiles->setRange(0, _total_blocks);
    }

    _progressBarFiles->setValue(cur_block);
    
    QCoreApplication::processEvents();
}

/*---------------------------------------------------------------------------*/