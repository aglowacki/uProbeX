/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ANNOTATIONTOOLBARWIDGET_H
#define ANNOTATIONTOOLBARWIDGET_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QAction>
#include <QToolBar>
#include <QCheckBox>

/*---------------------------------------------------------------------------*/

namespace gstar
{

/**
 * @brief The AnnotationToolBarWidget class is another way of adding
 *          AbstractGraphicsItems to the display.
 */
class AnnotationToolBarWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * @brief AnnotationToolBarWidget : Constructor.
    * @param parent : Widget parent.
    */
   AnnotationToolBarWidget(QWidget* parent = 0);
   
   /**
    * Deconstructor.
    */
   ~AnnotationToolBarWidget();

   /**
    * @brief getToolBar : Return a toobar widget.
    * @return
    */
   QWidget* getToolBar();

public slots:

   /**
    * @brief setEnabled
    * @param state
    */
   void setActionsEnabled(int state);

signals:

   /**
    * @brief clickCrossHair
    */
   void clickCrossHair();

   /**
    * @brief clickIntensityLine
    */
   void clickIntensityLine();

   /**
    * @brief clickIntensityPie
    */
   void clickIntensityPie();

   /**
    * @brief clickMarker
    */
   void clickMarker();

   /**
    * @brief clickMarker
    */
   void clickUProbeMarker();

   /**
    * @brief clickRuler
    */
   void clickRuler();

   /**
    * @brief enabledStateChanged
    * @param state
    */
   void enabledStateChanged(bool state);

private:

   /**
    * @brief m_crossHairAction
    */
   QAction* m_crossHairAction;

   /**
    * @brief m_intensityLineAction
    */
   QAction* m_intensityLineAction;

   /**
    * @brief m_intensityPieAction
    */
   QAction* m_intensityPieAction;

   /**
    * @brief m_markerAction
    */
   QAction* m_markerAction;

   /**
    * @brief m_rulerAction
    */
   QAction* m_rulerAction;

   /**
    * @brief m_uProbeMarkerAction
    */
   QAction* m_uProbeMarkerAction;

   /**
    * @brief m_toolbar
    */
   QToolBar* m_toolbar;

   /**
    * @brief m_chkSetVisible
    */
   QCheckBox* m_chkSetVisible;

};

}

/*---------------------------------------------------------------------------*/

#endif // ANNOTATIONTOOLBARWIDGET_H

/*---------------------------------------------------------------------------*/
