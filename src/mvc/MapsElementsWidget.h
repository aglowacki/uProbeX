﻿/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_ELEMENTS_WIDGET_H
#define MAPS_ELEMENTS_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <gstar/AbstractImageWidget.h>

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
#include "mvc/Multi_Layer_Widget.h"
#include "mvc/Intensity_Widget.h"

class HDF5PropertyWidget;
class QAbstractTableModel;
using gstar::AbstractGraphicsItem;
/*---------------------------------------------------------------------------*/

/**
 * @brief When open the acquisition window, the widget is showing for capturing
 * the image from the area detector writer, the window will also be updated to
 * show the image.
 */
class MapsElementsWidget
: public gstar::AbstractImageWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   MapsElementsWidget(int rows = 1, int cols = 1, QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~MapsElementsWidget();

   void setModel(MapsH5Model* h5_model);

   MapsH5Model *getModel(){return _model;}

   void redrawCounts();

public slots:

   /**
    * @brief Accept the window change state
    * @param oldState
    * @param newState
    */
   void windowChanged(Qt::WindowStates oldState, Qt::WindowStates newState);

   void displayCounts(const std::string analysis_type, const std::string element, int grid_idx = 0);

   QPixmap generate_pixmap(const std::string analysis_type, const std::string element, int grid_idx);

   void onAnalysisSelect(QString name);

   void onElementSelect(QString name, int viewIdx = 0);

   void onColormapSelect(QString name);

   void model_updated();

   void addHotSpotMask();

   void roiUpdated(gstar::HotSpotMaskGraphicsItem* ano, bool reload);

   void onGridDialog();

   void onNewGridLayout(int rows, int cols);

   void on_export_fit_params(data_struct::Fit_Parameters fit_params, data_struct::Fit_Element_Map_Dict elements_to_fit);

   void onSelectNormalizer(QString name);

   void on_export_csv_and_png(QPixmap, data_struct::ArrayXr*, data_struct::ArrayXr*, data_struct::ArrayXr*, data_struct::ArrayXr*, unordered_map<string, data_struct::ArrayXr>*);

protected:

   /**
    * @brief Create layout
    */
   void _createLayout();

   virtual void createActions();

   virtual void displayContextMenu(QWidget* parent, const QPoint& pos);

   MapsH5Model *_model;

   FitSpectraWidget* _spectra_widget;

   QLabel *_dataset_directory;

   QLabel *_dataset_name;

   QComboBox *_cb_analysis;

   QComboBox *_cb_colormap;

   QTabWidget *_tab_widget;

   QVector<QRgb> *_selected_colormap;

   QVector<QRgb> _gray_colormap;

   QVector<QRgb> _heat_colormap;

   QPushButton *_pb_perpixel_fitting;

   QAction *_addHotSpotMaskAction;
   
   QPushButton * _grid_button;

   ImageGridDialog iDiag;

   QComboBox* _cb_normalize;

   data_struct::ArrayXXr* _normalizer;

   Calibration_curve* _calib_curve;

   unordered_map<string, data_struct::Spectra> _roi_spectra;

   QTableWidget* _extra_pvs_table_widget;

   Multi_Layer_Widget* _multi_layer_widget;

   Intensity_Widget* _intensity_widget;

   //QTableWidget* _scaler_table_widget;

};


/*---------------------------------------------------------------------------*/

#endif /* MapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/

