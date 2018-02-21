/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "FitElementsTableModel.h"
#include <QDebug>

/*---------------------------------------------------------------------------*/

FitElementsTableModel::FitElementsTableModel(QObject* parent) : QAbstractTableModel(parent)
{
    // Initialize header data
    m_headers[HEADERS::SYMBOL] = tr("Symbol");
   // m_headers[HEADERS::CENTER] = tr("Center");
    //m_headers[HEADERS::WIDTH] = tr("Width");
    //m_headers[HEADERS::WIDTH_MULTI] = tr("Width Multiplier");
    m_headers[HEADERS::COUNTS] = tr("Counts");
}

/*---------------------------------------------------------------------------*/

FitElementsTableModel::~FitElementsTableModel()
{

    for(auto& itr : _nodes)
    {
        delete itr.second;
    }
    _nodes.clear();
    _row_indicies.clear();

}

/*---------------------------------------------------------------------------*/
/*
void FitElementsTableModel::setFitParams(data_struct::Fit_Parameters fit_params)
{

    _fit_parameters = fit_params;
    _row_indicies.clear();
    _row_indicies = _fit_parameters.names_to_array();

    std::sort(_row_indicies.begin(), _row_indicies.end());

    QModelIndex topLeft = index(0, 0);
    QModelIndex bottomRight = index(_row_indicies.size()-1, NUM_PROPS-1);
    emit dataChanged(topLeft, bottomRight);
    emit layoutChanged();

}

void FitElementsTableModel::setDisplayHeaderMinMax(bool val)
{
    if(val)
    {

    }
    else
    {

    }
}
*/

/*---------------------------------------------------------------------------*/

data_struct::Fit_Parameters FitElementsTableModel::getAsFitParams()
{
    data_struct::Fit_Parameters fit_params;
    for(auto& itr : _nodes)
    {
        TreeItem* node = itr.second;
        data_struct::Fit_Element_Map *element = node->element_data;
        fit_params.add_parameter(data_struct::Fit_Param(element->full_name(), node->itemData[1].toFloat(), data_struct::E_Bound_Type::FIT));
    }
    return fit_params;
}

/*---------------------------------------------------------------------------*/

void FitElementsTableModel::updateElementValues(data_struct::Fit_Parameters *fit_params)
{

    for(auto& itr : _nodes)
    {
        TreeItem* node = itr.second;
        data_struct::Fit_Element_Map *element = node->element_data;
        if(fit_params->contains(element->full_name()))
        {
            node->itemData[1] = QVariant(fit_params->at(element->full_name()).value);
        }
    }

}

/*---------------------------------------------------------------------------*/

int FitElementsTableModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
    {
        return static_cast<TreeItem*>(parent.internalPointer())->itemData.count();
    }

    return NUM_PROPS+1;
}

/*---------------------------------------------------------------------------*/

void FitElementsTableModel::updateFitElements(data_struct::Fit_Element_Map_Dict * elements_to_fit)
{
    if(elements_to_fit != nullptr)
    {
        //_elements_to_fit = *elements_to_fit;
        _row_indicies.clear();
        for(auto& itr : *elements_to_fit)
        {
            data_struct::Fit_Element_Map* element = itr.second;
            if(data_struct::Element_Info_Map::inst()->contains(element->symbol()))
            {
                _nodes[element->Z()] = new TreeItem();
                _nodes[element->Z()]->set_root(element);
                _row_indicies.push_back(element->Z());
            }
        }

        std::sort(_row_indicies.begin(), _row_indicies.end());

        QModelIndex topLeft = index(0, 0);
        QModelIndex bottomRight = index(_row_indicies.size()-1, NUM_PROPS-1);
        emit dataChanged(topLeft, bottomRight);
        emit layoutChanged();

    }
}

/*---------------------------------------------------------------------------*/

void FitElementsTableModel::clearAll()
{

    // Remove all rows
    while (rowCount(QModelIndex()) > 0)
    {
        removeRows(0, 1);
    }

}

/*---------------------------------------------------------------------------*/

QVariant FitElementsTableModel::data(const QModelIndex &index, int role) const
{

    // Mark unused
    Q_UNUSED(role);

    // Check valid index
    if (!index.isValid())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        TreeItem* node = static_cast<TreeItem*>(index.internalPointer());
        if(node)
        {
            if(node->itemData.count() > index.column())
                return node->itemData.at(index.column());
        }
    }

    /*
    // Check valid index
    if (index.row() >= _row_indicies.size() || index.row() < 0)
    {
        return QVariant();
    }

    int row = index.row();

    // Return values for display and edit roles
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if(index.parent().isValid())
        {
            int Z = _row_indicies[index.parent().row()];
            data_struct::Fit_Element_Map *parentItem = _nodes.at(Z)->element_data;
            return QVariant(parentItem->energy_ratios().at(index.column()).energy );
        }
        else
        {
            int Z = _row_indicies[row];
            data_struct::Fit_Element_Map *childItem = _nodes.at(Z)->element_data;
            // Insert data
            if (index.column() == HEADERS::SYMBOL) return QString(childItem->full_name().c_str());
            else if (index.column() == HEADERS::CENTER) return QVariant(childItem->center());
            else if (index.column() == HEADERS::WIDTH) return QVariant(childItem->width());
            else if (index.column() == HEADERS::WIDTH_MULTI) return QVariant(childItem->width_multi());
            else if (index.column() == HEADERS::COUNTS) return QVariant(-10.0);
            else return QVariant();
        }
    }
*/

    // Return empty data
    return QVariant();

}

/*---------------------------------------------------------------------------*/

Qt::ItemFlags FitElementsTableModel::flags(const QModelIndex &index) const
{

    // Check valid index
    if (!index.isValid())
    {
        return Qt::ItemIsSelectable;
    }

    TreeItem* node = static_cast<TreeItem*>(index.internalPointer());
    if(node->parentItem == nullptr)
    {
        if(index.column() == NUM_PROPS-1)
            return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
    }
    else
    {
        if(index.column() > 0 && node->props_editable)
            return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

/*---------------------------------------------------------------------------*/

QVariant FitElementsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    // Check this is DisplayRole
    if (role != Qt::DisplayRole) return QVariant();

    // Horizontal headers
    if (orientation == Qt::Horizontal)
    {
        if(section > (NUM_PROPS-1))
        {
            return QVariant();
        }
        else
        {
            return m_headers[section];
        }
    }

    // Return empty data
    return QVariant();

}

/*---------------------------------------------------------------------------*/

bool FitElementsTableModel::removeRows(int row,
                                     int count,
                                     const QModelIndex& parent)
{

    // Mark unused
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(parent);
    return true;
}

/*---------------------------------------------------------------------------*/

int FitElementsTableModel::rowCount(const QModelIndex &parent) const
{

    // Mark unused
    if(parent.isValid())
    {
        TreeItem* node = static_cast<TreeItem*>(parent.internalPointer());
        return node->childItems.count();
    }

    // Return number of rows
    return _row_indicies.size();

}

/*---------------------------------------------------------------------------*/

QModelIndex FitElementsTableModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid())
    {
        int Z = _row_indicies[row];
        TreeItem *childItem = _nodes.at(Z);
        return createIndex(row, column, childItem);
    }
    else
    {
        TreeItem* node = static_cast<TreeItem*>(parent.internalPointer());
        if(node && node->childItems.count() > row)
        {
            TreeItem* childNode = node->childItems.at(row);
            if(childNode)
            {
                return createIndex(row, column, childNode);
            }
        }
    }

    return QModelIndex();
}

/*---------------------------------------------------------------------------*/

data_struct::Fit_Element_Map* FitElementsTableModel::getElementByIndex(QModelIndex index) const
{
    if (index.isValid())
    {
        TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
        while(childItem->parentItem != nullptr)
        {
            childItem = childItem->parentItem;

        }
        return childItem->element_data;
    }
    return nullptr;
}

/*---------------------------------------------------------------------------*/

QModelIndex FitElementsTableModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();


    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem;

    if (parentItem == nullptr)
        return QModelIndex();

    int row = childItem->childNumber();
    return createIndex(row, 0, parentItem);
}

/*---------------------------------------------------------------------------*/

bool FitElementsTableModel::setData(const QModelIndex &index,
                                  const QVariant &value,
                                  int role)
{

    // Mark unused
    Q_UNUSED(role);


    // Check for valid index
    if (index.isValid() == false)
    {
        return false;
    }

    TreeItem* node = static_cast<TreeItem*>(index.internalPointer());
    if(node->parentItem == nullptr)
    {
        if(index.column() == HEADERS::COUNTS)
        {
            node->itemData[1] = value;
        }
    }
    else
    {
        if(index.column() > 0 && node->props_editable)
        {
            if(node->itemData.count() > index.column())
            {
                node->itemData[index.column()] = value;
            }
        }

    }
    // Emit dataChanged signal
    emit(dataChanged(index, index));

    // Return true
    return true;

}

/*---------------------------------------------------------------------------*/

bool FitElementsTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{

    // Mark unused
    Q_UNUSED(role);

    // Check for horizontal orientation
    if (orientation != Qt::Horizontal)
    {
        return false;
    }

    // Assign headers
    if (section < NUM_PROPS)
    {
        m_headers[section] = value.toString();
    }
    else
    {
        return false;
    }

    // Return true
    return true;

}

/*---------------------------------------------------------------------------*/





