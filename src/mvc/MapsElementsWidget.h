/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_ELEMENTS_WIDGET_H
#define MAPS_ELEMENTS_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <AbstractImageWidget.h>

#include <mvc/MapsH5Model.h>
#include <mvc/SpectraWidget.h>
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
   MapsElementsWidget(QWidget* parent = NULL);

   /**
    * Destructor.
    */
   ~MapsElementsWidget();

   void setModel(MapsH5Model* model);

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

protected:

   /**
    * @brief Create layout
    */
   void createLayout();

   std::unordered_map<std::string, std::unordered_map<std::string, gstar::ImageViewWidget*> > _imagesWidgets;

   MapsH5Model *_model;

   SpectraWidget* _spectra_widget;

   QComboBox *_cb_analysis;

   QComboBox *_cb_element;

};


/*---------------------------------------------------------------------------*/

#endif /* MapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/
