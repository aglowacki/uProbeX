/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <core/SubWindow.h>
#include <QMessageBox>
#include <QCloseEvent>
#include <QIcon>
#include <QMenu>

/*---------------------------------------------------------------------------*/

SubWindow::SubWindow(QWidget* parent, Qt::WindowFlags flags)
: QMdiSubWindow(parent, flags)
{

   m_id = QUuid::createUuid();
   //this->setAttribute(Qt::WindowSystemMenuHint, false);
   //this->systemMenu()->setVisible(false);
   setWindowIcon(QIcon(":/images/title.png"));

}

/*---------------------------------------------------------------------------*/

SubWindow::~SubWindow()
{

}

/*---------------------------------------------------------------------------*/

void SubWindow::closeEvent(QCloseEvent* closeEvent)
{

   Q_UNUSED(closeEvent);

   // If window is not acquisition, just let the close event pass through.
   emit windowClosing(this);

}

/*---------------------------------------------------------------------------*/
QUuid SubWindow::getUuid()
{

   return m_id;

}

/*---------------------------------------------------------------------------*/
