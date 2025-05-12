/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_ELEMENTS_WIDGET_H
#define MAPS_ELEMENTS_WIDGET_H

//---------------------------------------------------------------------------

#include <gstar/AbstractImageWidget.h>

#include <QTableWidget>
#include <QAbstractTableModel>
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
#include <mvc/CoLocalizationWidget.h>
#include <mvc/ImageSegROIDialog.h>
#include <mvc/ScatterPlotWidget.h>
#include <mvc/QuantificationWidget.h>
#include <mvc/RoiStatisticsWidget.h>
#include <gstar/MotorLookupTransformer.h>

using gstar::AbstractGraphicsItem;
//---------------------------------------------------------------------------

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
   MapsElementsWidget(int rows = 1, int cols = 1, bool create_image_nav=false, bool restore_floating=true, QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   virtual ~MapsElementsWidget();

   void setModel(MapsH5Model* h5_model);

   MapsH5Model *getModel(){return _model;}

   void setTabVisible(int idx, bool val) { _tab_widget->setTabVisible(idx, val);}

signals:
   void loaded_perc(int, int);

   void new_rois(QString, int);

public slots:

	void redrawCounts();

   void windowChanged(Qt::WindowStates oldState, Qt::WindowStates newState);

   void displayCounts(const std::string analysis_type, const std::string element, bool log_color, int grid_idx = 0);

   QPixmap generate_pixmap(const std::string analysis_type, const std::string element, bool log_color, int grid_idx);

   void onAnalysisSelect(QString name);

   void onElementSelect(QString name, int viewIdx = 0);

   void onColormapSelect(QString name);

   void model_updated();

   void openImageSegDialog();

   void openRoiStatsWidget();

   void roiUpdated(gstar::RoiMaskGraphicsItem* ano, bool reload);

   void onGridDialog();
   
   void onNewGridLayout(int rows, int cols);

   void on_export_fit_params(data_struct::Fit_Parameters<double> fit_params, data_struct::Fit_Element_Map_Dict<double> elements_to_fit);

   void onSelectNormalizer(QString name);

   void on_export_csv_and_png(QPixmap, ArrayDr*, ArrayDr*, ArrayDr*, ArrayDr*, std::unordered_map<std::string, ArrayDr>*);

   void on_min_max_contrast_changed();

   void on_global_contrast_changed(int);

   void on_log_color_changed(int);

   void on_invert_y_axis(int);

   void on_export_image_pressed();

   void on_export_images();

   void on_add_new_ROIs(std::vector<gstar::RoiMaskGraphicsItem*> roi_list);

   void on_delete_all_annotations(QString);

   void on_delete_annotation(QString, QString);

   void roiModelDataChanged(const QModelIndex& , const QModelIndex& );

   void roiTreeContextMenu(const QPoint&);

   void displayRoiContextMenu(QWidget* , const QPoint&);

   void annoTabChanged(int);

   void savePref();

   void onDockFloatChanged(bool floating);

   void plotPixelSpectra(const QPoint& pos);

protected:

   /**
    * @brief Create layout
    */
   void _createLayout(bool create_image_nav, bool restore_floating);

   virtual void createActions();

   virtual void displayContextMenu(QWidget* parent, const QPoint& pos);

   void _appendRoiTab();

   void _model_custom_spectra(const std::string& analysis_name, const std::vector<std::pair<int, int>>& pixel_list, ArrayDr* spec);

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

   QAction* _addKMeansRoiAction;
   
   QPushButton * _grid_button;

   ImageGridDialog iDiag;

   QComboBox* _cb_normalize;

   const data_struct::ArrayXXr<float>* _normalizer;

   Calibration_curve<double>* _calib_curve;

   std::unordered_map<std::string, data_struct::Spectra<double>> _roi_spectra;

   QTableWidget* _extra_pvs_table_widget;

   data_struct::Spectra<double> _int_spec;
   //QTableWidget* _scaler_table_widget;

   gstar::MinMaxSlider* _contrast_widget;

   QCheckBox* _global_contrast_chk;

   QCheckBox* _chk_log_color;

   QCheckBox* _chk_disp_color_ledgend;

   QCheckBox* _chk_invert_y;

   QPushButton *_btn_export_as_image;

   ExportMapsDialog* _export_maps_dialog;

   QWidget* _counts_window;

   QImage* _color_maps_ledgend;

   QLabel *_color_map_ledgend_lbl;

   QuantificationWidget* _quant_widget;

   CoLocalizationWidget* _co_loc_widget;

   ScatterPlotWidget* _scatter_plot_widget;

   ImageSegRoiDialog _img_seg_diag;

   RoiStatisticsWidget* _roi_stats_diag;

   float _min_contrast_perc;

   float _max_contrast_perc;

   QTreeView* m_roiTreeView;

   gstar::AnnotationTreeModel* m_roiTreeModel;

   QItemSelectionModel* m_roiSelectionModel;

   QWidget* m_roiTreeTabWidget;

   QPushButton* _btn_roi_img_seg;

   QPushButton* _btn_roi_stats;

   QDockWidget* _counts_dock;

   QDockWidget* _intspectra_dock;

   QDockWidget* _quant_dock;

   QDockWidget* _coloc_dock;

   QDockWidget* _scatter_dock;

   QDockWidget* _extra_dock;

   std::map< QString, QDockWidget*> _dockMap;

   gstar::MotorLookupTransformer _motor_trans;

   QTabWidget* _tw_image_controls;

   QAction* _plotPixelSpectraAction;

};


//---------------------------------------------------------------------------

#endif /* MapsElementsWidget_H_ */

//---------------------------------------------------------------------------

