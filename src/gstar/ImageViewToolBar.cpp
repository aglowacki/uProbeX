/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/ImageViewToolBar.h>
#include <gstar/ImageViewWidget.h>

#include<QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QComboBox>

using namespace gstar;

//---------------------------------------------------------------------------

ImageViewToolBar::ImageViewToolBar(ImageViewWidget* widget)
{
   connect(widget, SIGNAL(resetZoomToolBar()), this, SLOT(resetZoomToolBar()));

   m_toolbar = new QToolBar();
   m_cursorAction = new QAction(QIcon(":images/cursor.png"),
                                tr("Cursor"), 
                                nullptr);
   m_cursorAction->setCheckable(true);
   m_cursorAction->setChecked(true);
   connect(m_cursorAction, SIGNAL(triggered()), this, SLOT(clickCursor()));

   m_zoomInAction = new QAction(QIcon(":/images/zoomin.png"),
                                tr("Zoom In"), this);
   m_zoomInAction -> setCheckable(true);
   connect(m_zoomInAction, SIGNAL(triggered()), this, SLOT(clickZoomIn()));

   // Toolbar zoom out action
   m_zoomOutAction = new QAction(QIcon(":/images/zoomout.png"),
                                 tr("Zoom Out"), this);
   m_zoomOutAction -> setCheckable(true);
   connect(m_zoomOutAction, SIGNAL(triggered()), this, SLOT(clickZoomOut()));

   // Toolbar zoom to original image.
   m_zoomOriginalAction = new QAction(QIcon(":/images/zoomoriginal.png"),
                                       tr("Actual Size"), this);
   connect(m_zoomOriginalAction, SIGNAL(triggered()), this,
          SLOT(clickZoomOriginal()));

   // Toolbar fill action
   m_fillAction = new QAction(QIcon(":/images/fill.png"),
                              tr("Fill"), this);
   m_fillAction -> setCheckable(true);
   connect(m_fillAction, SIGNAL(triggered()), this, SLOT(clickFill()));

   QLabel* m_lbl = new QLabel("Zoom (%) :");
   m_zoomPercent = new QComboBox();
   m_zoomPercent->setEditable(true);

   m_imageWidget.push_back(widget);

   for(auto &itr : m_imageWidget)
        itr->setZoomPercentWidget(m_zoomPercent);

   m_zoomSettings.push_front(12.5);
   m_zoomSettings.push_front(25);
   m_zoomSettings.push_front(50);
   m_zoomSettings.push_front(100);
   m_zoomSettings.push_front(200);
   m_zoomSettings.push_front(400);
   m_zoomSettings.push_front(800);

   for (int i=0; i<m_zoomSettings.size(); i++)
   {
      m_zoomPercent->addItem(QString::number(m_zoomSettings.at(i)));
   }

   m_zoomPercent->setCurrentIndex(3);
   m_zoomPercent->setMinimumSize(80, 10);

   m_zoomGroup = new QActionGroup(this);
   m_zoomGroup->addAction(m_cursorAction);
   m_zoomGroup->addAction(m_zoomInAction);
   m_zoomGroup->addAction(m_zoomOutAction);
   m_zoomGroup->addAction(m_zoomOriginalAction);
   
   m_toolbar->addAction(m_cursorAction);
   m_toolbar->addAction(m_zoomInAction);
   m_toolbar->addAction(m_zoomOutAction);
   m_toolbar->addAction(m_zoomOriginalAction);
   m_toolbar->addAction(m_fillAction);
   m_toolbar->addSeparator();
   m_toolbar->addWidget(m_lbl);
   m_toolbar->addWidget(m_zoomPercent);

   // Set the fill button checked initially
   m_cursorAction->setChecked(true);
   m_fillAction->setChecked(true);
   m_zoomGroup->setEnabled(false);
   m_zoomPercent->setEnabled(false);

} 

//---------------------------------------------------------------------------

ImageViewToolBar::~ImageViewToolBar()
{

   if(m_cursorAction != nullptr)
   {  
      delete m_cursorAction;
      m_cursorAction = nullptr;
   }

}

//---------------------------------------------------------------------------

void ImageViewToolBar::appendImageViewWidget(ImageViewWidget* widget)
{
    m_imageWidget.push_back(widget);

    for(auto &itr : m_imageWidget)
         itr->setZoomPercentWidget(m_zoomPercent);
}

//---------------------------------------------------------------------------

void ImageViewToolBar::clearImageViewWidgets()
{
    m_imageWidget.clear();
}

//---------------------------------------------------------------------------

void ImageViewToolBar::clickCursor()
{

    for(auto &itr : m_imageWidget)
        itr->clickCursor();

}

//---------------------------------------------------------------------------

void ImageViewToolBar::clickZoomIn()
{
   for(auto &itr : m_imageWidget)
        itr->clickZoomIn();

}

//---------------------------------------------------------------------------

void ImageViewToolBar::resetZoomToolBar()
{
   m_zoomGroup->setEnabled(true);
   m_zoomPercent->setEnabled(true);
   m_fillAction->setChecked(false);
   m_cursorAction->setChecked(true);
}

//---------------------------------------------------------------------------

void ImageViewToolBar::clickFill()
{
   // Make tool bar button look checked
   m_cursorAction->setChecked(true);
   
   for(auto &itr : m_imageWidget)
        itr->clickFill(m_fillAction->isChecked());

   // Set scene mode
   if (m_fillAction->isChecked()) {
      m_zoomGroup->setEnabled(false);
      m_zoomPercent->setEnabled(false);
   }
   else {
      m_zoomGroup->setEnabled(true);
      m_zoomPercent->setEnabled(true);
   }

}

//---------------------------------------------------------------------------

void ImageViewToolBar::clickZoomOriginal()
{

    for(auto &itr : m_imageWidget)
        itr->clickZoomOriginal();

}

//---------------------------------------------------------------------------

void ImageViewToolBar::clickZoomOut()
{

    for(auto &itr : m_imageWidget)
        itr->clickZoomOut();

}

//---------------------------------------------------------------------------

QToolBar* ImageViewToolBar::getToolBar()
{

   return m_toolbar;

}

//---------------------------------------------------------------------------
