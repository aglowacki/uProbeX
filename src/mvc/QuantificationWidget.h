/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef QUANTIFICATION_WIDGET_H
#define QUANTIFICATION_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <gstar/AbstractImageWidget.h>
#include <QTableWidget>
#include <QComboBox>
#include <QSplitter>
#include <QtCharts/QLineSeries>
#include <QtCharts/QCategoryAxis>
#include <mvc/MapsH5Model.h>
#include <mvc/FitSpectraWidget.h>
#include "gstar/ImageViewWidget.h"
#include "gstar/Annotation/RoiMaskGraphicsItem.h"
#include "preferences/Preferences.h"


/*---------------------------------------------------------------------------*/

class QuantificationWidget : public gstar::AbstractImageWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   QuantificationWidget(QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   virtual ~QuantificationWidget();

   void setModel(MapsH5Model* h5_model);

   MapsH5Model *getModel(){return _model;}

public slots:

    void windowChanged(Qt::WindowStates oldState, Qt::WindowStates newState);

protected:

   /**
    * @brief Create layout
    */
   void _createLayout();

   bool _display_log10;

   QLogValueAxis* _axisYLog10;

   QCategoryAxis* _axisX;

   QValueAxis* _axisY;

   QAbstractAxis* _currentYAxis;

   QChart* _chart;

   QChartView* _chartView;

   QLineSeries* _line_series;

   QComboBox* _cb_analysis_types;

   QComboBox* _cb_scalers;

   MapsH5Model *_model;

   Calibration_curve<double>* _calib_curve;

};


/*---------------------------------------------------------------------------*/

#endif /* QuantificationWidget_H_ */

/*---------------------------------------------------------------------------*/

