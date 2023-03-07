/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef IMAGE_SEGMENT_WIDGET_H
#define IMAGE_SEGMENT_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <gstar/AbstractImageWidget.h>
#include <QTableWidget>
#include <QComboBox>
#include <QSplitter>
#include <mvc/MapsH5Model.h>
#include <mvc/FitSpectraWidget.h>
#include "gstar/ImageViewWidget.h"
#include "gstar/Annotation/RoiMaskGraphicsItem.h"
#include "preferences/Preferences.h"


/*---------------------------------------------------------------------------*/

class CoLocalizationWidget : public gstar::AbstractImageWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   CoLocalizationWidget(QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   virtual ~CoLocalizationWidget();

   void setModel(MapsH5Model* h5_model);

   MapsH5Model *getModel(){return _model;}

public slots:

	void redrawCounts();

    void windowChanged(Qt::WindowStates oldState, Qt::WindowStates newState);

    void displayCounts(const std::string analysis_type, const std::string element, bool log_color, int grid_idx = 0);

    void onNewGridLayout(int rows, int cols);

protected:

   /**
    * @brief Create layout
    */
   void _createLayout();

   virtual void createActions();

   virtual void displayContextMenu(QWidget* parent, const QPoint& pos);

   MapsH5Model *_model;

   QLabel *_dataset_directory;

   QLabel *_dataset_name;

   QPushButton *_btn_export_as_image;

   QWidget* _counts_window;

   QComboBox* _cb_red_element;

   QComboBox* _cb_green_element;

   QComboBox* _cb_blue_element;

};


/*---------------------------------------------------------------------------*/

#endif /* CoLocalizationWidget_H_ */

/*---------------------------------------------------------------------------*/

