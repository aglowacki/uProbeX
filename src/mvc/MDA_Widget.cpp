/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MDA_Widget.h>

#include <gstar/ImageViewWidget.h>

#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include "io/file/aps/aps_fit_params_import.h"
#include "io/file/csv_io.h"

using gstar::AbstractImageWidget;
using gstar::ImageViewWidget;

/*---------------------------------------------------------------------------*/

MDA_Widget::MDA_Widget(QWidget* parent) : QWidget(parent)
{
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
    connect(_spectra_widget, &FitSpectraWidget::export_csv_and_png, this, &MDA_Widget::on_export_csv);

    _cb_detector = new QComboBox(this);
    connect(_cb_detector, qOverload<const QString&>(&QComboBox::currentIndexChanged), this, &MDA_Widget::onDetectorSelect);

    _scaler_widget = new Scaler_Widget(this);

    QVBoxLayout* vbox = new QVBoxLayout();
    QHBoxLayout* hbox2 = new QHBoxLayout();
    QVBoxLayout* layout = new QVBoxLayout();

    hbox2->addWidget(new QLabel("Detector:"));
    hbox2->addWidget(_cb_detector);
    vbox->addItem(hbox2);
    vbox->addWidget(_spectra_widget);

    QStringList extra_pv_header = { "Name", "Value", "Unit", "Description" };

    _extra_pvs_table_widget = new QTableWidget(1, 4);
    _extra_pvs_table_widget->setHorizontalHeaderLabels(extra_pv_header);

    QWidget* window2 = new QWidget();
    window2->setLayout(vbox);

    _tab_widget->addTab(window2, "Integrated Spectra");
    _tab_widget->addTab(_scaler_widget, "Scalers");
    _tab_widget->addTab(_extra_pvs_table_widget, "Extra PV's");

    layout->addWidget(_tab_widget);

    setLayout(layout);
}

/*---------------------------------------------------------------------------*/

void MDA_Widget::setModel(std::shared_ptr<RAW_Model> model)
{
    if (model)
    {
        _spectra_widget->clearAllSpectra();
        _model = model;
        model_updated();
    }
}

/*---------------------------------------------------------------------------*/

void MDA_Widget::model_updated()
{
	if (_model)
	{

		Eigen::Index rows, cols;

		_model->getDims(rows, cols);
		data_struct::Scan_Info* scan_info = _model->getScanInfo();

		_scaler_widget->setModel(_model);

		if (scan_info != nullptr)
		{
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
		}

		disconnect(_cb_detector, qOverload<const QString&>(&QComboBox::currentIndexChanged), this, &MDA_Widget::onDetectorSelect);

		_cb_detector->clear();
		auto keys = _model->getDetectorKeys();
		for (unsigned int i : keys)
		{
			if (i == -1)
			{
				_cb_detector->addItem("det_sum");
			}
			else
			{
				_cb_detector->addItem(QString::number(i));
			}
		}

		connect(_cb_detector, qOverload<const QString&>(&QComboBox::currentIndexChanged), this, &MDA_Widget::onDetectorSelect);

		if (_model->getNumIntegratedSpectra() > 0)
		{
			onDetectorSelectIdx(0);
		}
	}
}

/*---------------------------------------------------------------------------*/

void MDA_Widget::on_export_fit_params(data_struct::Fit_Parameters fit_params, data_struct::Fit_Element_Map_Dict elements_to_fit)
{

    data_struct::Params_Override generic_po;
    unsigned int det = _cb_detector->currentText().toUInt();
    data_struct::Params_Override default_po;
    if (_model)
    {
        data_struct::Params_Override* po = _model->getParamOverrideOrAvg(det);
        
        QString save_path = _model->getPath();
        QString ext = "TXT";
        if (save_path.endsWith("/mda") || save_path.endsWith("\\mda"))
        {
            save_path.chop(3);
        }
        if (false == save_path.endsWith("/") || false == save_path.endsWith("\\"))
        {
            save_path += "/";
        }
        if (_cb_detector->currentText() == "det_sum")
        {
            save_path += "maps_fit_parameters_override.txt";
        }
        else
        {
            save_path += "maps_fit_parameters_override.txt" + _cb_detector->currentText();
            ext += _cb_detector->currentText();
        }
        QString fileName = QFileDialog::getSaveFileName(this, "Save parameters override", save_path, ext + " (*."+ext+");;All Files (*.*)");
        if (!fileName.isEmpty() && !fileName.isNull())
        {
            if (po == nullptr)
            {
                po = &generic_po;
            }
            data_struct::Fit_Parameters* nfit_params = &(po->fit_params);
            nfit_params->append_and_update(&fit_params);
            po->elements_to_fit.clear();
            for (const auto& itr : elements_to_fit)
            {
                //po->elements_to_fit[itr.first] = gen_element_map(itr.first);
                po->elements_to_fit[itr.first] = itr.second;
            }
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

/*---------------------------------------------------------------------------*/

void MDA_Widget::on_export_csv(QPixmap png, data_struct::ArrayXr* ev, data_struct::ArrayXr* int_spec, data_struct::ArrayXr* back_spec, data_struct::ArrayXr* fit_spec, unordered_map<string, data_struct::ArrayXr>* labeled_spectras)
{

    //QString path = QFileDialog::getSaveFileName(this, "Save CSV", "", "CSV (*.csv)");

    QString dirName = QFileDialog::getExistingDirectory(this, "Export directory", ".");

    // Dialog returns a nullptr string if user press cancel.
    if (dirName.isNull() || dirName.isEmpty()) return;
	if (_model)
	{

		QDir save_path = QDir(dirName);
		QString model_file_path = _model->getFilePath();
		QFileInfo model_file = QFileInfo(model_file_path);
		QString file_name = model_file.fileName();

		QString save_png = QDir::cleanPath(save_path.absolutePath() + QDir::separator() + file_name + "_int_spec.png");
		QString save_csv = QDir::cleanPath(save_path.absolutePath() + QDir::separator() + file_name + "_int_spec.csv");
		QString mesg = "";

		if (false == png.save(save_png, "PNG"))
		{
			mesg.append("Failed to save PNG of spectra: ");
			mesg.append(save_png);
			mesg.append("  ");
		}
		else
		{
			mesg.append("Saved PNG of spectra: ");
			mesg.append(save_png);
			mesg.append("  ");
		}


		//bool save_fit_and_int_spectra(const std::string fullpath, const data_struct::ArrayXr* energy, const data_struct::ArrayXr* spectra, const data_struct::ArrayXr* spectra_model, const data_struct::ArrayXr* background, const unordered_map<string, data_struct::ArrayXr*>* fit_int_def_spec)
		if (false == io::file::csv::save_fit_and_int_spectra(save_csv.toStdString(), ev, int_spec, fit_spec, back_spec, labeled_spectras))
		{
			mesg.append("Failed to save CSV of spectra: ");
			mesg.append(save_csv);
		}
		else
		{
			mesg.append("Saved CSV of spectra: ");
			mesg.append(save_csv);
		}

		QMessageBox::information(nullptr, "Export to CSV", mesg);
	}
}

/*---------------------------------------------------------------------------*/

void MDA_Widget::onDetectorSelect(const QString& det)
{
    unsigned int detector;
    
    if (det == "det_sum")
    {
        detector = -1;
    }
    else
    {
        detector = det.toUInt();
    }
    data_struct::Params_Override* po = _model->getParamOverrideOrAvg(detector);
    if (po != nullptr)
    {
		_spectra_widget->setParamOverride(po);
    }
    
    _spectra_widget->setIntegratedSpectra(_model->getIntegratedSpectra(detector));
}

/*---------------------------------------------------------------------------*/

void MDA_Widget::onDetectorSelectIdx(int idx)
{
    if (_cb_detector->count() > idx )
    {
        QString det = _cb_detector->itemText(idx);
        onDetectorSelect(det);
    }
}

/*---------------------------------------------------------------------------*/