/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsElementsWidget.h>

#include <gstar/ImageViewWidget.h>

#include <gstar/Annotation/RoiMaskGraphicsItem.h>

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
#include "core/ColorMap.h"

using gstar::AbstractImageWidget;
using gstar::ImageViewWidget;


/*---------------------------------------------------------------------------*/

MapsElementsWidget::MapsElementsWidget(int rows, int cols, bool create_image_nav, QWidget* parent)
    : AbstractImageWidget(rows, cols, parent)
{
    
    _model = nullptr;
    _normalizer = nullptr;
    _calib_curve = nullptr;
	_min_contrast_perc = 0;
	_max_contrast_perc = 1.0;
    
    _export_maps_dialog = nullptr;

	int r = 0;
    for (int i = 0; i < 256; ++i)
    {
        _gray_colormap.append(qRgb(i, i, i));
		if (i < 128)
		{
			_heat_colormap.append(qRgb(r, 0, 0));
			r += 2;
		}
		else if (i == 128)
		{
			r = 1;
			_heat_colormap.append(qRgb(255, r, 0));
		}
		else
		{
			_heat_colormap.append(qRgb(255, r, 0));
			r += 2;
		}
    }
	_selected_colormap = &_gray_colormap;
    _createLayout(create_image_nav);
}

/*---------------------------------------------------------------------------*/

MapsElementsWidget::~MapsElementsWidget()
{
/* //this is done elsewhere . should refactor it to be smart pointer
    if(_model != nullptr)
    {
        delete _model;
    }
    _model = nullptr;
*/
    if(_spectra_widget != nullptr)
    {
        delete _spectra_widget;
    }
    _spectra_widget = nullptr;

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::_createLayout(bool create_image_nav)
{

    _tab_widget = new QTabWidget();
    _spectra_widget = new FitSpectraWidget();
    connect(_spectra_widget, &FitSpectraWidget::export_fit_paramters, this, &MapsElementsWidget::on_export_fit_params);
    connect(_spectra_widget, &FitSpectraWidget::export_csv_and_png, this, &MapsElementsWidget::on_export_csv_and_png);

    _cb_analysis = new QComboBox(this);

    QHBoxLayout* hbox = new QHBoxLayout();
    QHBoxLayout* hbox2 = new QHBoxLayout();
    QVBoxLayout* counts_layout = new QVBoxLayout();
    QVBoxLayout* layout = new QVBoxLayout();

	connect(&iDiag, SIGNAL(onNewGridLayout(int, int)), this, SLOT(onNewGridLayout(int, int)));

    _dataset_directory = new QLabel();
    _dataset_name = new QLabel();
    hbox2->addWidget(new QLabel("Dataset: "));
    hbox2->addWidget(_dataset_directory);
    hbox2->addWidget(_dataset_name);
    hbox2->addItem(new QSpacerItem(9999, 40, QSizePolicy::Maximum));

    QSplitter* splitter = new QSplitter();
    splitter->setOrientation(Qt::Horizontal);
    splitter->addWidget(m_imageViewWidget);
    splitter->setStretchFactor(0, 1);
    splitter->addWidget(m_tabWidget);

    createToolBar(m_imageViewWidget, create_image_nav);
    counts_layout->addWidget(m_toolbar);
    counts_layout->addWidget(splitter);


	_cb_colormap = new QComboBox();
	_cb_colormap->addItem(STR_COLORMAP_GRAY);
	_cb_colormap->addItem(STR_COLORMAP_HEAT);

    const std::map<QString, QVector<QRgb> >* color_maps = ColorMap::inst()->color_maps();
    for (auto itr : *color_maps)
    {
        _cb_colormap->addItem(itr.first);
    }

    connect(_cb_colormap, SIGNAL(currentIndexChanged(QString)), this, SLOT(onColormapSelect(QString)));

    m_toolbar->addWidget(new QLabel(" ColorMap :"));
    m_toolbar->addWidget(_cb_colormap);

    _chk_log_color = new QCheckBox("Log scale");
    _chk_log_color->setChecked(false);
    connect(_chk_log_color, &QCheckBox::stateChanged, this, &MapsElementsWidget::on_log_color_changed);
    m_toolbar->addWidget(_chk_log_color);

    _grid_button = new QPushButton();
	_grid_button->setIcon(QIcon(":/images/grid.png"));
	_grid_button->setIconSize(QSize(15, 15)); 

	connect(_grid_button, SIGNAL(pressed()), this, SLOT(onGridDialog()));

    _cb_normalize = new QComboBox();
    _cb_normalize->setMinimumContentsLength(20);
    _cb_normalize->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
    _cb_normalize->addItem("1");
    connect(_cb_normalize, SIGNAL(currentIndexChanged(QString)), this, SLOT(onSelectNormalizer(QString)));

	m_toolbar->addWidget(_grid_button);
	m_toolbar->addWidget(_cb_analysis);
    m_toolbar->addWidget(new QLabel("  Normalize By: "));
    m_toolbar->addWidget(_cb_normalize);

    _global_contrast_chk = new QCheckBox("Global Contrast");
    _global_contrast_chk->setChecked(true);
    connect(_global_contrast_chk, &QCheckBox::stateChanged, this, &MapsElementsWidget::on_global_contrast_changed);
    m_toolbar->addWidget(_global_contrast_chk);
    _contrast_widget = new gstar::MinMaxSlider();
    connect(_contrast_widget, &gstar::MinMaxSlider::min_max_val_changed, this, &MapsElementsWidget::on_min_max_contrast_changed);
    m_toolbar->addWidget(_contrast_widget);

    _btn_export_as_image = new QPushButton("Export Images");
    connect(_btn_export_as_image, &QPushButton::pressed, this, &MapsElementsWidget::on_export_image_pressed);
    m_toolbar->addWidget(_btn_export_as_image);

    //_pb_perpixel_fitting = new QPushButton("Per Pixel Fitting");
    //counts_layout->addWidget(_pb_perpixel_fitting);

    QStringList extra_pv_header = { "Name", "Value", "Unit", "Description" };

    _extra_pvs_table_widget = new QTableWidget(1, 4);
    _extra_pvs_table_widget->setHorizontalHeaderLabels(extra_pv_header);

    QWidget *window = new QWidget();
    window->setLayout(counts_layout);

    _tab_widget->addTab(window, "Analyzed Counts");
    _tab_widget->addTab(_spectra_widget, DEF_STR_INT_SPECTRA);
    _tab_widget->addTab(_extra_pvs_table_widget, "Extra PV's");


    layout->addItem(hbox2);
    layout->addWidget(_tab_widget);

    //don't erase counts when mouse is off scene
    m_imageViewWidget->set_null_mouse_pos = false;
    connect(m_imageViewWidget, SIGNAL(cbLabelChanged(QString, int)), this, SLOT(onElementSelect(QString, int)));

	connect(m_imageViewWidget, &ImageViewWidget::parent_redraw, this, &MapsElementsWidget::redrawCounts);


    appendAnnotationTab();

    createActions();

    int rows = Preferences::inst()->getValue(STR_GRID_ROWS).toInt();
    int cols = Preferences::inst()->getValue(STR_GRID_COLS).toInt();
    if (rows < 1)
        rows = 1;
    if (cols < 1)
        cols = 1;
    onNewGridLayout(rows, cols);

    QString colormap = Preferences::inst()->getValue(STR_COLORMAP).toString();
    if (colormap.length() > 0)
    {
        _cb_colormap->setCurrentText(colormap);
    }
    //onColormapSelect(colormap);

    setLayout(layout);

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::onGridDialog()
{
	
	iDiag.show();

}
/*---------------------------------------------------------------------------*/

void MapsElementsWidget::on_global_contrast_changed(int state)
{
    if (state == Qt::CheckState::Checked)
    {
        _contrast_widget->setEnabled(true);
        m_imageViewWidget->setGlobalContrast(false);
    }
    else
    {
        _contrast_widget->setEnabled(false);
        m_imageViewWidget->setGlobalContrast(true);
    }
    
    redrawCounts();
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::on_log_color_changed(int state)
{
    redrawCounts();
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::on_min_max_contrast_changed()
{

	_min_contrast_perc = _contrast_widget->getUserMin() / 100.0;
    _max_contrast_perc = _contrast_widget->getUserMax() / 100.0;
	redrawCounts();

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::onNewGridLayout(int rows, int cols)
{
    const std::vector<QString> element_view_list = m_imageViewWidget->getLabelList();
    m_imageViewWidget->setSceneModelAndSelection(nullptr, nullptr);
    m_imageViewWidget->newGridLayout(rows, cols);
    model_updated();
    m_imageViewWidget->restoreLabels(element_view_list);
    redrawCounts();
    Preferences::inst()->setValue(STR_GRID_ROWS,rows);
    Preferences::inst()->setValue(STR_GRID_COLS,cols);
    Preferences::inst()->save();
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::addRoiMask()
{
    int w = m_imageViewWidget->scene()->getPixmapItem()->pixmap().width();
    int h = m_imageViewWidget->scene()->getPixmapItem()->pixmap().height();
   gstar::RoiMaskGraphicsItem* annotation = new gstar::RoiMaskGraphicsItem(w, h);
   insertAndSelectAnnotation(m_treeModel, m_annoTreeView, m_selectionModel, annotation);

   //QString name = ano->getName();
   //_spectra_widget->appendROISpectra()
   //            //data_struct Spectra = _model->load_roi(annotation->getROI());

   connect(annotation, &gstar::RoiMaskGraphicsItem::mask_updated, this, &MapsElementsWidget::roiUpdated);

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::roiUpdated(gstar::RoiMaskGraphicsItem* ano, bool reload)
{
    if (ano != nullptr && reload)
    {
        {

        }
    }
    
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::createActions()
{
    AbstractImageWidget::createActions();
    // TODO: change Roi to spectra region and add back in
    
    _addRoiMaskAction = new QAction("Add ROI Mask", this);

    connect(_addRoiMaskAction,
            SIGNAL(triggered()),
            this,
            SLOT(addRoiMask()));
            
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::displayContextMenu(QWidget* parent,
                                             const QPoint& pos)
{

   if (m_annotationsEnabled == false)
      return;

   QMenu menu(parent);
   menu.addAction(m_addMarkerAction);
   menu.addAction(m_addRulerAction);
   menu.addAction(_addRoiMaskAction);

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

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::onAnalysisSelect(QString name)
{	
    _calib_curve = _model->get_calibration_curve(name.toStdString(), _cb_normalize->currentText().toStdString());
    redrawCounts();
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::onElementSelect(QString name, int viewIdx)
{
    // update label on element select since it could be scaler
    if (_normalizer != nullptr && _calib_curve != nullptr)
    {
        int cnt = m_imageViewWidget->getViewCount();
        for (int i = 0; i < cnt; i++)
        {
            QString label = m_imageViewWidget->getLabelAt(i);
            if (_calib_curve->calib_curve.count(label.toStdString()) > 0)
            {
                m_imageViewWidget->setUnitLabel(i, "ug/cm2");
            }
            else
            {
                m_imageViewWidget->setUnitLabel(i, "cts/s");
            }
        }
    }

    QString analysisName = _cb_analysis->currentText();
    if(analysisName.length() > 0 && name.length() > 0)
    {
        displayCounts(analysisName.toStdString() , name.toStdString(), _chk_log_color->isChecked(), viewIdx);
    }
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::onColormapSelect(QString colormap)
{
	if(colormap == STR_COLORMAP_GRAY)
	{
		_selected_colormap = &_gray_colormap;
        Preferences::inst()->setValue(STR_COLORMAP, STR_COLORMAP_GRAY);
	}
	else if(colormap == STR_COLORMAP_HEAT)
	{
		_selected_colormap = &_heat_colormap;
        Preferences::inst()->setValue(STR_COLORMAP, STR_COLORMAP_HEAT);
	}
    else
    {
        _selected_colormap = ColorMap::inst()->get_color_map(colormap);
        if (_selected_colormap == nullptr)
        {
            _selected_colormap = &_gray_colormap;
            Preferences::inst()->setValue(STR_COLORMAP, STR_COLORMAP_GRAY);
        }
        else
        {
            Preferences::inst()->setValue(STR_COLORMAP, colormap);
        }
    }
	redrawCounts();
    
    Preferences::inst()->save();
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::onSelectNormalizer(QString name)
{
    if (name == "1")
    {
        _normalizer = nullptr;
        _calib_curve = nullptr;
        m_imageViewWidget->setUnitLabels("cts/s");
    }
    else
    {
        if (_model != nullptr)
        {
            std::unordered_map<std::string, data_struct::ArrayXXr<float>>* scalers = _model->getScalers();
            if (scalers->count(name.toStdString()) > 0)
            {
                _normalizer = &(scalers->at(name.toStdString()));
            }
            _calib_curve = _model->get_calibration_curve(_cb_analysis->currentText().toStdString(), name.toStdString());
        }
    }

    if (_normalizer != nullptr && _calib_curve != nullptr)
    {
        int cnt = m_imageViewWidget->getViewCount();
        for (int i = 0; i < cnt; i++)
        {
            QString label = m_imageViewWidget->getLabelAt(i);
            if (_calib_curve->calib_curve.count(label.toStdString()) > 0)
            {
                m_imageViewWidget->setUnitLabel(i, "ug/cm2");
            }
        }
        
    }

    redrawCounts();
}

 /*---------------------------------------------------------------------------*/

void MapsElementsWidget::setModel(MapsH5Model* model)
{
    if (_model != model)
    {
        _model = model;
        model_updated();
        if (_model != nullptr)
        {
            data_struct::Params_Override<double>* po = _model->getParamOverride();
            if (po != nullptr)
            {
                _spectra_widget->setParamOverride(po);
            }
            disconnect(_model, &MapsH5Model::model_int_spec_updated, _spectra_widget, &FitSpectraWidget::replot_integrated_spectra);
            _spectra_widget->clearFitIntSpectra();
            _spectra_widget->clearROISpectra();
            for (auto& itr : model->_fit_int_spec_dict)
            {
                _spectra_widget->appendFitIntSpectra(itr.first, itr.second);
            }

            for (auto& itr : model->_max_chan_spec_dict)
            {
                _spectra_widget->appendMaxChanSpectra(itr.first, itr.second);
            }
            

			_model->getIntegratedSpectra(_int_spec);
			_spectra_widget->setIntegratedSpectra(&_int_spec);
            connect(_model, &MapsH5Model::model_int_spec_updated, _spectra_widget, &FitSpectraWidget::replot_integrated_spectra);

            const data_struct::Scan_Info<double>* scan_info = _model->getScanInfo();

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
        m_imageWidgetToolBar->clickFill();
    }
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::on_export_csv_and_png(QPixmap png, ArrayDr* ev, ArrayDr* int_spec, ArrayDr* back_spec , ArrayDr* fit_spec, unordered_map<string, ArrayDr>* labeled_spectras)
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

    
    //bool save_fit_and_int_spectra(const std::string fullpath, const ArrayDr* energy, const ArrayDr* spectra, const ArrayDr* spectra_model, const ArrayDr* background, const unordered_map<string, ArrayDr*>* fit_int_def_spec)
    if (false == io::file::csv::save_fit_and_int_spectra_labeled(save_csv.toStdString(), ev, int_spec, fit_spec, back_spec, labeled_spectras))
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

void MapsElementsWidget::on_export_fit_params(data_struct::Fit_Parameters<double> fit_params, data_struct::Fit_Element_Map_Dict<double> elements_to_fit)
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
				else if (dataset_leftover.endsWith("h54"))
				{
					dataset_path += "4";
				}
				else if (dataset_leftover.endsWith("h55"))
				{
					dataset_path += "5";
				}
				else if (dataset_leftover.endsWith("h56"))
				{
					dataset_path += "6";
				}
				else if (dataset_leftover.endsWith("h57"))
				{
					dataset_path += "7";
				}
		}

        data_struct::Params_Override<double>* param_overrides = _model->getParamOverride();

        //check if file exists and warn user
        if (param_overrides != nullptr)
        {

            QString fileName = QFileDialog::getSaveFileName(this, "Save parameters override", dataset_path, tr("TXT (*.txt *.TXT);;All Files (*.*)"));
            if (fileName.length() > 0)
            {
                data_struct::Fit_Parameters<double>* nfit_params = &(param_overrides->fit_params);
                nfit_params->append_and_update(fit_params);
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
    if(_model == nullptr)
    {
        return;
    }

    disconnect(_cb_analysis, SIGNAL(currentIndexChanged(QString)), this, SLOT(onAnalysisSelect(QString)));

    _dataset_directory->setText(_model->getFilePath());
    _dataset_name->setText(_model->getDatasetName());

    std::string current_a;

    QString current_analysis = _cb_analysis->currentText();

    QString save_normal_val = _cb_normalize->currentText();
    
    _cb_normalize->clear();
    _cb_normalize->addItem("1");

    std::unordered_map<std::string, data_struct::ArrayXXr<float>>* scalers = _model->getScalers();
    
    if (scalers->count(STR_DS_IC) > 0)
    {
        _cb_normalize->addItem(QString(STR_DS_IC.c_str()));
    }
    if (scalers->count(STR_US_IC) > 0)
    {
        _cb_normalize->addItem(QString(STR_US_IC.c_str()));
    }
    if (scalers->count(STR_SR_CURRENT) > 0)
    {
        _cb_normalize->addItem(QString(STR_SR_CURRENT.c_str()));
    }

    /* // only show sr current, ds_ic, and us_ic for now
    for (const auto& itr : *scalers)
    {
        if (itr.first == STR_DS_IC || itr.first == STR_US_IC || itr.first == STR_SR_CURRENT)
        {
            continue;
        }
        _cb_normalize->addItem(QString(itr.first.c_str()));
    }
    */
    _cb_normalize->setMinimumWidth(_cb_normalize->minimumSizeHint().width());

    _cb_analysis->clear();
    std::vector<std::string> analysis_types = _model->getAnalyzedTypes();

    bool found_analysis = false;
    for(auto& itr: analysis_types)
    {
        QString newVal = QString(itr.c_str());
        _cb_analysis->addItem(newVal);
        current_a = itr;
        if(current_analysis == newVal)
        {
            found_analysis = true;
        }
    }

    //create save ordered vector by element Z number with K , L, M lines
    std::vector<std::string> element_lines;
    for (std::string el_name : data_struct::Element_Symbols)
    {
        element_lines.push_back(el_name);
    }
    for (std::string el_name : data_struct::Element_Symbols)
    {
        element_lines.push_back(el_name+"_L");
    }
    for (std::string el_name : data_struct::Element_Symbols)
    {
        element_lines.push_back(el_name+"_M");
    }

    element_lines.push_back(STR_COHERENT_SCT_AMPLITUDE);
    element_lines.push_back(STR_COMPTON_AMPLITUDE);
    element_lines.push_back(STR_SUM_ELASTIC_INELASTIC_AMP);
    element_lines.push_back(STR_TOTAL_FLUORESCENCE_YIELD);
    element_lines.push_back(STR_NUM_ITR);
    element_lines.push_back(STR_RESIDUAL);

    data_struct::Fit_Count_Dict<float> element_counts;
    _model->getAnalyzedCounts(current_a, element_counts);

    const std::vector<QString> element_view_list = m_imageViewWidget->getLabelList();
    
    m_imageViewWidget->clearLabels();
    //insert in z order
    for (std::string el_name : element_lines)
    {
        if(element_counts.count(el_name) > 0)
        {
            QString val = QString(el_name.c_str());
            m_imageViewWidget->addLabel(val);
        }

    }
//        for(auto& itr: *element_counts)
//        {
//            QString val = QString(itr.first.c_str());
//            m_imageViewWidget->addLabel(val);
//        }

    for (auto& itr : *scalers)
    {
        m_imageViewWidget->addLabel(QString(itr.first.c_str()));
    }

    if(false == found_analysis)
    {
        current_analysis = QString(current_a.c_str());
    }
    _cb_analysis->setCurrentText(current_analysis);

    m_imageViewWidget->restoreLabels(element_view_list);

    for (int cntr = 0; cntr < _cb_normalize->count(); cntr++)
    {
        if (save_normal_val == _cb_normalize->itemText(cntr))
        {
            _cb_normalize->setCurrentText(save_normal_val);
        }
    }

    redrawCounts();

    connect(_cb_analysis, SIGNAL(currentIndexChanged(QString)), this, SLOT(onAnalysisSelect(QString)));
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::redrawCounts()
{
    int view_cnt = m_imageViewWidget->getViewCount();
    std::string analysis_text = _cb_analysis->currentText().toStdString();

    if (view_cnt == 1)
    {
        for (int vidx = 0; vidx < view_cnt; vidx++)
        {
            QString element = m_imageViewWidget->getLabelAt(vidx);
            displayCounts(analysis_text, element.toStdString(), _chk_log_color->isChecked(), vidx);
        }
    }
    else
    {
        std::map<int, std::future<QPixmap> > job_queue;

        for (int vidx = 0; vidx < view_cnt; vidx++)
        {
            QString element = m_imageViewWidget->getLabelAt(vidx);
            
            job_queue[vidx] = Global_Thread_Pool::inst()->enqueue([this, vidx, analysis_text, element] { return generate_pixmap(analysis_text, element.toStdString(), _chk_log_color->isChecked(), vidx); });
        }

        while (job_queue.size() > 0)
        {
            std::vector<int> to_delete;
            for (auto& itr : job_queue)
            {
                m_imageViewWidget->resetCoordsToZero();
                m_imageViewWidget->scene(itr.first)->setPixmap(itr.second.get());
                if (m_imageHeightDim != nullptr && m_imageWidthDim != nullptr)
                {
                    m_imageHeightDim->setCurrentText(QString::number(m_imageViewWidget->scene(itr.first)->height()));
                    m_imageWidthDim->setCurrentText(QString::number(m_imageViewWidget->scene(itr.first)->width()));
                }
                to_delete.push_back(itr.first);
            }

            for (const auto& itr : to_delete)
            {
                job_queue.erase(itr);
            }
        }
    }
    //redraw annotations
    m_selectionModel->clear();
    m_imageViewWidget->setSceneModelAndSelection(m_treeModel, m_selectionModel);
}

/*---------------------------------------------------------------------------*/
/*
void MapsElementsWidget::_get_min_max_vals(float &min_val, float &max_val, const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& element_counts)
{
    
    gstar::RoiMaskGraphicsItem item(0,0);

    QList<gstar::AbstractGraphicsItem*>  Rois;
    if(m_treeModel != nullptr)
    {
        Rois = m_treeModel->get_all_of_type( item.classId() );
    }
    
    QImage *sum_of_masks = nullptr;
    if(Rois.size() > 0)
    {
        foreach(gstar::AbstractGraphicsItem* item , Rois)
        {
            gstar::RoiMaskGraphicsItem* mask = dynamic_cast<gstar::RoiMaskGraphicsItem*>(item);
            if(mask->isEnabled())
            {
                if(sum_of_masks == nullptr)
                {
                    sum_of_masks = mask->image_mask();
                }
                else
                {
                    QImage * tmpImg = mask->image_mask();
                    for(int row = 0; row < sum_of_masks->height(); row++)
                    {
                        for(int col = 0; col < sum_of_masks->width(); col++)
                        {
                            QColor color = tmpImg->pixelColor(col, row);
                            if(color.green() > 0)
                            {
                                sum_of_masks->setPixelColor(col, row, color);
                            }
                        }
                    }
                }
            }
        }
    }
    
    if(sum_of_masks != nullptr)
    {
        max_val = std::numeric_limits<float>::min();
        min_val = std::numeric_limits<float>::max();
        int height = static_cast<int>(element_counts.rows());
        int width = static_cast<int>(element_counts.cols());

        for(int row = 0; row < height; row++)
        {
            for(int col = 0; col < width; col++)
            {
                QColor color = sum_of_masks->pixelColor(col, row);
                //green is what we set the mask to so if it isn't green we use this for min max
                if(color.green() == 0)
                {
                    float val = element_counts(row, col);
                    min_val = std::min(min_val, val);
                    max_val = std::max(max_val, val);
                }
            }
        }
        if(max_val < min_val)
        {
            max_val = min_val;
        }
    }
    else
    {
        max_val = element_counts.maxCoeff();
        min_val = element_counts.minCoeff();
    }
    
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::displayCounts(const std::string analysis_type, const std::string element, bool log_color, int grid_idx)
{
	if (_model != nullptr)
	{
        data_struct::Fit_Count_Dict<float> fit_counts;
        _model->getAnalyzedCounts(analysis_type, fit_counts);
        data_struct::Fit_Count_Dict<float>* scalers = _model->getScalers();
        ArrayXXr<float> normalized;
        bool draw = false;
        int height = 0;
        int width = 0;
        
        if (fit_counts.count(element) > 0)
        {
            height = static_cast<int>(fit_counts.at(element).rows());
            width = static_cast<int>(fit_counts.at(element).cols());
            normalized = fit_counts.at(element);
            draw = true;
        }
        
        if (false == draw && scalers != nullptr)
        {
            if (scalers->count(element) > 0)
            {
                height = static_cast<int>(scalers->at(element).rows());
                width = static_cast<int>(scalers->at(element).cols());
                normalized = scalers->at(element);
                draw = true;
            }
        }

        if (draw)
        {
            if (log_color)
            {
                normalized = normalized.log10();
                normalized = normalized.unaryExpr([](float v) { return std::isfinite(v) ? v : 0.0f; });
            }

            m_imageViewWidget->resetCoordsToZero();
            if (m_imageHeightDim != nullptr && m_imageWidthDim != nullptr)
            {
                m_imageHeightDim->setCurrentText(QString::number(height));
                m_imageWidthDim->setCurrentText(QString::number(width));
            }
            QImage image(width, height, QImage::Format_Indexed8);
            image.setColorTable(*_selected_colormap);

            float counts_max;
            float counts_min;
            if (_normalizer != nullptr && _calib_curve != nullptr)
            {
                if (_calib_curve->calib_curve.count(element) > 0)
                {
                    double calib_val = _calib_curve->calib_curve.at(element);
                    normalized /= (*_normalizer);
                    normalized /= calib_val;
                    float min_coef = normalized.minCoeff();
                    if (std::isfinite(min_coef) == false)
                    {
                        min_coef = 0.0f;
                    }
                    normalized = normalized.unaryExpr([min_coef](float v) { return std::isfinite(v) ? v : min_coef; });
                }
            }

            gstar::CountsLookupTransformer* counts_lookup = m_imageViewWidget->getMouseTrasnformAt(grid_idx);
            if (counts_lookup != nullptr)
            {
                counts_lookup->setCounts(normalized);
            }

            counts_max = normalized.maxCoeff();
            counts_min = normalized.minCoeff();

            if (_global_contrast_chk->isChecked())
            {
                // normalize contrast
                counts_max = counts_min + ((counts_max - counts_min) * _max_contrast_perc);
                counts_min = counts_min + ((counts_max - counts_min) * _min_contrast_perc);
            }
            else
            {
				//get user min max from contrast control
				m_imageViewWidget->getMinMaxAt(grid_idx, counts_min, counts_max);
            }

            float max_min = counts_max - counts_min;
            for (int row = 0; row < height; row++)
            {
                for (int col = 0; col < width; col++)
                {
                    //first clamp the data to max min
                    float cnts = normalized(row, col);
                    cnts = std::min(counts_max, cnts);
                    cnts = std::max(counts_min, cnts);
                    //convert to pixel
                    uint data = (uint)(((cnts - counts_min) / max_min) * 255);
                    image.setPixel(col, row, data);
                }
            }
            m_imageViewWidget->scene(grid_idx)->setPixmap(QPixmap::fromImage(image.convertToFormat(QImage::Format_RGB32)));
        }
	}
    if (m_imageViewWidget != nullptr)
    {
        m_imageViewWidget->resetCoordsToZero();
    }

}

/*---------------------------------------------------------------------------*/

QPixmap MapsElementsWidget::generate_pixmap(const std::string analysis_type, const std::string element, bool log_color, int grid_idx)
{
    if (_model != nullptr)
    {
        data_struct::Fit_Count_Dict<float> fit_counts; 
        _model->getAnalyzedCounts(analysis_type, fit_counts);
        data_struct::Fit_Count_Dict<float>* scalers = _model->getScalers();
        ArrayXXr<float> normalized;
        bool draw = false;
        int height = 0;
        int width = 0;

        if (fit_counts.count(element) > 0)
        {
            height = static_cast<int>(fit_counts.at(element).rows());
            width = static_cast<int>(fit_counts.at(element).cols());
            normalized = fit_counts.at(element);
            draw = true;
        }

        if (false == draw && scalers != nullptr)
        {
            if (scalers->count(element) > 0)
            {
                height = static_cast<int>(scalers->at(element).rows());
                width = static_cast<int>(scalers->at(element).cols());
                normalized = scalers->at(element);
                draw = true;
            }
        }

        if (draw)
        {
            if (log_color)
            {
                normalized = normalized.log10();
                normalized = normalized.unaryExpr([](float v) { return std::isfinite(v) ? v : 0.0f; });
            }

            QImage image(width, height, QImage::Format_Indexed8);
            image.setColorTable(*_selected_colormap);

            float counts_max;
            float counts_min;
            if (_normalizer != nullptr && _calib_curve != nullptr)
            {
                if (_calib_curve->calib_curve.count(element) > 0)
                {
                    double calib_val = _calib_curve->calib_curve.at(element);
                    normalized /= (*_normalizer);
                    normalized /= calib_val;
                    float min_coef = normalized.minCoeff();
                    if (std::isfinite(min_coef) == false)
                    {
                        min_coef = 0.0f;
                    }
                    normalized = normalized.unaryExpr([min_coef](float v) { return std::isfinite(v) ? v : min_coef; });
                }
            }

            gstar::CountsLookupTransformer* counts_lookup = m_imageViewWidget->getMouseTrasnformAt(grid_idx);
            if (counts_lookup != nullptr)
            {
                counts_lookup->setCounts(normalized);
            }

            counts_max = normalized.maxCoeff();
            counts_min = normalized.minCoeff();
            
            if (_global_contrast_chk->isChecked())
            {
                // normalize contrast
                counts_max = counts_min + ((counts_max - counts_min) * _max_contrast_perc);
                counts_min = counts_min + ((counts_max - counts_min) * _min_contrast_perc);
            }
            else
            {
				//get user min max from contrast control
				m_imageViewWidget->getMinMaxAt(grid_idx, counts_min, counts_max);
            }

            float max_min = counts_max - counts_min;
            for (int row = 0; row < height; row++)
            {
                for (int col = 0; col < width; col++)
                {
                    //first clamp the data to max min
                    float cnts = normalized(row, col);
                    cnts = std::min(counts_max, cnts);
                    cnts = std::max(counts_min, cnts);
                    //convert to pixel
                    uint data = (uint)(((cnts - counts_min) / max_min) * 255);
                    image.setPixel(col, row, data);
                }
            }
            return QPixmap::fromImage(image.convertToFormat(QImage::Format_RGB32));
        }
    }
    return QPixmap();
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::windowChanged(Qt::WindowStates oldState,
                                       Qt::WindowStates newState)
{
    Q_UNUSED(oldState);

    if(Qt::WindowMaximized || Qt::WindowActive == newState)
    {
        m_imageViewWidget->resizeEvent(nullptr);
    }

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::on_export_image_pressed()
{

    //bring up dialog 
    if (_export_maps_dialog == nullptr)
    {
        QDir export_model_dir = _model->getDir();
        export_model_dir.cdUp();
        export_model_dir.mkdir("export");
        export_model_dir.cd("export");
        export_model_dir.mkdir(_model->getDatasetName());
        export_model_dir.cd(_model->getDatasetName());

        _export_maps_dialog = new ExportMapsDialog(export_model_dir.absolutePath());
        connect(_export_maps_dialog, &ExportMapsDialog::export_released, this, &MapsElementsWidget::on_export_images);
    }
    _export_maps_dialog->show();

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::on_export_images()
{
    _export_maps_dialog->setRunEnabled(false);
    
    //get all maps
    int view_cnt = m_imageViewWidget->getViewCount();
    std::string analysis_text = _cb_analysis->currentText().toStdString();
    std::map<std::string, std::future<QPixmap> > job_queue;


    QDir export_model_dir = _export_maps_dialog->get_dir();
    int cur = 0;
    _export_maps_dialog->status_callback(cur, view_cnt);
    for (int vidx = 0; vidx < view_cnt; vidx++)
    {
        QString element = m_imageViewWidget->getLabelAt(vidx);

        job_queue[element.toStdString()] = Global_Thread_Pool::inst()->enqueue([this, vidx, analysis_text, element] { return generate_pixmap(analysis_text, element.toStdString(), _chk_log_color->isChecked(), vidx); });
    }

    while (job_queue.size() > 0)
    {
        std::vector<std::string> to_delete;
        for (auto& itr : job_queue)
        {
            //m_imageViewWidget->scene(itr.first)->setPixmap(itr.second.get());
            if (_export_maps_dialog->get_save_tiff())
            {

            }
            if (_export_maps_dialog->get_save_png())
            {
                if (false == itr.second.get().save(QDir::cleanPath(export_model_dir.absolutePath() + QDir::separator() + QString(itr.first.c_str()) + ".png"), "PNG"))
                {
                    logE << "Could not save PNG for " << QDir::cleanPath(export_model_dir.absolutePath() + QDir::separator() + QString(itr.first.c_str()) + ".png").toStdString() << "\n";
                }
            }
            if (_export_maps_dialog->get_save_ascii())
            {

            }
            to_delete.push_back(itr.first);
            cur++;
            _export_maps_dialog->status_callback(cur, view_cnt);
        }

        for (const auto& itr : to_delete)
        {
            job_queue.erase(itr);
        }
    }

    _export_maps_dialog->setRunEnabled(true);
    _export_maps_dialog->on_open();
    _export_maps_dialog->close();
}