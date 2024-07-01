/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_IMAGE_VIEW_TOOL_BAR_H
#define GSTAR_IMAGE_VIEW_TOOL_BAR_H

//---------------------------------------------------------------------------

#include <vector>
#include <QObject>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QComboBox>

//---------------------------------------------------------------------------

namespace gstar
{

class ImageViewWidget;

class ImageViewToolBar
: public QObject
{
   Q_OBJECT

public:

   ImageViewToolBar(ImageViewWidget* m_widget);

   ~ImageViewToolBar();

   void appendImageViewWidget(ImageViewWidget* widget);

   void clearImageViewWidgets();

   QToolBar* getToolBar();

public slots:

   /**
    * Handle click on cursor tool bar icon
    */
   void clickCursor();

   /**
    * Handle click on zoom in tool bar icon. Switch to zoom in mode.
    */
   void clickZoomIn();

   /**
    * Handle click on fill tool bar icon. Switch to fill mode.
    */
   void clickFill();
   /**
    * Handle click on zoom back to original tool bar icon.
    */
   void clickZoomOriginal();

   /**
    * Handle click on zoom out tool bar icon. Switch to zoom out mode.
    */
   void clickZoomOut();

   /*
    * Slot performs a resets zoom toolbar to default cursor state.
    */
   void resetZoomToolBar();

private:

   QToolBar* m_toolbar;

   /**
    * Cursor
    */
   QAction* m_cursorAction;

   /**
    * Zoom in
    */
   QAction* m_zoomInAction;

   /**
    * Zoom out
    */
   QAction* m_zoomOutAction;

   /**
    * Zoom to original
    */
   QAction* m_zoomOriginalAction;

   /**
    * Fill
    */
   QAction* m_fillAction;

   /**
    * Group for zoom in, out and fill tools
    */
   QActionGroup* m_zoomGroup;

   /**
    * m_imageWidget
    */
   std::vector<ImageViewWidget*> m_imageWidget;

   /**
    * Current image zoom value.
    */
   QComboBox* m_zoomPercent;

   /**
    * Zoom settings
    */
   QList<float> m_zoomSettings;

};

}

#endif
