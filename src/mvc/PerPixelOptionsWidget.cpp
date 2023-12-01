/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/PerPixelOptionsWidget.h>
#include <QApplication>
#include <data_struct/analysis_job.h>
#include <core/process_whole.h>
#include <QScrollArea>
#include <QMessageBox>

/*---------------------------------------------------------------------------*/

PerPixelOptionsWidget::PerPixelOptionsWidget(QWidget *parent) : QWidget(parent)
{

    _cur_file = 0;
    _cur_block = 0;
    _total_blocks = 0;
    _canceled = false;
    status_callback_func = std::bind(&PerPixelOptionsWidget::status_callback, this, std::placeholders::_1, std::placeholders::_2);
    createLayout();

}

/*---------------------------------------------------------------------------*/

PerPixelOptionsWidget::~PerPixelOptionsWidget()
{

 
}

/*---------------------------------------------------------------------------*/

void PerPixelOptionsWidget::createLayout()
{

    _progressBarBlocks = new QProgressBar();
    _progressBarBlocks->setRange(0,100);
    _progressBarFiles = new QProgressBar();
    _progressBarFiles->setRange(0, 100);

    _btn_run = new QPushButton("Run");
    connect(_btn_run, &QPushButton::released, this, &PerPixelOptionsWidget::onRunning);
    _btn_cancel = new QPushButton("Cancel");
    connect(_btn_cancel, &QPushButton::released, this, &PerPixelOptionsWidget::onCancel);

    _processing_grp = new QGroupBox();
    QVBoxLayout* v_proc_layout = new QVBoxLayout();
    _proc_roi = new QCheckBox("Region of Interest Analysis");
    _proc_nnls = new QCheckBox("Non-Negative Least Squares Analysis");
    _proc_matrix = new QCheckBox("Per Pixel Matrix Analysis");

    v_proc_layout->addWidget(_proc_roi);
    v_proc_layout->addWidget(_proc_nnls);
    v_proc_layout->addWidget(_proc_matrix);

    _processing_grp->setLayout(v_proc_layout);
    _processing_grp->setTitle("Processing Options");

    _saving_grp = new QGroupBox();
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

    _saving_grp->setLayout(v_save_layout);
    _saving_grp->setTitle("Export Options");

    _file_list_model = new QStandardItemModel();
    _file_list_view = new QListView();
    _file_list_view->setModel(_file_list_model);
    _file_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QHBoxLayout* buttonlayout = new QHBoxLayout();
    buttonlayout->addWidget(_btn_run);
    buttonlayout->addWidget(_btn_cancel);

    _proc_save_layout = new QHBoxLayout();
    _proc_save_layout->addWidget(_processing_grp);
    _proc_save_layout->addWidget(_saving_grp);

    QHBoxLayout* hbox_progresss_blocks = new QHBoxLayout();
    hbox_progresss_blocks->addWidget(new QLabel("Current Fitting:"));
    hbox_progresss_blocks->addWidget(_progressBarBlocks);

    QHBoxLayout* hbox_progresss_files = new QHBoxLayout();
    hbox_progresss_files->addWidget(new QLabel("All Dataset Files:"));
    hbox_progresss_files->addWidget(_progressBarFiles);

    QHBoxLayout* detector_hbox = new QHBoxLayout();
    detector_hbox->addWidget(new QLabel("Detectors  "));
    detector_hbox->addWidget(_le_detectors);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addItem(detector_hbox);
    layout->addItem(_proc_save_layout);
    layout->addWidget(_file_list_view);
    layout->addItem(buttonlayout);
    layout->addItem(hbox_progresss_blocks);
    layout->addItem(hbox_progresss_files);
    
    setLayout(layout);
}

/*---------------------------------------------------------------------------*/

void PerPixelOptionsWidget::updateFileList(QStringList file_list)
{
    _progressBarFiles->setValue(0);
    _progressBarBlocks->setValue(0);

    _file_list_model->clear();

    _file_list.clear();
    _file_list = file_list;

    if (file_list.size() == 0)
    {
        _btn_run->setEnabled(false);
    }
    else
    {
        _btn_run->setEnabled(true);
    }

    for(auto & itr : file_list)
    {
        _file_list_model->appendRow(new QStandardItem(QIcon(":/images/circle_gray.png"), itr));
    }
}

//---------------------------------------------------------------------------

void PerPixelOptionsWidget::onCancel()
{
    _canceled = true;
    emit onStop();
}

/*---------------------------------------------------------------------------*/

void PerPixelOptionsWidget::status_callback(size_t cur_block, size_t total_blocks)
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
 
    if (_canceled)
    {
        throw 20;
    }

    QCoreApplication::processEvents();
}

//---------------------------------------------------------------------------

void PerPixelOptionsWidget::setRunning()
{
    _canceled = false;
    _btn_run->setEnabled(false);
    _processing_grp->setEnabled(false);
    _saving_grp->setEnabled(false);
    _le_detectors->setEnabled(false);
    _file_list_view->setEnabled(false);
}

//---------------------------------------------------------------------------

void PerPixelOptionsWidget::setStoped()
{
    _progressBarFiles->setValue(_total_file_range);
    _progressBarBlocks->setValue(_total_blocks);

    _btn_run->setEnabled(true);
    _processing_grp->setEnabled(true);
    _saving_grp->setEnabled(true);
    _le_detectors->setEnabled(true);
    _file_list_view->setEnabled(true);
}

//---------------------------------------------------------------------------
