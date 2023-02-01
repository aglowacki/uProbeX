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

enum class ROI_ACTION_MODES { OFF, ADD, ERASE };

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

   void clearAllRoiMasks();

   void addRoiMask(gstar::RoiMaskGraphicsItem* roi);

   std::vector<gstar::RoiMaskGraphicsItem*> getAllROIs();

   void setActionMode(ROI_ACTION_MODES mode) { _action_mode = mode; }

   void setRoiBrushSize(int val);

public slots:

   /**
    * @brief windowChanged: Accept the window change state
    * @param oldState
    * @param newState
    */
   void windowChanged(Qt::WindowStates oldState, Qt::WindowStates newState);

protected slots:

    void mouseOverPixel(int x, int y);

    void mousePressEvent(QGraphicsSceneMouseEvent*);

    void mouseReleaseEvent(QGraphicsSceneMouseEvent*);

    void currentRoiChanged(const QModelIndex& current, const QModelIndex& previous);

protected:

   /**
    * @brief Create layout
    */
   void createLayout();

private:     

    ROI_ACTION_MODES _action_mode;

    bool _first_pixmap_set;

    gstar::RoiMaskGraphicsItem* _selected_roi;

    QSize _roi_brush_size;

    bool _mouse_down;
};


/*---------------------------------------------------------------------------*/

#endif /* ImageSegWidget_H_ */

/*---------------------------------------------------------------------------*/

