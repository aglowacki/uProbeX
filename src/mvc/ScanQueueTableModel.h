//-----------------------------------------------------------------------------
 // Copyright (c) 2024, UChicago Argonne, LLC
 // See LICENSE file.
 //---------------------------------------------------------------------------

#ifndef ScanQueueTableModel_H
#define ScanQueueTableModel_H

//---------------------------------------------------------------------------

#include <QAbstractTableModel>
#include "mvc/BlueskyPlan.h"
#include <QMimeData>
#include <QIODevice>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class ScanQueueTableModel : public QAbstractTableModel 
{

    Q_OBJECT

public:
    ScanQueueTableModel(QObject* parent = nullptr) : QAbstractTableModel(parent) 
    {
        _headers[0] = "Type";
    }
    //---------------------------------------------------------------------------
    int rowCount(const QModelIndex& parent = QModelIndex()) const override 
    {
        return _data.size();
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override 
    {
        
        if(_data.size() > 0)
        {
            return _data.at(0).parameters.size() + 1;
        }
        return 10; 
    }
    //---------------------------------------------------------------------------
    void appendRow(const BlueskyPlan& row)
    {
        int rown = _data.size();
        QModelIndex gIndex = index(rown, 0, QModelIndex());
        beginInsertRows(gIndex, rown, rown);
        _data.append(row);
        endInsertRows();
    }
    
    //---------------------------------------------------------------------------

    void clear()
    {
        _data.clear();
    }

    //---------------------------------------------------------------------------

    const BlueskyPlan& item(int row)
    {
        return _data.at(row);
    }

    //---------------------------------------------------------------------------

    const QString& getNameAtRow(int row)
    {
        if(_data.size() > row)
        {
            return _data.at(row).name;
        }
        return "";
    }

    //---------------------------------------------------------------------------

    Qt::DropActions supportedDropActions() const override
    {
        return Qt::MoveAction;
    }

    //---------------------------------------------------------------------------

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override 
    {
        if (action == Qt::IgnoreAction)
        {
            return true;
        }
        if(data != nullptr)
        {
            if( data->hasFormat("application/x-qabstractitemmodeldatalist") )
            {
                QByteArray bytes=data->data("application/x-qabstractitemmodeldatalist");
                QDataStream stream(&bytes,QIODevice::QIODevice::ReadOnly);
                int srow, scol;
                QMap<int,  QVariant> roleDataMap;
                stream >> srow >> scol >> roleDataMap;
                emit moveScanRow(srow, parent.row());
            }
            
        }
        return QAbstractTableModel::dropMimeData(data, action, row, 0, parent);
    }

    //---------------------------------------------------------------------------

    bool moveRows(const QModelIndex &srcParent, int srcRow, int count, const QModelIndex &dstParent, int dstChild) override
    {
        /*beginMoveRows(QModelIndex(), srcRow, srcRow + count - 1, QModelIndex(), dstChild);
        for(int i = 0; i<count; ++i) 
        {
            m_data.insert(dstChild + i, m_data[srcRow]);
            int removeIndex = dstChild > srcRow ? srcRow : srcRow+1;
            m_data.removeAt(removeIndex);
        }
        endMoveRows();
        */
        emit moveScanRow(srcParent.row(), dstParent.row());
        return true;
    }

    //---------------------------------------------------------------------------

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override 
    {
        if (!index.isValid() || index.row() >= _data.size())
        {
            return QVariant();
        }

        const BlueskyPlan& rowData = _data[index.row()];

        if(index.column() > rowData.parameters.size() + 1)
        {
            return QVariant();
        }

        if (role == Qt::DisplayRole) 
        {
            if(index.column() == 0)
            {
                return rowData.type;
            }
            
            int idx = 0;
            for( auto itr: rowData.parameters)
            {
                if(idx == index.column() -1 )
                {
                    return itr.second.default_val;
                }
                idx++;
            }
            
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
            if(section == 0)
            {
                return _headers[section];
            }
            else
            {
                if(_data.size() > 0)
                {
                    const BlueskyPlan& rowData = _data[0];
                    
                    int idx = 0;
                    for( auto itr: rowData.parameters)
                    {
                        if(idx == section -1 )
                        {
                            return itr.second.name;
                        }
                        idx++;
                    }
                }
                return QVariant(" ");
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

        if(index.column() == 0)
        {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
        }
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    
    //---------------------------------------------------------------------------

    void setAllData(std::vector<BlueskyPlan>& scans)
    {
        beginResetModel();
        _data.clear();
        for(auto itr : scans)
        {
            _data.append(itr);
        }
        endResetModel();
    }

    //---------------------------------------------------------------------------

    bool setData(const QModelIndex& index, const QVariant& value, int role) override
    {
        if (role == Qt::EditRole && index.isValid())
        {
            if( index.row() < _data.size() && index.column() != 0)
            {
                int idx = 1;
                for (auto &itr : _data[index.row()].parameters)
                {
                    if(idx == index.column())
                    {
                        // this will be refreshed from qserver
                        itr.second.default_val = value.toString();
                        emit planChanged(_data.at(index.row()));
                        return true;
                    }
                    idx ++;
                }   
            }
        }
        return false;

    }
    //---------------------------------------------------------------------------

signals:
    void moveScanRow(int, int);
    void planChanged(const BlueskyPlan&);  

private:
    QList<BlueskyPlan> _data;

    QString _headers[4];
};


//---------------------------------------------------------------------------

#endif /* ScanQueueTableModel.h */

//---------------------------------------------------------------------------