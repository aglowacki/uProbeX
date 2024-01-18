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

    _processing_grp = new QGroupBox();
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
    _export_ascii->setChecked(false);

    _export_all = new QCheckBox("Exports all elements: Unchecked exports only elements in view");
    _export_all->setChecked(true);

    _save_screen = new QCheckBox("Screenshot elements in view");
    _save_screen->setChecked(false);

    v_proc_layout->addItem(h_layout);
    v_proc_layout->addWidget(_export_tiff);
    v_proc_layout->addWidget(_export_png);
    v_proc_layout->addWidget(_export_ascii);
    v_proc_layout->addWidget(_export_all);
    v_proc_layout->addWidget(_save_screen);

    _processing_grp->setLayout(v_proc_layout);
    _processing_grp->setTitle("Export Image Options");

    QHBoxLayout* buttonlayout = new QHBoxLayout();
    buttonlayout->addWidget(_btn_run);
    buttonlayout->addWidget(_btn_cancel);

    QHBoxLayout* proc_save_layout = new QHBoxLayout();
    proc_save_layout->addWidget(_processing_grp);
    
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addItem(proc_save_layout);
    layout->addItem(buttonlayout);
    layout->addWidget(_progressBarFiles);
    
    setLayout(layout);
}

/*---------------------------------------------------------------------------*/

void ExportMapsDialog::setRunEnabled(bool val)
{
    _processing_grp->setEnabled(val);
    _btn_run->setEnabled(val); 
    QCoreApplication::processEvents();
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