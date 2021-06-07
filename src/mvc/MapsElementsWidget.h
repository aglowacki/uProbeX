/*-----------------------------------------------------------------------------
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
#include "mvc/MultiElementsWidget.h"
#include "mvc/MultiLayerWidget.h"
#include "mvc/IntensityWidget.h"

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

   MapsElementsWidget(QWidget* parent = nullptr);

   ~MapsElementsWidget();

   void setModel(std::shared_ptr<MapsH5Model> h5_model);

   std::shared_ptr<MapsH5Model> getModel(){return _model;}

   MultiElementsWidget* multi_element_widget() const { return _multi_elements_widget; }

public slots:

   void model_updated();

   void on_export_fit_params(data_struct::Fit_Parameters fit_params, data_struct::Fit_Element_Map_Dict elements_to_fit);

   void on_export_csv_and_png(QPixmap, data_struct::ArrayXr*, data_struct::ArrayXr*, data_struct::ArrayXr*, data_struct::ArrayXr*, unordered_map<string, data_struct::ArrayXr>*);

protected:

   /**
    * @brief Create layout
    */
   void _createLayout();

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

   //QTableWidget* _scaler_table_widget;

};


/*---------------------------------------------------------------------------*/

#endif /* MapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/

