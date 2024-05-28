/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef Roi_Statistics_Widget_H
#define Roi_Statistics_Widget_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "data_struct/fit_parameters.h"
#include "gstar/Annotation/RoiMaskGraphicsItem.h"
#include "core/defines.h"
#include "data_struct/element_info.h"

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

   void setData(std::unordered_map<std::string, data_struct::ArrayXXr<float>>& img_data, std::vector<gstar::RoiMaskGraphicsItem*>& roi_list);
  
   void clear_all();

   void append_roi(gstar::RoiMaskGraphicsItem* roi);

public slots:

	void onExport();

	void onClose();

protected:

   void _createLayout();

private:

	QTableWidget* _table_widget;

	QPushButton* _btn_export;

	QPushButton* _btn_close;

	QStringList _base_header;

	std::vector<std::string> _element_lines_list;

	std::vector<std::string> _scalers_to_add_first_list;

	std::vector<std::string> _final_counts_to_add_before_scalers;
};


/*---------------------------------------------------------------------------*/

#endif /* RoiStatisticsWidget.h */

/*---------------------------------------------------------------------------*/

