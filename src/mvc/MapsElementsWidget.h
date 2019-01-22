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

class HDF5PropertyWidget;
class QAbstractTableModel;

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
   MapsElementsWidget(QWidget* parent = nullptr);

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

   void displayCounts(std::string analysis_type, std::string element);

   void onAnalysisSelect(QString name);

   void onElementSelect(QString name);

   void model_updated();

protected:

   /**
    * @brief Create layout
    */
   void createLayout();

   std::unordered_map<std::string, std::unordered_map<std::string, gstar::ImageViewWidget*> > _imagesWidgets;

   MapsH5Model *_model;

   FitSpectraWidget* _spectra_widget;

   QComboBox *_cb_analysis;

   QComboBox *_cb_element;

   QTabWidget *_tab_widget;

   QVector<QRgb> _grayscale;
};


/*---------------------------------------------------------------------------*/

#endif /* MapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/
