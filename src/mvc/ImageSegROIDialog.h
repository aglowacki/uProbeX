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
#include <mvc/ImageSegWidget.h>
#include <opencv2/opencv.hpp>
#include "gstar/Annotation/RoiMaskGraphicsItem.h"

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
	void onNewROIs(std::vector<gstar::RoiMaskGraphicsItem*> roi_list);

public slots:
	void onRun();

	void onAccept();

	void onSetTech(QString name);

	void onImgSelection(QStandardItem* item);

protected:

   void createLayout();

   bool _get_img(ArrayXXr<float>& int_img);

   QImage _generate_img(ArrayXXr<float>& int_img);

   //std::vector<QImage> _generate_images(int num_images, cv::Mat& mat);

   //QImage _generate_sum_image(cv::Mat& mat, ArrayXXr<float>& bg_img, uchar alpha=127);

   QLayout* _createKMeansLayout();

private:
	QLabel* _techLabel;
	QComboBox* _cb_tech;
	
	QVBoxLayout* _techLayout;

	QPushButton* _runBtn;
	QPushButton* _acceptBtn;
	QPushButton* _cancelBtn;
  
	QListView* _img_names_view;
	QStandardItemModel* _img_list_model;
		
	ImageSegWidget* _int_img_widget;

	QVector<QRgb>* _selected_colormap;

	std::map<QString, QLayout*> _layout_map;

	std::map<QString, data_struct::ArrayXXr<float>> _img_data;

	std::map<int, QColor> _color_map;

	// kmeans controls
	QComboBox* _km_TermCriteria;
	QComboBox* _km_Centers;
	QLineEdit* _km_le_MAX_ITER;
	QLineEdit* _km_le_epsilon;
	QLineEdit* _km_nfeatures;
	QLineEdit* _km_attempts;

	QSize _image_size;

};


/*---------------------------------------------------------------------------*/

#endif /* ImageSegRoiDialog_H_ */

/*---------------------------------------------------------------------------*/

