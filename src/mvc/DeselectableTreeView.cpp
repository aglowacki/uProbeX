/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/DeselectableTreeView.h>

#include <QModelIndex>
#include <QMouseEvent>

/*---------------------------------------------------------------------------*/

DeselectableTreeView::DeselectableTreeView(QWidget *parent) : QTreeView(parent)
{

}

/*---------------------------------------------------------------------------*/

DeselectableTreeView::~DeselectableTreeView()
{

}

/*---------------------------------------------------------------------------*/

void DeselectableTreeView::mousePressEvent(QMouseEvent *event)
{

   clearSelection();
   QTreeView::mousePressEvent(event);

}

/*---------------------------------------------------------------------------*/
