/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ScanQueueWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "core/defines.h"

//---------------------------------------------------------------------------

ScanQueueWidget::ScanQueueWidget(QWidget *parent) : QWidget(parent)
{

    _createLayout();

}

//---------------------------------------------------------------------------

ScanQueueWidget::~ScanQueueWidget()
{

}

//---------------------------------------------------------------------------

void ScanQueueWidget::_createLayout()
{

    QVBoxLayout* layout = new QVBoxLayout();
    
    _scan_table_view = new QTableView();
    layout->addWidget(new QLabel("Scan Queue"));
    layout->addWidget(_scan_table_view);

    _te_qs_console = new QTextEdit(this);
    _te_qs_console->scrollBarWidgets(Qt::AlignRight);
 
    layout->addWidget(new QLabel("QServer Log"));
    layout->addWidget(_te_qs_console);
    setLayout(layout);

}

//---------------------------------------------------------------------------

void ScanQueueWidget::newDataArrived(const QString& data)
{
    _te_qs_console->append(data);
}

//---------------------------------------------------------------------------