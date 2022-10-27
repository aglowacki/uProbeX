/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef IMAGE_SEGMENT_WIDGET_H
#define IMAGE_SEGMENT_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <gstar/AbstractImageWidget.h>

#include <opencv2/opencv.hpp>

#include <QTableWidget>
#include <QComboBox>
#include <QSplitter>
#include <mvc/MapsH5Model.h>
#include <mvc/FitSpectraWidget.h>
#include "gstar/ImageViewWidget.h"
#include "gstar/Annotation/RoiMaskGraphicsItem.h"
#include "preferences/Preferences.h"


/*---------------------------------------------------------------------------*/

class ImageSegmentWidget
: public gstar::AbstractImageWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   ImageSegmentWidget(int rows = 1, int cols = 1, bool create_image_nav=false, QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~ImageSegmentWidget();

   void setModel(MapsH5Model* h5_model);

   MapsH5Model *getModel(){return _model;}

public slots:

	void redrawCounts();

   void windowChanged(Qt::WindowStates oldState, Qt::WindowStates newState);

   void displayCounts(const std::string analysis_type, const std::string element, bool log_color, int grid_idx = 0);

   //QPixmap generate_pixmap(const std::string analysis_type, const std::string element, bool log_color, int grid_idx);

   //void onAnalysisSelect(QString name);

   //void onElementSelect(QString name, int viewIdx = 0);

   void addRoiMask();

   void roiUpdated(gstar::RoiMaskGraphicsItem* ano, bool reload);

   //void onGridDialog();

   void onNewGridLayout(int rows, int cols);

   /*
   void on_export_csv_and_png(QPixmap, ArrayDr*, ArrayDr*, ArrayDr*, ArrayDr*, unordered_map<string, ArrayDr>*);
   
   void on_export_image_pressed();

   void on_export_images();
   */

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

   QTabWidget *_tab_widget;

   QAction *_addRoiMaskAction;
   
   QPushButton * _grid_button;

   unordered_map<string, data_struct::Spectra<double>> _roi_spectra;

   data_struct::Spectra<double> _int_spec;

   QPushButton *_btn_export_as_image;

   QWidget* _counts_window;

};


/*---------------------------------------------------------------------------*/

#endif /* ImageSegmentWidget_H_ */

/*---------------------------------------------------------------------------*/

