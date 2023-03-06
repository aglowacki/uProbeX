/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/CoLocalizationWidget.h>

//---------------------------------------------------------------------------

CoLocalizationWidget::CoLocalizationWidget(QWidget* parent) : gstar::AbstractImageWidget(1, 1, parent)
{
    _model = nullptr;
    _createLayout();
}

//---------------------------------------------------------------------------

CoLocalizationWidget::~CoLocalizationWidget()
{
/* //this is done elsewhere . should refactor it to be smart pointer
    if(_model != nullptr)
    {
        delete _model;
    }
    _model = nullptr;
*/

}

//---------------------------------------------------------------------------

void CoLocalizationWidget::_createLayout()
{

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_imageViewWidget);

    //_imageViewWidget->setSceneModel(m_treeModel);
    //_imageViewWidget->setSceneSelectionModel(m_selectionModel);
    /*
    _imageViewWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_imageViewWidget,
        SIGNAL(customContextMenuRequested(const QPoint&)),
        this,
        SLOT(viewContextMenu(const QPoint&)));
        */

    //don't erase counts when mouse is off scene
    //_imageViewWidget->set_null_mouse_pos = false;
    //connect(_imageViewWidget, SIGNAL(cbLabelChanged(QString, int)), this, SLOT(onElementSelect(QString, int)));

	//connect(_imageViewWidget, &gstar::ImageViewWidget::parent_redraw, this, &CoLocalizationWidget::redrawCounts);

    //createActions();

    /*
    int rows = Preferences::inst()->getValue(STR_GRID_ROWS).toInt();
    int cols = Preferences::inst()->getValue(STR_GRID_COLS).toInt();
    if (rows < 1)
        rows = 1;
    if (cols < 1)
        cols = 1;
    onNewGridLayout(rows, cols);
    */

    /*
    QString colormap = Preferences::inst()->getValue(STR_COLORMAP).toString();
    if (colormap.length() > 0)
    {
        _cb_colormap->setCurrentText(colormap);
    }
    //onColormapSelect(colormap);
    */
    setLayout(layout);

}

//---------------------------------------------------------------------------
/*
void CoLocalizationWidget::onGridDialog()
{
	
	iDiag.show();

}

//---------------------------------------------------------------------------

void CoLocalizationWidget::on_global_contrast_changed(int state)
{
    if (state == Qt::CheckState::Checked)
    {
        _contrast_widget->setEnabled(true);
        _imageViewWidget->setGlobalContrast(false);
    }
    else
    {
        _contrast_widget->setEnabled(false);
        _imageViewWidget->setGlobalContrast(true);
    }
    
    redrawCounts();
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::on_log_color_changed(int state)
{
    Preferences::inst()->setValue(STR_LOG_SCALE_COLOR, _chk_log_color->isChecked());
    Preferences::inst()->setValue(STR_DISPLAY_COLOR_LEDGEND, _chk_disp_color_ledgend->isChecked());
    redrawCounts();
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::on_min_max_contrast_changed()
{

	_min_contrast_perc = _contrast_widget->getUserMin() / 100.0;
    _max_contrast_perc = _contrast_widget->getUserMax() / 100.0;
	redrawCounts();

}
*/
//---------------------------------------------------------------------------

void CoLocalizationWidget::onNewGridLayout(int rows, int cols)
{
    const std::vector<QString> element_view_list = m_imageViewWidget->getLabelList();
    m_imageViewWidget->setSceneModelAndSelection(nullptr, nullptr);
    m_imageViewWidget->newGridLayout(rows, cols);
    //model_updated();
    m_imageViewWidget->restoreLabels(element_view_list);
    redrawCounts();
    Preferences::inst()->setValue(STR_GRID_ROWS,rows);
    Preferences::inst()->setValue(STR_GRID_COLS,cols);
    Preferences::inst()->save();
}

//---------------------------------------------------------------------------
/*
void CoLocalizationWidget::addRoiMask()
{
    int w = _imageViewWidget->scene()->getPixmapItem()->pixmap().width();
    int h = _imageViewWidget->scene()->getPixmapItem()->pixmap().height();
   gstar::RoiMaskGraphicsItem* annotation = new gstar::RoiMaskGraphicsItem(w, h);
   insertAndSelectAnnotation(m_treeModel, m_annoTreeView, m_selectionModel, annotation);

   //QString name = ano->getName();
   //_spectra_widget->appendROISpectra()
   //            //data_struct Spectra = _model->load_roi(annotation->getROI());

   connect(annotation, &gstar::RoiMaskGraphicsItem::mask_updated, this, &CoLocalizationWidget::roiUpdated);

}
*/
//---------------------------------------------------------------------------

void CoLocalizationWidget::roiUpdated(gstar::RoiMaskGraphicsItem* ano, bool reload)
{
    if (ano != nullptr && reload)
    {
        {

        }
    }
    
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::createActions()
{
    /*
    _addRoiMaskAction = new QAction("Add ROI Mask", this);

    connect(_addRoiMaskAction,
            SIGNAL(triggered()),
            this,
            SLOT(addRoiMask()));
      */      
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::displayContextMenu(QWidget* parent,
                                             const QPoint& pos)
{
    /*
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
   */
}

//---------------------------------------------------------------------------
/*
void CoLocalizationWidget::onAnalysisSelect(QString name)
{	
    _calib_curve = _model->get_calibration_curve(name.toStdString(), _cb_normalize->currentText().toStdString());
    redrawCounts();
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::onElementSelect(QString name, int viewIdx)
{
    // update label on element select since it could be scaler
    if (_normalizer != nullptr && _calib_curve != nullptr)
    {
        int cnt = _imageViewWidget->getViewCount();
        for (int i = 0; i < cnt; i++)
        {
            QString label = _imageViewWidget->getLabelAt(i);
            if (_calib_curve->calib_curve.count(label.toStdString()) > 0)
            {
                _imageViewWidget->setUnitLabel(i, "ug/cm2");
            }
            else
            {
                _imageViewWidget->setUnitLabel(i, "cts/s");
            }
        }
    }

    if (_model != nullptr && _model->regionLinks().count(name) > 0)
    {
        _imageViewWidget->scene(viewIdx)->setPixmap(QPixmap::fromImage((_model->regionLinks().at(name))));
    }
    else
    {
        QString analysisName = _cb_analysis->currentText();
        if (analysisName.length() > 0 && name.length() > 0)
        {
            displayCounts(analysisName.toStdString(), name.toStdString(), _chk_log_color->isChecked(), viewIdx);
        }
    }
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::onColormapSelect(QString colormap)
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
    _color_maps_ledgend->setColorTable(*_selected_colormap);
    _color_map_ledgend_lbl->setPixmap(QPixmap::fromImage(_color_maps_ledgend->convertToFormat(QImage::Format_RGB32)));

	redrawCounts();
    
    Preferences::inst()->save();
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::onSelectNormalizer(QString name)
{
    if (name == "1")
    {
        _normalizer = nullptr;
        _calib_curve = nullptr;
        _imageViewWidget->setUnitLabels("cts/s");
    }
    else
    {
        if (_model != nullptr)
        {
            std::map<std::string, data_struct::ArrayXXr<float>>* scalers = _model->getScalers();
            if (scalers->count(name.toStdString()) > 0)
            {
                _normalizer = &(scalers->at(name.toStdString()));
            }
            _calib_curve = _model->get_calibration_curve(_cb_analysis->currentText().toStdString(), name.toStdString());
        }
    }

    if (_normalizer != nullptr && _calib_curve != nullptr)
    {
        int cnt = _imageViewWidget->getViewCount();
        for (int i = 0; i < cnt; i++)
        {
            QString label = _imageViewWidget->getLabelAt(i);
            if (_calib_curve->calib_curve.count(label.toStdString()) > 0)
            {
                _imageViewWidget->setUnitLabel(i, "ug/cm2");
            }
        }
        
    }

    redrawCounts();
}
*/
 //---------------------------------------------------------------------------

void CoLocalizationWidget::setModel(MapsH5Model* model)
{
    if (_model != model)
    {
        _model = model;
        //model_updated();
        if (_model != nullptr)
        {
        }
    }
}

//---------------------------------------------------------------------------
/*
void CoLocalizationWidget::on_export_csv_and_png(QPixmap png, ArrayDr* ev, ArrayDr* int_spec, ArrayDr* back_spec , ArrayDr* fit_spec, unordered_map<string, ArrayDr>* labeled_spectras)
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

//---------------------------------------------------------------------------

void CoLocalizationWidget::model_updated()
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

    std::map<std::string, data_struct::ArrayXXr<float>>* scalers = _model->getScalers();
    
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

    std::vector<std::string> final_counts_to_add_before_scalers;

    final_counts_to_add_before_scalers.push_back(STR_COHERENT_SCT_AMPLITUDE);
    final_counts_to_add_before_scalers.push_back(STR_COMPTON_AMPLITUDE);
    final_counts_to_add_before_scalers.push_back(STR_SUM_ELASTIC_INELASTIC_AMP);
    final_counts_to_add_before_scalers.push_back(STR_TOTAL_FLUORESCENCE_YIELD);
    final_counts_to_add_before_scalers.push_back(STR_NUM_ITR);
    final_counts_to_add_before_scalers.push_back(STR_RESIDUAL);

    data_struct::Fit_Count_Dict<float> element_counts;
    _model->getAnalyzedCounts(current_a, element_counts);

    const std::vector<QString> element_view_list = _imageViewWidget->getLabelList();
    
    // get copy of elements to add the ones that were missed, usually pileups
    data_struct::Fit_Count_Dict<float> element_counts_not_added;
    _model->getAnalyzedCounts(current_a, element_counts_not_added);

    _imageViewWidget->clearLabels();

    // insert any region links
    for (const auto& itr : _model->regionLinks())
    {
        _imageViewWidget->addLabel(itr.first);
    }

    // insert in z order
    for (std::string el_name : element_lines)
    {
        if(element_counts.count(el_name) > 0)
        {
            QString val = QString(el_name.c_str());
            _imageViewWidget->addLabel(val);
            element_counts_not_added.erase(el_name);
        }

    }

    //add leftovers ( pile ups )
    for(auto& itr: element_counts_not_added)
    {
        // if it is not in the final add then add it
        if(std::find(final_counts_to_add_before_scalers.begin(), final_counts_to_add_before_scalers.end(), itr.first) == final_counts_to_add_before_scalers.end())
        {
            QString val = QString(itr.first.c_str());
            _imageViewWidget->addLabel(val);
        }
    }

    // add end of element list that are not elements
    for (auto& itr : final_counts_to_add_before_scalers)
    {
        QString val = QString(itr.c_str());
        _imageViewWidget->addLabel(val);
    }
    

    std::vector<std::string> scalers_to_add_first;
    scalers_to_add_first.push_back(STR_SR_CURRENT);
    scalers_to_add_first.push_back(STR_US_IC);
    scalers_to_add_first.push_back(STR_DS_IC);
    scalers_to_add_first.push_back(STR_ELT);
    scalers_to_add_first.push_back(STR_ELAPSED_LIVE_TIME);
    scalers_to_add_first.push_back(STR_ERT);
    scalers_to_add_first.push_back(STR_ELAPSED_REAL_TIME);
    scalers_to_add_first.push_back(STR_INPUT_COUNTS);
    scalers_to_add_first.push_back(STR_ICR);
    scalers_to_add_first.push_back("INCNT");
    scalers_to_add_first.push_back(STR_OUTPUT_COUNTS);
    scalers_to_add_first.push_back(STR_OCR);
    scalers_to_add_first.push_back("OUTCNT");
    scalers_to_add_first.push_back(STR_DEAD_TIME);

    scalers_to_add_first.push_back("abs_cfg");
    scalers_to_add_first.push_back("abs_ic");

    scalers_to_add_first.push_back("H_dpc_cfg");
    scalers_to_add_first.push_back("V_dpc_cfg");
    
    scalers_to_add_first.push_back("DPC1_IC");
    scalers_to_add_first.push_back("DPC2_IC");

    scalers_to_add_first.push_back("dia1_dpc_cfg");
    scalers_to_add_first.push_back("dia2_dpc_cfg");

    scalers_to_add_first.push_back("CFG_1");
    scalers_to_add_first.push_back(STR_CFG_2);
    scalers_to_add_first.push_back(STR_CFG_3);
    scalers_to_add_first.push_back(STR_CFG_4);
    scalers_to_add_first.push_back(STR_CFG_5);
    scalers_to_add_first.push_back("CFG_6");
    scalers_to_add_first.push_back("CFG_7");
    scalers_to_add_first.push_back("CFG_8");
    scalers_to_add_first.push_back("CFG_9");


    std::map<std::string, data_struct::ArrayXXr<float>> left_over_scalers = *scalers;
    // add scalers in certain order
    for (auto& itr : scalers_to_add_first)
    {
        if(scalers->count(itr) > 0)
        {
            _imageViewWidget->addLabel(QString(itr.c_str()));
            left_over_scalers.erase(itr);
        }
    }

    // add rest of scalers
    for (auto& itr : left_over_scalers)
    {
        _imageViewWidget->addLabel(QString(itr.first.c_str()));
    }

    if(false == found_analysis)
    {
        current_analysis = QString(current_a.c_str());
    }
    _cb_analysis->setCurrentText(current_analysis);

    _imageViewWidget->restoreLabels(element_view_list);

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
*/
//---------------------------------------------------------------------------

void CoLocalizationWidget::redrawCounts()
{
    int view_cnt = m_imageViewWidget->getViewCount();
    std::string analysis_text = "NNLS";//_cb_analysis->currentText().toStdString();

    //if (view_cnt == 1)
    {
        for (int vidx = 0; vidx < view_cnt; vidx++)
        {
            QString element = m_imageViewWidget->getLabelAt(vidx);
            displayCounts(analysis_text, element.toStdString(), false, vidx);
        }
    }
    /*
    else
    {
        std::map<int, std::future<QPixmap> > job_queue;

        for (int vidx = 0; vidx < view_cnt; vidx++)
        {
            QString element = _imageViewWidget->getLabelAt(vidx);
            if (_model != nullptr && _model->regionLinks().count(element) > 0)
            {
                _imageViewWidget->scene(vidx)->setPixmap(QPixmap::fromImage((_model->regionLinks().at(element))));
            }
            else
            {
                job_queue[vidx] = Global_Thread_Pool::inst()->enqueue([this, vidx, analysis_text, element] { return generate_pixmap(analysis_text, element.toStdString(), false, vidx); });
            }
        }

        while (job_queue.size() > 0)
        {
            std::vector<int> to_delete;
            for (auto& itr : job_queue)
            {
                _imageViewWidget->resetCoordsToZero();
                _imageViewWidget->scene(itr.first)->setPixmap(itr.second.get());
                if (m_imageHeightDim != nullptr && m_imageWidthDim != nullptr)
                {
                    m_imageHeightDim->setCurrentText(QString::number(_imageViewWidget->scene(itr.first)->height()));
                    m_imageWidthDim->setCurrentText(QString::number(_imageViewWidget->scene(itr.first)->width()));
                }
                to_delete.push_back(itr.first);
            }

            for (const auto& itr : to_delete)
            {
                job_queue.erase(itr);
            }
        }
    }
    */
    //redraw annotations
    m_selectionModel->clear();
    //m_imageViewWidget->setSceneModelAndSelection(_treeModel, _selectionModel);
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::displayCounts(const std::string analysis_type, const std::string element, bool log_color, int grid_idx)
{
	if (_model != nullptr)
	{
        data_struct::Fit_Count_Dict<float> fit_counts;
        _model->getAnalyzedCounts(analysis_type, fit_counts);
        std::map<std::string, data_struct::ArrayXXr<float>>* scalers = _model->getScalers();
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
            m_imageViewWidget->resetCoordsToZero();
            /*
            if (m_imageHeightDim != nullptr && m_imageWidthDim != nullptr)
            {
                m_imageHeightDim->setCurrentText(QString::number(height));
                m_imageWidthDim->setCurrentText(QString::number(width));
            }
            */
            QImage image(width, height, QImage::Format_Indexed8);
            //image.setColorTable(*_selected_colormap);

            float counts_max;
            float counts_min;

            counts_max = normalized.maxCoeff();
            counts_min = normalized.minCoeff();

    		//get user min max from contrast control
			m_imageViewWidget->getMinMaxAt(grid_idx, counts_min, counts_max);

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

//---------------------------------------------------------------------------
/*
QPixmap CoLocalizationWidget::generate_pixmap(const std::string analysis_type, const std::string element, bool log_color, int grid_idx)
{
    if (_model != nullptr)
    {
        data_struct::Fit_Count_Dict<float> fit_counts; 
        _model->getAnalyzedCounts(analysis_type, fit_counts);
        std::map<std::string, data_struct::ArrayXXr<float>>* scalers = _model->getScalers();
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

            counts_max = normalized.maxCoeff();
            counts_min = normalized.minCoeff();
            
            //get user min max from contrast control
			_imageViewWidget->getMinMaxAt(grid_idx, counts_min, counts_max);

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

            if (height > 3 || _chk_disp_color_ledgend->isChecked() == true)
            {
                // add color map ledgend
                int startH = height * .1;
                int endH = height - startH;
                int startW = width + 1;
                int endW = width + cm_ledgend;
                if (cm_ledgend == 3)
                {
                    endW--;
                }

                float inc = 255.0 / float(endH - startH);
                float fcol = 255.0;

                for (int row = 0; row < height; row++)
                {
                    if (row >= startH && row <= endH)
                    {
                        for (int col = width; col < width + cm_ledgend; col++)
                        {
                            if (col >= startW && col <= endW)
                            {
                                image.setPixel(col, row, uint(fcol));
                            }
                            else
                            {
                                image.setPixel(col, row, uint(127));
                            }
                        }
                        fcol -= inc;
                        fcol = std::max(fcol, float(0.0));
                    }
                    else
                    {
                        for (int col = width; col < width + cm_ledgend; col++)
                        {
                            image.setPixel(col, row, uint(127));
                        }
                    }
                }
            }

            return QPixmap::fromImage(image.convertToFormat(QImage::Format_RGB32));
        }
    }
    return QPixmap();
}
*/
//---------------------------------------------------------------------------

void CoLocalizationWidget::windowChanged(Qt::WindowStates oldState,
                                       Qt::WindowStates newState)
{
    Q_UNUSED(oldState);

    if(Qt::WindowMaximized || Qt::WindowActive == newState)
    {
        m_imageViewWidget->resizeEvent(nullptr);
    }

}

//---------------------------------------------------------------------------
/*
void CoLocalizationWidget::on_export_image_pressed()
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
        connect(_export_maps_dialog, &ExportMapsDialog::export_released, this, &CoLocalizationWidget::on_export_images);
    }
    _export_maps_dialog->show();

}

//---------------------------------------------------------------------------

void CoLocalizationWidget::on_export_images()
{
    _export_maps_dialog->setRunEnabled(false);
    
    //get all maps
    int view_cnt = _imageViewWidget->getViewCount();
    std::string analysis_text = _cb_analysis->currentText().toStdString();
    std::map<std::string, std::future<QPixmap> > job_queue;


    QDir export_model_dir = _export_maps_dialog->get_dir();
    int cur = 0;

    QDir png_dir = export_model_dir;
    QDir tif8_dir = export_model_dir;
    QDir tiff_dir = export_model_dir;
    QDir ascii_dir = export_model_dir;

    if (_export_maps_dialog->get_save_png())
    {
        png_dir.mkdir("PNG");
        png_dir.cd("PNG");
    }
    if (_export_maps_dialog->get_save_tiff())
    {
        tif8_dir.mkdir("TIF_8bit");
        tif8_dir.cd("TIF_8bit");
        tiff_dir.mkdir("TIF_32FP");
        tiff_dir.cd("TIF_32FP");
    }


    if (_export_maps_dialog->get_save_png() || _export_maps_dialog->get_save_tiff())
    {
        if (_export_maps_dialog->get_export_all())
        {
            std::vector<std::string> normalizers = { STR_DS_IC , STR_US_IC, STR_SR_CURRENT, "Counts" };

            std::vector<std::string> analysis_types = _model->getAnalyzedTypes();
            for (auto& a_itr : analysis_types)
            {
                data_struct::Fit_Count_Dict<float> element_counts;
                _model->getAnalyzedCounts(a_itr, element_counts);
             
                if (_export_maps_dialog->get_save_png())
                {
                    png_dir.mkdir(QString(a_itr.c_str()));
                    png_dir.cd(QString(a_itr.c_str()));
                }
                if (_export_maps_dialog->get_save_tiff())
                {
                    tif8_dir.mkdir(QString(a_itr.c_str()));
                    tif8_dir.cd(QString(a_itr.c_str()));
                    tiff_dir.mkdir(QString(a_itr.c_str()));
                    tiff_dir.cd(QString(a_itr.c_str()));
                }

                // Save 8 bit png and/or tiff
                for (auto& e_itr : element_counts)
                {
                    QPixmap pixmap = generate_pixmap(a_itr, e_itr.first, false, -1);
                    if (_export_maps_dialog->get_save_png())
                    {
                        std::string save_file_name = a_itr + "-" + e_itr.first + ".png";
                        if (false == pixmap.save(QDir::cleanPath(png_dir.absolutePath() + QDir::separator() + QString(save_file_name.c_str())), "PNG"))
                        {
                            logE << "Could not save PNG for " << QDir::cleanPath(png_dir.absolutePath() + QDir::separator() + QString(save_file_name.c_str())).toStdString() << "\n";
                        }
                    }
                    if (_export_maps_dialog->get_save_tiff())
                    {
                        std::string save_file_name = a_itr + "-" + e_itr.first + "_8bit.tif";
                        if (false == pixmap.save(QDir::cleanPath(tif8_dir.absolutePath() + QDir::separator() + QString(save_file_name.c_str())), "TIFF"))
                        {
                            logE << "Could not save 8 bit TIFF for " << QDir::cleanPath(tif8_dir.absolutePath() + QDir::separator() + QString(save_file_name.c_str())).toStdString() << "\n";
                        }
                    }
                }

                // save normalized 32 bit float tiff
                if (_export_maps_dialog->get_save_tiff())
                {
                    data_struct::ArrayXXr<float>* normalizer = nullptr;
                    for (auto n_itr : normalizers)
                    {
                        tiff_dir.mkdir(QString(n_itr.c_str()));
                        tiff_dir.cd(QString(n_itr.c_str()));

                        std::map<std::string, data_struct::ArrayXXr<float>>* scalers = _model->getScalers();
                        if (scalers->count(n_itr) > 0)
                        {
                            normalizer = &(scalers->at(n_itr));
                        }
                        Calibration_curve<double>* calib_curve = _model->get_calibration_curve(a_itr, n_itr);

                        for (auto& e_itr : element_counts)
                        {
                            std::string save_file_name_fp = a_itr + "-" + e_itr.first + "-";
                            TIFF* tif = nullptr;

                            data_struct::ArrayXXr<float> counts = e_itr.second;

                            //out_stream << e_itr.first;
                            if (calib_curve != nullptr && normalizer != nullptr)
                            {
                                if (calib_curve->calib_curve.count(e_itr.first) > 0)
                                {
                                    save_file_name_fp += n_itr;

                                    float calib_val = static_cast<float>(calib_curve->calib_curve.at(e_itr.first));

                                    counts /= (*normalizer);
                                    counts /= calib_val;
                                }
                                else
                                {
                                    save_file_name_fp += "cts_s";
                                }
                            }
                            else
                            {
                                save_file_name_fp += "cts_s";
                            }

                            save_file_name_fp += ".tif";

                            QString save_path = QDir::cleanPath(tiff_dir.absolutePath() + QDir::separator() + QString(save_file_name_fp.c_str()));
                            if ((tif = TIFFOpen(save_path.toStdString().c_str(), "w")) == NULL)
                            {
                                logE << "Could not open " << save_path.toStdString() << " for writing\n";
                                continue;
                            }

                            TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, e_itr.second.cols());
                            TIFFSetField(tif, TIFFTAG_IMAGELENGTH, e_itr.second.rows());
                            TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 32);
                            TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
                            TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, e_itr.second.rows());
                            TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP);
                            //TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
                            TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
                            TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
                            //TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);

                            // Write the information to the file
                            // need to normaize the data by IC also 
                            TIFFWriteEncodedStrip(tif, 0, counts.data(), 4 * counts.cols() * counts.rows());
                            TIFFClose(tif);
                        }

                        tiff_dir.cdUp();
                    }
                }

                if (_export_maps_dialog->get_save_png())
                {
                    png_dir.cdUp();
                }
                if (_export_maps_dialog->get_save_tiff())
                {
                    tif8_dir.cdUp();
                    tiff_dir.cdUp();
                }
            }
        }
        else
        {
            _export_maps_dialog->status_callback(cur, view_cnt);
            for (int vidx = 0; vidx < view_cnt; vidx++)
            {
                QString element = _imageViewWidget->getLabelAt(vidx);
                std::string el_str = element.toStdString();

                job_queue[el_str] = Global_Thread_Pool::inst()->enqueue([this, vidx, analysis_text, element] { return generate_pixmap(analysis_text, element.toStdString(), _chk_log_color->isChecked(), vidx); });


                // save normalized 32 bit float tiff
                std::string save_file_name_fp = analysis_text + "-" + el_str + "-";
                Calibration_curve<double>* calib_curve = _model->get_calibration_curve(analysis_text, _cb_normalize->currentText().toStdString());
                TIFF* tif = nullptr;
                data_struct::ArrayXXr<float> counts;

                data_struct::Fit_Count_Dict<float> element_counts;
                _model->getAnalyzedCounts(analysis_text, element_counts);
                if (element_counts.count(el_str) > 0)
                {
                    counts = element_counts.at(el_str);

                    if (calib_curve != nullptr && _normalizer != nullptr)
                    {
                        if (calib_curve->calib_curve.count(el_str) > 0)
                        {
                            save_file_name_fp += _cb_normalize->currentText().toStdString();

                            float calib_val = static_cast<float>(calib_curve->calib_curve.at(el_str));

                            counts /= (*_normalizer);
                            counts /= calib_val;
                        }
                    }
                }
                else if (_model->getScalers()->count(el_str) > 0)
                {
                    counts = _model->getScalers()->at(el_str);
                }
                save_file_name_fp += ".tif";

                QString save_path = QDir::cleanPath(tiff_dir.absolutePath() + QDir::separator() + QString(save_file_name_fp.c_str()));
                if ((tif = TIFFOpen(save_path.toStdString().c_str(), "w")) == NULL)
                {
                    logE << "Could not open " << save_path.toStdString() << " for writing\n";
                    continue;
                }

                TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, counts.cols());
                TIFFSetField(tif, TIFFTAG_IMAGELENGTH, counts.rows());
                TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 32);
                TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
                TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, counts.rows());
                TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP);
                //TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
                TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
                TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
                //TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);

                // Write the information to the file
                // need to normaize the data by IC also 
                TIFFWriteEncodedStrip(tif, 0, counts.data(), 4 * counts.cols() * counts.rows());
                TIFFClose(tif);

            }

            while (job_queue.size() > 0)
            {
                std::vector<std::string> to_delete;
                for (auto& itr : job_queue)
                {
                    QPixmap pixmap = itr.second.get();

                    if (_export_maps_dialog->get_save_png())
                    {
                        if (false == pixmap.save(QDir::cleanPath(png_dir.absolutePath() + QDir::separator() + QString(itr.first.c_str()) + ".png"), "PNG"))
                        {
                            logE << "Could not save PNG for " << QDir::cleanPath(png_dir.absolutePath() + QDir::separator() + QString(itr.first.c_str()) + ".png").toStdString() << "\n";
                        }
                    }
                    if (_export_maps_dialog->get_save_tiff())
                    {
                        //save 8 bit tiff
                        if (false == pixmap.save(QDir::cleanPath(tif8_dir.absolutePath() + QDir::separator() + QString(itr.first.c_str()) + ".tif"), "TIFF"))
                        {
                            logE << "Could not save 8 bit TIFF for " << QDir::cleanPath(tif8_dir.absolutePath() + QDir::separator() + QString(itr.first.c_str()) + ".tif").toStdString() << "\n";
                        }
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
        }
    }
    if (_export_maps_dialog->get_save_ascii())
    {
        ascii_dir.mkdir("ASCII");
        ascii_dir.cd("ASCII");

        std::vector<std::string> normalizers = { STR_DS_IC , STR_US_IC, STR_SR_CURRENT, "Counts" };

        const std::vector<float> x_axis = _model->get_x_axis();
        const std::vector<float> y_axis = _model->get_y_axis();

        std::vector<std::string> analysis_types = _model->getAnalyzedTypes();

        size_t cur = 0;
        size_t total = analysis_types.size() * x_axis.size() * y_axis.size() * normalizers.size();
        _export_maps_dialog->status_callback(cur, total);


        for (auto& a_itr : analysis_types)
        {
            data_struct::Fit_Count_Dict<float> element_counts;
            _model->getAnalyzedCounts(a_itr, element_counts);

            data_struct::ArrayXXr<float>* normalizer = nullptr;

            ascii_dir.mkdir(QString(a_itr.c_str()));
            ascii_dir.cd(QString(a_itr.c_str()));

            for (auto n_itr : normalizers)
            {

                ascii_dir.mkdir(QString(n_itr.c_str()));
                ascii_dir.cd(QString(n_itr.c_str()));

                std::map<std::string, data_struct::ArrayXXr<float>>* scalers = _model->getScalers();
                if (scalers->count(n_itr) > 0)
                {
                    normalizer = &(scalers->at(n_itr));
                }
                Calibration_curve<double>* calib_curve = _model->get_calibration_curve(a_itr, n_itr);

                std::string save_file_name = ascii_dir.absolutePath().toStdString() + QDir::separator().toLatin1() + _model->getDatasetName().toStdString();

                std::string sub_save_file = save_file_name + "-" + a_itr + "-" + n_itr + ".csv";
                std::ofstream out_stream(sub_save_file);

                logI << save_file_name << "\n";

                if (out_stream.is_open())
                {

                    out_stream << "ascii information for file: " << _model->getDatasetName().toStdString() << "\n";



                    if (a_itr == STR_FIT_ROI || n_itr == "Counts")
                    {
                        out_stream << "Analysis " << a_itr << " in cts/s \n";
                    }
                    else
                    {
                        out_stream << "Analysis " << a_itr << " Normalized by " << n_itr << " ug/cm2 \n";
                    }

                    out_stream << "Y Pixel, X Pixel, Y Position, X Position, ";
                    for (auto& e_itr : element_counts)
                    {
                        out_stream << e_itr.first;
                        if (calib_curve != nullptr && normalizer != nullptr)
                        {
                            if (calib_curve->calib_curve.count(e_itr.first) > 0)
                            {
                                out_stream << " (ug/cm2) ";
                            }
                            else
                            {
                                out_stream << " (cts/s) ";
                            }
                        }
                        else
                        {
                            out_stream << " (cts/s) ";
                        }
                        out_stream << " , ";
                    }
                    out_stream << "\n";

                    for (int yidx = 0; yidx < y_axis.size(); yidx++)
                    {
                        for (int xidx = 0; xidx < x_axis.size(); xidx++)
                        {

                            out_stream << yidx << " , " << xidx << " , " << y_axis.at(yidx) << " , " << x_axis.at(xidx) << " , ";

                            for (auto& e_itr : element_counts)
                            {
                                float calib_val = 1.0;
                                double val = 1.0;
                                float e_val = (e_itr.second)(yidx, xidx);
                                if (a_itr == STR_FIT_ROI || n_itr == "Counts")
                                {
                                    val = e_val;
                                }
                                else
                                {
                                    if (calib_curve != nullptr && normalizer != nullptr)
                                    {
                                        if (calib_curve->calib_curve.count(e_itr.first) > 0)
                                        {
                                            calib_val = static_cast<float>(calib_curve->calib_curve.at(e_itr.first));
                                            float n_val = (*normalizer)(yidx, xidx);
                                            val = e_val / n_val / calib_val;
                                        }
                                        else
                                        {
                                            val = e_val;
                                        }
                                    }
                                    else
                                    {
                                        val = e_val;
                                    }
                                }
                                out_stream << val << " , ";
                            }
                            cur++;
                            _export_maps_dialog->status_callback(cur, total);
                            out_stream << "\n";
                        }
                        if (false == _export_maps_dialog->isActiveWindow())
                        {
                            out_stream.close();
                            _export_maps_dialog->setRunEnabled(true);
                            return;
                        }
                        out_stream << "\n";
                    }
                    out_stream.close();
                }
                else
                {
                    logE << "Could not save PNG for " << save_file_name << "\n";
                }

                ascii_dir.cdUp();
            }
            ascii_dir.cdUp();
        }
    }
    if (_export_maps_dialog->get_save_screen())
    {

        QPixmap pixmap(_counts_window->rect().size());
        _counts_window->render(&pixmap, QPoint(), QRegion(_counts_window->rect()));

        QDateTime date = QDateTime::currentDateTime();
        QString formattedTime = date.toString("yyyy.MM.dd_hh.mm.ss");
        QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();

        qDebug() << "Date:" + formattedTime;
        if (false == pixmap.save(QDir::cleanPath(export_model_dir.absolutePath() + QDir::separator() + QString("screenshot_"+formattedTime+".png")), "PNG"))
        {
            logE << "Could not save PNG for " << QDir::cleanPath(export_model_dir.absolutePath() + QDir::separator() + QString("screenshot_" + formattedTime + ".png")).toStdString() << "\n";
        }
    }

    _export_maps_dialog->setRunEnabled(true);
    _export_maps_dialog->on_open();
    _export_maps_dialog->close();
}
*/

