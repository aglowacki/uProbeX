/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/PreferencesDisplay.h>
#include <preferences/Preferences.h>

#include <QSpinBox>
#include <QLabel>
#include <QFormLayout>

/*---------------------------------------------------------------------------*/

PreferencesDisplay::PreferencesDisplay(QWidget* parent) : QWidget(parent)
{

   m_font = new QSpinBox;
   m_font->setRange(6, 40);
   m_font->setSingleStep(2);
   m_font->setValue(10);

   m_windowTitle = new QLineEdit("");

   m_decimalPreci = new QSpinBox;
   m_decimalPreci->setRange(1, 7);
   m_decimalPreci->setSingleStep(1);
   m_decimalPreci->setValue(1);

   QFont font;
   font.setBold(true);
   font.setWeight(75);

   QLabel* lblFont = new QLabel("Font size:");
   lblFont->setFont(font);

   QLabel* lblTitle = new QLabel("Window Title:");
   lblTitle->setFont(font);

   QLabel* lblDeciPrecision = new QLabel("Number of precision:");
   lblDeciPrecision->setFont(font);

   QLabel* lblUseDarkTheme = new QLabel("Use Dark Theme:");
   lblUseDarkTheme->setFont(font);

   bool use_dark_theme = Preferences::inst()->getValue(STR_PFR_USE_DARK_THEME).toBool();
   _useDarkTheme = new QCheckBox("Restart required");
   _useDarkTheme->setChecked(use_dark_theme);

   QFormLayout* mainLayout = new QFormLayout();
   mainLayout->addRow(lblFont, m_font);
   mainLayout->addRow(lblTitle, m_windowTitle);
   mainLayout->addRow(lblDeciPrecision, m_decimalPreci);
   mainLayout->addRow(lblUseDarkTheme, _useDarkTheme);

   setLayout(mainLayout);

}

/*---------------------------------------------------------------------------*/

PreferencesDisplay::~PreferencesDisplay()
{

}

/*---------------------------------------------------------------------------*/

void PreferencesDisplay::acceptChanges()
{

}

/*---------------------------------------------------------------------------*/

int PreferencesDisplay::getFontSize()
{

   return m_font->value();
   
}

/*---------------------------------------------------------------------------*/

QString PreferencesDisplay::getWindowTitle()
{

   return m_windowTitle->text();

}

/*---------------------------------------------------------------------------*/

bool PreferencesDisplay::getUseDarkTheme()
{
    return _useDarkTheme->isChecked();
}

/*---------------------------------------------------------------------------*/

int PreferencesDisplay::getDecimalPrecision()
{

   return m_decimalPreci->value();

}

/*---------------------------------------------------------------------------*/

void PreferencesDisplay::setFontSize(int size)
{

   if (size < m_font->minimum() || size > m_font->maximum())
   {
      //TODO: Remove the hard-coded value
      m_font->setValue(8);
   }
   else
   {
      m_font->setValue(size);
   }

}

/*---------------------------------------------------------------------------*/

void PreferencesDisplay::setWindowTitle(QString title)
{

   m_windowTitle->setText(title);

}

/*---------------------------------------------------------------------------*/

void PreferencesDisplay::setDecimalPrecision(int number)
{

   if (number < m_decimalPreci->minimum() || number > m_decimalPreci->maximum())
   {
      m_decimalPreci->setValue(1);
   }
   else
   {
      m_decimalPreci->setValue(number);
   }

}

/*---------------------------------------------------------------------------*/

