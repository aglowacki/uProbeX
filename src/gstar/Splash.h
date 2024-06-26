/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_SPLASH_H
#define GSTAR_SPLASH_H

//---------------------------------------------------------------------------

#include <QDialog>
#include <QPushButton>
#include <QString>
#include <QTextEdit>
#include <QWidget>

//---------------------------------------------------------------------------

namespace gstar
{

/**
 * @brief Splash screen and about dialog
 *
 * A splash screen and about dialog box.
 */
class Splash
: public QDialog
{

   Q_OBJECT

public:

   /**
    * Constructor
    *
    * @param parent - the Qt parent widget
    * @param f - Qt window flags
    * @param title - the application's title
    * @param aboutMode - true if it's to function as an about dialog, false
    *                    for a splash screen
    */
   Splash(QWidget* parent = 0,
          Qt::WindowFlags f = Qt::Popup,
          const QString& title = "",
          bool aboutMode = false);

   /**
    * Append a message to the splash screen message box
    *
    * @param msg - message to append to splash screen
    */
   void appendMessage(const QString& msg);

   /**
    * Append a message to the splash screen message box and call
    * QCoreApplication::processEvents()
    *
    * @param msg - message to append to splash screen
    */
   void appendMessageAndProcess(const QString& msg);

   /**
    * Clear all messages in splash screen message box
    */
   void clear();

private:

   /**
    * The application's title
    */
   QString m_title;

   /**
    * About mode flag
    */
   bool m_aboutMode;

   /**
    * Message / status box
    */
   QTextEdit* m_status;

   /**
    * OK button for about mode
    */
   QPushButton* m_btnOK;

};

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
