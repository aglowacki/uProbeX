/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "FitParamsTableModel.h"
#include <QDebug>

/*---------------------------------------------------------------------------*/

FitParamsTableModel::FitParamsTableModel(QObject* parent) : QAbstractTableModel(parent)
{

    // Initialize header data
    m_headers[NAME] = tr("Name");
    m_headers[VALUE] = tr("Value");
    m_headers[MIN_VAL] = tr("Min Value");
    m_headers[MAX_VAL] = tr("Max Value");
    m_headers[STEP_SIZE] = tr("Step Size");
    m_headers[BOUND_TYPE] = tr("Is Fixed");

}

/*---------------------------------------------------------------------------*/

void FitParamsTableModel::setFitParams(data_struct::xrf::Fit_Parameters* fit_params)
{
    if(fit_params != nullptr)
    {
        _fit_parameters = fit_params;
        _row_indicies.clear();
        _row_indicies = _fit_parameters->names_to_array();

        QModelIndex topLeft = index(0, 0);
        QModelIndex bottomRight = index(_row_indicies.size()-1, NUM_PROPS-1);
        emit dataChanged(topLeft, bottomRight);

    }
}

/*---------------------------------------------------------------------------*/

void FitParamsTableModel::clearAll()
{

    // Remove all rows
    while (rowCount(QModelIndex()) > 0)
    {
        removeRows(0, 1);
    }

}

/*---------------------------------------------------------------------------*/

QVariant FitParamsTableModel::data(const QModelIndex &index, int role) const
{

    // Mark unused
    Q_UNUSED(role);

    // Check valid index
    if (!index.isValid())
    {
        return QVariant();
    }

    // Check valid index
    if (index.row() >= _row_indicies.size() || index.row() < 0 || _fit_parameters == nullptr)
    {
        return QVariant();
    }

    int row = index.row();

    // Return values for display and edit roles
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        data_struct::xrf::Fit_Param fitp = (*_fit_parameters)[_row_indicies[row]];
        // Insert data
        if (index.column() == NAME) return QString(fitp.name.c_str());
        else if (index.column() == VALUE) return fitp.value;
        else if (index.column() == MIN_VAL) return fitp.min_val;
    }


    // Return empty data
    return QVariant();

}

/*---------------------------------------------------------------------------*/

Qt::ItemFlags FitParamsTableModel::flags(const QModelIndex &index) const
{

    // Check valid index
    if (!index.isValid())
    {
        return Qt::ItemIsSelectable;
    }

    if (index.column() > 0)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
    }
    else
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

}

/*---------------------------------------------------------------------------*/

QVariant FitParamsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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

bool FitParamsTableModel::removeRows(int row,
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

int FitParamsTableModel::rowCount(const QModelIndex &parent) const
{

    // Mark unused
    Q_UNUSED(parent);

    // Return number of rows
    return _row_indicies.size();

}

/*---------------------------------------------------------------------------*/

bool FitParamsTableModel::setData(const QModelIndex &index,
                                  const QVariant &value,
                                  int role)
{

    // Mark unused
    Q_UNUSED(role);


    // Check for valid index
    if (index.isValid() == false || _fit_parameters == nullptr)
    {
        return false;
    }

    // Get column and row
    int column = index.column();
    int row = index.row();
    std::string fitp_name = _row_indicies[row];


    if (column == VALUE)
    {
        bool ok = false;
        double dval = value.toFloat(&ok);
        if(ok)
        {
           (*_fit_parameters)[fitp_name].value = dval;
        }
        else
        {
            return false;
        }

    }
    else if (column == MIN_VAL)
    {
        qDebug()<<value.toString();
        //m_solverAttrs[row].setDescription(value.toString());
    }
    else if (column == MAX_VAL)
    {
        //m_solverAttrs[row].setIsEnable(value.toBool());
    }
    else
    {
        return false;
    }

    // Emit dataChanged signal
    emit(dataChanged(index, index));

    // Return true
    return true;

}

/*---------------------------------------------------------------------------*/

bool FitParamsTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
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





