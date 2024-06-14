/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GEN_SCAN_VLM_WIDGET_H
#define GEN_SCAN_VLM_WIDGET_H

//---------------------------------------------------------------------------

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
//#include <QCheckBox>
#include <QEventLoop>
#include <QFileInfo>
#include <mvc/MapsH5Model.h>
#include "gstar/Annotation/UProbeRegionGraphicsItem.h"

//---------------------------------------------------------------------------


class GenScanVlmWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
	GenScanVlmWidget(QWidget *parent = nullptr);

   /**
    * Destructor.
    */
   ~GenScanVlmWidget();

   void updateFileList(const std::map<QString, QFileInfo>& file_map);

   void setDir(QString directory);

signals:

    void processed_list_update(QStringList);

public slots:

    void runProcessing();

   void setStoped();

protected:

    void _clear_regions();

   /**
    * @brief Create layout
    */
   void createLayout();

   bool _processing;

   QString _directory_name;

   QLineEdit* _gen_name_le;

   QLineEdit* _scene_width;

   QLineEdit* _scene_heigh;

   QLineEdit* _scene_motor_x_start;

   QLineEdit* _scene_motor_x_end;

   QLineEdit* _scene_motor_y_start;

   QLineEdit* _scene_motor_y_end;

   std::map<QString, QFileInfo> _file_map;

   QProgressBar *_progressBarFiles;

   QPushButton* _btn_gen;

   QPushButton* _btn_cancel;

   QStandardItemModel* _file_list_model;

   QListView* _file_list_view;

   std::map<QString, gstar::UProbeRegionGraphicsItem*> _dataset_region_map;
};


//---------------------------------------------------------------------------

#endif 

//---------------------------------------------------------------------------
