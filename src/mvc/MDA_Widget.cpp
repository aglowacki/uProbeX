/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MDA_Widget.h>

#include <gstar/ImageViewWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include "io/file/aps/aps_fit_params_import.h"

using gstar::AbstractImageWidget;
using gstar::ImageViewWidget;

/*---------------------------------------------------------------------------*/

MDA_Widget::MDA_Widget(QWidget* parent) : QWidget(parent)
{

    _model = nullptr;
    createLayout();

}

/*---------------------------------------------------------------------------*/

MDA_Widget::~MDA_Widget()
{

}

/*---------------------------------------------------------------------------*/

void MDA_Widget::createLayout()
{

    _tab_widget = new QTabWidget();
    _spectra_widget = new FitSpectraWidget();
    connect(_spectra_widget, &FitSpectraWidget::export_fit_paramters, this, &MDA_Widget::on_export_fit_params);

    _cb_detector = new QComboBox(this);
    connect(_cb_detector, qOverload<const QString&>(&QComboBox::currentIndexChanged), this, &MDA_Widget::onDetectorSelect);

    _cb_scaler = new QComboBox(this);
    connect(_cb_scaler, qOverload<const QString&>(&QComboBox::currentIndexChanged), this, &MDA_Widget::onScalerSelect);

    QHBoxLayout* hbox = new QHBoxLayout();
    QVBoxLayout* vbox = new QVBoxLayout();
    QHBoxLayout* hbox2 = new QHBoxLayout();
    QVBoxLayout* scalers_layout = new QVBoxLayout();
    QVBoxLayout* layout = new QVBoxLayout();

    hbox->addWidget(new QLabel(" Scaler:"));
    hbox->addWidget(_cb_scaler);
    scalers_layout->addItem(hbox);

    _scaler_table_widget = new QTableWidget(2,2);
    scalers_layout->addWidget(_scaler_table_widget);

    hbox2->addWidget(new QLabel("Detector:"));
    hbox2->addWidget(_cb_detector);
    vbox->addItem(hbox2);
    vbox->addWidget(_spectra_widget);

    QStringList extra_pv_header = { "Name", "Value", "Unit", "Description" };

    _extra_pvs_table_widget = new QTableWidget(1, 4);
    _extra_pvs_table_widget->setHorizontalHeaderLabels(extra_pv_header);
    scalers_layout->addWidget(_extra_pvs_table_widget);

    QWidget *window = new QWidget();
    window->setLayout(scalers_layout);

    QWidget* window2 = new QWidget();
    window2->setLayout(vbox);

    _tab_widget->addTab(window, "Scalers");
    _tab_widget->addTab(window2, "Integrated Spectra");
    _tab_widget->addTab(_extra_pvs_table_widget, "Extra PV's");

    layout->addWidget(_tab_widget);

    setLayout(layout);
}

/*---------------------------------------------------------------------------*/

void MDA_Widget::setModel(MDA_Model* model)
{
    if(_model == model)
    {
        return;
    }
    _spectra_widget->clearAllSpectra();
    _model = model;
    model_updated();
}

/*---------------------------------------------------------------------------*/

void MDA_Widget::model_updated()
{
    if(_model == nullptr)
    {
        return;
    }
    int rows, cols;

    _model->getDims(rows, cols);
    data_struct::Scan_Info* scan_info = _model->getScanInfo();

    _scaler_table_widget->setRowCount(rows);
    _scaler_table_widget->setColumnCount(cols);

    for (const auto& itr : scan_info->scaler_maps)
    {
        _cb_scaler->addItem(QString::fromLatin1(itr.name.c_str(), itr.name.length()));
    }

    onScalerSelect(_cb_scaler->itemText(0));

    _extra_pvs_table_widget->setRowCount(scan_info->extra_pvs.size());
    int i = 0;
    for (const auto& itr : scan_info->extra_pvs)
    {
        _extra_pvs_table_widget->setItem(i, 0, new QTableWidgetItem(QString::fromLatin1(itr.name.c_str(), itr.name.length())));
        _extra_pvs_table_widget->setItem(i, 1, new QTableWidgetItem(QString::fromLatin1(itr.value.c_str(), itr.value.length())));
        _extra_pvs_table_widget->setItem(i, 2, new QTableWidgetItem(QString::fromLatin1(itr.unit.c_str(), itr.unit.length())));
        _extra_pvs_table_widget->setItem(i, 3, new QTableWidgetItem(QString::fromLatin1(itr.description.c_str(), itr.description.length())));
        i++;
    }

    disconnect(_cb_detector, qOverload<const QString&>(&QComboBox::currentIndexChanged), this, &MDA_Widget::onDetectorSelect);
    
    _cb_detector->clear();

    for (unsigned int i = 0; i < _model->getNumIntegratedSpectra(); i++)
    {
        _cb_detector->addItem(QString::number(i));
    }

    connect(_cb_detector, qOverload<const QString&>(&QComboBox::currentIndexChanged), this, &MDA_Widget::onDetectorSelect);

    if (_model->getNumIntegratedSpectra() > 0)
    {
        onDetectorSelect("0");
    }
}

/*---------------------------------------------------------------------------*/

void MDA_Widget::on_export_fit_params(data_struct::Fit_Parameters fit_params)
{
    unsigned int det = _cb_detector->currentText().toUInt();
    if (_model != nullptr)
    {
        data_struct::Params_Override* po  = _model->getParamOverride(det);
        if (po != nullptr)
        {
            QString save_path = _model->getPath();
            if (save_path.endsWith("/mda") || save_path.endsWith("\\mda"))
            {
                save_path.chop(3);
            }
            save_path += "maps_fit_parameters_override.txt" + _cb_detector->currentText();
            QString fileName = QFileDialog::getSaveFileName(this, "Save parameters override", save_path, "TXT"+ _cb_detector->currentText()+" (*.txt"+ _cb_detector->currentText()+" *.TXT"+_cb_detector->currentText()+")");
            if (!fileName.isEmpty() && !fileName.isNull())
            {
                data_struct::Fit_Parameters* nfit_params = &(po->fit_params);
                nfit_params->append_and_update(&fit_params);
                if (io::file::aps::save_parameters_override(fileName.toStdString(), po))
                {
                    QMessageBox::information(nullptr, "Export Fit Parameters", "Saved");
                }
                else
                {
                    QMessageBox::critical(nullptr, "Export Fit Parameters", "Failed to Saved");
                }
            }
        }
    }
    
}

/*---------------------------------------------------------------------------*/

void MDA_Widget::onDetectorSelect(const QString& det)
{
    unsigned int detector = det.toUInt();

    data_struct::Params_Override* po = _model->getParamOverride(detector);
    if (po != nullptr)
    {
		_spectra_widget->setParamOverride(po);
    }

    _spectra_widget->setIntegratedSpectra(_model->getIntegratedSpectra(detector));
}

/*---------------------------------------------------------------------------*/

void MDA_Widget::onScalerSelect(const QString& det)
{
    const data_struct::ArrayXXr* scaler = nullptr;
    int rows, cols;
    _model->getDims(rows, cols);
    std::string name = det.toStdString();
    data_struct::Scan_Info* scan_info = _model->getScanInfo();
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
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                _scaler_table_widget->setItem(i, j, new QTableWidgetItem(QString::number((*scaler)(i, j))));
            }
        }
    }
}

/*---------------------------------------------------------------------------*/