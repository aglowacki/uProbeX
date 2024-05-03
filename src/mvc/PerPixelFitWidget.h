/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PER_PIXEL_FIT_WIDGET_H
#define PER_PIXEL_FIT_WIDGET_H

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
#include <thread>
#include "mvc/PerPixelOptionsWidget.h"

/*---------------------------------------------------------------------------*/


class PerPixelFitWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
	PerPixelFitWidget(QWidget *parent = nullptr);

   /**
    * Destructor.
    */
   ~PerPixelFitWidget();

   void updateFileList(QStringList file_list) { _optionsWidget->updateFileList(file_list); }

   void setDir(std::string directory);

signals:

    void processed_list_update(QStringList);

public slots:

    void runProcessing();
   
protected:

   /**
    * @brief Create layout
    */
   void createLayout();
   
   PerPixelOptionsWidget* _optionsWidget;

   std::string _directory;

};


/*---------------------------------------------------------------------------*/

#endif 

/*---------------------------------------------------------------------------*/
