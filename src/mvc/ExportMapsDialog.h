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
#include "core/defines.h"

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

   std::string _directory;

   QProgressBar *_progressBarFiles;

   QPushButton *_btn_run;

   QPushButton *_btn_cancel;

   QCheckBox* _export_tiff;

   QCheckBox* _export_png;

   QCheckBox* _export_ascii;
   /*
   QCheckBox* _save_avg;

   QCheckBox* _save_v9;

   QCheckBox* _save_csv;
   
   QCheckBox* _save_exchange;

   QCheckBox* _perform_quantification;
   */
   
};


/*---------------------------------------------------------------------------*/

#endif 

/*---------------------------------------------------------------------------*/
