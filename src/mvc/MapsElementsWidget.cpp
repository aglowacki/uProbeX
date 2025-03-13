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
#include <tiffio.h>
#include "io/file/aps/aps_fit_params_import.h"
#include <gstar/CountsLookupTransformer.h>
#include <limits>
#include "core/GlobalThreadPool.h"
#include "io/file/csv_io.h"
#include "core/ColorMap.h"

using gstar::AbstractImageWidget;
using gstar::ImageViewWidget;

#define ANNO_TAB 0
#define ROI_TAB 1

//---------------------------------------------------------------------------

MapsElementsWidget::MapsElementsWidget(int rows, int cols, bool create_image_nav, bool restore_floating, QWidget* parent)
    : AbstractImageWidget(rows, cols, parent)
{
    
    _model = nullptr;
    _roi_stats_diag = nullptr;
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

    connect(&_img_seg_diag, &ImageSegRoiDialog::onNewROIs, this, &MapsElementsWidget::on_add_new_ROIs);
    setAttribute(Qt::WA_DeleteOnClose, true);
    _createLayout(create_image_nav, restore_floating);
}

//---------------------------------------------------------------------------

MapsElementsWidget::~MapsElementsWidget()
{

    _co_loc_widget->setModel(nullptr);
    _scatter_plot_widget->setModel(nullptr);
/* //this is done elsewhere . should refactor it to be smart pointer
    if(_model != nullptr)
    {
        delete _model;
    }
    _model = nullptr;
*/
}

//---------------------------------------------------------------------------

void MapsElementsWidget::_createLayout(bool create_image_nav, bool restore_floating)
{

    QHBoxLayout *tmp_layout;
    QWidget *tmp_widget;

    _tw_image_controls = new QTabWidget();

    _tab_widget = new QTabWidget();
    _spectra_widget = new FitSpectraWidget();
    connect(_spectra_widget, &FitSpectraWidget::export_fit_paramters, this, &MapsElementsWidget::on_export_fit_params);
    connect(_spectra_widget, &FitSpectraWidget::export_csv_and_png, this, &MapsElementsWidget::on_export_csv_and_png);

    _cb_analysis = new QComboBox(this);

    //QWidget* toolbar_widget = new QWidget();
    //QHBoxLayout* toolbar_hbox = new QHBoxLayout();

    QHBoxLayout* hbox = new QHBoxLayout();
    QHBoxLayout* hbox2 = new QHBoxLayout();
    QVBoxLayout* counts_layout = new QVBoxLayout();
    QVBoxLayout* layout = new QVBoxLayout();

	connect(&iDiag, &ImageGridDialog::onNewGridLayout, this, &MapsElementsWidget::onNewGridLayout);

    _dataset_directory = new QLabel();
    _dataset_name = new QLabel();
    hbox2->addWidget(new QLabel("Dataset: "));
    hbox2->addWidget(_dataset_directory);
    //hbox2->addWidget(_dataset_name);
    hbox2->addItem(new QSpacerItem(9999, 40, QSizePolicy::Maximum));


    _anim_widget = new AnnimateSlideWidget();
    _anim_widget->setAnimWidget(m_tabWidget);
      
    QSplitter* splitter = new QSplitter();
    splitter->setOrientation(Qt::Horizontal);
    splitter->addWidget(m_imageViewWidget);
    splitter->setStretchFactor(0, 1);
    splitter->addWidget(_anim_widget);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, true);

    createToolBar(m_imageViewWidget, create_image_nav);

	_cb_colormap = new QComboBox();
	_cb_colormap->addItem(STR_COLORMAP_GRAY);
	_cb_colormap->addItem(STR_COLORMAP_HEAT);

    const std::map<QString, QVector<QRgb> >* color_maps = ColorMap::inst()->color_maps();
    for (auto itr : *color_maps)
    {
        _cb_colormap->addItem(itr.first);
    }

    connect(_cb_colormap, &QComboBox::currentTextChanged, this, &MapsElementsWidget::onColormapSelect);

    _color_map_ledgend_lbl = new QLabel();
    _color_maps_ledgend = new QImage(256, 10, QImage::Format_Indexed8);
    _color_maps_ledgend->setColorTable(_gray_colormap);
    for (uint c = 0; c < 256; c++)
    {
        for (int r = 0; r < 10; r++)
        {
            _color_maps_ledgend->setPixel(c, r, c);
        }
    }
    
    _color_map_ledgend_lbl->setPixmap(QPixmap::fromImage(_color_maps_ledgend->convertToFormat(QImage::Format_RGB32)));

    QWidget* color_maps_widgets = new QWidget();
    QHBoxLayout* colormapsHBox = new QHBoxLayout();
    colormapsHBox->addWidget(new QLabel(" ColorMap :"));
    colormapsHBox->addWidget(_cb_colormap);
    colormapsHBox->addWidget(_color_map_ledgend_lbl);
    color_maps_widgets->setLayout(colormapsHBox);

    _chk_disp_color_ledgend = new QCheckBox("Display Color Ledgend");
    _chk_disp_color_ledgend->setChecked(Preferences::inst()->getValue(STR_LOG_SCALE_COLOR).toBool());
    connect(_chk_disp_color_ledgend, &QCheckBox::stateChanged, this, &MapsElementsWidget::on_log_color_changed);

    _chk_log_color = new QCheckBox("Log scale");
    _chk_log_color->setChecked(Preferences::inst()->getValue(STR_DISPLAY_COLOR_LEDGEND).toBool());
    connect(_chk_log_color, &QCheckBox::stateChanged, this, &MapsElementsWidget::on_log_color_changed);

    _chk_invert_y = new QCheckBox("Invert Y Axis");
    _chk_invert_y->setChecked(Preferences::inst()->getValue(STR_INVERT_Y_AXIS).toBool());
    connect(_chk_invert_y, &QCheckBox::stateChanged, this, &MapsElementsWidget::on_invert_y_axis);

    _grid_button = new QPushButton();
	_grid_button->setIcon(QIcon(":/images/grid.png"));
	_grid_button->setIconSize(QSize(15, 15)); 

	connect(_grid_button, &QPushButton::pressed, this, &MapsElementsWidget::onGridDialog);

    _btn_export_as_image = new QPushButton("Export Images");
    connect(_btn_export_as_image, &QPushButton::pressed, this, &MapsElementsWidget::on_export_image_pressed);

    QWidget* options_widgets = new QWidget();
    QVBoxLayout* optionsVBox = new QVBoxLayout();
    QHBoxLayout* optionsHboxS = new QHBoxLayout();
    QHBoxLayout* optionsHboxM = new QHBoxLayout();
    optionsVBox->addWidget(_chk_log_color);
    optionsVBox->addWidget(_chk_disp_color_ledgend);
    optionsVBox->addWidget(_chk_invert_y);
    optionsHboxS->addWidget(new QLabel("Layout:"));
    optionsHboxS->addWidget(_grid_button);
    optionsHboxS->addWidget(_btn_export_as_image);

    optionsHboxM->addItem(optionsVBox);
    optionsHboxM->addItem(optionsHboxS);
    options_widgets->setLayout(optionsHboxM);

    
    _cb_normalize = new QComboBox();
    _cb_normalize->setMinimumContentsLength(20);
    _cb_normalize->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    _cb_normalize->addItem("1");
    connect(_cb_normalize, &QComboBox::currentTextChanged, this, &MapsElementsWidget::onSelectNormalizer);

    _global_contrast_chk = new QCheckBox("Global Contrast");
    _global_contrast_chk->setChecked(true);
    connect(_global_contrast_chk, &QCheckBox::stateChanged, this, &MapsElementsWidget::on_global_contrast_changed);
    
    _contrast_widget = new gstar::MinMaxSlider();
    _contrast_widget->setMinimumWidth(200);
    connect(_contrast_widget, &gstar::MinMaxSlider::min_max_val_changed, this, &MapsElementsWidget::on_min_max_contrast_changed);    

    //_pb_perpixel_fitting = new QPushButton("Per Pixel Fitting");
    //counts_layout->addWidget(_pb_perpixel_fitting);

    QStringList extra_pv_header = { "Name", "Value", "Unit", "Description" };

    _extra_pvs_table_widget = new QTableWidget(1, 4);
    _extra_pvs_table_widget->setHorizontalHeaderLabels(extra_pv_header);
    _extra_pvs_table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    
    QWidget* w_normalize = new QWidget();
    QHBoxLayout* hbox_normalize = new QHBoxLayout();
    hbox_normalize->addWidget(_cb_analysis);
    hbox_normalize->addWidget(new QLabel("  Normalize By: "));
    hbox_normalize->addWidget(_cb_normalize);
    w_normalize->setLayout(hbox_normalize);

    QWidget* w_contrast = new QWidget();
    QHBoxLayout* hbox_contrast = new QHBoxLayout();
    hbox_contrast->addWidget(_global_contrast_chk);
    hbox_contrast->addWidget(_contrast_widget);
    w_contrast->setLayout(hbox_contrast);


    _tw_image_controls->addTab(m_toolbar, "Zoom");
    _tw_image_controls->addTab(color_maps_widgets, "Color Map");
    _tw_image_controls->addTab(options_widgets, "Options");
    _tw_image_controls->addTab(w_normalize, "Normalize");
    _tw_image_controls->addTab(w_contrast, "Contrast");
    _tw_image_controls->setProperty("padding", QVariant("1px"));
    //toolbar_widget->setLayout(toolbar_hbox);

    //_tabWidget->addTab(color_maps_widgets, "Color Map");

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumHeight(110);
    scrollArea->setMaximumHeight(140);
    //scrollArea->setWidget(toolbar_widget);
    scrollArea->setWidget(_tw_image_controls);

    counts_layout->addWidget(scrollArea);
    counts_layout->addWidget(splitter);

    counts_layout->setSpacing(0);
	counts_layout->setContentsMargins(0, 0, 0, 0);

    _counts_window = new QWidget();
    _counts_window->setLayout(counts_layout);

    _co_loc_widget = new CoLocalizationWidget();
    _quant_widget = new QuantificationWidget();

    _scatter_plot_widget = new ScatterPlotWidget();
    connect(_scatter_plot_widget, &ScatterPlotWidget::updateProgressBar, this, &MapsElementsWidget::loaded_perc);

    _counts_dock = new QDockWidget("Analyzed Counts", this);
    _counts_dock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	_counts_dock->setWidget(_counts_window);
    _counts_dock->setContentsMargins(0, 0, 0, 0);
    _dockMap[STR_COUNTS_DOCK] = _counts_dock;
        
    _intspectra_dock = new QDockWidget(DEF_STR_INT_SPECTRA, this);
    _intspectra_dock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	_intspectra_dock->setWidget(_spectra_widget);
    _intspectra_dock->setContentsMargins(0, 0, 0, 0);
    _dockMap[STR_INTSPEC_DOCK] = _intspectra_dock;

    _quant_dock = new QDockWidget("Quantification", this);
    _quant_dock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	_quant_dock->setWidget(_quant_widget);
    _quant_dock->setContentsMargins(0, 0, 0, 0);
    _dockMap[STR_QUANT_DOCK] = _quant_dock;

    _coloc_dock = new QDockWidget("CoLocalization", this);
    _coloc_dock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	_coloc_dock->setWidget(_co_loc_widget);
    _coloc_dock->setContentsMargins(0, 0, 0, 0);
    _dockMap[STR_COLOR_DOCK] = _coloc_dock;

    _scatter_dock = new QDockWidget("Scatter Plot", this);
    _scatter_dock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	_scatter_dock->setWidget(_scatter_plot_widget);
    _scatter_dock->setContentsMargins(0, 0, 0, 0);
    _dockMap[STR_SCATTER_DOCK] = _scatter_dock;
 
    _extra_dock = new QDockWidget("Extra PV's", this);
    _extra_dock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	_extra_dock->setWidget(_extra_pvs_table_widget);
    _extra_dock->setContentsMargins(0, 0, 0, 0);
    _dockMap[STR_EXTRA_DOCK] = _extra_dock;

    // This is done this way so that you can undock a widget and change tabs without hiding it.
    tmp_layout = new QHBoxLayout();
	tmp_layout->setSpacing(0);
	tmp_layout->setContentsMargins(0, 0, 0, 0);
    tmp_layout->addWidget(_counts_dock);
    tmp_widget = new QWidget();
    tmp_widget->setLayout(tmp_layout);
    _tab_widget->addTab(tmp_widget, "Analyzed Counts");
    
    tmp_layout = new QHBoxLayout();
    tmp_layout->setSpacing(0);
	tmp_layout->setContentsMargins(0, 0, 0, 0);
    tmp_layout->addWidget(_intspectra_dock);
    tmp_widget = new QWidget();
    tmp_widget->setLayout(tmp_layout);
    _tab_widget->addTab(tmp_widget, DEF_STR_INT_SPECTRA);

    tmp_layout = new QHBoxLayout();
    tmp_layout->setSpacing(0);
	tmp_layout->setContentsMargins(0, 0, 0, 0);
    tmp_layout->addWidget(_quant_dock);
    tmp_widget = new QWidget();
    tmp_widget->setLayout(tmp_layout);
    _tab_widget->addTab(tmp_widget, "Quantification");

    tmp_layout = new QHBoxLayout();
    tmp_layout->setSpacing(0);
	tmp_layout->setContentsMargins(0, 0, 0, 0);
    tmp_layout->addWidget(_coloc_dock);
    tmp_widget = new QWidget();
    tmp_widget->setLayout(tmp_layout);
    _tab_widget->addTab(tmp_widget, "CoLocalization");

    tmp_layout = new QHBoxLayout();
    tmp_layout->setSpacing(0);
	tmp_layout->setContentsMargins(0, 0, 0, 0);
    tmp_layout->addWidget(_scatter_dock);
    tmp_widget = new QWidget();
    tmp_widget->setLayout(tmp_layout);
    _tab_widget->addTab(tmp_widget, "Scatter Plot");

    tmp_layout = new QHBoxLayout();
    tmp_layout->setSpacing(0);
	tmp_layout->setContentsMargins(0, 0, 0, 0);
    tmp_layout->addWidget(_extra_dock);
    tmp_widget = new QWidget();
    tmp_widget->setLayout(tmp_layout);
    _tab_widget->addTab(tmp_widget, "Extra PV's");


    _tab_widget->setProperty("padding", QVariant("1px"));

    layout->addItem(hbox2);
    layout->addWidget(_tab_widget);

    //don't erase counts when mouse is off scene
    m_imageViewWidget->set_null_mouse_pos = false;
    connect(m_imageViewWidget, &ImageViewWidget::cbLabelChanged, this, &MapsElementsWidget::onElementSelect);

	connect(m_imageViewWidget, &ImageViewWidget::parent_redraw, this, &MapsElementsWidget::redrawCounts);


    appendAnnotationTab();

    _appendRoiTab();

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

    connect(m_treeModel, &gstar::AnnotationTreeModel::deletedNode, this, &MapsElementsWidget::on_delete_annotation);
    connect(m_treeModel, &gstar::AnnotationTreeModel::deleteAll, this, &MapsElementsWidget::on_delete_all_annotations);

    connect(m_tabWidget, &QTabWidget::currentChanged, this, &MapsElementsWidget::annoTabChanged);

    restoreGeometry(Preferences::inst()->getValue(STR_MAPS_WIDGET_GEOMETRY).toByteArray());
    //restoreState(Preferences::inst()->getValue(STR_MAPS_WIDGET_WINDOWSTATE).toByteArray());

    setCoordinateModel(new gstar::CoordinateModel(&_motor_trans));

    hbox->setSpacing(0);
	hbox->setContentsMargins(0, 0, 0, 0);
    hbox2->setSpacing(0);
	hbox2->setContentsMargins(0, 0, 0, 0);

	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    if(restore_floating)
    {
        for (auto& mItr : _dockMap)
        {
            QVariant variant = Preferences::inst()->getValue(mItr.first+"_floating");
            if (variant.isValid())
            {
                mItr.second->setFloating(variant.toBool());
            }
            variant = Preferences::inst()->getValue(mItr.first + "_geometry");
            if (variant.isValid())
            {
                mItr.second->restoreGeometry(variant.toByteArray());
            }
            
            connect(mItr.second, &QDockWidget::topLevelChanged, this, &MapsElementsWidget::onDockFloatChanged);
        }
    }
   
}

//---------------------------------------------------------------------------

void MapsElementsWidget::onDockFloatChanged(bool floating)
{
    for (auto& mItr : _dockMap)
    {
        Preferences::inst()->setValue(mItr.first + "_floating", mItr.second->isFloating());
        //Preferences::inst()->setValue(mItr.first + "_geometry", mItr.second->saveGeometry());
    }
}

//---------------------------------------------------------------------------

void MapsElementsWidget::savePref()
{
    for (auto& mItr : _dockMap)
    {
        Preferences::inst()->setValue(mItr.first+"_floating", mItr.second->isFloating());
        Preferences::inst()->setValue(mItr.first + "_geometry", mItr.second->saveGeometry());
    }
}

//---------------------------------------------------------------------------

void MapsElementsWidget::_appendRoiTab()
{   
    m_roiTreeModel = new gstar::AnnotationTreeModel();
    connect(m_roiTreeModel,&gstar::AnnotationTreeModel::dataChanged,this,&MapsElementsWidget::roiModelDataChanged);

    m_roiSelectionModel = new QItemSelectionModel(m_roiTreeModel);

    m_roiTreeView = new QTreeView();
    //m_roiTreeView->setPalette(pal);
    //m_roiTreeView->setAutoFillBackground(true);
    m_roiTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_roiTreeView->setAnimated(true);
    m_roiTreeView->setModel(m_roiTreeModel);
    m_roiTreeView->setHeaderHidden(true);
    m_roiTreeView->setSelectionModel(m_roiSelectionModel);
    m_roiTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_roiTreeView,&QTreeView::customContextMenuRequested,this,&MapsElementsWidget::roiTreeContextMenu);
    /*
    connect(m_roiTreeView,
        doubleClicked,
        this,
        roiTreeDoubleClicked);
        */
    //infoLayout->addWidget(m_annotationToolbar->getToolBar());

    QHBoxLayout *roi_button_layout = new QHBoxLayout();

    QVBoxLayout* roiVbox = new QVBoxLayout();
    _btn_roi_img_seg = new QPushButton("ROI Dialog");
    connect(_btn_roi_img_seg, &QPushButton::released, this, &MapsElementsWidget::openImageSegDialog);

    _btn_roi_stats = new QPushButton("ROI Statistics");
    connect(_btn_roi_stats, &QPushButton::released, this, &MapsElementsWidget::openRoiStatsWidget);

    roi_button_layout->addWidget(_btn_roi_img_seg);
    roi_button_layout->addWidget(_btn_roi_stats);

    roiVbox->addItem(roi_button_layout);
    roiVbox->addWidget(m_roiTreeView);

    m_roiTreeTabWidget = new QWidget(this);
    //m_treeTabWidget->setPalette(pal);
    //m_treeTabWidget->setAutoFillBackground(true);
    m_roiTreeTabWidget->setLayout(roiVbox);

    m_tabWidget->addTab(m_roiTreeTabWidget, QIcon(), "ROI's");
}

//---------------------------------------------------------------------------

void MapsElementsWidget::roiTreeContextMenu(const QPoint& pos)
{

    displayRoiContextMenu(m_roiTreeView, m_roiTreeView->viewport()->mapToGlobal(pos));

}

//---------------------------------------------------------------------------

void MapsElementsWidget::displayRoiContextMenu(QWidget* parent, const QPoint& pos)
{

    //if (m_roisEnabled == false)
    //    return;

    QMenu menu(parent);
    menu.addAction(_addKMeansRoiAction);
    /*
    if (m_roiTreeModel != nullptr && m_roiTreeModel->rowCount() > 0)
    {
        if (m_roiSelectionModel->hasSelection())
        {
            menu.addSeparator();
            menu.addAction(m_duplicateAction);
            menu.addSeparator();
            menu.addAction(m_deleteAction);
        }
    }
    */
    QAction* result = menu.exec(pos);
    if (result == nullptr)
    {
        m_roiSelectionModel->clearSelection();
    }

}


//---------------------------------------------------------------------------

void MapsElementsWidget::roiModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{

    m_roiTreeView->resizeColumnToContents(topLeft.column());
    m_roiTreeView->resizeColumnToContents(bottomRight.column());

}

//---------------------------------------------------------------------------

void MapsElementsWidget::annoTabChanged(int idx)
{
    if (idx == ANNO_TAB) // Annotations
    {
        m_imageViewWidget->setSceneModel(m_treeModel);
        _spectra_widget->displayROIs(false);
    }
    else if (idx == ROI_TAB) //ROI's
    {
        m_imageViewWidget->setSceneModel(m_roiTreeModel);
        _spectra_widget->displayROIs(true);
    }

}

//---------------------------------------------------------------------------

void MapsElementsWidget::onGridDialog()
{
	
	iDiag.show();

}
//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------

void MapsElementsWidget::on_log_color_changed(int state)
{
    Preferences::inst()->setValue(STR_LOG_SCALE_COLOR, _chk_log_color->isChecked());
    Preferences::inst()->setValue(STR_DISPLAY_COLOR_LEDGEND, _chk_disp_color_ledgend->isChecked());
    redrawCounts();
}

//---------------------------------------------------------------------------

void MapsElementsWidget::on_invert_y_axis(int state)
{
    Preferences::inst()->setValue(STR_INVERT_Y_AXIS, _chk_invert_y->isChecked());
    redrawCounts();
}

//---------------------------------------------------------------------------

void MapsElementsWidget::on_min_max_contrast_changed()
{

	_min_contrast_perc = _contrast_widget->getUserMin() / 100.0;
    _max_contrast_perc = _contrast_widget->getUserMax() / 100.0;
	redrawCounts();

}

//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------

void MapsElementsWidget::openImageSegDialog()
{
    // Bring up dialog for settings and run kmeans.
    if (_model != nullptr)
    {
        std::string analysis_text = _cb_analysis->currentText().toStdString();

        data_struct::Fit_Count_Dict<float> fit_counts;
        _model->getAnalyzedCounts(analysis_text, fit_counts);
        std::map<std::string, data_struct::ArrayXXr<float>>* scalers = _model->getScalers();
        if (scalers != nullptr)
        {
            for (auto& itr : *scalers)
            {
                fit_counts.insert(itr);
            }
        }
        _img_seg_diag.setColorMap(_selected_colormap);
        _img_seg_diag.setImageData(fit_counts);
        
        // add any roi's that were loaded.
        std::vector<gstar::RoiMaskGraphicsItem*> roi_list;
        QImage i;
        QColor q;
        gstar::RoiMaskGraphicsItem item(i, q, 0);
        if (m_treeModel != nullptr)
        {
            m_roiTreeModel->get_all_of_type(item.classId(), roi_list);
        }
        for (auto itr : roi_list)
        {
            _img_seg_diag.append_roi((gstar::RoiMaskGraphicsItem*)(itr->duplicate()));
        }
        m_roiTreeModel->clearAll();
        _spectra_widget->deleteAllROISpectra();

        _img_seg_diag.show();
    }
}

//---------------------------------------------------------------------------

void MapsElementsWidget::openRoiStatsWidget()
{
    if (_model != nullptr)
    {
        if (_roi_stats_diag == nullptr)
        {
            _roi_stats_diag = new RoiStatisticsWidget();
        }
        _roi_stats_diag->clear_all();

        std::string analysis_text = _cb_analysis->currentText().toStdString();

        data_struct::Fit_Count_Dict<float> fit_counts;
        _model->getAnalyzedCounts(analysis_text, fit_counts);
        std::map<std::string, data_struct::ArrayXXr<float>>* scalers = _model->getScalers();
        if (scalers != nullptr)
        {
            for (auto& itr : *scalers)
            {
                fit_counts.insert(itr);
            }
        }
        // add any roi's that were loaded.
        std::vector<gstar::RoiMaskGraphicsItem*> roi_list;
        QImage i;
        QColor c;
        gstar::RoiMaskGraphicsItem item(i, c, 0);
        if (m_treeModel != nullptr)
        {
            m_roiTreeModel->get_all_of_type(item.classId(), roi_list);
        }

        const std::vector<float> x_axis = _model->get_x_axis();
        const std::vector<float> y_axis = _model->get_y_axis();

        int xidx = x_axis.size() / 2;
        int yidx = y_axis.size() / 2;

        float sq_area = ((x_axis[xidx+1] - x_axis[xidx]) * (y_axis[yidx+1] - y_axis[yidx]));

        _roi_stats_diag->setData(_model->getDir(), _model->getDatasetName(), _cb_analysis->currentText(), _cb_normalize->currentText(), sq_area, fit_counts, roi_list, _normalizer, _calib_curve);

        _roi_stats_diag->show();
    }
}

//---------------------------------------------------------------------------

void MapsElementsWidget::roiUpdated(gstar::RoiMaskGraphicsItem* ano, bool reload)
{
    if (ano != nullptr && reload)
    {

    }
    
}

//---------------------------------------------------------------------------

void MapsElementsWidget::createActions()
{
    AbstractImageWidget::createActions();
    // TODO: change Roi to spectra region and add back in
    /*
    _addRoiMaskAction = new QAction("Add ROI Mask", this);

    connect(_addRoiMaskAction,
            triggered,
            this,
            addRoiMask);
            */
    _addKMeansRoiAction = new QAction("ROI Image Seg Dialog", this);

    connect(_addKMeansRoiAction,&QAction::triggered,this,&MapsElementsWidget::openImageSegDialog);
            
}

//---------------------------------------------------------------------------

void MapsElementsWidget::displayContextMenu(QWidget* parent,
                                             const QPoint& pos)
{

   if (m_annotationsEnabled == false)
      return;

   QMenu menu(parent);
   menu.addAction(m_addMarkerAction);
   menu.addAction(m_addRulerAction);

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

//---------------------------------------------------------------------------

void MapsElementsWidget::onAnalysisSelect(QString name)
{	
    _calib_curve = _model->get_calibration_curve(name.toStdString(), _cb_normalize->currentText().toStdString());
    _co_loc_widget->onSetAnalysisType(name);
    _scatter_plot_widget->setAnalysisType(name);
    redrawCounts();
}

//---------------------------------------------------------------------------

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

    if (_model != nullptr && _model->regionLinks().count(name) > 0)
    {
        m_imageViewWidget->scene(viewIdx)->setPixmap(QPixmap::fromImage((_model->regionLinks().at(name))));
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
    _color_maps_ledgend->setColorTable(*_selected_colormap);
    _color_map_ledgend_lbl->setPixmap(QPixmap::fromImage(_color_maps_ledgend->convertToFormat(QImage::Format_RGB32)));

	redrawCounts();
    
    Preferences::inst()->save();
}

//---------------------------------------------------------------------------

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

    _scatter_plot_widget->setQuantType(name);

    redrawCounts();
}

 //---------------------------------------------------------------------------

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
            _spectra_widget->setDatasetDir(_model->getDir());

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
            // add map_roi's 
            // clear old roi's 
            m_roiTreeModel->clearAll();
            _model->loadAllRoiMaps();
            for (auto& itr : _model->get_map_rois())
            {
                int width = (int)m_imageViewWidget->scene()->width();
                int height = (int)m_imageViewWidget->scene()->height();
                logI<< "Loading roi: "<< itr.first<<"\n";
                gstar::RoiMaskGraphicsItem* roi = new gstar::RoiMaskGraphicsItem(QString(itr.first.c_str()), itr.second.color, itr.second.color_alpha, width, height, itr.second.pixel_list);
                insertAndSelectAnnotation(m_roiTreeModel, m_roiTreeView, m_roiSelectionModel, roi);
                if (itr.second.int_spec.count(_model->getDatasetName().toStdString()) > 0)
                {
                    _spectra_widget->appendROISpectra(itr.first, (ArrayDr*)&(itr.second.int_spec.at(_model->getDatasetName().toStdString())), itr.second.color);
                }
            }

            _co_loc_widget->setModel(_model);
            _scatter_plot_widget->setModel(_model);
            _quant_widget->setModel(_model);

            QString analysis_text = _cb_analysis->currentText();
            if (analysis_text.length() > 0)
            {
                _co_loc_widget->onSetAnalysisType(analysis_text);
                _scatter_plot_widget->setAnalysisType(analysis_text);
            }

            _motor_trans.setMotors(_model->get_x_axis(), _model->get_y_axis());
            
            annoTabChanged(m_tabWidget->currentIndex());
        }
        m_imageWidgetToolBar->clickFill();
    }
}

//---------------------------------------------------------------------------

void MapsElementsWidget::on_export_csv_and_png(QPixmap png, ArrayDr* ev, ArrayDr* int_spec, ArrayDr* back_spec , ArrayDr* fit_spec, std::unordered_map<std::string, ArrayDr>* labeled_spectras)
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
            for (int a = 0; a < MAX_DETECTORS; a++)
            {
                QString check = "h5" + QString::number(a);
                if (dataset_leftover.endsWith(check))
                {
                    dataset_path += QString::number(a);
                    break;
                }
            }
		}

        data_struct::Params_Override<double>* param_overrides = _model->getParamOverride();

        //check if file exists and warn user
        if (param_overrides != nullptr)
        {

            QString fileName = QFileDialog::getSaveFileName(this, "Save parameters override", dataset_path, tr("All Files(*.*)"));
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
        else
        {
            QMessageBox::critical(nullptr, "Export Fit Parameters", "Param override variable is nullptr, can't save data.");
        }
    }
    else
        {
            QMessageBox::critical(nullptr, "Export Fit Parameters", "Model is nullptr, can't save data.");
        }
}

//---------------------------------------------------------------------------

void MapsElementsWidget::model_updated()
{
    if(_model == nullptr)
    {
        return;
    }

    disconnect(_cb_analysis, &QComboBox::currentTextChanged, this, &MapsElementsWidget::onAnalysisSelect);

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
    if (scalers->count(STR_US_FM) > 0)
    {
        _cb_normalize->addItem(QString(STR_US_FM.c_str()));
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
    bool first = true;
    for(auto& itr: analysis_types)
    {
        QString newVal = QString(itr.c_str());
        _cb_analysis->addItem(newVal);
        if (first)
        {
            current_a = itr;
            first = false;
        }
        if(current_analysis == newVal)
        {
            found_analysis = true;
        }
    }

    std::vector<std::string> element_lines;
	std::vector<std::string> scalers_to_add_first;
	std::vector<std::string> final_counts_to_add_before_scalers;
	gen_insert_order_lists(element_lines, scalers_to_add_first, final_counts_to_add_before_scalers);


    data_struct::Fit_Count_Dict<float> element_counts;
    _model->getAnalyzedCounts(current_a, element_counts);

    const std::vector<QString> element_view_list = m_imageViewWidget->getLabelList();
    
    // get copy of elements to add the ones that were missed, usually pileups
    data_struct::Fit_Count_Dict<float> element_counts_not_added;
    _model->getAnalyzedCounts(current_a, element_counts_not_added);

    m_imageViewWidget->clearLabels();

    // insert any region links
    for (const auto& itr : _model->regionLinks())
    {
        m_imageViewWidget->addLabel(itr.first);
    }

    // insert in z order
    for (std::string el_name : element_lines)
    {
        if(element_counts.count(el_name) > 0)
        {
            QString val = QString(el_name.c_str());
            m_imageViewWidget->addLabel(val);
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
            m_imageViewWidget->addLabel(val);
        }
    }

    // add end of element list that are not elements
    for (auto& itr : final_counts_to_add_before_scalers)
    {
        QString val = QString(itr.c_str());
        m_imageViewWidget->addLabel(val);
    }
    

    std::map<std::string, data_struct::ArrayXXr<float>> left_over_scalers = *scalers;
    // add scalers in certain order
    for (auto& itr : scalers_to_add_first)
    {
        if(itr.length() > 0 && scalers->count(itr) > 0)
        {
            m_imageViewWidget->addLabel(QString(itr.c_str()));
            left_over_scalers.erase(itr);
        }
    }

    // add rest of scalers
    for (auto& itr : left_over_scalers)
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

    connect(_cb_analysis, &QComboBox::currentTextChanged, this, &MapsElementsWidget::onAnalysisSelect);
}

//---------------------------------------------------------------------------

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
            if (_model != nullptr && _model->regionLinks().count(element) > 0)
            {
                m_imageViewWidget->scene(vidx)->setPixmap(QPixmap::fromImage((_model->regionLinks().at(element))));
            }
            else
            {
                job_queue[vidx] = Global_Thread_Pool::inst()->enqueue([this, vidx, analysis_text, element] { return generate_pixmap(analysis_text, element.toStdString(), _chk_log_color->isChecked(), vidx); });
            }
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

    annoTabChanged(m_tabWidget->currentIndex());
}

//---------------------------------------------------------------------------
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
*/
//---------------------------------------------------------------------------

void MapsElementsWidget::displayCounts(const std::string analysis_type, const std::string element, bool log_color, int grid_idx)
{
    m_imageViewWidget->scene(grid_idx)->setPixmap(generate_pixmap(analysis_type, element, log_color, grid_idx));
/*
    
	if (_model != nullptr)
	{
        
        ArrayXXr<float> normalized;
        
        GenerateImageProp props;
        props.analysis_type = analysis_type;
        props.element = element;
        props.log_color = log_color;
        props.selected_colormap = _selected_colormap;
        props.normalizer = _normalizer;
        props.calib_curve = _calib_curve;
        props.show_legend = _chk_disp_color_ledgend->isChecked();
        props.invert_y = _chk_invert_y->isChecked();
        props.global_contrast = _global_contrast_chk->isChecked();
        if (_global_contrast_chk->isChecked())
        {
            props.contrast_max = _max_contrast_perc;
            props.contrast_min =  _min_contrast_perc;
        }
        else
        {
            m_imageViewWidget->getMinMaxAt(grid_idx, props.contrast_min, props.contrast_max);
        }
        m_imageViewWidget->scene(grid_idx)->setPixmap(_model->gen_pixmap(props, normalized));
        if(normalized.rows() > 0 && normalized.cols() > 0)
        {
            if (m_imageHeightDim != nullptr && m_imageWidthDim != nullptr)
            {
                m_imageHeightDim->setCurrentText(QString::number(normalized.rows()));
                m_imageWidthDim->setCurrentText(QString::number(normalized.cols()));
            }
            m_imageViewWidget->resetCoordsToZero();
            m_imageViewWidget->setCountsTrasnformAt(grid_idx, normalized);
        }
	}
    if (m_imageViewWidget != nullptr)
    {
        m_imageViewWidget->resetCoordsToZero();
    }
    */
}

//---------------------------------------------------------------------------

QPixmap MapsElementsWidget::generate_pixmap(const std::string analysis_type, const std::string element, bool log_color, int grid_idx)
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
            // add to width for color maps ledgend
            int cm_ledgend = width * .05; // add 5% width for ledgend
            if (cm_ledgend == 0)
            {
                cm_ledgend = 3;
            }
            if (height <= 3 || _chk_disp_color_ledgend->isChecked() == false)
            {
                cm_ledgend = 0;
            }

            QImage image(width + cm_ledgend, height, QImage::Format_Indexed8);
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

            m_imageViewWidget->setCountsTrasnformAt(grid_idx, normalized);

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
            if (_chk_invert_y->isChecked())
            {
                image = image.mirrored(false, true);
            }
            return QPixmap::fromImage(image.convertToFormat(QImage::Format_RGB32));
        }
    }
    return QPixmap();
}

//---------------------------------------------------------------------------

void MapsElementsWidget::windowChanged(Qt::WindowStates oldState,
                                       Qt::WindowStates newState)
{
    Q_UNUSED(oldState);

    if(Qt::WindowMaximized || Qt::WindowActive == newState)
    {
        m_imageViewWidget->resizeEvent(nullptr);
    }

}

//---------------------------------------------------------------------------

void MapsElementsWidget::on_add_new_ROIs(std::vector<gstar::RoiMaskGraphicsItem*> roi_list)
{
    if (_model != nullptr)
    {
        _model->clearAllMapRois();

        for (auto& itr : roi_list)
        {
            insertAndSelectAnnotation(m_roiTreeModel, m_roiTreeView, m_roiSelectionModel, itr->duplicate());
            std::vector<std::pair<int, int>> pixel_list;
            itr->to_roi_vec(pixel_list);
            data_struct::Spectra<double>* int_spectra = new data_struct::Spectra<double>();
            if (io::file::HDF5_IO::inst()->load_integrated_spectra_analyzed_h5_roi(_model->getFilePath().toStdString(), int_spectra, pixel_list))
            {
                struct Map_ROI roi(itr->getName().toStdString(), itr->getColor(), itr->alphaValue(), pixel_list, _model->getDatasetName().toStdString(),  *int_spectra);

                _model->appendMapRoi(itr->getName().toStdString(), roi);
                _spectra_widget->appendROISpectra(itr->getName().toStdString(), int_spectra, itr->getColor());
            }
        }
    
        _model->saveAllRoiMaps();
    
        //refresh roi's
        _scatter_plot_widget->setModel(_model);
        // update num rois in file widget
        emit new_rois(_model->getDatasetName(), roi_list.size());
    }
    _spectra_widget->replot_integrated_spectra(false);
    annoTabChanged(ROI_TAB);
    _img_seg_diag.clear_all_rois();
    _img_seg_diag.clear_image();
}

//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------

void MapsElementsWidget::on_export_images()
{
    _export_maps_dialog->setRunEnabled(false);
    
    //get all maps
    int view_cnt = m_imageViewWidget->getViewCount();
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
            std::vector<std::string> normalizers = { STR_DS_IC , STR_US_IC, STR_US_FM, STR_SR_CURRENT, "Counts" };

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
                QString element = m_imageViewWidget->getLabelAt(vidx);
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
        ascii_dir.mkdir("CSV");
        ascii_dir.cd("CSV");

        std::vector<std::string> normalizers = { STR_DS_IC , STR_US_IC, STR_US_FM, STR_SR_CURRENT, "Counts" };

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

                    out_stream << "\n";
                    // check if there are any ROI's and export them
                    const std::unordered_map<std::string, Map_ROI> rois = _model->get_map_rois();
                    for (auto itr_roi : rois)
                    {
                        out_stream << "ROI: "<<itr_roi.first<<"\n";
                        for (auto& p_itr : itr_roi.second.pixel_list)
                        {
                            int yidx = p_itr.second;
                            int xidx = p_itr.first;
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
                            out_stream << "\n";
                        }    
                        out_stream << "\n";                
                    }
                    out_stream.close();
                }
                else
                {
                    logE << "Could not save file for " << save_file_name << "\n";
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
        QString formattedTime = date.toString("yyyy_MM_dd_hh_mm_ss");
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

//---------------------------------------------------------------------------

void MapsElementsWidget::on_delete_all_annotations(QString classid)
{
    QImage i;
    QColor c;
    gstar::RoiMaskGraphicsItem item(i,c,0);

    if (classid == item.classId())
    {
        _spectra_widget->deleteAllROISpectra();
    }
}

//---------------------------------------------------------------------------

void MapsElementsWidget::on_delete_annotation(QString classid, QString name)
{
    QImage i;
    QColor c;
    gstar::RoiMaskGraphicsItem item(i, c, 0);

    if (classid == item.classId())
    {
        _spectra_widget->deleteROISpectra(name.toStdString());
    }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------