/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MDA_WIDGET_H
#define MDA_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <QComboBox>
#include <QTableWidget>
#include <gstar/AbstractImageWidget.h>
#include <mvc/FitSpectraWidget.h>
#include <mvc/Scaler_Widget.h>

/*---------------------------------------------------------------------------*/

class MDA_Widget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   MDA_Widget(QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~MDA_Widget();

   void setModel(std::shared_ptr<RAW_Model> model);

   std::shared_ptr<RAW_Model> getModel(){return _model;}

public slots:

   void model_updated();

   void onDetectorSelect(const QString& det);

   void onDetectorSelectIdx(int idx);

   void on_export_csv(QPixmap, data_struct::ArrayXr*, data_struct::ArrayXr*, data_struct::ArrayXr*, data_struct::ArrayXr*, unordered_map<string, data_struct::ArrayXr>*);

   //void onScalerSelect(const QString& det);

   void on_export_fit_params(data_struct::Fit_Parameters fit_params, data_struct::Fit_Element_Map_Dict elements_to_fit);

protected:

   void createLayout();

   FitSpectraWidget* _spectra_widget;

   QTabWidget* _tab_widget;

   std::shared_ptr<RAW_Model> _model;

   QComboBox* _cb_detector;

   //QComboBox* _cb_scaler;

   //QTableWidget* _scaler_table_widget;

   Scaler_Widget* _scaler_widget;

   QTableWidget* _extra_pvs_table_widget;

};


/*---------------------------------------------------------------------------*/

#endif /* MapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/