/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PREFERENCES_DISPLAY_H
#define PREFERENCES_DISPLAY_H

/*---------------------------------------------------------------------------*/

#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include <QButtonGroup>
#include <QComboBox>
#include <QCheckBox>

class QSpinBox;

/*---------------------------------------------------------------------------*/

/**
 * @brief Preferences dialog page for configuring histogram plots.
 */
class PreferencesDisplay
: public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor
    *
    * @param parent - the Qt parent widget
    */
   PreferencesDisplay(QWidget* parent = 0);

   /**
    * Default destructor
    */
   ~PreferencesDisplay();

   /**
    * Process changes when parent dialog calls its accept
    */
   void acceptChanges();

   /**
    * Return the font size value set in the widget
    */
   int getFontSize();

   /**
    * Return the window title
    */
   QString getWindowTitle();

   /**
    * Return the decimal number
    */
   int getDecimalPrecision();

   /**
    * Set the font size
    */
   void setFontSize(int size);

   /**
    * @brief setWindowTitle
    * @param title
    */
   void setWindowTitle(QString title);

   /**
    * @brief setDecimalPrecision
    * @param number
    */
   void setDecimalPrecision(int number);

   void reload_themes();

public slots:
   void themeChanged(QString);

private:

   /**
    * Font value
    */
   QSpinBox* m_font;

   /**
    * Number of decimal precision
    */
   QSpinBox* m_decimalPreci;

   /**
    * @brief string to display on window title
    */
   QLineEdit* m_windowTitle;

   QComboBox* _cb_themes;

   QCheckBox* _ck_show_dataset_on_select;
};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
