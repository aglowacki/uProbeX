/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef UPGRADE_ROI_DIALOG_H
#define UPGRADE_ROI_DIALOG_H

//---------------------------------------------------------------------------

#include <QDialog>
#include <QTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QComboBox>
#include <QEventLoop>
#include <QLineEdit>
#include <QGroupBox>
#include <qfileinfo.h>
#include <unordered_map>
#include "MapsH5Model.h"

//---------------------------------------------------------------------------
const QString STR_V9_ROIS_DIR = "rois_v9";

class UpgradeRoiDialog : public QDialog
{

   Q_OBJECT

public:

    /**
    * Constructor.
    */
    UpgradeRoiDialog();

    /**
    * Destructor.
    */
    ~UpgradeRoiDialog();

    void setDirectory(QDir directory);

public slots:
 
    void runProcessing();
 
    void onCancelClose();

protected:

   /**
    * @brief Create layout
    */
   void _createLayout();

   bool _load_v9_rois(QString fname, MapsH5Model* model, QString& out_hdf_file);

   bool _get_filesnames_in_directory(QDir dir, QString sub_dir_name, QList <QString> suffex, std::map<QString, QFileInfo>* fileinfo_list);

   QStringList _file_list;
   
   QProgressBar *_progressBarFiles;

   QProgressBar* _progressBarBlocks;

   QPushButton *_btn_run;

   QPushButton *_btn_cancel;

   QListView* _file_list_view;

   QStandardItemModel* _file_list_model;

   QDir _directory;

   QLineEdit* _le_detectors;

   std::map<QString, QFileInfo> _h5_fileinfo_list;

   std::map<QString, QFileInfo> _roi_fileinfo_list;

   QList <QString> _all_h5_suffex;

   QList <QString> _all_roi_suffex;

   size_t _total_itr;

   std::map<int, QColor> _color_map;

   bool _running;
};


//---------------------------------------------------------------------------

#endif 

//---------------------------------------------------------------------------
