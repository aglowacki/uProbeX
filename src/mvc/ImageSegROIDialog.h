/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef IMAGE_SEG_ROI_DIALOG_H
#define IMAGE_SEG_ROI_DIALOG_H

/*---------------------------------------------------------------------------*/

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QStringListModel>
#include <QListWidgetItem>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include "data_struct/fit_parameters.h"
#include <gstar/AbstractImageWidget.h>

/*---------------------------------------------------------------------------*/

class ImageSegRoiDialog : public QDialog
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   ImageSegRoiDialog();

   /**
    * Destructor.
    */
   ~ImageSegRoiDialog();

   void setImageData(std::unordered_map<std::string, data_struct::ArrayXXr<float>>& img_data);

   void setColorMap(QVector<QRgb>* selected_colormap);

signals:
	void onNewROIs(std::vector<QPoint> roi_list);

public slots:
	void onRun();

	void onAccept();

	void onSetTech(QString name);

	void onImgSelection(QStandardItem* item);

protected:

   void createLayout();

   QLayout* _createKMeansLayout();

private:
	QLabel* _techLabel;
	QComboBox* _cb_tech;
	
	QVBoxLayout* _techLayout;

	QPushButton* runBtn;
	QPushButton* acceptBtn;
	QPushButton* cancelBtn;
  
	QListView* _img_names_view;
	QStandardItemModel* _img_list_model;

	QListView* _roi_names_view;
	QStandardItemModel* _roi_list_model;

	
	//gstar::AbstractImageWidget* _int_img_widget;
	gstar::ImageViewWidget* _int_img_widget;
	QVector<QRgb>* _selected_colormap;

	std::map<QString, QLayout*> _layout_map;

	std::map<QString, data_struct::ArrayXXr<float>> _img_data;

};


/*---------------------------------------------------------------------------*/

#endif /* ImageSegRoiDialog_H_ */

/*---------------------------------------------------------------------------*/

