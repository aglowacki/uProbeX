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
#include <QComboBox>
#include <QEventLoop>
#include <QLineEdit>
#include <QGroupBox>
#include <qfileinfo.h>
#include <thread>
#include <unordered_map>
#include <data_struct/analysis_job.h>
#include <mvc/PerPixelOptionsWidget.h>
//#include "mvc/OptimizerOptionsWidget.h"

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

public slots:
    void runProcessing();

protected:

   /**
    * @brief Create layout
    */
   void createLayout();
   /*
   QListView* _file_list_view;

   QStandardItemModel* _file_list_model;
   */
   std::string _directory;

   //QLineEdit* _le_detectors;

   //QComboBox* _cb_opt_method;

   std::unordered_map<QString, QFileInfo> _roi_map;

   //size_t _total_itr;

   PerPixelOptionsWidget* _optionsWidget;

   //OptimizerOptionsWidget* _optimizer_widget;

   data_struct::Analysis_Job<double> _analysis_job;

   bool _canceled;
};


/*---------------------------------------------------------------------------*/

#endif 

/*---------------------------------------------------------------------------*/
