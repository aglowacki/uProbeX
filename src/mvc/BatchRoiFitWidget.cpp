/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/BatchRoiFitWidget.h>
#include <QApplication>
#include <data_struct/analysis_job.h>
#include <core/process_whole.h>
#include <QScrollArea>
#include <QMessageBox>

/*---------------------------------------------------------------------------*/

BatchRoiFitWidget::BatchRoiFitWidget(std::string directory, QWidget *parent) : QWidget(parent)
{

    _directory = directory;
    if (_directory[directory.length() - 1] != DIR_END_CHAR)
    {
        _directory += DIR_END_CHAR;
    }
    createLayout();

}

/*---------------------------------------------------------------------------*/

BatchRoiFitWidget::~BatchRoiFitWidget()
{

 
}

/*---------------------------------------------------------------------------*/

void BatchRoiFitWidget::createLayout()
{

    _progressBarBlocks = new QProgressBar();
    _progressBarBlocks->setRange(0,100);
    _progressBarFiles = new QProgressBar();
    _progressBarFiles->setRange(0, 100);

    _btn_run = new QPushButton("Run");
    connect(_btn_run, &QPushButton::released, this, &BatchRoiFitWidget::runProcessing);
    _btn_cancel = new QPushButton("Cancel");
    connect(_btn_cancel, &QPushButton::released, this, &BatchRoiFitWidget::close);

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

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addItem(detector_hbox);
    //layout->addItem(_proc_save_layout);
    layout->addWidget(_file_list_view);
    layout->addItem(buttonlayout);
    layout->addItem(hbox_progresss_blocks);
    layout->addItem(hbox_progresss_files);
    
    setLayout(layout);
}

/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/

void BatchRoiFitWidget::runProcessing()
{
    _btn_run->setEnabled(false);
    _le_detectors->setEnabled(false);
    _file_list_view->setEnabled(false);


    //run in thread
    data_struct::Analysis_Job<double> analysis_job;
    analysis_job.dataset_directory = _directory;
    
   
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
    
    
    if (io::file::init_analysis_job_detectors(&analysis_job))
    {
        _progressBarBlocks->setRange(0, _roi_map.size());

        std::map<int, std::map<std::string, data_struct::Fit_Parameters<double>>> roi_fit_params;
        int i = 0;
        for (auto& itr : _roi_map)
        {
            i++;
            _progressBarFiles->setValue(i);
            QCoreApplication::processEvents();
            optimize_single_roi(analysis_job, itr.second.fileName().toStdString(), roi_fit_params);
        }
        _progressBarFiles->setValue(i);
        QCoreApplication::processEvents();
       
    }
    
    

    _btn_run->setEnabled(true);
    _le_detectors->setEnabled(true);
    _file_list_view->setEnabled(true);
    _btn_cancel->setText("Close");

}

/*---------------------------------------------------------------------------*/