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

   void setModel(MapsWorkspaceModel* model);

   void update_file_list() { if(_mapsFilsWidget != nullptr) {_mapsFilsWidget->model_done_loading();}}

signals:
	void newH5ModelSelected(MapsH5Model*);

	void widgetClosed();

public slots:
   void loadList_H5(QStringList sl);

   void unloadList_H5(QStringList sl);

   void h5IndexChanged(const QString &text);

   void onNewH5ModelSelected(MapsH5Model*);

protected:

	void closeEvent(QCloseEvent *event);

   void createLayout();

   MapsWorkspaceFilesWidget* _mapsFilsWidget;

   MapsWorkspaceModel* _model;

   MapsElementsWidget* _imageGrid;

   QComboBox *_image_name_cb;

   QPushButton *_left_btn;

   QPushButton *_right_btn;

   map<QString, MapsH5Model*> _h5_model_map;

};


/*---------------------------------------------------------------------------*/

#endif /* ImageStackControlWidget_H_ */

/*---------------------------------------------------------------------------*/

