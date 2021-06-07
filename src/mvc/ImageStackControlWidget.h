/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef IMAGE_STACK_CONTROL_WIDGET_H
#define IMAGE_STACK_CONTROL_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QLabel>
#include <mvc/MapsWorkspaceModel.h>
#include <QPushButton>
#include <QComboBox>
#include <QStringListModel>
#include <QListWidgetItem>
#include "mvc/VLM_Widget.h"
#include "mvc/MDA_Widget.h"
#include "mvc/MapsElementsWidget.h"
#include "mvc/MapsWorkspaceFilesWidget.h"

/*---------------------------------------------------------------------------*/

/**
 * @brief When open the acquisition window, the widget is showing for capturing
 * the image from the area detector writer, the window will also be updated to
 * show the image.
 */
class ImageStackControlWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   ImageStackControlWidget(QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~ImageStackControlWidget();

   void setModel(std::shared_ptr<MapsWorkspaceModel> model);

   void update_file_list();

signals:

	void widgetClosed();

public slots:
   void onLoad_Model(const QString name, MODEL_TYPE mt);

   void onUnloadList_Model(const QStringList sl, MODEL_TYPE mt);

   void model_IndexChanged(const QString &text);

   void onPrevFilePressed();

   void onNextFilePressed();

   void update_progress_bar(int val, int amt);

   void onLinkRegionToDataset(QString item_name, QString vlm_file_path, QImage image);

protected:

	void closeEvent(QCloseEvent *event);

   void createLayout();

   MapsWorkspaceFilesWidget* _mapsFilsWidget;

   std::shared_ptr<MapsWorkspaceModel> _model;

   MapsElementsWidget* _imageGrid;

   VLM_Widget *_vlm_widget;

   MDA_Widget* _mda_widget;

   QComboBox *_image_name_cb;

   QStringListModel _image_name_cb_model;

   QPushButton *_left_btn;

   QPushButton *_right_btn;

   map<QString, std::shared_ptr<MapsH5Model> > _h5_model_map;

   map<QString, std::shared_ptr<RAW_Model> > _raw_model_map;

   map<QString, std::shared_ptr<VLM_Model> > _vlm_model_map;

   QProgressBar* _load_progress;

   //QDialog _raw_file_dialog;

};


/*---------------------------------------------------------------------------*/

#endif /* ImageStackControlWidget_H_ */

/*---------------------------------------------------------------------------*/

