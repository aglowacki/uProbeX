/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef IMAGE_GRID_WIDGET_H
#define IMAGE_GRID_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QLabel>
#include <mvc/MapsWorkspaceModel.h>
#include <QPushButton>
#include <QComboBox>
#include <QStringListModel>
#include <QListWidgetItem>
#include <gstar/ImageViewWidget.h>

/*---------------------------------------------------------------------------*/

/**
 * @brief When open the acquisition window, the widget is showing for capturing
 * the image from the area detector writer, the window will also be updated to
 * show the image.
 */
class ImageGridWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   ImageGridWidget(QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~ImageGridWidget();

 public slots:
 
	 void setModel(MapsH5Model* model);

protected:

   void createLayout();

   MapsH5Model* _model;

   vector<gstar::ImageViewWidget*> _imageViewArray;
 
};


/*---------------------------------------------------------------------------*/

#endif /* ImageGridWidget_H_ */

/*---------------------------------------------------------------------------*/

