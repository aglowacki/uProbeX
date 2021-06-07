/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MULTI_LAYER_WIDGET_H
#define MULTI_LAYER_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <QComboBox>
#include <gstar/AbstractImageWidget.h>

/*---------------------------------------------------------------------------*/

class Multi_Layer_Widget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   Multi_Layer_Widget(QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~Multi_Layer_Widget();


protected:

   void createLayout();

   gstar::ImageViewToolBar* m_imageWidgetToolBar;

   gstar::ImageViewWidget* m_imageViewWidget;

};


/*---------------------------------------------------------------------------*/

#endif /* MapsElementsWidget_H_ */

/*---------------------------------------------------------------------------*/