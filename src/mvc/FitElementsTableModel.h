/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef FitElementsTableModel_H_
#define FitElementsTableModel_H_

#include <algorithm>
#include <QAbstractTableModel>
#include <QList>
#include <QModelIndex>
#include <QVariant>
#include "data_struct/fit_element_map.h"

/*---------------------------------------------------------------------------*/

/**
 * @brief model to control the solver table
 */
class FitElementsTableModel
: public QAbstractTableModel
{

   Q_OBJECT

public:

   /**
    * Properties corresponding to columns in table.
    */
   enum HEADERS {
      SYMBOL,
   //   CENTER,
//      WIDTH,
//      WIDTH_MULTI,
      COUNTS,
      NUM_PROPS
   };

   /**
    * Constructor
    *
    * @param parent - parent Qt widget
    */
   FitElementsTableModel(QObject* parent = 0);

   ~FitElementsTableModel();

   void setDisplayHeaderMinMax(bool val);

   data_struct::Fit_Parameters getAsFitParams();

   void updateFitElements(data_struct::Fit_Element_Map_Dict * elements_to_fit);

   void updateElementValues(data_struct::Fit_Parameters *fit_params);

   data_struct::Fit_Element_Map *getElementByIndex(QModelIndex index) const;

   /**
    * @brief Clear all
    */
   void clearAll();

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   int columnCount(const QModelIndex &parent) const override;

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   QVariant data(const QModelIndex &index,
                 int role = Qt::DisplayRole) const;

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   Qt::ItemFlags flags(const QModelIndex &index) const;


   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   QVariant headerData(int section,
                       Qt::Orientation orientation,
                       int role) const;

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   /*bool insertRows(int row, int count,
                   const QModelIndex& parent = QModelIndex());*/

   QModelIndex index(int row, int column,
                         const QModelIndex &parent = QModelIndex()) const override;

   QModelIndex parent(const QModelIndex &index) const override;

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   bool removeRows(int row,
                   int count,
                   const QModelIndex& parent = QModelIndex());

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   int rowCount(const QModelIndex &parent) const;

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   bool setData(const QModelIndex &index,
                const QVariant &value,
                int role = Qt::EditRole);

   /**
    * @brief Reimplemented from QAbstractItemModel. See Qt documentation.
    */
   bool setHeaderData(int section,
                      Qt::Orientation orientation,
                      const QVariant &value,
                      int role = Qt::EditRole);

   void appendElement(data_struct::Fit_Element_Map* element);

   void deleteElementIndex(int row);

private:

   class TreeItem
   {
   public:
       explicit TreeItem(TreeItem *parent = nullptr, bool editable = false)
       {
           element_data = nullptr;
           parentItem = parent;
           props_editable = editable;
       }
       ~TreeItem()
       {
           for(TreeItem* t : childItems)
           {
                t->parentItem = nullptr;
                delete t;
           }
           childItems.clear();
           parentItem = nullptr;
           element_data = nullptr;
           props_editable = false;
       }

       void set_root(data_struct::Fit_Element_Map* element)
       {
           element_data = element;
           parentItem = nullptr;
           if(element == nullptr)
               return;

           std::string symb = element->full_name();
           //symbol
           itemData.push_back(QVariant(symb.c_str()));
           //count
           itemData.push_back(QVariant(0.0));

           TreeItem* child;

           child = new TreeItem(this);
           child->itemData.push_back(QVariant("Center"));
           child->itemData.push_back(QVariant(element->center()));
           childItems.push_back(child);

           child = new TreeItem(this);
           child->itemData.push_back(QVariant("Width"));
           child->itemData.push_back(QVariant(element->width()));
           child->itemData.push_back(QVariant(element->width_multi()));
           childItems.push_back(child);

           //child = new TreeItem(this, true);
           //child->itemData.push_back(QVariant("Width Multiplier"));
           //child->itemData.push_back(QVariant(element->width_multi()));
           //childItems.append(child);

           std::vector<float> multi_vec = element->energy_ratio_multipliers();
           int i =0;
           for(auto& itr : element->energy_ratios())
           {
               child = new TreeItem(this, true);
                switch(itr.ptype)
                {
                    case  data_struct::Element_Param_Type::Ka_Line:
                        child->itemData.push_back(QVariant("Ka"));
                    break;
                    case  data_struct::Element_Param_Type::Kb_Line:
                        child->itemData.push_back(QVariant("Kb"));
                    break;
                    case  data_struct::Element_Param_Type::L_Line:
                        child->itemData.push_back(QVariant("L"));
                    break;
                    case  data_struct::Element_Param_Type::M_Line:
                        child->itemData.push_back(QVariant("M"));
                    break;
                }
                child->itemData.push_back(QVariant(itr.energy));
                child->itemData.push_back(QVariant(multi_vec[i]));
                //child->itemData.push_back(QVariant(itr.mu_fraction));
                //child->itemData.push_back(QVariant(itr.ratio));
                childItems.push_back(child);
                i++;
           }
       }
//       TreeItem *child(int number){ return childItems.value(number); }
//       int childCount() const {return childItems.count();}
//       int columnCount() const {return itemData.count();}
//       QVariant data(int column) const{return itemData.value(column);}
//       bool insertChildren(int position, int count, int columns)
//       {
//           if (position < 0 || position > childItems.size())
//               return false;

//           for (int row = 0; row < count; ++row) {
//               QVector<QVariant> data(columns);
//               TreeItem *item = new TreeItem(data, this);
//               childItems.insert(position, item);
//           }

//           return true;
//       }
//       bool insertColumns(int position, int columns)
//       {
//           if (position < 0 || position > itemData.size())
//                   return false;

//           for (int column = 0; column < columns; ++column)
//               itemData.insert(position, QVariant());

//           foreach (TreeItem *child, childItems)
//               child->insertColumns(position, columns);

//           return true;
//       }
//       TreeItem *parent(){return parentItem;}
//       bool removeChildren(int position, int count)
//       {
//           if (position < 0 || position + count > childItems.size())
//               return false;

//           for (int row = 0; row < count; ++row)
//               delete childItems.takeAt(position);

//           return true;
//       }
//       bool removeColumns(int position, int columns)
//       {
//           if (position < 0 || position + columns > itemData.size())
//                   return false;

//           for (int column = 0; column < columns; ++column)
//               itemData.remove(position);

//           foreach (TreeItem *child, childItems)
//               child->removeColumns(position, columns);

//           return true;
//       }
       int childIndex(TreeItem* t)
       {
            for(int i=0; i< childItems.size(); i++)
            {
                if(childItems[i] == t)
                    return i;
            }
            return -1;
       }

       int childNumber() const
       {
           if(parentItem != nullptr)
           {
               if(parentItem->childItems.size() > 1)
               {
                return parentItem->childIndex(const_cast<TreeItem*>(this));
               }
               return 0;
           }
           return 0;
       }
       bool setData(int column, const QVariant &value)
       {
           if (column < 0 || column >= itemData.size())
               return false;

           itemData[column] = value;
           return true;
       }

       data_struct::Fit_Element_Map* element_data;
       std::vector<TreeItem*> childItems;
       QVector<QVariant> itemData;
       TreeItem *parentItem;
       bool props_editable;
   };



   /**
    * @brief Header names
    */
   QString m_headers[HEADERS::NUM_PROPS];

   //used to sort by Z
   std::vector<int> _row_indicies;
   //indexed by Z
   std::map<int, TreeItem*> _nodes;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
