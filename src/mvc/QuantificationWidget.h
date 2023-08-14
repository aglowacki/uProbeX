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

   MapsH5Model *_model;

};


/*---------------------------------------------------------------------------*/

#endif /* QuantificationWidget_H_ */

/*---------------------------------------------------------------------------*/

