/*-----------------------------------------------------------------------------
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
#include "gstar/Annotation/RoiMaskGraphicsItem.h"
#include "mvc/ImageGridDialog.h"
#include "preferences/Preferences.h"
#include "gstar/MinMaxSlider.h"
#include <mvc/ExportMapsDialog.h>

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
   MapsElementsWidget(int rows = 1, int cols = 1, bool create_image_nav=false, QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~MapsElementsWidget();

   void setModel(MapsH5Model* h5_model);

   MapsH5Model *getModel(){return _model;}

public slots:

	void redrawCounts();

   void windowChanged(Qt::WindowStates oldState, Qt::WindowStates newState);

   void displayCounts(const std::string analysis_type, const std::string element, bool log_color, int grid_idx = 0);

   QPixmap generate_pixmap(const std::string analysis_type, const std::string element, bool log_color, int grid_idx);

   void onAnalysisSelect(QString name);

   void onElementSelect(QString name, int viewIdx = 0);

   void onColormapSelect(QString name);

   void model_updated();

   void addRoiMask();

   void roiUpdated(gstar::RoiMaskGraphicsItem* ano, bool reload);

   void onGridDialog();

   void onNewGridLayout(int rows, int cols);

   void on_export_fit_params(data_struct::Fit_Parameters<double> fit_params, data_struct::Fit_Element_Map_Dict<double> elements_to_fit);

   void onSelectNormalizer(QString name);

   void on_export_csv_and_png(QPixmap, ArrayDr*, ArrayDr*, ArrayDr*, ArrayDr*, unordered_map<string, ArrayDr>*);

   void on_min_max_contrast_changed();

   void on_global_contrast_changed(int);

   void on_log_color_changed(int);

   void on_export_image_pressed();

   void on_export_images();

protected:

   /**
    * @brief Create layout
    */
   void _createLayout(bool create_image_nav);

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

   QAction *_addRoiMaskAction;
   
   QPushButton * _grid_button;

   ImageGridDialog iDiag;

   QComboBox* _cb_normalize;

   data_struct::ArrayXXr<float>* _normalizer;

   Calibration_curve<double>* _calib_curve;

   unordered_map<string, data_struct::Spectra<double>> _roi_spectra;

   QTableWidget* _extra_pvs_table_widget;

   data_struct::Spectra<double> _int_spec;
   //QTableWidget* _scaler_table_widget;

   gstar::MinMaxSlider* _contrast_widget;

   QCheckBox* _global_contrast_chk;

   QCheckBox* _chk_log_color;

   QPushButton *_btn_export_as_image;

   ExportMapsDialog* _export_maps_dialog;

   QWidget* _counts_window;

   float _min_contrast_perc;

   float _max_contrast_perc;
};


/*---------------------------------------------------------------------------*/

#endif /* MapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/

