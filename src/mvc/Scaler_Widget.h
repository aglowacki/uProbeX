/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SCALER_WIDGET_H
#define SCALER_WIDGET_H

//---------------------------------------------------------------------------

#include <QComboBox>
#include <QTableWidget>
#include <QLabel>
#include <mvc/RAW_Model.h>

//---------------------------------------------------------------------------

class Scaler_Widget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   Scaler_Widget(QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~Scaler_Widget();

   void setModel(RAW_Model* model);

   RAW_Model *getModel(){return _model;}

public slots:

   void onScalerSelect(const QString& det);

protected:

   void createLayout();

   QComboBox* _cb_scaler;

   QTableWidget* _scaler_table_widget;

   RAW_Model* _model;

};


//---------------------------------------------------------------------------

#endif /* SCALER_WIDGET_H */

//---------------------------------------------------------------------------