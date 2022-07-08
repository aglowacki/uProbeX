/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef EXPORT_MAPS_DIALOG_H
#define EXPORT_MAPS_DIALOG_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QTextEdit>
#include <QDir>
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
	ExportMapsDialog(QDir directory, QWidget *parent = nullptr);

   /**
    * Destructor.
    */
   ~ExportMapsDialog();

    void status_callback(size_t cur, size_t total);

    void setRunEnabled(bool val);

    bool get_save_tiff() { return _export_tiff->isChecked(); }

    bool get_save_png() { return _export_png->isChecked(); }

    bool get_save_ascii() { return _export_ascii->isChecked(); }

    bool get_export_all() { return _export_all->isChecked(); }

    bool get_save_screen() { return _save_screen->isChecked(); }

    QDir get_dir() { return _directory; }

signals:

    void export_released();

public slots:

    void on_browse();

    void on_open();

protected:

   /**
    * @brief Create layout
    */
   void createLayout();

   QDir _directory;

   QGroupBox* _processing_grp;

   QProgressBar *_progressBarFiles;

   QPushButton *_btn_run;

   QPushButton *_btn_cancel;

   QCheckBox* _export_tiff;

   QCheckBox* _export_png;

   QCheckBox* _export_ascii;

   QCheckBox* _export_all;

   QCheckBox* _save_screen;

   QLabel* _le_dir;
   
   size_t _total_blocks;

};


/*---------------------------------------------------------------------------*/

#endif 

/*---------------------------------------------------------------------------*/
