/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MultiElementsWidget.h>

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


using gstar::AbstractImageWidget;
using gstar::ImageViewWidget;


/*---------------------------------------------------------------------------*/

MultiElementsWidget::MultiElementsWidget(int rows, int cols, QWidget* parent)
    : AbstractImageWidget(rows, cols, parent)
{
    _normalizer = nullptr;
    _calib_curve = nullptr;
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
    _createLayout();
}

/*---------------------------------------------------------------------------*/

MultiElementsWidget::~MultiElementsWidget()
{

}

/*---------------------------------------------------------------------------*/

void MultiElementsWidget::_createLayout()
{
    _cb_analysis = new QComboBox(this);
    QVBoxLayout* counts_layout = new QVBoxLayout();
    
	connect(&iDiag, SIGNAL(onNewGridLayout(int, int)), this, SLOT(onNewGridLayout(int, int)));

    QSplitter* splitter = new QSplitter();
    splitter->setOrientation(Qt::Horizontal);
    splitter->addWidget(m_imageViewWidget);
    splitter->setStretchFactor(0, 1);
    splitter->addWidget(m_tabWidget);

    createToolBar(m_imageViewWidget);
    counts_layout->addWidget(m_toolbar);
    counts_layout->addWidget(splitter);

	_cb_colormap = new QComboBox();
	_cb_colormap->addItem(STR_COLORMAP_GRAY);
	_cb_colormap->addItem(STR_COLORMAP_HEAT);
    connect(_cb_colormap, SIGNAL(currentIndexChanged(QString)), this, SLOT(onColormapSelect(QString)));

    m_toolbar->addWidget(new QLabel(" ColorMap :"));
    m_toolbar->addWidget(_cb_colormap);

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
	
    //don't erase counts when mouse is off scene
    m_imageViewWidget->set_null_mouse_pos = false;
    connect(m_imageViewWidget, SIGNAL(cbLabelChanged(QString, int)), this, SLOT(onElementSelect(QString, int)));

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

    setLayout(counts_layout);

}

/*---------------------------------------------------------------------------*/

void MultiElementsWidget::onGridDialog()
{
	
	iDiag.show();

}

/*---------------------------------------------------------------------------*/

void MultiElementsWidget::onNewGridLayout(int rows, int cols)
{
    const std::vector<QString> element_view_list = m_imageViewWidget->getLabelList();
    m_imageViewWidget->newGridLayout(rows, cols);
    model_updated();
    m_imageViewWidget->restoreLabels(element_view_list);
    redrawCounts();
    Preferences::inst()->setValue(STR_GRID_ROWS,rows);
    Preferences::inst()->setValue(STR_GRID_COLS,cols);
    Preferences::inst()->save();
}

/*---------------------------------------------------------------------------*/

void MultiElementsWidget::addHotSpotMask()
{
    int w = m_imageViewWidget->scene()->getPixmapItem()->pixmap().width();
    int h = m_imageViewWidget->scene()->getPixmapItem()->pixmap().height();
   gstar::HotSpotMaskGraphicsItem* annotation = new gstar::HotSpotMaskGraphicsItem(w, h);
   insertAndSelectAnnotation(m_treeModel, m_annoTreeView, m_selectionModel, annotation);

   //QString name = ano->getName();
   //_spectra_widget->appendROISpectra()
   //            //data_struct Spectra = _model->load_roi(annotation->getROI());

   connect(annotation, &gstar::HotSpotMaskGraphicsItem::mask_updated, this, &MultiElementsWidget::roiUpdated);

}

/*---------------------------------------------------------------------------*/

void MultiElementsWidget::roiUpdated(gstar::HotSpotMaskGraphicsItem* ano, bool reload)
{
    if (ano != nullptr && reload)
    {
        {

        }
    }
    
}

/*---------------------------------------------------------------------------*/

void MultiElementsWidget::createActions()
{
    AbstractImageWidget::createActions();
    // TODO: change hotspot to spectra region and add back in
    
    _addHotSpotMaskAction = new QAction("Add ROI Mask", this);

    connect(_addHotSpotMaskAction,
            SIGNAL(triggered()),
            this,
            SLOT(addHotSpotMask()));
            
}

/*---------------------------------------------------------------------------*/

void MultiElementsWidget::displayContextMenu(QWidget* parent,
                                             const QPoint& pos)
{

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

}

/*---------------------------------------------------------------------------*/

void MultiElementsWidget::onAnalysisSelect(QString name)
{	
	if (_model)
	{
		_calib_curve = _model->get_calibration_curve(name.toStdString(), _cb_normalize->currentText().toStdString());
		redrawCounts();
	}
}

/*---------------------------------------------------------------------------*/

void MultiElementsWidget::onElementSelect(QString name, int viewIdx)
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
        displayCounts(analysisName.toStdString() , name.toStdString(), viewIdx);
    }
}

/*---------------------------------------------------------------------------*/

void MultiElementsWidget::onColormapSelect(QString colormap)
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
	redrawCounts();
    
    Preferences::inst()->save();
}

/*---------------------------------------------------------------------------*/

void MultiElementsWidget::onSelectNormalizer(QString name)
{
    if (name == "1")
    {
        _normalizer = nullptr;
        _calib_curve = nullptr;
        m_imageViewWidget->setUnitLabels("cts/s");
    }
    else
    {
        if (_model)
        {
            std::unordered_map<std::string, data_struct::ArrayXXr>* scalers = _model->getScalers();
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

void MultiElementsWidget::setModel(std::shared_ptr<MapsH5Model> model)
{
    if (model)
    {
        _model = model;
        model_updated();
        m_imageWidgetToolBar->clickFill();
    }
}

/*---------------------------------------------------------------------------*/

void MultiElementsWidget::model_updated()
{
	if (_model)
	{
		disconnect(_cb_analysis, SIGNAL(currentIndexChanged(QString)), this, SLOT(onAnalysisSelect(QString)));

		std::string current_a;

		QString current_analysis = _cb_analysis->currentText();

		QString save_normal_val = _cb_normalize->currentText();

		_cb_normalize->clear();
		_cb_normalize->addItem("1");

		std::unordered_map<std::string, data_struct::ArrayXXr>* scalers = _model->getScalers();

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
		for (auto& itr : analysis_types)
		{
			QString newVal = QString(itr.c_str());
			_cb_analysis->addItem(newVal);
			current_a = itr;
			if (current_analysis == newVal)
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
			element_lines.push_back(el_name + "_L");
		}
		for (std::string el_name : data_struct::Element_Symbols)
		{
			element_lines.push_back(el_name + "_M");
		}

		element_lines.push_back(STR_COHERENT_SCT_AMPLITUDE);
		element_lines.push_back(STR_COMPTON_AMPLITUDE);
		element_lines.push_back(STR_SUM_ELASTIC_INELASTIC_AMP);
		element_lines.push_back(STR_TOTAL_FLUORESCENCE_YIELD);
		element_lines.push_back(STR_NUM_ITR);
		element_lines.push_back(STR_RESIDUAL);

		data_struct::Fit_Count_Dict* element_counts = _model->getAnalyzedCounts(current_a);

		const std::vector<QString> element_view_list = m_imageViewWidget->getLabelList();
		if (element_counts != nullptr)
		{
			m_imageViewWidget->clearLabels();
			//insert in z order
			for (std::string el_name : element_lines)
			{
				if (element_counts->count(el_name) > 0)
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
		}

		for (auto& itr : *scalers)
		{
			m_imageViewWidget->addLabel(QString(itr.first.c_str()));
		}

		if (false == found_analysis)
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
}

/*---------------------------------------------------------------------------*/

void MultiElementsWidget::redrawCounts()
{
    int view_cnt = m_imageViewWidget->getViewCount();
    std::string analysis_text = _cb_analysis->currentText().toStdString();

    if (view_cnt == 1)
    {
        for (int vidx = 0; vidx < view_cnt; vidx++)
        {
            QString element = m_imageViewWidget->getLabelAt(vidx);
            displayCounts(analysis_text, element.toStdString(), vidx);
        }
    }
    else
    {
        std::map<int, std::future<QPixmap> > job_queue;

        for (int vidx = 0; vidx < view_cnt; vidx++)
        {
            QString element = m_imageViewWidget->getLabelAt(vidx);
            
            job_queue[vidx] = Global_Thread_Pool::inst()->enqueue([this, vidx, analysis_text, element] { return generate_pixmap(analysis_text, element.toStdString(), vidx); });
        }

        while (job_queue.size() > 0)
        {
            std::vector<int> to_delete;
            for (auto& itr : job_queue)
            {
                m_imageViewWidget->resetCoordsToZero();
                m_imageViewWidget->scene(itr.first)->setPixmap(itr.second.get());

                m_imageHeightDim->setCurrentText(QString::number(m_imageViewWidget->scene(itr.first)->height()));
                m_imageWidthDim->setCurrentText(QString::number(m_imageViewWidget->scene(itr.first)->width()));
                to_delete.push_back(itr.first);
            }

            for (const auto& itr : to_delete)
            {
                job_queue.erase(itr);
            }
        }
    }

}

/*---------------------------------------------------------------------------*/
/*
void MultiElementsWidget::_get_min_max_vals(float &min_val, float &max_val, const Eigen::Array<real_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& element_counts)
{
    
    gstar::HotSpotMaskGraphicsItem item(0,0);

    QList<gstar::AbstractGraphicsItem*>  hotspots;
    if(m_treeModel != nullptr)
    {
        hotspots = m_treeModel->get_all_of_type( item.classId() );
    }
    
    QImage *sum_of_masks = nullptr;
    if(hotspots.size() > 0)
    {
        foreach(gstar::AbstractGraphicsItem* item , hotspots)
        {
            gstar::HotSpotMaskGraphicsItem* mask = dynamic_cast<gstar::HotSpotMaskGraphicsItem*>(item);
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

void MultiElementsWidget::displayCounts(const std::string analysis_type, const std::string element, int grid_idx)
{
	if (_model)
	{
		data_struct::Fit_Count_Dict* fit_counts = _model->getAnalyzedCounts(analysis_type);
        data_struct::Fit_Count_Dict* scalers = _model->getScalers();
        ArrayXXr normalized;
        bool draw = false;
        int height = 0;
        int width = 0;
        if (fit_counts != nullptr)
        {
            if (fit_counts->count(element) > 0)
            {
                height = static_cast<int>(fit_counts->at(element).rows());
                width = static_cast<int>(fit_counts->at(element).cols());
                normalized = fit_counts->at(element);
                draw = true;
            }
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
            m_imageHeightDim->setCurrentText(QString::number(height));
            m_imageWidthDim->setCurrentText(QString::number(width));
            QImage image(width, height, QImage::Format_Indexed8);
            image.setColorTable(*_selected_colormap);

            float counts_max;
            float counts_min;
            if (_normalizer != nullptr && _calib_curve != nullptr)
            {
                if (_calib_curve->calib_curve.count(element) > 0)
                {
                    real_t calib_val = _calib_curve->calib_curve.at(element);
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

QPixmap MultiElementsWidget::generate_pixmap(const std::string analysis_type, const std::string element, int grid_idx)
{
    if (_model)
    {
        data_struct::Fit_Count_Dict* fit_counts = _model->getAnalyzedCounts(analysis_type);
        data_struct::Fit_Count_Dict* scalers = _model->getScalers();
        ArrayXXr normalized;
        bool draw = false;
        int height = 0;
        int width = 0;
        if (fit_counts != nullptr)
        {
            if (fit_counts->count(element) > 0)
            {
                height = static_cast<int>(fit_counts->at(element).rows());
                width = static_cast<int>(fit_counts->at(element).cols());
                normalized = fit_counts->at(element);
                draw = true;
            }
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
            QImage image(width, height, QImage::Format_Indexed8);
            image.setColorTable(*_selected_colormap);

            float counts_max;
            float counts_min;
            if (_normalizer != nullptr && _calib_curve != nullptr)
            {
                if (_calib_curve->calib_curve.count(element) > 0)
                {
                    real_t calib_val = _calib_curve->calib_curve.at(element);
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

void MultiElementsWidget::windowChanged(Qt::WindowStates oldState,
                                       Qt::WindowStates newState)
{
    Q_UNUSED(oldState);

    if(Qt::WindowMaximized || Qt::WindowActive == newState)
    {
        m_imageViewWidget->resizeEvent(nullptr);
    }

}

/*---------------------------------------------------------------------------*/
