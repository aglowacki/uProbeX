/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SUB_WINDOW_H
#define SUB_WINDOW_H

/*---------------------------------------------------------------------------*/

#include <QMdiSubWindow>
#include <QUuid>

/*---------------------------------------------------------------------------*/

/**
 * @brief The SubWindow class provides a QMdiSubWindow that emits a signal
 * when it's closed.
 *
 * SubWindow is derived from QMdiSubWindow.
 */
class SubWindow
: public QMdiSubWindow
  {

   Q_OBJECT

public:

   /**
    * Constructor - See QMdiSubWindow for more information.
    *
    * @param parent the parent widget.
    * @param flags window flags.
    */
   SubWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);

   /**
    * Destructor
    */
   ~SubWindow();

   /**
    * @brief Get if is acquisition window
    */
   bool getIsAcquisitionWindow();

   /**
    * @brief UUID associate with this window.
    */
   QUuid getUuid();

   /**
    * @brief Set if is acquisition window
    */
   void setIsAcquisitionWindow(bool flag);

protected:

   /**
    * @brief Overridden closeEvent() from QMdiSubWindow
    *
    * @param closeEvent the QCloseEvent from Qt.
    */
   virtual void closeEvent(QCloseEvent* closeEvent);

signals:

   /**
    * @brief Window is closing signal. Emitted from within overridden closeEvent().
    *
    * @param subWin pointer to the SubWindow that is closing.
    */
   void windowClosing(SubWindow* subwin);

   /**
    * @brief Window obtained keyboard focus. Emitted from focusInEvent.
    *
    * @param subWin pointer to the current window.
    */
   void windowInFocus(SubWindow* subwin);

   /**
    * @brief Window lost keyboard focus. Emitted from focusOutEvent.
    *
    * @param subWin pointer to the current window.
    */
   void windowOutFocus(SubWindow* subwin);

private:

   QUuid m_id;

   bool m_isAcquisitionWindow;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
