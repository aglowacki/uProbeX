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
#include <gstar/CountsLookupTransformer.h>
#include <limits>


using gstar::AbstractImageWidget;
using gstar::ImageViewWidget;

/*---------------------------------------------------------------------------*/

MapsElementsWidget::MapsElementsWidget(int rows, int cols, QWidget* parent)
    : AbstractImageWidget(rows, cols, parent)
{

    _model = nullptr;
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
    //m_toolbar->addWidget(_cb_element);
	
    //_pb_perpixel_fitting = new QPushButton("Per Pixel Fitting");
    //counts_layout->addWidget(_pb_perpixel_fitting);

    QWidget *window = new QWidget();
    window->setLayout(counts_layout);

    _tab_widget->addTab(window, "Analyzed Counts");
    _tab_widget->addTab(_spectra_widget, "Integrated Spectra");

    layout->addItem(hbox2);
    layout->addWidget(_tab_widget);

    //don't erase counts when mouse is off scene
    m_imageViewWidget->set_null_mouse_pos = false;
    connect(m_imageViewWidget, SIGNAL(cbLabelChanged(QString, int)), this, SLOT(onElementSelect(QString, int)));

    appendAnnotationTab();

    createActions();

    setLayout(layout);

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::onGridDialog()
{
	
	iDiag.show();

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::onNewGridLayout(int rows, int cols)
{
    const std::vector<QString> element_view_list = m_imageViewWidget->getLabelList();
    m_imageViewWidget->newGridLayout(rows, cols);
    model_updated();
    m_imageViewWidget->restoreLabels(element_view_list);
    redrawCounts();
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

    redrawCounts();

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::onElementSelect(QString name, int viewIdx)
{
    QString analysisName = _cb_analysis->currentText();
    if(analysisName.length() > 0 && name.length() > 0)
    {
        displayCounts(analysisName.toStdString() , name.toStdString(), viewIdx);
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

void MapsElementsWidget::setModel(MapsH5Model* model)
{
    _model = model;
    ////_counts_lookup->setModel(model);
    model_updated();

    _spectra_widget->setH5Model(_model);
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

    data_struct::Fit_Count_Dict* element_counts = _model->getAnalyzedCounts(current_a);

    const std::vector<QString> element_view_list = m_imageViewWidget->getLabelList();
    if(element_counts != nullptr)
    {
        m_imageViewWidget->clearLabels();
        //insert in z order
        for (std::string el_name : element_lines)
        {
            if(element_counts->count(el_name) > 0)
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

    if(false == found_analysis)
    {
        current_analysis = QString(current_a.c_str());
    }
    _cb_analysis->setCurrentText(current_analysis);

    m_imageViewWidget->restoreLabels(element_view_list);

    redrawCounts();

    connect(_cb_analysis, SIGNAL(currentIndexChanged(QString)), this, SLOT(onAnalysisSelect(QString)));
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::redrawCounts()
{
    int view_cnt = m_imageViewWidget->getViewCount();
    std::string analysis_text = _cb_analysis->currentText().toStdString();
    for(int vidx = 0; vidx < view_cnt; vidx++)
    {
        QString element = m_imageViewWidget->getLabelAt(vidx);
        displayCounts(analysis_text, element.toStdString(), vidx);
    }
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

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::displayCounts(const std::string analysis_type, const std::string element, int grid_idx)
{
	if (_model != nullptr)
	{
		data_struct::Fit_Count_Dict* fit_counts = _model->getAnalyzedCounts(analysis_type);
		if (fit_counts != nullptr)
		{
            if (fit_counts->count(element) > 0)
            {
                gstar::CountsLookupTransformer* counts_lookup = m_imageViewWidget->getMouseTrasnformAt(grid_idx);
                if(counts_lookup != nullptr)
                {
                    counts_lookup->setModel(_model);
                    counts_lookup->setAnalyaisElement(analysis_type, element);
                }
                m_imageViewWidget->resetCoordsToZero();

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
                m_imageViewWidget->scene(grid_idx)->setPixmap(QPixmap::fromImage(image.convertToFormat(QImage::Format_RGB32)));
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
