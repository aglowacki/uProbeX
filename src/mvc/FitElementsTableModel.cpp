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
    m_headers[HEADERS::Z] = tr("Z");
    m_headers[HEADERS::NAME] = tr("Name");
    m_headers[HEADERS::VALUE] = tr("Value");
}

/*---------------------------------------------------------------------------*/
/*
void FitElementsTableModel::setFitParams(data_struct::xrf::Fit_Parameters fit_params)
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

data_struct::xrf::Fit_Parameters FitElementsTableModel::getAsFitParams()
{
    data_struct::xrf::Fit_Parameters fit_params;
    for(auto& itr : _elements)
    {
        data_struct::xrf::Fit_Element_Map *element = itr.second;
        fit_params.add_parameter(element->full_name(), data_struct::xrf::Fit_Param(element->full_name(), 0.0001, data_struct::xrf::E_Bound_Type::FIT));
    }
    return fit_params;
}

void FitElementsTableModel::updateElementValues(data_struct::xrf::Fit_Parameters *fit_params)
{

    for(auto& itr : _elements)
    {
        //if( fit_params->contains(itr.first) )

    }
}

int FitElementsTableModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
    {
        return 1;
    }

    return NUM_PROPS;
}

void FitElementsTableModel::updateFitElements(data_struct::xrf::Fit_Element_Map_Dict * elements_to_fit)
{
    if(elements_to_fit != nullptr)
    {
        //_elements_to_fit = *elements_to_fit;
        _row_indicies.clear();
        for(auto& itr : *elements_to_fit)
        {
            if(data_struct::xrf::Element_Info_Map::inst()->contains(itr.first))
            {
                data_struct::xrf::Fit_Element_Map* element = itr.second;
                _elements[element->Z()] = element;
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

    // Check valid index
    if (index.row() >= _row_indicies.size() || index.row() < 0)
    {
        return QVariant();
    }

    int row = index.row();

    // Return values for display and edit roles
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {

        int Z = _row_indicies[row];
        data_struct::xrf::Fit_Element_Map *childItem = _elements.at(Z);
        // Insert data
        if (index.column() == HEADERS::Z) return QVariant(Z);
        else if (index.column() == HEADERS::NAME) return QString(childItem->full_name().c_str());
        //else if (index.column() == VALUE) return fitp.value;
        //else if (index.column() == MIN_VAL) return fitp.min_val;
        //else if (index.column() == MAX_VAL) return fitp.max_val;
        //else if (index.column() == STEP_SIZE) return fitp.step_size;
        //else if (index.column() == BOUND_TYPE) return fitp.bound_type; //QString(fitp.bound_type_str().c_str());
        else return QVariant();

    }


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

    if (index.column() > 1)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
    }
    else
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

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
    Q_UNUSED(parent);

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
        data_struct::xrf::Fit_Element_Map *childItem = _elements.at(Z);
        return createIndex(row, column, childItem);
    }
    /*
    else
    {
        data_struct::xrf::Fit_Element_Map *childItem = static_cast<data_struct::xrf::Fit_Element_Map *>(parent);
    }
    */

    return QModelIndex();
}

/*---------------------------------------------------------------------------*/

data_struct::xrf::Fit_Element_Map* FitElementsTableModel::getElementByIndex(QModelIndex index) const
{
    if (index.isValid())
    {
        QModelIndex parent = index.parent();
        if (parent.isValid() )
        {
            int Z = _row_indicies[parent.row()];
            return _elements.at(Z);
        }
        else
        {
            int Z = _row_indicies[index.row()];
            return _elements.at(Z);
        }
    }
    return nullptr;
}

/*---------------------------------------------------------------------------*/

QModelIndex FitElementsTableModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    //TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    //TreeItem *parentItem = childItem->parentItem();

    //if (parentItem == rootItem)
        return QModelIndex();

    //return createIndex(parentItem->row(), 0, parentItem);
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

    // Get column and row
    int column = index.column();
    int row = index.row();
    int Z = _row_indicies[row];

/*
    bool ok = false;
    double dval = value.toFloat(&ok);

    if (column == VALUE)
    {
        if(ok)
           _fit_parameters[fitp_name].value = dval;
        else
            return false;
    }
    else if (column == MIN_VAL)
    {
        if(ok)
           _fit_parameters[fitp_name].min_val = dval;
        else
            return false;
    }
    else if (column == MAX_VAL)
    {
        if(ok)
           _fit_parameters[fitp_name].max_val = dval;
        else
            return false;
    }
    else if (column == STEP_SIZE)
    {
        if(ok)
           _fit_parameters[fitp_name].step_size = dval;
        else
            return false;
    }
    else if (column == BOUND_TYPE)
    {
        _fit_parameters[fitp_name].bound_type = (data_struct::xrf::E_Bound_Type)value.toInt();
    }
    else
    {
        return false;
    }

    // Emit dataChanged signal
    emit(dataChanged(index, index));
*/
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





