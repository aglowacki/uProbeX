/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef TXM_DESELECTABLE_TREE_VIEW_H
#define TXM_DESELECTABLE_TREE_VIEW_H

//---------------------------------------------------------------------------

#include <QTreeView>

//---------------------------------------------------------------------------

/**
 * @brief Abstract model class.
 */
class DeselectableTreeView
: public QTreeView
{

   Q_OBJECT

public:

   /**
    * Constructor
    */
   DeselectableTreeView(QWidget *parent = 0);

   virtual ~DeselectableTreeView();

private:

   /**
    * @brief mousePressEvent
    */
    virtual void mousePressEvent(QMouseEvent *event);

};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
