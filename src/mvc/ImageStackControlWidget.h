/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
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

protected:

   void createLayout();

   MapsWorkspaceModel* _model;

   QComboBox *_image_name_cb;

   QPushButton *_left_btn;

   QPushButton *_right_btn;

};


/*---------------------------------------------------------------------------*/

#endif /* ImageStackControlWidget_H_ */

/*---------------------------------------------------------------------------*/

