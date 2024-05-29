//-----------------------------------------------------------------------------
 // Copyright (c) 2024, UChicago Argonne, LLC
 // See LICENSE file.
 //---------------------------------------------------------------------------

#ifndef FileTableModel_H
#define FileTableModel_H

//---------------------------------------------------------------------------

#include <QAbstractTableModel>
#include <preferences/Preferences.h>

//---------------------------------------------------------------------------

enum File_Loaded_Status {UNLOADED, LOADED, FAILED_LOADING};

//---------------------------------------------------------------------------
struct RowData 
{
    RowData(QIcon i, QString s, double n)
    {
        icon = i;
        text = s;
        number = n;
        status = UNLOADED;
        number2 = 0;
    }
    QIcon icon;
    QString text;
    double number;
    File_Loaded_Status status;
    int number2;
};
//---------------------------------------------------------------------------

class FileTableModel : public QAbstractTableModel 
{
public:
    FileTableModel(QObject* parent = nullptr) : QAbstractTableModel(parent) 
    {
        QString fs = "Size ";
        switch(Preferences::inst()->getValue(STR_PRF_FILE_SIZE).toInt())
        {
            case 0:
                break;
            case 1: 
                fs += "(Kb)";
                break;
            case 2: 
                fs += "(Mb)";
                break;
            case 3: 
                fs += "(Gb)";
                break;
            default:
                break;
        }
        _headers[0] = tr("L");
        _headers[1] = tr("Name");
        _headers[2] = fs;
        _headers[3] = tr("# ROI's");
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override 
    {
        return _data.size();
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override 
    {
        return 4; 
    }

    void appendRow(const RowData& row)
    {
        int rown = _data.size();
        QModelIndex gIndex = index(rown, 0, QModelIndex());
        beginInsertRows(gIndex, rown, rown);
        _data.append(row);
        endInsertRows();
    }

    void clear()
    {
        _data.clear();
    }

    const RowData& item(int row)
    {
        return _data.at(row);
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override 
    {
        if (!index.isValid() || index.row() >= _data.size() || index.column() >= 3)
        {
            return QVariant();
        }

        const RowData& rowData = _data[index.row()];

        if (role == Qt::DecorationRole && index.column() == 0)
        {
            return rowData.icon;
        }
        else if (role == Qt::DisplayRole) 
        {
            if (index.column() == 1)
            {
                return rowData.text;
            }
            else if (index.column() == 2)
            {
                return QString::number(rowData.number, 'f', 2);
            }
        }
        return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const
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

    Qt::ItemFlags flags(const QModelIndex &index) const
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    void updateStatus(File_Loaded_Status status, const QString& filename)
    {
        int i=0;
        for(auto &itr : _data)
        {        
            if(filename == itr.text)
            {
                QModelIndex index = createIndex(i, 0, &itr);
                itr.status = status;
                switch(status)
                {
                case UNLOADED:
                    itr.icon = QIcon(":/images/circle_gray.png");
                    emit dataChanged(index, index);
                    break;
                case LOADED:
                    itr.icon = QIcon(":/images/circle_green.png");
                    emit dataChanged(index, index);
                    break;
                case FAILED_LOADING:
                    itr.icon = QIcon(":/images/circle_red.png");
                    emit dataChanged(index, index);
                    break;
                }
                break;
            }
            i++;
        }
    }

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override
    {
        if(column < 0)
        {
            return;
        }
        beginResetModel();
        std::sort(_data.begin(), _data.end(), [&](const RowData& rowA, const RowData& rowB) 
        {
            switch (column)
            {
                case 0: // icon
                    switch(order)
                    {
                        case Qt::AscendingOrder:
                            return rowA.status < rowB.status;
                        case Qt::DescendingOrder:
                            return rowA.status > rowB.status;
                    };
                case 1: // file name
                    switch(order)
                    {
                        case Qt::AscendingOrder:
                            return rowA.text < rowB.text;
                        case Qt::DescendingOrder:
                            return rowA.text > rowB.text;
                    };
                    //return (rowA.text.compare(rowB.text, order) == 0);
                case 2: // file size
                    switch(order)
                    {
                        case Qt::AscendingOrder:
                            return rowA.number < rowB.number;
                        case Qt::DescendingOrder:
                            return rowA.number > rowB.number;
                    };
                case 3: // # roi's
                    switch(order)
                    {
                        case Qt::AscendingOrder:
                            return rowA.number2 < rowB.number2;
                        case Qt::DescendingOrder:
                            return rowA.number2 > rowB.number2;
                    };
            };
        });
        endResetModel();
    }

private:
    QList<RowData> _data;

    QString _headers[4];
};


//---------------------------------------------------------------------------

#endif /* FileTableModel.h */

//---------------------------------------------------------------------------