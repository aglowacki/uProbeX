/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/PreferencesDisplay.h>
#include <preferences/Preferences.h>

#include <QSpinBox>
#include <QLabel>
#include <QFormLayout>
#include <QDir>
#include <QCoreApplication>

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
   font.setWeight(QFont::Thin);

   QLabel* lblFont = new QLabel("Font size:");
   lblFont->setFont(font);

   QLabel* lblTitle = new QLabel("Window Title:");
   lblTitle->setFont(font);

   QLabel* lblDeciPrecision = new QLabel("Number of precision:");
   lblDeciPrecision->setFont(font);

   QLabel* lblUseDarkTheme = new QLabel("Display Theme (Restart required):");
   lblUseDarkTheme->setFont(font);

   _cb_themes = new QComboBox();
   reload_themes();
   QString str_theme = Preferences::inst()->getValue(STR_PFR_THEME).toString();
   int idx = _cb_themes->findText(str_theme);
   if (idx > -1)
   {
       _cb_themes->setCurrentIndex(idx);
   }

   QFormLayout* mainLayout = new QFormLayout();
   mainLayout->addRow(lblFont, m_font);
   mainLayout->addRow(lblTitle, m_windowTitle);
   mainLayout->addRow(lblDeciPrecision, m_decimalPreci);
   mainLayout->addRow(lblUseDarkTheme, _cb_themes);

   connect(_cb_themes, &QComboBox::currentTextChanged, this, &PreferencesDisplay::themeChanged);

   setLayout(mainLayout);

}

/*---------------------------------------------------------------------------*/

PreferencesDisplay::~PreferencesDisplay()
{

}

/*---------------------------------------------------------------------------*/

void PreferencesDisplay::reload_themes()
{
    QString colormapDir = QCoreApplication::applicationDirPath();
    QDir directory(colormapDir);
    directory.cd("../themes");

    if (!directory.exists())
    {
        QString warn_msg = "Cannot find the " + colormapDir + " directory";
        qWarning(warn_msg.toStdString().c_str());
        //logW<< "Cannot find the " << directory.path()<<"\n";
        return;
    }
    directory.setFilter(QDir::Files | QDir::NoSymLinks);

    QFileInfoList list = directory.entryInfoList();

    _cb_themes->clear();
    _cb_themes->addItem(STR_DEFAULT);
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.suffix() == "qss")
        {
            _cb_themes->addItem(fileInfo.baseName());
        }
    }
}

/*---------------------------------------------------------------------------*/

void PreferencesDisplay::themeChanged(QString val)
{
    Preferences::inst()->setValue(STR_PFR_THEME, val);
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

