/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MULTI_ELEMENTS_WIDGET_H
#define MULTI_ELEMENTS_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <memory>
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

/*---------------------------------------------------------------------------*/

/**
 * @brief When open the acquisition window, the widget is showing for capturing
 * the image from the area detector writer, the window will also be updated to
 * show the image.
 */
class MultiElementsWidget : public gstar::AbstractImageWidget
{

   Q_OBJECT

public:

   MultiElementsWidget(int rows = 1, int cols = 1, QWidget* parent = nullptr);

   ~MultiElementsWidget();

   void setModel(std::shared_ptr<MapsH5Model> h5_model);

   std::shared_ptr<MapsH5Model> getModel(){return _model;}

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

   void onSelectNormalizer(QString name);

protected:

   /**
    * @brief Create layout
    */
   void _createLayout();

   virtual void createActions();

   virtual void displayContextMenu(QWidget* parent, const QPoint& pos);

   std::shared_ptr<MapsH5Model> _model;

   QComboBox *_cb_analysis;

   QComboBox *_cb_colormap;

   QVector<QRgb> *_selected_colormap;

   QVector<QRgb> _gray_colormap;

   QVector<QRgb> _heat_colormap;

   QAction *_addHotSpotMaskAction;
   
   QPushButton * _grid_button;

   ImageGridDialog iDiag;

   QComboBox* _cb_normalize;

   data_struct::ArrayXXr* _normalizer;

   Calibration_curve* _calib_curve;

   unordered_map<string, data_struct::Spectra> _roi_spectra;

   //QTableWidget* _scaler_table_widget;

};


/*---------------------------------------------------------------------------*/

#endif /* MultiElementsWidget_H_ */

/*---------------------------------------------------------------------------*/

