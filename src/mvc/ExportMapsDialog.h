/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef EXPORT_MAPS_DIALOG_H
#define EXPORT_MAPS_DIALOG_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
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
#include <QApplication>
#include <QGroupBox>
#include <QScrollArea>
#include <QMessageBox>

/*---------------------------------------------------------------------------*/


class ExportMapsDialog : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
	ExportMapsDialog(std::string directory, QWidget *parent = nullptr);

   /**
    * Destructor.
    */
   ~ExportMapsDialog();

    void updateFileList(QStringList file_list);

    void status_callback(size_t cur_block, size_t total_blocks);

signals:

    void processed_list_update();

public slots:
    void runProcessing();
   
protected:

   /**
    * @brief Create layout
    */
   void createLayout();

   
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

   QLineEdit* _le_detectors;
   
};


/*---------------------------------------------------------------------------*/

#endif 

/*---------------------------------------------------------------------------*/
