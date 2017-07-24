/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef TXM_MOTION_N_WAY_WIDGET_H
#define TXM_MOTION_N_WAY_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QList>

class QLabel;
class MotorController;
class MotorModel;
class MotorModelImpl;
class ValueWidget;

/*---------------------------------------------------------------------------*/

/**
 * @brief Widget for n way motion
 */
class MotionNWayWidget
: public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor. See Qt documentation for QWidget.
    *
    * @param names - list of names for each motor
    * @param parent - Qt parent widget
    */
   MotionNWayWidget(const QStringList& names, QWidget* parent = 0);

   /**
    * Destructor
    */
   ~MotionNWayWidget();

   /**
    * Get the descriptive name of the widget that appears on the screen.
    *
    * @return descriptive name of widget
    */
   QString getName();

   /**
    * Set the descriptive name of the widget that appears on the screen.
    *
    * @param name - descriptive name of widget
    */
   void setName(const QString& name);

   /**
    * Set the motor model implementation instance for motor N. This object
    * does not take ownership of the model.
    *
    * @param n - index of motor model
    * @param model - motor model implementation
    */
   void setNModelImpl(int n, MotorModelImpl* model);

private:

   /**
    * Determines the width of the longest string for a given font
    *
    * @param names
    * @param font
    */
   int computeWidth(const QStringList& names, const QFont& font);

   /**
    * Create components
    *
    * @param names - list of names for each motor
    */
   void createComponents(const QStringList& names);

private:

   /**
    * Stage MVC controller
    */
   QList<MotorController*> m_controller;

   /**
    * Stage MVC model
    */
   QList<MotorModel*> m_model;

   /**
    * Descriptive display name
    */
   QLabel* m_name;

   /**
    * Stage MVC widget
    */
   QList<ValueWidget*> m_widget;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
