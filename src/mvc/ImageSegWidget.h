/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef IMG_SEG_WIDGET_H
#define IMG_SEG_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <gstar/AbstractImageWidget.h>

/*---------------------------------------------------------------------------*/

/**
 * @brief Widget used to display VLM workspaces. Used with VLM_Model.
 */
class ImageSegWidget
: public gstar::AbstractImageWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   ImageSegWidget(QWidget* parent = nullptr);


   /**
    * Destructor.
    */
   ~ImageSegWidget();

   /**
    * @brief widgetChanged
    */
   void widgetChanged(bool enable);

   void setPixMap(QPixmap pix);

public slots:

   /**
    * @brief windowChanged: Accept the window change state
    * @param oldState
    * @param newState
    */
   void windowChanged(Qt::WindowStates oldState, Qt::WindowStates newState);

protected slots:


protected:

   /**
    * @brief Create layout
    */
   void createLayout();

private:     

    bool _first_pixmap_set;
};


/*---------------------------------------------------------------------------*/

#endif /* ImageSegWidget_H_ */

/*---------------------------------------------------------------------------*/

