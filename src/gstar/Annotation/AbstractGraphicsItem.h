/*-----------------------------------------------------------------------------
 * Copyright (c) 2013, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ABSTRACTGRAPHICSITEM_H
#define ABSTRACTGRAPHICSITEM_H

/*---------------------------------------------------------------------------*/

#include <QGraphicsObject>
#include <QModelIndex>

/*---------------------------------------------------------------------------*/

namespace gstar
{

class AnnotationProperty;

/**
 * @brief The AbstractGraphicsItem class is the glue between QGraphicsScene
 *          and QAbstractItemModel classes. It is derived from QGraphicsObject
 *          so that it can be placed into a QGraphcisScene and be drawn in a
 *          QGraphicsView. It also contains a list of AnnotationProperty which
 *          are used for displaying the objects properties in a QTreeView.
 *          AbstractGraphicsItem is an abstract class so you need to derive
 *          from it and implement the following function: boundingRect,
 *          displayName, duplicate, paint, updateModel, updateView, and
 *          calculate.
 */
class AbstractGraphicsItem : public QGraphicsObject
{

   Q_OBJECT

public:

    /**
    * @brief AbstractGraphicsItem constructor.
    * @param parent
    */
   AbstractGraphicsItem(AbstractGraphicsItem* parent = 0);

   /**
    * @brief Destructor.
    */
   ~AbstractGraphicsItem();

   /**
    * @brief appendChild : Adds a child node.
    * @param child : Node to be added.
    */
   void appendChild(AbstractGraphicsItem* child);

   /**
    * @brief appendProperty : Add a property.
    * @param prop : Property to add.
    */
   void appendProperty(AnnotationProperty* prop);

   /**
    * @brief boundingRect
    * @return
    */
   virtual QRectF boundingRectMarker() const = 0;

   /**
    * @brief child : Returns a child node at a specific row.
    * @param row : Row of child to be returned.
    * @return
    */
   AbstractGraphicsItem* child(int row);

   /**
    * @brief childList : Returns a list of all the child nodes.
    * @return
    */
   QList<AbstractGraphicsItem*> childList() const;

   /**
    * @brief childCount : Get the number of children.
    * @return
    */
   int childCount() const;

   /**
    * @brief classId : Unique id for each AbstractGraphicsItem. Used in
    *                   GraphicsModel when adding AbstractGraphicsItem's to it.
    * @return : The classes typeid.name() cast to a QString.
    */
   QString classId();

   /**
    * @brief clearProperties
    */
   void clearProperties();

   /**
    * @brief columnCount : Get the number of properties this class has.
    * @return : Number of properties.
    */
   int columnCount() const;

   /**
    * @brief data : Get the data of a property this class has.
    * @param row : Currently not used but in the future should be the data of
    *               a childs property.
    * @param column : Offset of the property to return.
    * @return : The QVariant value of AnnotationProperty class.
    */
   QVariant data(int row, int column) const;

   /**
    * @brief displayName : The display name of the class. Cleaner version of
    *                       classId().
    * @return
    */
   virtual const QString displayName() const = 0;

   /**
    * @brief duplicate : Make a copy of the AbstractGraphicsItem. Memory is
    *                     allocated in this function so the caller has to
    *                     dispose of it.
    * @return
    */
   virtual AbstractGraphicsItem* duplicate() = 0;

   /**
    * @brief displayFlags : Used in GraphicsModel for QAbstractItemView flags
    *                        function. GraphicsItem has flags function also,
    *                        so it was changed to displayFlags.
    * @param row : Currently not used but may be used in the future for child
    *               flags.
    * @param column : Offset of the property flag to return.
    * @return
    */
   Qt::ItemFlags displayFlags(int row, int column) const;

   /**
    * @brief hasChild : Returns if it contains the specific child in its list.
    * @param child : Child node to check if exists in list.
    * @return
    */
   bool hasChild(AbstractGraphicsItem* child);

   /**
    * @brief parent : Returns Parent node.
    * @return
    */
   AbstractGraphicsItem* parent() const;

   /**
    * @brief prependProperty : Insert property to beginning of tehe list.
    * @param prop : Property to append.
    */
   void prependProperty(AnnotationProperty* prop);

   /**
    * @brief properties : Get a list of all the properites in the node.
    * @return
    */
   QList<AnnotationProperty*> properties() const;

   /**
    * @brief propertyValue
    * @param name
    * @return
    */
   QVariant propertyValue(QString name) const;

   /**
    * @brief removeChild : Removes a specific child node.
    * @param item : Node to be removed.
    */
   void removeChild(AbstractGraphicsItem* item);

   /**
    * @brief removeChileAt : Removes a child node at a specific row.
    * @param row : Offset of child node to remove.
    */
   void removeChildAt(int row);

   /**
    * @brief row : This returns the offset of this item in his parents child
    *               node list. If this item is the third child node added in
    *               its parent list, it will return two (0 based, so 2 is 3rd
    *               item).
    * @return
    */
   int row() const;

   /**
    * @brief setAllChildrenSelected : Recursively calls set selected for all
    *                                  child nodes.
    * @param select : True or False
    */
   void setAllChildrenSelected(bool select);

   /**
    * @brief setCoordinatePrecision : set precision
    * @param number : number of precision
    */
   //void setCoordinatePrecision(int number);

   /**
    * @brief setData : Set QVariant value for a specific property.
    * @param index : Index of the property to set.
    * @param value : Value to set for the property.
    * @return : True if value was set, false otherwise.
    */
   bool setData(const QModelIndex& index, const QVariant& value);

   /**
    * @brief setParent : Set a parent node.
    * @param parent : Parent node.
    */
   void setParent(AbstractGraphicsItem* parent);

   /**
    * @brief setPropertyValue : Set property value.
    * @param name : Property name.
    * @param value : Property value.
    */
   void setPropertyValue(QString name, QVariant value);

public slots:

   /**
    * @brief calculate : Virtual function called when model or view are changed.
    *                     Can be used to update a AnnotationProperty that is not
    *                     displayed such as a length or intensity.
    */
   virtual void calculate() = 0;

   /**
    * @brief updateModel : Virtual function that is called when model needs to
    *                       be updated from view change.
    */
   virtual void updateModel() = 0;

   /**
    * @brief updateView : Virtual function that is called when view needs to be
    *                      updated from model change.
    */
   virtual void updateView() = 0;

signals:

   /**
    * @brief viewUpdated : Signal send when view(QGraphicsScene) property is
    *                       changed.
    */
   void viewUpdated(AbstractGraphicsItem*);

protected slots:

   /**
    * @brief modelChanged : Slot for AnnotationProperty changes.
    */
   void modelChanged();

   /**
    * @brief viewChanged : Slot for QGraphicsItem changes.
    */
   void viewChanged();

protected:

   /**
    * @brief connectAllProperties : Connects properties to modelChanged() slot.
    */
   void connectAllProperties();

   /**
    * @brief connectAllViewItems : Connects QGraphicsItem signals to
    *                               viewChanged() slot.
    */
   void connectAllViewItems();

   /**
    * @brief getFirstViewTransform
    * @return
    */
   QTransform getFirstViewTransform() const;

   /**
    * @brief disconnectAllProperties : Disconnects properties from
    *                                   modelChanged() slot.
    */
   void disconnectAllProperties();

   /**
    * @brief disconnectAllViewItems : Disconnects QGraphicsItem signals to
    *                                  viewChanged() slot.
    */
   void disconnectAllViewItems();

   /**
    * @brief itemChanged : Stop the GraphicsItems from leaving the scene bounds.
    * @param change : Type of change that happened.
    * @param value : New value from the chnage.
    * @return
    */
   QVariant itemChange(GraphicsItemChange change, const QVariant& value);

   /**
    * @brief initializePropertyName
    */
   void initializePropertyName();

protected:

   /**
    * @brief m_children : List of child nodes for scene.
    */
   QList<AbstractGraphicsItem*> m_children;

   /**
    * @brief m_data : List of AnnotationPropertys for model.
    */
   QList<AnnotationProperty*> m_data;

   /**
    * @brief m_parent : Parent node.
    */
   AbstractGraphicsItem* m_parent;

   /**
    * @brief m_decimalPreci
    */
   //int m_decimalPreci;

};

}

/*---------------------------------------------------------------------------*/

#endif // ABSTRACTGRAPHICSITEM_H

/*---------------------------------------------------------------------------*/
