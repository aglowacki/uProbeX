/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef Roi_Statistics_Widget_H
#define Roi_Statistics_Widget_H

/*---------------------------------------------------------------------------*/

#include <QWidget>

/*#include <QLabel>
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
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "gstar/Annotation/RoiMaskGraphicsItem.h"
*/
/*---------------------------------------------------------------------------*/

class RoiStatisticsWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   RoiStatisticsWidget();

   /**
    * Destructor.
    */
   ~RoiStatisticsWidget();

   void setImageData(std::unordered_map<std::string, data_struct::ArrayXXr<float>>& img_data);
  
   void clear_all();

   void append_roi(gstar::RoiMaskGraphicsItem* roi);

public slots:

	void onExport();

	void onClose();

protected:

   void _createLayout();

private:
/*
	QTabWidget* _techTabs;

	QPushButton* _runBtn;
	QPushButton* _acceptBtn;
	QPushButton* _cancelBtn;
  
	QListView* _img_names_view;
	QStandardItemModel* _img_list_model;
	QCheckBox* _chk_normalize_sum;
	
	ImageSegWidget* _int_img_widget;

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

	int _next_color;
	*/
};


/*---------------------------------------------------------------------------*/

#endif /* RoiStatisticsWidget.h */

/*---------------------------------------------------------------------------*/

