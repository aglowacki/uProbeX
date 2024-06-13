/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/GenScanVlmWidget.h>

//---------------------------------------------------------------------------

GenScanVlmWidget::GenScanVlmWidget(QWidget *parent) : QWidget(parent)
{

    _processing = false;
    createLayout();

}

//---------------------------------------------------------------------------

GenScanVlmWidget::~GenScanVlmWidget()
{

 
}

//---------------------------------------------------------------------------

void GenScanVlmWidget::createLayout()
{
    _progressBarFiles = new QProgressBar();
    
    _btn_gen = new QPushButton("Generate");
    connect(_btn_gen, &QPushButton::released, this, &GenScanVlmWidget::runProcessing);
    _btn_cancel = new QPushButton("Cancel");
    connect(_btn_cancel, &QPushButton::released, this, &GenScanVlmWidget::setStoped);

    _file_list_model = new QStandardItemModel();
    _file_list_view = new QListView();
    _file_list_view->setModel(_file_list_model);
    _file_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
 
    QHBoxLayout* buttonlayout = new QHBoxLayout();
    buttonlayout->addWidget(_btn_gen);
    buttonlayout->addWidget(_btn_cancel);

    QVBoxLayout* layout = new QVBoxLayout();
    //layout->addItem(detector_hbox);
    //layout->addItem(_proc_save_layout);
    layout->addWidget(_file_list_view);
    layout->addItem(buttonlayout);
    layout->addWidget(_progressBarFiles);

    setLayout(layout);

}

//---------------------------------------------------------------------------

void GenScanVlmWidget::setDir(QString directory)
{
   _directory_name = directory;
}

//---------------------------------------------------------------------------

void GenScanVlmWidget::updateFileList(const std::map<QString, QFileInfo>& file_map)
{
    _file_list_model->clear();
    _file_map.clear();

    for(auto &itr : file_map)
    {
        if(_file_map.count(itr.second.baseName()) == 0)
        {
            _file_map[itr.second.baseName()] = itr.second;
            _file_list_model->appendRow(new QStandardItem(QIcon(":/images/circle_gray.png"), itr.second.baseName()));
        }
    }
    _progressBarFiles->setRange(0, _file_map.size());
}

//---------------------------------------------------------------------------

void GenScanVlmWidget::setStoped()
{
    if(false == _processing)
    {
        QWidget::close();
    }
    _processing = false;
}

//---------------------------------------------------------------------------

void GenScanVlmWidget::runProcessing()
{
    _btn_gen->setEnabled(false);
    _processing = true;
    int i=0;
    for(auto& itr: _file_map)
    {
        data_struct::ArrayTr<float> x_arr;
        data_struct::ArrayTr<float> y_arr;
        if(false == _processing)
        {
            break;
        }
        if(MapsH5Model::load_x_y_motors_only(itr.second.absoluteFilePath(), x_arr, y_arr))
        {

        }
        i++;
        _progressBarFiles->setValue(i);
    }
    _processing = false;
    _btn_gen->setEnabled(true);  

}

//---------------------------------------------------------------------------