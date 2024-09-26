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
        _headers[0] = "name";
        _headers[1] = "type";
        _headers[2] = "user";
        _headers[3] = "uuid";
    }
    //---------------------------------------------------------------------------
    int rowCount(const QModelIndex& parent = QModelIndex()) const override 
    {
        return _data.size();
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override 
    {
        return 4; 
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
        if (!index.isValid() || index.row() >= _data.size() || index.column() >= 4)
        {
            return QVariant();
        }

        const BlueskyPlan& rowData = _data[index.row()];

        if (role == Qt::DisplayRole) 
        {
            switch(index.column())
            {
            case 0:
                return rowData.name;
            case 1:
                return rowData.type;
            case 2:
                return rowData.user;
            case 3:
                return rowData.uuid;
            };
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
            if(section > 4)
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

        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
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
            if( index.row() < _data.size() && index.column() == 2)
            {
                _data[index.row()].user = value.toString();
                return true;
            }
            return false;
        }
        return false;

    }
    //---------------------------------------------------------------------------

signals:
    void moveScanRow(int, int);


private:
    QList<BlueskyPlan> _data;

    QString _headers[4];
};


//---------------------------------------------------------------------------

#endif /* ScanQueueTableModel.h */

//---------------------------------------------------------------------------