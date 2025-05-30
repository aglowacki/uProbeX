/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef Roi_Statistics_Widget_H
#define Roi_Statistics_Widget_H

//---------------------------------------------------------------------------

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "data_struct/fit_parameters.h"
#include "gstar/Annotation/RoiMaskGraphicsItem.h"
#include "core/defines.h"
#include "data_struct/element_info.h"
#include "mvc/MapsH5Model.h"

//---------------------------------------------------------------------------

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

   void setData(QDir model_dir,
					QString dataset_name,
               QString fitting_name, 
					QString normalizer_name,
               float sq_area,
               std::unordered_map<std::string, data_struct::ArrayXXr<float>>& img_data,
               std::vector<gstar::RoiMaskGraphicsItem*>& roi_list,
               const data_struct::ArrayXXr<float>* normalizer,
               const Calibration_curve<double>* calib_curve);
  
   void clear_all();

   void append_roi(gstar::RoiMaskGraphicsItem* roi);

public slots:

	void onExport();

	void onClose();

protected:

   void _createLayout();

private:

   void _insert_item(QString roiName,
                     QString imgName, 
                     float sq_area,
                     const data_struct::ArrayXXr<float>& img,
                     const std::vector<std::pair<int, int>>& roi_pixels, 
                     int i,
                     const data_struct::ArrayXXr<float>* normalizer,
                     const Calibration_curve<double>* calib_curve);

   QString _str_export_filename;

   QDir _export_dir;

	QTableWidget* _table_widget;

	QPushButton* _btn_export;

	QPushButton* _btn_close;

	QStringList _base_header;

	std::vector<std::string> _element_lines_list;

	std::vector<std::string> _scalers_to_add_first_list;

	std::vector<std::string> _final_counts_to_add_before_scalers;
};


//---------------------------------------------------------------------------

#endif /* RoiStatisticsWidget.h */

//---------------------------------------------------------------------------

