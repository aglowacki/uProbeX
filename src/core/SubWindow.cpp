/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <core/SubWindow.h>
#include <QDebug>
#include <QMessageBox>
#include <QCloseEvent>
#include <QIcon>
#include <QMenu>

/*---------------------------------------------------------------------------*/

SubWindow::SubWindow(QWidget* parent, Qt::WindowFlags flags)
: QMdiSubWindow(parent, flags)
{

   m_id = QUuid::createUuid();
   m_isAcquisitionWindow = false;
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

   if (m_isAcquisitionWindow == true)
   {
      int ret = QMessageBox::critical(this, tr("Warning"),
         tr("Are you sure you want to close the acquistion window?\n\n"),
            QMessageBox::Ok|QMessageBox::Cancel);

      switch (ret)
      {
      case QMessageBox::Ok:
         closeEvent->accept();
         emit windowClosing(this);
         break;
      case QMessageBox::Cancel:
         closeEvent->ignore();
         break;
      default:
         break;
      }
   }
   else
   {
      // If window is not acquisition, just let the close event pass through.
      emit windowClosing(this);
   }

}

/*---------------------------------------------------------------------------*/

bool SubWindow::getIsAcquisitionWindow()
{

   return m_isAcquisitionWindow;

}

/*---------------------------------------------------------------------------*/
QUuid SubWindow::getUuid()
{

   return m_id;

}

/*---------------------------------------------------------------------------*/

void SubWindow::setIsAcquisitionWindow(bool flag)
{

   m_isAcquisitionWindow = flag;
   setAttribute(Qt::WA_DeleteOnClose, !flag);

}

/*---------------------------------------------------------------------------*/
