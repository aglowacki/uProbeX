/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef IMG_SEG_WIDGET_H
#define IMG_SEG_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <gstar/AbstractImageWidget.h>
#include "gstar/Annotation/RoiMaskGraphicsItem.h"

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
   virtual ~ImageSegWidget();

   /**
    * @brief widgetChanged
    */
   void widgetChanged(bool enable);

   void setPixMap(QPixmap pix);

   void setImageFromArray(ArrayXXr<float>& img_arr, QVector<QRgb>& colormap);

   void clearAllRoiMasks();

   void addRoiMask(gstar::RoiMaskGraphicsItem* roi);

   std::vector<gstar::RoiMaskGraphicsItem*> getAllROIs();

   int getROIsCount();

   void setActionMode(gstar::DRAW_ACTION_MODES mode);

   void setRoiBrushSize(int val);

   void invertSelectedRoiMask();

public slots:

   /**
    * @brief windowChanged: Accept the window change state
    * @param oldState
    * @param newState
    */
   void windowChanged(Qt::WindowStates oldState, Qt::WindowStates newState);

protected slots:

    void currentRoiChanged(const QModelIndex& current, const QModelIndex& previous);

protected:

   /**
    * @brief Create layout
    */
   void createLayout();

private:     

    gstar::DRAW_ACTION_MODES _draw_action_mode;

    bool _first_pixmap_set;

    gstar::RoiMaskGraphicsItem* _selected_roi;

    QSize _roi_brush_size;

    bool _mouse_down;
};


/*---------------------------------------------------------------------------*/

#endif /* ImageSegWidget_H_ */

/*---------------------------------------------------------------------------*/

