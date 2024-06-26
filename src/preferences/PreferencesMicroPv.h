/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PREFERENCES_MICROPV_H
#define PREFERENCES_MICROPV_H

//---------------------------------------------------------------------------

#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include <QButtonGroup>
#include <QSpinBox>

//---------------------------------------------------------------------------

/**
 * @brief Preferences dialog page for configuring histogram plots.
 */
class PreferencesMicroPv
: public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor
    *
    * @param parent - the Qt parent widget
    */
   PreferencesMicroPv(QWidget* parent = 0);

   /**
    * Default destructor
    */
   ~PreferencesMicroPv();

   /**
    * Process changes when parent dialog calls its accept
    */
   void acceptChanges();

   /**
    * @brief getMicroProbeXPv
    * @return
    */
   QString getMicroProbeXPv();

   /**
    * @brief getMicroProbeYPv
    * @return
    */
   QString getMicroProbeYPv();

   /**
    * @brief setMicroProbXYPv
    * @param val
    */
   void setMicroProbeXPv(QString val);

   /**
    * @brief setMicroProbeYPv
    * @param val
    */
   void setMicroProbeYPv(QString val);

private:

   /**
    * @brief m_microProbeXPv
    */
   QLineEdit* m_microProbeXPv;

   /**
    * @brief m_microProbeYPv
    */
   QLineEdit* m_microProbeYPv;

};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
