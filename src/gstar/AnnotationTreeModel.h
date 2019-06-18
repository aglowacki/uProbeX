/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ANNOTATION_TREE_MODEL_H
#define ANNOTATION_TREE_MODEL_H

/*---------------------------------------------------------------------------*/

#include <QAbstractItemModel>
#include <QMap>

/*---------------------------------------------------------------------------*/

class QItemSelectionModel;

namespace gstar
{

class AbstractGraphicsItem;

/**
 * @brief The AnnotationTreeModel class
 */
class AnnotationTreeModel : public QAbstractItemModel
{

   Q_OBJECT

public:

    /**
    * @brief AnnotationTreeModel
    * @param parent
    */
   AnnotationTreeModel(QObject* parent = 0);

   /**
    *
    */
   ~AnnotationTreeModel();

   /**
    * @brief appendNode
    * @param item
    * @return
    */
   QModelIndex appendNode(AbstractGraphicsItem* item);

   /**
    * @brief calculate: calls calculate function for all annotations
    */
   void calculate();

   /**
    * @brief columnCount
    * @param parent
    * @return
    */
   int columnCount(const QModelIndex& parent = QModelIndex()) const;

   /**
    * @brief data
    * @param index
    * @param role
    * @return
    */
   QVariant data(const QModelIndex& index, int role) const;

   /**
    * @brief duplicate
    * @param index
    * @return
    */
   QModelIndex duplicateNode(const QModelIndex& index);

   /**
    * @brief flags
    * @param index
    * @return
    */
   Qt::ItemFlags flags(const QModelIndex& index) const;

   /**
    * @brief headerData
    * @param section
    * @param orientation
    * @param role
    * @return
    */
   QVariant headerData(int section,
                       Qt::Orientation orientation,
                       int role = Qt::DisplayRole) const;

   /**
    * @brief index
    * @param row
    * @param column
    * @param parent
    * @return
    */
   QModelIndex index(int row,
                     int column,
                     const QModelIndex& parent = QModelIndex()) const;

   /**
    * @brief insertRows
    * @param row
    * @param count
    * @param parent
    * @return
    */
   bool insertRows(int row,
                   int count,
                   const QModelIndex& parent = QModelIndex());

   /**
    * @brief parent
    * @param index
    * @return
    */
   QModelIndex parent(const QModelIndex& index)const;

   /**
    * @brief removeRow : Remove one row.
    * @param row : Index of the row.
    * @param parent : Parent index.
    * @return
    */
   bool removeRow(int row,
                   const QModelIndex& parent = QModelIndex());

   /**
    * @brief removeRows : Remove a specific number of rows.
    * @param row : Index to start at.
    * @param count : Number of rows to be removed.
    * @param parent : Parent index.
    * @return
    */
   bool removeRows(int row,
                   int count,
                   const QModelIndex& parent = QModelIndex());

   /**
    * @brief rowCount : Number of rows in the model.
    * @param parent : Parent index.
    * @return
    */
   int rowCount(const QModelIndex& parent = QModelIndex()) const;

   /**
    * @brief setData : Set data for a Item in the model.
    * @param index
    * @param value
    * @param role
    * @return
    */
   bool setData(const QModelIndex& index,
                const QVariant& value,
                int role = Qt::EditRole);

   QList<AbstractGraphicsItem*> get_all_of_type(const QString type_name);

public slots:

   /**
    * @brief refreshModel
    * @param item
    */
   void refreshModel(AbstractGraphicsItem* item);

protected:

   /**
    * @brief createGroup
    * @param item
    */
   AbstractGraphicsItem* createGroup(AbstractGraphicsItem* item);

   /**
    * @brief recursiveCalculate
    * @param item
    */
   void recursiveCalculate(AbstractGraphicsItem* pItem);

protected:

   /**
    * @brief m_groups : The header rows for each specific GraphicsItem is an
    *                    EmptyGraphicsItem so it won't be displayed in the
    *                    QGraphicsScene. To be able to quickly see if we have
    *                    an EmptyGraphicsItem header for the GraphicsItem, we
    *                    store the class typeid as the first part and a pointer
    *                    to the EmptyGraphicsItem header as the second part.
    */
   QMap<QString, AbstractGraphicsItem*> m_groups;

   /**
    * @brief m_groups : Used for incrementing the index for each added
    *                    AbstractGraphicsItem. This also prevents incorrect
    *                    indexes or duplicates if an item is removed.
    */
   QMap<QString, int> m_groupsCnt;

   /**
    * @brief m_root
    */
   AbstractGraphicsItem* m_root;

};

}

/*---------------------------------------------------------------------------*/

#endif // ANNOTATION_TREE_MODEL_H

/*---------------------------------------------------------------------------*/
