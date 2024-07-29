/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SCAN_QUEUE_WIDGET_H
#define SCAN_QUEUE_WIDGET_H

//---------------------------------------------------------------------------

#include <QWidget>
#include <QTextEdit>
#include <QTableView>

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

   QTableView* _scan_table_view;

};


//---------------------------------------------------------------------------

#endif /* LiveLiveMapsElementsWidget_H_ */

//---------------------------------------------------------------------------
