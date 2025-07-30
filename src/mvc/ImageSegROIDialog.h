/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef IMAGE_SEG_ROI_DIALOG_H
#define IMAGE_SEG_ROI_DIALOG_H

//---------------------------------------------------------------------------

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QStringListModel>
#include <QListWidgetItem>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include "data_struct/fit_parameters.h"
#include <mvc/ImageSegWidget.h>
#include "gstar/Annotation/RoiMaskGraphicsItem.h"
#include "mvc/SpectraWidget.h"
#include <QTabWidget>
#include <mvc/MapsH5Model.h>
#include <QProgressBar>
#ifdef _BUILD_WITH_OPENCV
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#endif


//---------------------------------------------------------------------------

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

   void setModel(MapsH5Model* model);

   void setColorMap(QVector<QRgb>* selected_colormap);

   void clear_all_rois();

   void clear_image();

   void append_roi(gstar::RoiMaskGraphicsItem* roi) { _int_img_widget->addRoiMask(roi); }

signals:
	void onNewROIs(std::vector<gstar::RoiMaskGraphicsItem*> roi_list);

public slots:
	void onRun();

	void onAccept();

	void onImgSelection(QStandardItem* item);

	void onClose();

	void onNewROI();

	void onInvertROI();

private slots:

	void onTabChanged(int);

	void updateCustomCursor(int);

	void manualActionChanged(QString);

	void onNormalizeChanged(int);

	void onPlot();

protected:

   void createLayout();

   bool _get_img(ArrayXXr<float>& int_img, bool normalize);

	void _model_custom_spectra(const std::string& analysis_name, const std::vector<std::pair<int, int>>& pixel_list, ArrayDr* spec);

	QWidget* _createKMeansLayout();

	QWidget* _createDBScanLayout();

	QWidget* _createManualLayout();

	QWidget* _createPlotOptionsLayout();

private:
	QTabWidget* _techTabs;

	QPushButton* _runBtn;
	QPushButton* _acceptBtn;
	QPushButton* _cancelBtn;
  
	QListView* _img_names_view;
	QStandardItemModel* _img_list_model;
	QCheckBox* _chk_normalize_sum;
	
	ImageSegWidget* _int_img_widget;

	SpectraWidget* _spectra_widget;

	QVector<QRgb>* _selected_colormap;

	std::map<QString, QWidget*> _layout_map;

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

	//QComboBox* _cb_selected_roi;

	QPushButton* _manual_btn_add_roi;
	QComboBox* _manual_cb_action;
	QSpinBox* _manual_sp_brush_size;
	QPushButton* _manual_invert_roi;

	QCheckBox* _plot_ck_model_nnls;
	QCheckBox* _plot_ck_model_matrix;
	QCheckBox* _plot_ck_model_svd;

	int _next_color;

	QTabWidget* _tab_widget;

	QPushButton* _plotBtn;

	 MapsH5Model* _model;

	 QProgressBar* _pb_pixels;

	 QProgressBar* _pb_rois;
};


//---------------------------------------------------------------------------

#endif /* ImageSegRoiDialog_H_ */

//---------------------------------------------------------------------------

