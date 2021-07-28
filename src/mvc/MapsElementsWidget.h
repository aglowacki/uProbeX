﻿/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_ELEMENTS_WIDGET_H
#define MAPS_ELEMENTS_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <QWidget>

#include <QTableWidget>
#include <mvc/MapsH5Model.h>
#include <mvc/FitSpectraWidget.h>
#include <QComboBox>
#include <unordered_map>
#include <gstar/CoordinateWidget.h>
#include <gstar/CountsLookupTransformer.h>
#include "gstar/Annotation/HotSpotMaskGraphicsItem.h"
#include "mvc/ImageGridDialog.h"
#include "preferences/Preferences.h"
#include "gstar/MinMaxSlider.h"

class HDF5PropertyWidget;
class QAbstractTableModel;
using gstar::AbstractGraphicsItem;
/*---------------------------------------------------------------------------*/

/**
 * @brief When open the acquisition window, the widget is showing for capturing
 * the image from the area detector writer, the window will also be updated to
 * show the image.
 */
class MapsElementsWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   MapsElementsWidget(int rows = 1, int cols = 1, bool create_image_nav=false, QWidget* parent = nullptr);

   ~MapsElementsWidget();

   void setModel(std::shared_ptr<MapsH5Model> h5_model);

   std::shared_ptr<MapsH5Model> getModel(){return _model;}

public slots:

	void redrawCounts();

   void windowChanged(Qt::WindowStates oldState, Qt::WindowStates newState);

   void displayCounts(const std::string analysis_type, const std::string element, int grid_idx = 0);

   QPixmap generate_pixmap(const std::string analysis_type, const std::string element, int grid_idx);

   void onAnalysisSelect(QString name);

   void onElementSelect(QString name, int viewIdx = 0);

   void onColormapSelect(QString name);

   void model_updated();

   void on_export_fit_params(data_struct::Fit_Parameters fit_params, data_struct::Fit_Element_Map_Dict elements_to_fit);

   void on_export_csv_and_png(QPixmap, data_struct::ArrayXr*, data_struct::ArrayXr*, data_struct::ArrayXr*, data_struct::ArrayXr*, unordered_map<string, data_struct::ArrayXr>*);

   void on_min_max_contrast_changed();

   void on_global_contrast_changed(int);

protected:

   /**
    * @brief Create layout
    */
   void _createLayout(bool create_image_nav);

   virtual void createActions();

   virtual void displayContextMenu(QWidget* parent, const QPoint& pos);

   std::shared_ptr<MapsH5Model> _model;

   FitSpectraWidget* _spectra_widget;

   QLabel *_dataset_directory;

   QLabel *_dataset_name;
  
   QTabWidget *_tab_widget;

   unordered_map<string, data_struct::Spectra> _roi_spectra;

   QTableWidget* _extra_pvs_table_widget;

   MultiElementsWidget* _multi_elements_widget;

   MultiLayerWidget* _multi_layer_widget;

   IntensityWidget* _intensity_widget;

   data_struct::Spectra _int_spec;
   //QTableWidget* _scaler_table_widget;

   gstar::MinMaxSlider* _contrast_widget;

   QCheckBox* _global_contrast_chk;

   real_t _min_contrast_perc;

   real_t _max_contrast_perc;
};


/*---------------------------------------------------------------------------*/

#endif /* MapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/

