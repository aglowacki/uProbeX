/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef BATCH_ROI_FIT_WIDGET_H
#define BATCH_ROI_FIT_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QThread>
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
#include <QCheckBox>
#include <QEventLoop>
#include <QLineEdit>
#include <QGroupBox>
#include <qfileinfo.h>
#include <thread>

/*---------------------------------------------------------------------------*/


class BatchRoiFitWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
	BatchRoiFitWidget(std::string directory, QWidget *parent = nullptr);

   /**
    * Destructor.
    */
   ~BatchRoiFitWidget();

    void updateFileList(std::unordered_map<QString, QFileInfo> roi_map);

    void status_callback(size_t cur_block, size_t total_blocks);

signals:

    void processed_list_update(QStringList);

public slots:
    void runProcessing();
   
protected:

   /**
    * @brief Create layout
    */
   void createLayout();

   QStringList _file_list;
   
   QTextEdit *_textEdit;

   QProgressBar *_progressBarFiles;

   QProgressBar* _progressBarBlocks;

   QPushButton *_btn_run;

   QPushButton *_btn_cancel;

   QListView* _file_list_view;

   QStandardItemModel* _file_list_model;

   QCheckBox* _proc_roi;

   QCheckBox* _proc_nnls;

   QCheckBox* _proc_matrix;

   QCheckBox* _save_avg;

   QCheckBox* _save_v9;

   QCheckBox* _save_csv;
   
   QCheckBox* _save_exchange;

   QCheckBox* _perform_quantification;

   std::string _directory;

   size_t _cur_file;

   size_t _cur_block;
   
   QLineEdit* _le_detectors;
   
   QHBoxLayout* _proc_save_layout;

   QGroupBox* _saving_grp;

   QGroupBox* _processing_grp;

   size_t _total_blocks;

   std::unordered_map<QString, QFileInfo> _roi_map;
};


/*---------------------------------------------------------------------------*/

#endif 

/*---------------------------------------------------------------------------*/
