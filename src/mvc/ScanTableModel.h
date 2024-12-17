//-----------------------------------------------------------------------------
 // Copyright (c) 2024, UChicago Argonne, LLC
 // See LICENSE file.
 //---------------------------------------------------------------------------

#ifndef ScanTableModel_H
#define ScanTableModel_H

//---------------------------------------------------------------------------

#include <QAbstractTableModel>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class ScanTableModel : public QAbstractTableModel 
{
public:
    ScanTableModel(QObject* parent = nullptr) : QAbstractTableModel(parent) 
    {
        _headers[0] = "Name";
        _headers[1] = "Value";
        _headers[2] = "Desc";
    }
    //---------------------------------------------------------------------------
    int rowCount(const QModelIndex& parent = QModelIndex()) const override 
    {
        return _data.size();
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override 
    {
        return 3; 
    }
    //---------------------------------------------------------------------------
    void appendRow(const BlueskyParam& row)
    {
        int rown = _data.size();
        QModelIndex gIndex = index(rown, 0, QModelIndex());
        beginInsertRows(gIndex, rown, rown);
        _data.push_back(row);
        endInsertRows();
    }
    
    //---------------------------------------------------------------------------

    void clear()
    {
        _data.clear();
    }

    //---------------------------------------------------------------------------

    const BlueskyParam& item(int row)
    {
        return _data.at(row);
    }

    //---------------------------------------------------------------------------

	void getCurrentParams(BlueskyPlan& plan)
    {
        plan.parameters.clear();
        plan.parameters = _data;
    }

    //---------------------------------------------------------------------------

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override 
    {
        if (!index.isValid() || index.row() >= _data.size() || index.column() >= 4)
        {
            return QVariant();
        }

        const BlueskyParam& rowData = _data[index.row()];

        if (role == Qt::DisplayRole) 
        {
            switch(index.column())
            {
            case 0:
                return rowData.name;
            case 1:
                return rowData.default_val;
            case 2:
                return rowData.description;
            };
        }
        else if(role == Qt::EditRole && index.column() == 1)
        {
            return rowData.default_val;
        }
        return QVariant();
    }

    //---------------------------------------------------------------------------

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        // Check this is DisplayRole
        if (role != Qt::DisplayRole) return QVariant();

        // Horizontal headers
        if (orientation == Qt::Horizontal)
        {
            if(section > 3)
            {
                return QVariant();
            }
            else
            {
                return _headers[section];
            }
        }

        // Return empty data
        return QVariant();
    }
    
    //---------------------------------------------------------------------------

    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        if (!index.isValid())
        {
            return Qt::NoItemFlags;
        }

        // Get desired index
        int c = index.column();
        if(c == 1)
        {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
        }

        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    
    //---------------------------------------------------------------------------

    void setAllData(BlueskyPlan& scan)
    {
        beginResetModel();
        _data.clear();
        _data = scan.parameters;
        endResetModel();
    }

    //---------------------------------------------------------------------------

    bool setData(const QModelIndex& index, const QVariant& value, int role) override
    {
        if (role == Qt::EditRole && index.isValid())
        {
            if( index.row() < _data.size() && index.column() == 1)
            {
                QString pre_val = _data[index.row()].default_val;
                if(pre_val == "True" || pre_val == "False")
                {
                    int cb_idx = value.toInt();
                    if (cb_idx == 1) // index 0 = false, 1 = true
                    {
                        _data[index.row()].default_val = "True";
                    }
                    else
                    {
                        _data[index.row()].default_val = "False";
                    }
                }
                else
                {
                    _data[index.row()].default_val = value.toString();
                }
                return true;
            }
            return false;
        }
        return false;

    }
    //---------------------------------------------------------------------------

private:
    std::vector<BlueskyParam> _data;

    QString _headers[3];
};


//---------------------------------------------------------------------------

#endif /* ScanTableModel.h */

//---------------------------------------------------------------------------