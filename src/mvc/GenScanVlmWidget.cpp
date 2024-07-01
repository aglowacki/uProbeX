/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/GenScanVlmWidget.h>
#include <gstar/GStarResource.h>
#include <QFileDialog>
#include "mvc/TIFF_Model.h"

//---------------------------------------------------------------------------

GenScanVlmWidget::GenScanVlmWidget(QWidget *parent) : QWidget(parent)
{

    _processing = false;
    createLayout();

}

//---------------------------------------------------------------------------

GenScanVlmWidget::~GenScanVlmWidget()
{

    _clear_regions();

}

//---------------------------------------------------------------------------

void GenScanVlmWidget::createLayout()
{
    _progressBarFiles = new QProgressBar();
    _progressBarFiles->setRange(0,100);
    
    _btn_scan = new QPushButton("Scan");
    _btn_scan->setEnabled(false);
    connect(_btn_scan, &QPushButton::released, this, &GenScanVlmWidget::runProcessing);

    _btn_gen = new QPushButton("Generate");
    connect(_btn_gen, &QPushButton::released, this, &GenScanVlmWidget::_generate);
    _btn_cancel = new QPushButton("Cancel");
    connect(_btn_cancel, &QPushButton::released, this, &GenScanVlmWidget::setStoped);

    _file_list_model = new QStandardItemModel();
    _file_list_view = new QListView();
    _file_list_view->setModel(_file_list_model);
    _file_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
 
    QHBoxLayout* buttonlayout = new QHBoxLayout();
    buttonlayout->addWidget(_btn_scan);
    buttonlayout->addWidget(_btn_gen);
    buttonlayout->addWidget(_btn_cancel);

    QDateTime date = QDateTime::currentDateTime();
    QString formattedTime = date.toString("yyyy_MM_dd_hh_mm_ss");

    QLabel* gen_name_label = new QLabel("VLM Scene Name: ");
    _gen_name_le = new QLineEdit("ScanArea_"+formattedTime);
    QHBoxLayout* name_hbox = new QHBoxLayout();
    name_hbox->addWidget(gen_name_label);
    name_hbox->addWidget(_gen_name_le);

    _btn_browse = new QPushButton("Browse");
    connect(_btn_browse, &QPushButton::released, this, &GenScanVlmWidget::on_browse);
    _background_img_loc_le = new QLineEdit("");
    QHBoxLayout* img_hbox = new QHBoxLayout();
    img_hbox->addWidget(new QLabel("Background Image Loc: "));
    img_hbox->addWidget(_background_img_loc_le);
    img_hbox->addWidget(_btn_browse);

    _scene_width = new QLineEdit("5000");
    _scene_heigh = new QLineEdit("5000");
    _scene_motor_x_start = new QLineEdit("-10");
    _scene_motor_x_end = new QLineEdit("10");
    _scene_motor_y_start = new QLineEdit("-10");
    _scene_motor_y_end = new QLineEdit("10");
    
    QHBoxLayout* scene_box = new QHBoxLayout();
    scene_box->addWidget(new QLabel("Scene Width: "));
    scene_box->addWidget(_scene_width);
    scene_box->addWidget(new QLabel("px    "));
    scene_box->addWidget(new QLabel("Scene Height: "));
    scene_box->addWidget(_scene_heigh);
    scene_box->addWidget(new QLabel("px"));

    QHBoxLayout* motor_box = new QHBoxLayout();
    motor_box->addWidget(new QLabel("Motor X Start: "));
    motor_box->addWidget(_scene_motor_x_start);
    motor_box->addWidget(new QLabel("mm    "));
    motor_box->addWidget(new QLabel("Motor X End: "));
    motor_box->addWidget(_scene_motor_x_end);
    motor_box->addWidget(new QLabel("mm"));
    QHBoxLayout* motor_y_box = new QHBoxLayout();
    motor_y_box->addWidget(new QLabel("Motor Y Start: "));
    motor_y_box->addWidget(_scene_motor_y_start);
    motor_y_box->addWidget(new QLabel("mm    "));
    motor_y_box->addWidget(new QLabel("Motor Y End: "));
    motor_y_box->addWidget(_scene_motor_y_end);
    motor_y_box->addWidget(new QLabel("mm"));

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addItem(name_hbox);
    layout->addItem(img_hbox);
    layout->addItem(scene_box);
    layout->addItem(motor_box);
    layout->addItem(motor_y_box);
    layout->addWidget(_file_list_view);
    layout->addItem(buttonlayout);
    layout->addWidget(_progressBarFiles);

    setLayout(layout);

}

//---------------------------------------------------------------------------

void GenScanVlmWidget::on_browse()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open Background image", 
                                                    _directory_name,
                                                    "VLM (*.tiff *.tif *.sws)");

    // Dialog returns a nullptr string if user press cancel.
    if (fileName.isNull() || fileName.isEmpty()) return;

    _background_img_loc_le->setText(fileName);
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
    if(_file_map.size() > 0)
    {
        _btn_scan->setEnabled(true);
        _progressBarFiles->setRange(0, _file_map.size());
    }
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

void GenScanVlmWidget::_clear_regions()
{
    _dataset_calib_map.clear();
    _dataset_region_map.clear();
}

//---------------------------------------------------------------------------

void GenScanVlmWidget::runProcessing()
{
    _btn_gen->setEnabled(false);
    _processing = true;

    _clear_regions();
    float min_x_val = std::numeric_limits<float>::max();
    float max_x_val = std::numeric_limits<float>::min();
    float min_y_val = std::numeric_limits<float>::max();
    float max_y_val = std::numeric_limits<float>::min();

    int x_amt = 0;
    int y_amt = 0;
    float avg_x_diff = 0;
    float avg_y_diff = 0;
    int xidx = 0;
    int yidx = 0;
    for (int i = 0; i < _file_list_model->rowCount(); ++i)
    {
        data_struct::ArrayTr<float> x_arr;
        data_struct::ArrayTr<float> y_arr;
        if(false == _processing)
        {
            break;
        }

        QString fname = _file_list_model->item(i)->data(Qt::DisplayRole).toString();
        QFileInfo finfo = _file_map.at(fname);

        if(MapsH5Model::load_x_y_motors_only(finfo.absoluteFilePath(), x_arr, y_arr))
        {
            // only load rect greater than 9x9
            if (x_arr.size() > 9 && y_arr.size() > 9)
            {
                xidx = x_arr.size() / 2;
                avg_x_diff += std::abs(x_arr[xidx + 1] - x_arr[xidx]);
                x_amt++;

                yidx = y_arr.size() / 2;
                avg_y_diff += std::abs(y_arr[yidx + 1] - y_arr[yidx]);
                y_amt++;

                min_x_val = std::min(min_x_val, x_arr.minCoeff());
                max_x_val = std::max(max_x_val, x_arr.maxCoeff());

                min_y_val = std::min(min_y_val, y_arr.minCoeff());
                max_y_val = std::max(max_y_val, y_arr.maxCoeff());

                QMap<QString, QString> region_map;
                region_map[gstar::UPROBE_PRED_POS_X] = QString::number(x_arr[xidx]);
                region_map[gstar::UPROBE_PRED_POS_Y] = QString::number(y_arr[yidx]);
                region_map[gstar::UPROBE_WIDTH] = QString::number(x_arr.size());
                region_map[gstar::UPROBE_RECT_W] = QString::number(x_arr.size());
                region_map[gstar::UPROBE_HEIGHT] = QString::number(y_arr.size());
                region_map[gstar::UPROBE_RECT_H] = QString::number(y_arr.size());
                region_map[gstar::UPROBE_MICRO_POS_X] = QString::number(x_arr[xidx]);
                region_map[gstar::UPROBE_MICRO_POS_Y] = QString::number(y_arr[yidx]);
                region_map[gstar::UPROBE_NAME] = fname;
                _dataset_region_map[fname] = region_map;

                /*
                gstar::UProbeRegionGraphicsItem* region = new gstar::UProbeRegionGraphicsItem();
                region->setPropertyValue(gstar::UPROBE_PRED_POS_X, QVariant(x_arr[xidx]));
                region->setPropertyValue(gstar::UPROBE_PRED_POS_Y, QVariant(y_arr[yidx]));
                region->setHeight(y_arr.size());
                region->setWidth(x_arr.size());
                //region->setPos(QPointF(marker[gstar::UPROBE_REAL_POS_X].toDouble(), marker[gstar::UPROBE_REAL_POS_Y].toDouble()));
                region->setPropertyValue(gstar::UPROBE_NAME, QVariant(fname));
                
                _dataset_region_map[fname] = region;
                */
                _file_list_model->item(i)->setData(QIcon(":/images/circle_green.png"), Qt::DecorationRole);
            }
            else
            {
                _file_list_model->item(i)->setData(QIcon(":/images/circle_red.png"), Qt::DecorationRole);
            }
        }
        else
        {
            _file_list_model->item(i)->setData(QIcon(":/images/circle_red.png"), Qt::DecorationRole);
        }
        _progressBarFiles->setValue(i+1);
    }

    if (x_amt > 0)
    {
        avg_x_diff /= (float)(x_amt);
    }

    if (y_amt > 0)
    {
        avg_y_diff /= (float)(y_amt);
    }

    float diff_x = max_x_val - min_x_val;
    float diff_y = max_y_val - min_y_val;

    float pad_min_x_val = min_x_val - (diff_x * .05);
    float pad_max_x_val = max_x_val + (diff_x * .05);

    float pad_min_y_val = min_y_val - (diff_y * .05);
    float pad_max_y_val = max_y_val + (diff_y * .05);

    _scene_motor_x_start->setText(QString::number(pad_min_x_val));
    _scene_motor_x_end->setText(QString::number(pad_max_x_val));

    _scene_motor_y_start->setText(QString::number(pad_min_y_val));
    _scene_motor_y_end->setText(QString::number(pad_max_y_val));

    //
    //  A      B
    //
    //  C      D
    //

    QMap<QString, QString> a_marker;
    a_marker[gstar::UPROBE_NAME] = "a";
    a_marker[gstar::UPROBE_LIGHT_POS_X] = "0.0";
    a_marker[gstar::UPROBE_LIGHT_POS_Y] = "0.0";
    a_marker[gstar::UPROBE_LIGHT_POS_Z] = "0.0";
    a_marker[gstar::UPROBE_PRED_POS_X] = QString::number(min_x_val);
    a_marker[gstar::UPROBE_PRED_POS_Y] = QString::number(min_y_val);
    a_marker[gstar::UPROBE_MICRO_POS_X] = QString::number(min_x_val);
    a_marker[gstar::UPROBE_MICRO_POS_Y] = QString::number(min_y_val);

    QMap<QString, QString> b_marker;
    b_marker[gstar::UPROBE_NAME] = "b";
    b_marker[gstar::UPROBE_LIGHT_POS_X] = "0.0";
    b_marker[gstar::UPROBE_LIGHT_POS_Y] = "0.0";
    b_marker[gstar::UPROBE_LIGHT_POS_Z] = "0.0";
    b_marker[gstar::UPROBE_PRED_POS_X] = QString::number(max_x_val);
    b_marker[gstar::UPROBE_PRED_POS_Y] = QString::number(min_y_val);
    b_marker[gstar::UPROBE_MICRO_POS_X] = QString::number(max_x_val);
    b_marker[gstar::UPROBE_MICRO_POS_Y] = QString::number(min_y_val);

    QMap<QString, QString> c_marker;
    c_marker[gstar::UPROBE_NAME] = "c";
    c_marker[gstar::UPROBE_LIGHT_POS_X] = "0.0";
    c_marker[gstar::UPROBE_LIGHT_POS_Y] = "0.0";
    c_marker[gstar::UPROBE_LIGHT_POS_Z] = "0.0";
    c_marker[gstar::UPROBE_PRED_POS_X] = QString::number(min_x_val);
    c_marker[gstar::UPROBE_PRED_POS_Y] = QString::number(max_y_val);
    c_marker[gstar::UPROBE_MICRO_POS_X] = QString::number(min_x_val);
    c_marker[gstar::UPROBE_MICRO_POS_Y] = QString::number(max_y_val);
    
    QMap<QString, QString> d_marker;
    d_marker[gstar::UPROBE_NAME] = "d";
    d_marker[gstar::UPROBE_LIGHT_POS_X] = "0.0";
    d_marker[gstar::UPROBE_LIGHT_POS_Y] = "0.0";
    d_marker[gstar::UPROBE_LIGHT_POS_Z] = "0.0";
    d_marker[gstar::UPROBE_PRED_POS_X] = QString::number(max_x_val);
    d_marker[gstar::UPROBE_PRED_POS_Y] = QString::number(max_y_val);
    d_marker[gstar::UPROBE_MICRO_POS_X] = QString::number(max_x_val);
    d_marker[gstar::UPROBE_MICRO_POS_Y] = QString::number(max_y_val);

    _dataset_calib_map["A"] = a_marker;
    _dataset_calib_map["B"] = b_marker;
    _dataset_calib_map["C"] = c_marker;
    _dataset_calib_map["D"] = d_marker;

    _processing = false;
    _btn_gen->setEnabled(true);  

}

//---------------------------------------------------------------------------

void GenScanVlmWidget::_generate()
{
    _btn_browse->setEnabled(false);
    _btn_gen->setEnabled(false);
    _btn_scan->setEnabled(false);
    QString xml_name =_directory_name+QDir::separator()+"vlm"+QDir::separator()+_gen_name_le->text()+".xml";
    QString tif_name =_directory_name+QDir::separator()+"vlm"+QDir::separator()+_gen_name_le->text()+".tif";
    TIFF_Model model;
    if(_background_img_loc_le->text().size() > 0)
    {
        //QImage img(_background_img_loc_le->text());
        //model.set_background_img(img);
        // copy tiff to vlm folder as name
    }
    else
    {
        QImage img(QSize(_scene_width->text().toInt(), _scene_heigh->text().toInt()), QImage::Format_RGB32);
        img.fill(Qt::gray);
        model.set_background_img(img);
    }
    float f_scene_w = _scene_width->text().toFloat();
    float f_scene_h = _scene_heigh->text().toFloat();

    float w10 = f_scene_w * .05;
    float h10 = f_scene_h * .05;
    _dataset_calib_map["A"][gstar::UPROBE_LIGHT_POS_X] = QString::number(w10);
    _dataset_calib_map["A"][gstar::UPROBE_REAL_POS_X] = QString::number(w10);
    _dataset_calib_map["A"][gstar::UPROBE_LIGHT_POS_Y] = QString::number(h10);
    _dataset_calib_map["A"][gstar::UPROBE_REAL_POS_Y] = QString::number(h10);
    _dataset_calib_map["B"][gstar::UPROBE_LIGHT_POS_X] = QString::number(f_scene_w - w10);
    _dataset_calib_map["B"][gstar::UPROBE_REAL_POS_X] = QString::number(f_scene_w - w10);
    _dataset_calib_map["B"][gstar::UPROBE_LIGHT_POS_Y] = QString::number(h10);
    _dataset_calib_map["B"][gstar::UPROBE_REAL_POS_Y] = QString::number(h10);
    _dataset_calib_map["C"][gstar::UPROBE_LIGHT_POS_X] = QString::number(w10);
    _dataset_calib_map["C"][gstar::UPROBE_REAL_POS_X] = QString::number(w10);
    _dataset_calib_map["C"][gstar::UPROBE_LIGHT_POS_Y] = QString::number(f_scene_h - h10);
    _dataset_calib_map["C"][gstar::UPROBE_REAL_POS_Y] = QString::number(f_scene_h - h10);
    _dataset_calib_map["D"][gstar::UPROBE_LIGHT_POS_X] = QString::number(f_scene_w - w10);
    _dataset_calib_map["D"][gstar::UPROBE_REAL_POS_X] = QString::number(f_scene_w - w10);
    _dataset_calib_map["D"][gstar::UPROBE_LIGHT_POS_Y] = QString::number(f_scene_h - h10);
    _dataset_calib_map["D"][gstar::UPROBE_REAL_POS_Y] = QString::number(f_scene_h - h10);

    float min_x_mot = _scene_motor_x_start->text().toFloat();
    float max_x_mot = _scene_motor_x_end->text().toFloat();

    float x_tot_mot = std::abs(max_x_mot - min_x_mot);

    float min_y_mot = _scene_motor_y_start->text().toFloat();
    float max_y_mot = _scene_motor_y_end->text().toFloat();

    float y_tot_mot = std::abs(max_y_mot - min_y_mot);

    for (auto itr : _dataset_calib_map)
    {
        model.add_calib_marker(itr.second);
    }
    for (auto itr : _dataset_region_map)
    {
        float mx = itr.second[gstar::UPROBE_MICRO_POS_X].toFloat();
        float per_x = std::abs( mx - min_x_mot) / x_tot_mot;
        float px = per_x * f_scene_w;
        int width = itr.second[gstar::UPROBE_WIDTH].toInt();
        int tlx = px - (width * 0.5);
        itr.second[gstar::UPROBE_REAL_POS_X] = QString::number(px);
        itr.second[gstar::UPROBE_LIGHT_POS_X] = QString::number(px);
        itr.second[gstar::UPROBE_RECT_TLX] = QString::number(tlx);
        float my = itr.second[gstar::UPROBE_MICRO_POS_Y].toFloat();
        float per_y = std::abs( my - min_y_mot) / y_tot_mot;
        float py = per_y * f_scene_h;
        int height = itr.second[gstar::UPROBE_HEIGHT].toInt();
        int tly = py - (height * 0.5);
        itr.second[gstar::UPROBE_REAL_POS_Y] = QString::number(py);
        itr.second[gstar::UPROBE_LIGHT_POS_Y] = QString::number(py);
        itr.second[gstar::UPROBE_RECT_TLY] = QString::number(tly);
        model.add_region_marker(itr.second);
    }
    if(model.save_img(tif_name))
    {
        model.save_xml(xml_name);
    }
    emit new_scan_area(tif_name);
    close();

    _btn_browse->setEnabled(true);
    _btn_gen->setEnabled(true);
    _btn_scan->setEnabled(true);
}

//---------------------------------------------------------------------------