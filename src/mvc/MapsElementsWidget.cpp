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

#include <limits>


using gstar::AbstractImageWidget;
using gstar::ImageViewWidget;

/*---------------------------------------------------------------------------*/

MapsElementsWidget::MapsElementsWidget(int rows, int cols, QWidget* parent)
    : AbstractImageWidget(rows, cols, parent)
{

    _model = nullptr;
    for (int i = 0; i < 256; ++i)
    {
        _gray_colormap.append(qRgb(i, i, i));
        _heat_colormap.append(qRgb(i, 0, 0));
    }
	_selected_colormap = &_gray_colormap;
    _createLayout();

}

/*---------------------------------------------------------------------------*/

MapsElementsWidget::~MapsElementsWidget()
{
/*
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

void MapsElementsWidget::_createLayout()
{

    _tab_widget = new QTabWidget();
    _spectra_widget = new FitSpectraWidget();

    _cb_analysis = new QComboBox(this);
    _cb_element = new QComboBox(this);

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
    createToolBar(m_imageViewWidget);
    counts_layout->addWidget(m_toolbar);
    counts_layout->addWidget(splitter);

	_cb_colormap = new QComboBox();
	_cb_colormap->addItem("Grayscale");
	_cb_colormap->addItem("Heatmap");
    connect(_cb_colormap, SIGNAL(currentIndexChanged(QString)), this, SLOT(onColormapSelect(QString)));

    m_toolbar->addWidget(new QLabel(" ColorMap :"));
    m_toolbar->addWidget(_cb_colormap);

	_grid_button = new QPushButton();
	_grid_button->setIcon(QIcon(":/images/grid.png"));
	_grid_button->setIconSize(QSize(15, 15)); 

	connect(_grid_button, SIGNAL(pressed()), this, SLOT(onGridDialog()));

	m_toolbar->addWidget(_grid_button);
	m_toolbar->addWidget(_cb_analysis);
	m_toolbar->addWidget(_cb_element);
	
    //hbox->addWidget(_cb_analysis);
    //hbox->addWidget(_cb_element);
    //counts_layout->addItem(hbox);

    //_pb_perpixel_fitting = new QPushButton("Per Pixel Fitting");
    //counts_layout->addWidget(_pb_perpixel_fitting);


    QWidget *window = new QWidget();
    window->setLayout(counts_layout);

    _tab_widget->addTab(window, "Counts");
    _tab_widget->addTab(_spectra_widget, "Integrated Spectra");

    layout->addItem(hbox2);
    layout->addWidget(_tab_widget);


    _counts_lookup = new gstar::CountsLookupTransformer();
    _counts_coord_model = new gstar::CoordinateModel(_counts_lookup);

    _counts_coord_widget = new gstar::CoordinateWidget();
    //_counts_coord_widget->setVisible(true, false, false);
    _counts_coord_widget->setModel(_counts_coord_model);
    _counts_coord_widget->setLabel("Counts:", "Min:", "Max:");
    _counts_coord_widget->setUnitsLabel("cts/s");
    m_imageViewWidget->layout()->addWidget(_counts_coord_widget);

    //don't erase counts when mouse is off scene
    m_imageViewWidget->set_null_mouse_pos = false;

    gstar::ImageViewScene* scene = m_imageViewWidget->scene();

    connect(scene, SIGNAL(mouseOverPixel(int, int)),
            this, SLOT(mouseOverPixel(int, int)));

    appendAnnotationTab();

    createActions();

    setLayout(layout);

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::onGridDialog()
{
	
	iDiag.show();

}

void MapsElementsWidget::onNewGridLayout(int rows, int cols)
{
	m_imageViewWidget->newGridLayout(rows, cols);
}
/*---------------------------------------------------------------------------*/

void MapsElementsWidget::addHotSpotMask()
{
    int w = m_imageViewWidget->scene()->getPixmapItem()->pixmap().width();
    int h = m_imageViewWidget->scene()->getPixmapItem()->pixmap().height();
   gstar::HotSpotMaskGraphicsItem* annotation = new gstar::HotSpotMaskGraphicsItem(w, h);
   insertAndSelectAnnotation(m_treeModel, m_annoTreeView, m_selectionModel, annotation);

   connect(annotation, SIGNAL(mask_updated()), this, SLOT(hotspotUpdated()));

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::hotspotUpdated()
{
    redrawCounts();
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::mouseOverPixel(int x, int y)
{

   _counts_coord_widget -> setCoordinate(x, y, 0);

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::createActions()
{
    AbstractImageWidget::createActions();
    _addHotSpotMaskAction = new QAction("Add Hotspot Mask", this);

    connect(_addHotSpotMaskAction,
            SIGNAL(triggered()),
            this,
            SLOT(addHotSpotMask()));
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

void MapsElementsWidget::onAnalysisSelect(QString name)
{	
    QString elementName = _cb_element->currentText();
    bool found_element = false;
	if (_model != nullptr)
	{
		data_struct::Fit_Count_Dict* analysis_type = _model->getAnalyzedCounts(name.toStdString());
		if (analysis_type != nullptr)
		{
			disconnect(_cb_element, SIGNAL(currentIndexChanged(QString)), this, SLOT(onElementSelect(QString)));
			_cb_element->clear();
			//std::vector<std::string> elements = analysis_type->get_count_names();
			std::string lastName;
			for (auto& itr : *analysis_type)
			{
				QString val = QString(itr.first.c_str());
				_cb_element->addItem(val);
				if (elementName == val)
				{
					found_element = true;
				}
				lastName = itr.first;
			}
			if (found_element)
			{
				_cb_element->setCurrentText(elementName);
				displayCounts(name.toStdString(), elementName.toStdString());
			}
			else
			{
				_cb_element->setCurrentText(QString(lastName.c_str()));
				displayCounts(name.toStdString(), lastName);
			}
			connect(_cb_element, SIGNAL(currentIndexChanged(QString)), this, SLOT(onElementSelect(QString)));
		}
	}
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::onElementSelect(QString name)
{
    QString analysisName = _cb_analysis->currentText();
    if(analysisName.length() > 0 && name.length() > 0)
    {
        displayCounts(analysisName.toStdString() , name.toStdString());
    }
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::onColormapSelect(QString name)
{
    QString colormap = _cb_colormap->currentText();
	if(colormap == "Grayscale")
	{
		_selected_colormap = &_gray_colormap;
	}
	else
	{
		_selected_colormap = &_heat_colormap;
	}
	redrawCounts();
}

 /*---------------------------------------------------------------------------*/

void MapsElementsWidget::setModel(MapsH5Model* model,
                                  data_struct::Fit_Parameters* fit_params,
                                  data_struct::Fit_Element_Map_Dict *elements_to_fit)
{
    _model = model;
    _counts_lookup->setModel(model);
    model_updated();

	if (_model != nullptr)
	{
		if (_model->is_integrated_spectra_loaded())
		{
			//_spectra_widget->setModels(fit_params, elements_to_fit, model);
			_spectra_widget->setModels(_model, fit_params, elements_to_fit);
		}
	}
	else
	{
		_spectra_widget->setModels(nullptr, nullptr, nullptr);
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
    disconnect(_cb_element, SIGNAL(currentIndexChanged(QString)), this, SLOT(onElementSelect(QString)));

    _dataset_directory->setText(_model->getFilePath());
    _dataset_name->setText(_model->getDatasetName());

    std::string current_a;
    std::string current_e;

    QString current_analysis = _cb_analysis->currentText();
    QString current_element = _cb_element->currentText();

    _cb_analysis->clear();
    std::vector<std::string> analysis_types = _model->getAnalyzedTypes();

    //m_imageWidgetToolBar->clearImageViewWidget();
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

    data_struct::Fit_Count_Dict* analysis_type = _model->getAnalyzedCounts(current_a);
    bool found_element = false;
    if(analysis_type != nullptr)
    {
        _cb_element->clear();
        for(auto& itr: *analysis_type)
        {
            QString val = QString(itr.first.c_str());
            _cb_element->addItem(val);
            current_e = itr.first;
            if(current_element == val)
            {
                found_element = true;
            }
        }
    }

    if(false == found_analysis)
    {
        current_analysis = QString(current_a.c_str());
    }
    if(false == found_element)
    {
        current_element = QString(current_e.c_str());
    }

    _cb_analysis->setCurrentText(current_analysis);
    _cb_element->setCurrentText(current_element);
    displayCounts(current_analysis.toStdString(), current_element.toStdString());

    connect(_cb_analysis, SIGNAL(currentIndexChanged(QString)), this, SLOT(onAnalysisSelect(QString)));
    connect(_cb_element, SIGNAL(currentIndexChanged(QString)), this, SLOT(onElementSelect(QString)));
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::redrawCounts()
{
    displayCounts(_cb_analysis->currentText().toStdString(), _cb_element->currentText().toStdString());
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::_get_min_max_vals(float &min_val, float &max_val, const Eigen::Array<real_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& element_counts)
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

void MapsElementsWidget::displayCounts(const std::string analysis_type, const std::string element)
{
	if (_model != nullptr)
	{
		data_struct::Fit_Count_Dict* fit_counts = _model->getAnalyzedCounts(analysis_type);
		if (fit_counts != nullptr)
		{
			if (fit_counts->count(element) > 0)
			{
				_counts_lookup->setAnalyaisElement(analysis_type, element);
				_counts_coord_widget->setCoordinate(0, 0, 0);
				int height = static_cast<int>(fit_counts->at(element).rows());
				int width = static_cast<int>(fit_counts->at(element).cols());
				m_imageHeightDim->setCurrentText(QString::number(height));
				m_imageWidthDim->setCurrentText(QString::number(width));
				QImage image(width, height, QImage::Format_Indexed8);
				image.setColorTable(*_selected_colormap);

				float counts_max;
				float counts_min;
				_get_min_max_vals(counts_min, counts_max, fit_counts->at(element));
				float max_min = counts_max - counts_min;
				for (int row = 0; row < height; row++)
				{
					for (int col = 0; col < width; col++)
					{
						//first clamp the data to max min
						float cnts = fit_counts->at(element)(row, col);
						cnts = std::min(counts_max, cnts);
						cnts = std::max(counts_min, cnts);
						//convert to pixel
						uint data = (uint)(((cnts - counts_min) / max_min) * 255);
						image.setPixel(col, row, data);
					}
				}
				m_imageViewWidget->scene()->setPixmap(QPixmap::fromImage(image.convertToFormat(QImage::Format_RGB32)));
			}
		}
	}
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
