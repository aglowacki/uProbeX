/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsElementsWidget.h>

#include <gstar/ImageViewWidget.h>

#include <gstar/Annotation/HotSpotMaskGraphicsItem.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include "io/file/aps/aps_fit_params_import.h"
#include <gstar/CountsLookupTransformer.h>
#include <limits>
#include "core/GlobalThreadPool.h"
#include "io/file/csv_io.h"

/*---------------------------------------------------------------------------*/

MapsElementsWidget::MapsElementsWidget(QWidget* parent) : QWidget(parent)
{
    _multi_layer_widget = nullptr;
	_multi_elements_widget = nullptr;
	_createLayout();
}

/*---------------------------------------------------------------------------*/

MapsElementsWidget::~MapsElementsWidget()
{
    if(_spectra_widget != nullptr)
    {
        delete _spectra_widget;
    }
    _spectra_widget = nullptr;

    if (_multi_layer_widget != nullptr)
    {
        delete _multi_layer_widget;
    }
    _multi_layer_widget = nullptr;

	
	if (_multi_elements_widget != nullptr)
	{
		delete _multi_elements_widget;
	}
	_multi_elements_widget = nullptr;
	
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::_createLayout()
{

    _tab_widget = new QTabWidget();
    _spectra_widget = new FitSpectraWidget();
    connect(_spectra_widget, &FitSpectraWidget::export_fit_paramters, this, &MapsElementsWidget::on_export_fit_params);
    connect(_spectra_widget, &FitSpectraWidget::export_csv_and_png, this, &MapsElementsWidget::on_export_csv_and_png);

    _multi_layer_widget = new MultiLayerWidget();

    QHBoxLayout* hbox = new QHBoxLayout();
    QVBoxLayout* layout = new QVBoxLayout();

    _dataset_directory = new QLabel();
    _dataset_name = new QLabel();
    hbox->addWidget(new QLabel("Dataset: "));
    hbox->addWidget(_dataset_directory);
    hbox->addWidget(_dataset_name);
    hbox->addItem(new QSpacerItem(9999, 40, QSizePolicy::Maximum));

	_multi_elements_widget = new MultiElementsWidget(1,1);
	
    QStringList extra_pv_header = { "Name", "Value", "Unit", "Description" };

    _extra_pvs_table_widget = new QTableWidget(1, 4);
    _extra_pvs_table_widget->setHorizontalHeaderLabels(extra_pv_header);

    _tab_widget->addTab(_multi_elements_widget, "Analyzed Counts");
    _tab_widget->addTab(_spectra_widget, DEF_STR_INT_SPECTRA);
    _tab_widget->addTab(_extra_pvs_table_widget, "Extra PV's");
    _tab_widget->addTab(_multi_layer_widget, "Multi-Element");


    layout->addItem(hbox);
    layout->addWidget(_tab_widget);

    createActions();
	
    setLayout(layout);

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::createActions()
{
	/*
    AbstractImageWidget::createActions();
    // TODO: change hotspot to spectra region and add back in
    
    _addHotSpotMaskAction = new QAction("Add ROI Mask", this);

    connect(_addHotSpotMaskAction,
            SIGNAL(triggered()),
            this,
            SLOT(addHotSpotMask()));
      */      
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::displayContextMenu(QWidget* parent, const QPoint& pos)
{
	/*
   if (m_annotationsEnabled == false)
      return;

   QMenu menu(parent);
   menu.addAction(m_addMarkerAction);
   menu.addAction(m_addRulerAction);
   menu.addAction(_addHotSpotMaskAction);

   if (m_treeModel != nullptr && m_treeModel->rowCount() > 0)
   {
      if (m_selectionModel->hasSelection())
      {
         menu.addSeparator();
         menu.addAction(m_duplicateAction);
         menu.addSeparator();
         menu.addAction(m_deleteAction);
      }
   }

   //ruler properties
   menu.addSeparator();
   menu.addAction(m_showRulerDialogAction);

   QAction* result = menu.exec(pos);
   if (result == nullptr)
   {
      m_selectionModel->clearSelection();
   }
   */
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void MapsElementsWidget::setModel(std::shared_ptr<MapsH5Model> model)
{
    if (model)
    {
        _model = model;
        model_updated();
        if (_model)
        {
			_multi_elements_widget->setModel(_model);
            data_struct::Params_Override* po = _model->getParamOverride();
            if (po != nullptr)
            {
                _spectra_widget->setParamOverride(po);
            }
            disconnect(_model.get(), &MapsH5Model::model_int_spec_updated, _spectra_widget, &FitSpectraWidget::replot_integrated_spectra);
            _spectra_widget->clearFitIntSpectra();
            _spectra_widget->clearROISpectra();
            for (auto& itr : model->_fit_int_spec_dict)
            {
                _spectra_widget->appendFitIntSpectra(itr.first, itr.second);
            }

            _spectra_widget->setIntegratedSpectra((data_struct::ArrayXr*)_model->getIntegratedSpectra());
            connect(_model.get(), &MapsH5Model::model_int_spec_updated, _spectra_widget, &FitSpectraWidget::replot_integrated_spectra);

            const data_struct::Scan_Info* scan_info = _model->getScanInfo();

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

        }
    }
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::on_export_csv_and_png(QPixmap png, data_struct::ArrayXr* ev, data_struct::ArrayXr* int_spec, data_struct::ArrayXr* back_spec , data_struct::ArrayXr* fit_spec, unordered_map<string, data_struct::ArrayXr>* labeled_spectras)
{
    QDir save_path = QDir(_dataset_directory->text());
    QFileInfo file_info = QFileInfo(_dataset_directory->text());
    QString file_name = file_info.fileName();
    QString save_png;
    QString save_csv;
    save_path.cdUp();
    QString mesg = "";
    bool found = false;
    if (save_path.cd("output"))
    {
        found = true;
    }
    else
    {
        save_path.cdUp();
        if (save_path.cd("output"))
        {
            found = true;
        }
    }

    if (false == found)
    {
        QString dirName = QFileDialog::getExistingDirectory(this, "Export directory", ".");

        // Dialog returns a nullptr string if user press cancel.
        if (dirName.isNull() || dirName.isEmpty()) return;

        save_path = QDir(dirName);

    }

    save_png = QDir::cleanPath(save_path.absolutePath() + QDir::separator() + file_name + "_int_spec.png");
    save_csv = QDir::cleanPath(save_path.absolutePath() + QDir::separator() + file_name + "_int_spec.csv");

    
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

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::on_export_fit_params(data_struct::Fit_Parameters fit_params, data_struct::Fit_Element_Map_Dict elements_to_fit)
{
    if (_model != nullptr)
    {
        
        QString dataset_path = _model->getFilePath();

		QStringList path_list = dataset_path.split("img.dat");
		dataset_path = path_list[0];
		dataset_path += "maps_fit_parameters_override.txt";

		if (path_list.size() > 1)
		{
				QString dataset_leftover = path_list[1];
				if (dataset_leftover.endsWith("h50"))
				{
					dataset_path += "0";
				}
				else if (dataset_leftover.endsWith("h51"))
				{
					dataset_path += "1";
				}
				else if (dataset_leftover.endsWith("h52"))
				{
					dataset_path += "2";
				}
				else if (dataset_leftover.endsWith("h53"))
				{
					dataset_path += "3";
				}
		}

        data_struct::Params_Override* param_overrides = _model->getParamOverride();

        //check if file exists and warn user
        if (param_overrides != nullptr)
        {

            QString fileName = QFileDialog::getSaveFileName(this, "Save parameters override", dataset_path, tr("TXT (*.txt *.TXT);;All Files (*.*)"));
            if (fileName.length() > 0)
            {
                data_struct::Fit_Parameters* nfit_params = &(param_overrides->fit_params);
                nfit_params->append_and_update(&fit_params);
                param_overrides->elements_to_fit.clear();
                for (const auto& itr : elements_to_fit)
                {
					param_overrides->elements_to_fit[itr.first] = itr.second;
                }
                if (io::file::aps::save_parameters_override(fileName.toStdString(), param_overrides))
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

void MapsElementsWidget::model_updated()
{
    if(_model)
    {
        return;
    }

    _dataset_directory->setText(_model.get()->getFilePath());
    _dataset_name->setText(_model.get()->getDatasetName());

}

/*---------------------------------------------------------------------------*/
