/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_ELEMENTS_WIDGET_H
#define MAPS_ELEMENTS_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <gstar/AbstractImageWidget.h>

#include <mvc/MapsH5Model.h>
#include <mvc/FitSpectraWidget.h>
#include <QComboBox>
#include <unordered_map>
#include <gstar/CoordinateWidget.h>
#include <gstar/CountsLookupTransformer.h>
#include "gstar/Annotation/HotSpotMaskGraphicsItem.h"
#include "mvc/ImageGridDialog.h"

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

   //void setModel(MapsH5Model* model);
   void setModel(MapsH5Model* h5_model,
                 data_struct::Fit_Parameters* fit_params,
                 data_struct::Fit_Element_Map_Dict *elements_to_fit);

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

   void onAnalysisSelect(QString name);

   void onElementSelect(QString name, int viewIdx = 0);

   void onColormapSelect(QString name);

   void model_updated();

   void addHotSpotMask();

   void hotspotUpdated();

   void onGridDialog();

   void onNewGridLayout(int rows, int cols);

protected:

   /**
    * @brief Create layout
    */
   void _createLayout();

   void _get_min_max_vals(float &min_val, float &max_val, const Eigen::Array<real_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& element_counts);

   virtual void createActions();

   virtual void displayContextMenu(QWidget* parent, const QPoint& pos);

   std::unordered_map<std::string, std::unordered_map<std::string, gstar::ImageViewWidget*> > _imagesWidgets;

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

  // gstar::CoordinateWidget* _counts_coord_widget;

   //gstar::CoordinateModel *_counts_coord_model;

   //gstar::CountsLookupTransformer *_counts_lookup;

   QAction *_addHotSpotMaskAction;
   
   QPushButton * _grid_button;

   ImageGridDialog iDiag;

};


/*---------------------------------------------------------------------------*/

#endif /* MapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/
