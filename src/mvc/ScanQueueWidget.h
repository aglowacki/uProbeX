/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SCAN_QUEUE_WIDGET_H
#define SCAN_QUEUE_WIDGET_H

//---------------------------------------------------------------------------

#include <QWidget>
#include <QTextEdit>

//---------------------------------------------------------------------------


class ScanQueueWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   ScanQueueWidget(QWidget *parent = nullptr);

   /**
    * Destructor.
    */
   ~ScanQueueWidget();

public slots:

   void newDataArrived(const QString &);

protected:

   /**
    * @brief Create layout
    */
   void _createLayout();

   QTextEdit *_te_qs_console;

//   QLineEdit *_qline_ip_addr;

//   QLineEdit *_qline_port;

//   QPushButton *_btn_update;
};


//---------------------------------------------------------------------------

#endif /* LiveLiveMapsElementsWidget_H_ */

//---------------------------------------------------------------------------
