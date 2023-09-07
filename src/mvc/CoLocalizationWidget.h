/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef COLOCALIZATION_WIDGET_H
#define COLOCALIZATION_WIDGET_H

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

    void windowChanged(Qt::WindowStates oldState, Qt::WindowStates newState);

    void onNewGridLayout(int rows, int cols);

    void onSetAnalysisType(QString name);

    void onColorSelected(QString);

    void onQuadViewChanged(int);

    void onExportPng();

protected:

   /**
    * @brief Create layout
    */
   void _createLayout();

   MapsH5Model *_model;

   QLabel *_dataset_directory;

   QLabel *_dataset_name;

   QPushButton *_btn_export_as_image;

   QWidget* _counts_window;

   QComboBox* _cb_red_element;

   QComboBox* _cb_green_element;

   QComboBox* _cb_blue_element;

   QCheckBox* _ck_quad_view;

   QString _curAnalysis;

   data_struct::Fit_Count_Dict<float> _fit_counts;

   QVector<QRgb> _red_colormap;

   QVector<QRgb> _green_colormap;

   QVector<QRgb> _blue_colormap;
   
   bool _first_pixmap_set;
};


/*---------------------------------------------------------------------------*/

#endif /* CoLocalizationWidget_H_ */

/*---------------------------------------------------------------------------*/

