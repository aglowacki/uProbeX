/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/Scaler_Widget.h>
#include <QVBoxLayout>
#include <QHBoxLayout>

/*---------------------------------------------------------------------------*/

Scaler_Widget::Scaler_Widget(QWidget* parent) : QWidget(parent)
{

    _model = nullptr;
    createLayout();

}

/*---------------------------------------------------------------------------*/

Scaler_Widget::~Scaler_Widget()
{

}

/*---------------------------------------------------------------------------*/

void Scaler_Widget::createLayout()
{

    _cb_scaler = new QComboBox(this);
    connect(_cb_scaler, qOverload<const QString&>(&QComboBox::currentIndexChanged), this, &Scaler_Widget::onScalerSelect);

    QVBoxLayout* vbox = new QVBoxLayout();
    QHBoxLayout* hbox = new QHBoxLayout();
    QVBoxLayout* scalers_layout = new QVBoxLayout();
    
    hbox->addWidget(new QLabel(" Scaler:"));
    hbox->addWidget(_cb_scaler);
    scalers_layout->addItem(hbox);

    _scaler_table_widget = new QTableWidget(2,2);
    scalers_layout->addWidget(_scaler_table_widget);

    setLayout(scalers_layout);

}

/*---------------------------------------------------------------------------*/

void Scaler_Widget::setModel(RAW_Model* model)
{
    if (_model != model)
    {
        //_spectra_widget->clearAllSpectra();
        _model = model;

        Eigen::Index rows, cols;

        _model->getDims(rows, cols);

        _scaler_table_widget->setRowCount(rows);
        _scaler_table_widget->setColumnCount(cols);
        data_struct::Scan_Info* scan_info = _model->getScanInfo();
        if (scan_info != nullptr)
        {
            for (const auto& itr : scan_info->scaler_maps)
            {
                _cb_scaler->addItem(QString::fromLatin1(itr.name.c_str(), itr.name.length()));
            }
            onScalerSelect(_cb_scaler->itemText(0));
        }
    }
}

/*---------------------------------------------------------------------------*/

void Scaler_Widget::onScalerSelect(const QString& det)
{
    const data_struct::ArrayXXr* scaler = nullptr;
    Eigen::Index rows, cols;
    _model->getDims(rows, cols);
    std::string name = det.toStdString();
    data_struct::Scan_Info* scan_info = _model->getScanInfo();
    if (scan_info != nullptr)
    {
        for (const auto& itr : scan_info->scaler_maps)
        {
            if (itr.name == name)
            {
                scaler = &(itr.values);
                break;
            }
        }

        if (scaler != nullptr)
        {
            int minrows = std::min(rows, scaler->rows());
            int mincols = std::min(cols, scaler->cols());
            for (int i = 0; i < minrows; i++)
            {
                for (int j = 0; j < mincols; j++)
                {
                    _scaler_table_widget->setItem(i, j, new QTableWidgetItem(QString::number((*scaler)(i, j))));
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*/