/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "mvc/UpgradeRoiDialog.h"
#include <QApplication>
#include <core/process_whole.h>
#include <QScrollArea>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDir>

//---------------------------------------------------------------------------

UpgradeRoiDialog::UpgradeRoiDialog() : QDialog()
{
    _all_roi_suffex.append("roi");
    _all_h5_suffex.append("h5");
    _all_h5_suffex.append("h50");
    _all_h5_suffex.append("h51");
    _all_h5_suffex.append("h52");
    _all_h5_suffex.append("h53");

    _color_map.insert({ 0, QColor(Qt::red) });
    _color_map.insert({ 1, QColor(Qt::green) });
    _color_map.insert({ 2, QColor(Qt::blue) });
    _color_map.insert({ 3, QColor(Qt::yellow) });
    _color_map.insert({ 4, QColor(Qt::magenta) });
    _color_map.insert({ 5, QColor(Qt::white) });
    _color_map.insert({ 6, QColor(Qt::gray) });
    _color_map.insert({ 7, QColor(Qt::cyan) });
    _color_map.insert({ 8, QColor(Qt::darkGray) });
    _color_map.insert({ 9, QColor(Qt::darkGreen) });
    _color_map.insert({ 10, QColor(Qt::darkMagenta) });
    _color_map.insert({ 11, QColor(Qt::darkBlue) });
    _color_map.insert({ 12, QColor(Qt::darkRed) });
    _color_map.insert({ 13, QColor(Qt::darkYellow) });
    _color_map.insert({ 14, QColor(Qt::darkCyan) });
    _color_map.insert({ 15, QColor(Qt::black) });

    _running = false;
    _createLayout();
}

//---------------------------------------------------------------------------

UpgradeRoiDialog::~UpgradeRoiDialog()
{

 
}

//---------------------------------------------------------------------------

void UpgradeRoiDialog::_createLayout()
{

    _progressBarBlocks = new QProgressBar();
    _progressBarBlocks->setRange(0,100);
    _progressBarFiles = new QProgressBar();
    _progressBarFiles->setRange(0, 100);

    _btn_run = new QPushButton("Convert");
    connect(_btn_run, &QPushButton::released, this, &UpgradeRoiDialog::runProcessing);
    _btn_cancel = new QPushButton("Cancel");
    connect(_btn_cancel, &QPushButton::released, this, &UpgradeRoiDialog::onCancelClose);

    _le_detectors = new QLineEdit("0,1,2,3");
    
    _file_list_model = new QStandardItemModel();
    _file_list_view = new QListView();
    _file_list_view->setModel(_file_list_model);
    _file_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    QHBoxLayout* buttonlayout = new QHBoxLayout();
    buttonlayout->addWidget(_btn_run);
    buttonlayout->addWidget(_btn_cancel);
   
    QHBoxLayout* hbox_progresss_blocks = new QHBoxLayout();
    hbox_progresss_blocks->addWidget(new QLabel("Current:"));
    hbox_progresss_blocks->addWidget(_progressBarBlocks);

    QHBoxLayout* hbox_progresss_files = new QHBoxLayout();
    hbox_progresss_files->addWidget(new QLabel("All Dataset Files:"));
    hbox_progresss_files->addWidget(_progressBarFiles);

    QHBoxLayout* detector_hbox = new QHBoxLayout();
    detector_hbox->addWidget(new QLabel("Detectors  "));
    detector_hbox->addWidget(_le_detectors);

    

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addItem(detector_hbox);
    layout->addWidget(_file_list_view);
    layout->addItem(buttonlayout);
    layout->addItem(hbox_progresss_blocks);
    layout->addItem(hbox_progresss_files);
    
    this->setLayout(layout);
}

//---------------------------------------------------------------------------

void UpgradeRoiDialog::setDirectory(QDir directory)
{
    _directory = directory;
    _total_itr = 0;
    _running = false;
    _get_filesnames_in_directory(_directory, "img.dat", _all_h5_suffex, &_h5_fileinfo_list);
    _get_filesnames_in_directory(_directory, "rois", _all_roi_suffex, &_roi_fileinfo_list);
    _file_list_model->clear();
    for (auto& itr : _roi_fileinfo_list)
    {
        _file_list_model->appendRow(new QStandardItem(itr.first));
    }
}

void UpgradeRoiDialog::onCancelClose()
{
    if (false == _running)
    {
        close();
    }
    else
    {
        _running = false;
    }
}

//---------------------------------------------------------------------------

void UpgradeRoiDialog::runProcessing()
{
    _btn_run->setEnabled(false);
    _le_detectors->setEnabled(false);
    _file_list_view->setEnabled(false);
    _btn_cancel->setText("Cancel");
    _running = true;
    QCoreApplication::processEvents();
    QDir v9RoiDir(_directory.absolutePath() + QDir::separator() + STR_V9_ROIS_DIR);
    
    if (false == v9RoiDir.exists())
    {
        _directory.mkdir(STR_V9_ROIS_DIR);
    }

    int proc_total = _roi_fileinfo_list.size(); //  * detector_num_arr.size();
    _progressBarFiles->setRange(0, proc_total);

    int i = 0;
    for (auto& itr : _roi_fileinfo_list)
    {
        QCoreApplication::processEvents();
        MapsH5Model model;
        QString hdf_filename;
        if (_load_v9_rois(itr.first, &model, hdf_filename))
        {
            QFileInfo fileinfo(hdf_filename);
            QString hdf_path = _directory.absolutePath() + QDir::separator() + "rois" + QDir::separator() + fileinfo.baseName() + ".r0i";
            model.saveAllRoiMaps(hdf_path);

            // move old roi from rois to rois_v9 
            QString oldName = _directory.absolutePath() + QDir::separator() + "rois" + QDir::separator() + itr.first;
            QString newName = _directory.absolutePath() + QDir::separator() + STR_V9_ROIS_DIR + QDir::separator() + itr.first;
            _directory.rename(oldName, newName);

            if (false == _running)
            {
                break;
            }
        }
        i++;
        _progressBarFiles->setValue(i);
    }

    _progressBarFiles->setValue(proc_total);
    _running = false;
    _btn_run->setEnabled(true);
    _le_detectors->setEnabled(true);
    _file_list_view->setEnabled(true);
    _btn_cancel->setText("Close");

}

//---------------------------------------------------------------------------

bool UpgradeRoiDialog::_get_filesnames_in_directory(QDir dir, QString sub_dir_name, QList <QString> suffex, std::map<QString, QFileInfo>* fileinfo_list)
{
    if (sub_dir_name.length() > 0)
    {
        if (!dir.cd(sub_dir_name))
        {
            //QString warn_msg = "Cannot find the "+sub_dir_name+" directory";
            //qWarning(warn_msg.toStdString().c_str());
            //logW<< "Cannot find the " << sub_dir_name.toStdString() << " directory";
            return false;
        }
    }
    dir.setFilter(QDir::Files | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        if (suffex.contains(fileInfo.suffix()))
        {
            //if (chk_func(fileInfo)) //TODO: maybe have option for checks 
            {
                fileinfo_list->emplace(fileInfo.fileName(), fileInfo);
            }
        }
    }

    if (sub_dir_name.length() > 0 && sub_dir_name != ".")
    {
        dir.cd("..");
    }
    return true;
}

//---------------------------------------------------------------------------

bool UpgradeRoiDialog::_load_v9_rois(QString fname, MapsH5Model* model, QString &out_hdf_file)
{

    // check V9 ROI's first
    // get 4 numbers in dataset name to ref roi's
    QRegularExpression re("[0-9][0-9][0-9][0-9]");
    QRegularExpression axo_re("axo_std");

    QRegularExpressionMatch match = re.match(fname);
    QRegularExpressionMatch axo_match = axo_re.match(fname);
    if (match.hasMatch() || axo_match.hasMatch())
    {
        QString dataset_num;
        if (match.hasMatch())
        {
            QStringList matched_tests = match.capturedTexts();
            dataset_num = matched_tests.first();
        }
        if (axo_match.hasMatch())
        {
            dataset_num = axo_match.capturedTexts().first();
        }
        QRegularExpression re2(dataset_num);


        for (auto& itr : _h5_fileinfo_list)
        {
            QRegularExpressionMatch match2 = re2.match(itr.first);
            if (match2.hasMatch())
            {
                logI << "ROI : H5 = " << itr.first.toStdString() << " roi = " << fname.toStdString() << "\n";

                std::map<std::string, std::vector<std::pair<int, int>>> rois;
                std::string search_filename;
                //data_struct::Detector<double>* detector;
                out_hdf_file = itr.first;
                try
                {
                    QString roi_file_path = _directory.absolutePath() + QDir::separator() + "rois" + QDir::separator() + fname;
                    if (io::file::aps::load_v9_rois(roi_file_path.toStdString(), rois))
                    {
                        _progressBarBlocks->setRange(0, rois.size());
                        
                        int clr_idx = 0;
                        for (auto& roi_itr : rois)
                        {
                            _progressBarBlocks->setValue(clr_idx);
                            Spectra<double>* int_spectra = new Spectra<double>();
                            QString hdf_file_path = _directory.absolutePath() + QDir::separator() + "img.dat" + QDir::separator() + itr.first;
                            if (io::file::HDF5_IO::inst()->load_integrated_spectra_analyzed_h5_roi(hdf_file_path.toStdString(), int_spectra, roi_itr.second))
                            {
                                //                                        color,   alpha
                                struct Map_ROI map_roi(roi_itr.first, _color_map.at(clr_idx), 50, roi_itr.second, itr.first.toStdString(), *int_spectra);
                                clr_idx++;
                                model->appendMapRoi(roi_itr.first, map_roi);
                            }
                            else
                            {
                                logW << "Could not load intspec roi from file " << itr.first.toStdString() << "\n";
                            }

                            if (false == _running)
                            {
                                return false;
                            }
                        }

                        // move to v9rois folder when done

                        _progressBarBlocks->setValue(rois.size());
                        return true;
                    }
                    else
                    {
                        logW << "Could not load v9 roi from file " << fname.toStdString() << "\n";
                    }
                }
                catch (std::exception e)
                {
                    logE << e.what() << "\n";
                }
                    
            }
        }
    }
    else
    {
        logW << "Cound not match regular expression [0-9][0-9][0-9][0-9] with filename " << fname.toStdString() << "\n";
    }
    return false;
}

//---------------------------------------------------------------------------

