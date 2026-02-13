/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/CoLocalizationWidget.h>

//---------------------------------------------------------------------------

CoLocalizationWidget::CoLocalizationWidget(QWidget* parent) : gstar::AbstractImageWidget(1, 1, parent)
{
    _model = nullptr;
    _first_pixmap_set = true;
    int r = 0;
    for (int i = 0; i < 256; ++i)
    {
        _red_colormap.append(qRgb(i, 0, 0));
        _green_colormap.append(qRgb(0, i, 0));
        _blue_colormap.append(qRgb(0, 0, i));
    }

    setAnnotationsEnabled(false);
    _createLayout();
    createActions();
}

//---------------------------------------------------------------------------

CoLocalizationWidget::~CoLocalizationWidget()
{

}

//---------------------------------------------------------------------------

void CoLocalizationWidget::_createLayout()
{

    QVBoxLayout* layout = new QVBoxLayout();
    QLayout* gen_layout = generateDefaultLayout(true);

    m_imageViewWidget->setCoordsVisible(false);
    m_imageViewWidget->setSelectorVisible(false);
    m_imageViewWidget->setCountsVisible(false);
    
    //connect(_contrast_widget, &ContrastWidget::global_contrast_change, this, &CoLocalizationWidget::on_global_contrast_changed);
    connect(_contrast_widget, &ContrastWidget::call_redraw, this, &CoLocalizationWidget::redraw);

    _cb_red_element = new QComboBox();
    connect(_cb_red_element, &QComboBox::currentTextChanged, this, &CoLocalizationWidget::onColorSelected);
    _cb_green_element = new QComboBox();
    connect(_cb_green_element, &QComboBox::currentTextChanged, this, &CoLocalizationWidget::onColorSelected);
    _cb_blue_element = new QComboBox();
    connect(_cb_blue_element, &QComboBox::currentTextChanged, this, &CoLocalizationWidget::onColorSelected);

    _btn_export_as_image = new QPushButton("Export as PNG");
    connect(_btn_export_as_image, &QPushButton::released, this, &CoLocalizationWidget::onExportPng);

    _ck_quad_view = new QCheckBox("Quad View");
    // TODO: save and load from pref check value
    connect(_ck_quad_view, &QCheckBox::stateChanged, this, &CoLocalizationWidget::onQuadViewChanged);

    QHBoxLayout* hb = new QHBoxLayout();
    hb->addStretch();
    hb->addWidget(new QLabel("Red Element"));
    hb->addWidget(_cb_red_element);

    hb->addWidget(new QLabel("Green Element"));
    hb->addWidget(_cb_green_element);

    hb->addWidget(new QLabel("Blue Element"));
    hb->addWidget(_cb_blue_element);

    hb->addWidget(_ck_quad_view);

    hb->addWidget(_btn_export_as_image);
    hb->addStretch();
    layout->addItem(hb);
    
    //appendAnnotationTab(false);

    layout->addItem(gen_layout);
    //layout->addWidget(m_imageViewWidget);

    //don't erase counts when mouse is off scene
    //_imageViewWidget->set_null_mouse_pos = false;

    //createActions();

    layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);

}

//---------------------------------------------------------------------------

void CoLocalizationWidget::redraw()
{
    onColorSelected(" ");
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::onQuadViewChanged(int value)
{
    if (value == 0)
    {
        onNewGridLayout(1, 1);
    }
    else
    {
        onNewGridLayout(2, 2);
    }
    onColorSelected(" ");
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::onColorSelected(QString name)
{

    QImage red_img;
    QImage green_img;
    QImage blue_img;
    QImage sum_img;

    QPixmap red_pixmap;
    QPixmap green_pixmap;
    QPixmap blue_pixmap;
    QPixmap sum_pixmap;

    bool first = true;

    ArrayXXr<float> int_img;

    ArrayXXr<float> normalized;
    GenerateImageProp props;
    props.analysis_type = _curAnalysis.toStdString();
    props.log_color = false;
    props.normalizer = nullptr;
    props.calib_curve = nullptr;
    props.contrast_limits = _contrast_widget->get_contrast_limits();
    props.show_legend = false;
    props.invert_y = Preferences::inst()->getValue(STR_INVERT_Y_AXIS).toBool();
    props.global_contrast = _contrast_widget->is_global_contrast_checked();
    if (props.global_contrast)
    {
        props.contrast_max = _contrast_widget->max_contrast_perc();
        props.contrast_min =  _contrast_widget->min_contrast_perc();
    }
    else
    {
        if(!m_imageViewWidget->getMinMaxAt(0, props.contrast_min, props.contrast_max))
        {
            props.contrast_max = _contrast_widget->max_contrast_perc();
            props.contrast_min = _contrast_widget->min_contrast_perc();
            props.global_contrast = true;
        }
    }
    
    if (_fit_counts.count(_cb_red_element->currentText().toStdString()) > 0)
    {
        props.element = _cb_red_element->currentText().toStdString();
        props.selected_colormap = &_red_colormap;
        red_pixmap = _model->gen_pixmap(props, normalized);
        sum_pixmap = red_pixmap;
        first = false;
    }
    if (_fit_counts.count(_cb_green_element->currentText().toStdString()) > 0)
    {
        props.element = _cb_green_element->currentText().toStdString();
        props.selected_colormap = &_green_colormap;
        green_pixmap = _model->gen_pixmap(props, normalized);
        if (first)
        {
            sum_pixmap = green_pixmap;
            first = false;
        }
        else
        {
            QPainter p(&sum_pixmap);
            p.setCompositionMode(QPainter::CompositionMode_Plus);
            p.drawPixmap(0, 0, green_pixmap);
            p.end();
        }
    }
    if (_fit_counts.count(_cb_blue_element->currentText().toStdString()) > 0)
    {
        props.element = _cb_blue_element->currentText().toStdString();
        props.selected_colormap = &_blue_colormap;
        blue_pixmap = _model->gen_pixmap(props, normalized);
        if (first)
        {
            sum_pixmap = blue_pixmap;
            first = false;
        }
        else
        {
            QPainter p(&sum_pixmap);
            p.setCompositionMode(QPainter::CompositionMode_Plus);
            p.drawPixmap(0, 0, blue_pixmap);
            p.end();
        }
    }

    if (_ck_quad_view->isChecked())
    {
        m_imageViewWidget->setSubScenePixmap(0, red_pixmap);
        m_imageViewWidget->setSubScenePixmap(1, green_pixmap);
        m_imageViewWidget->setSubScenePixmap(2, blue_pixmap);
        m_imageViewWidget->setSubScenePixmap(3, sum_pixmap);
    }
    else
    {
        m_imageViewWidget->setScenePixmap(sum_pixmap);
    }
    if (_first_pixmap_set)
    {
        m_imageViewWidget->clickFill();
        _first_pixmap_set = false;
    }
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::onNewGridLayout(int rows, int cols)
{
    //const std::vector<QString> element_view_list = m_imageViewWidget->getLabelList();
    m_imageViewWidget->setSceneModelAndSelection(nullptr, nullptr);
    m_imageViewWidget->newGridLayout(rows, cols);

    m_imageViewWidget->setCoordsVisible(false);
    m_imageViewWidget->setSelectorVisible(false);
    m_imageViewWidget->setCountsVisible(false);
    
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::onSetAnalysisType(QString name)
{
    _curAnalysis = name;

    _cb_red_element->clear();
    _cb_green_element->clear();
    _cb_blue_element->clear();

    _cb_red_element->addItem(QString(" "));
    _cb_green_element->addItem(QString(" "));
    _cb_blue_element->addItem(QString(" "));

    if (_model != nullptr)
    {
        data_struct::Fit_Count_Dict<float> fit_counts;
        _model->getAnalyzedCounts(name.toStdString(), fit_counts);
        const std::map<std::string, data_struct::ArrayXXr<float>>* scalers = _model->getScalers();
        if (scalers != nullptr)
        {
            for (auto& itr : *scalers)
            {
                fit_counts.insert(itr);
            }
        }

        //copy
        _fit_counts = fit_counts;

        std::vector<std::string> element_lines;
        std::vector<std::string> scalers_to_add_first;
        std::vector<std::string> final_counts_to_add_before_scalers;
        gen_insert_order_lists(element_lines, scalers_to_add_first, final_counts_to_add_before_scalers);

        // insert in z order
        for (std::string el_name : element_lines)
        {
            if (fit_counts.count(el_name) > 0)
            {
                _cb_red_element->addItem(QString(el_name.c_str()));
                _cb_green_element->addItem(QString(el_name.c_str()));
                _cb_blue_element->addItem(QString(el_name.c_str()));
                fit_counts.erase(el_name);
            }

        }
        
        // add end of element list that are not elements
        for (auto& itr : final_counts_to_add_before_scalers)
        {
            if (fit_counts.count(itr) > 0)
            {
                _cb_red_element->addItem(QString(itr.c_str()));
                _cb_green_element->addItem(QString(itr.c_str()));
                _cb_blue_element->addItem(QString(itr.c_str()));
                fit_counts.erase(itr);
            }
        }

        // add scalers in certain order
        for (auto& itr : scalers_to_add_first)
        {
            if (fit_counts.count(itr) > 0)
            {
                _cb_red_element->addItem(QString(itr.c_str()));
                _cb_green_element->addItem(QString(itr.c_str()));
                _cb_blue_element->addItem(QString(itr.c_str()));
                fit_counts.erase(itr);
            }
        }

        // add rest of scalers
        for (auto& itr : fit_counts)
        {
            _cb_red_element->addItem(QString(itr.first.c_str()));
            _cb_green_element->addItem(QString(itr.first.c_str()));
            _cb_blue_element->addItem(QString(itr.first.c_str()));
        }
    }
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::setModel(MapsH5Model* model)
{
    if (_model != model)
    {
        _model = model;
        //model_updated();
        if (_model != nullptr)
        {
            onSetAnalysisType(_curAnalysis);
        }
    }
}

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

void CoLocalizationWidget::onExportPng()
{
    if (_model != nullptr)
    {
        QDir export_model_dir = _model->getDir();
        QFileInfo finfo(export_model_dir.absolutePath());
        export_model_dir.cdUp();
        export_model_dir.cdUp();
        export_model_dir.cd("output");

        QDateTime date = QDateTime::currentDateTime();
        QString formattedTime = date.toString("yyyy_MM_dd_hh_mm_ss");
        QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();

        QPixmap pixmap(rect().size());
        render(&pixmap, QPoint(), QRegion(rect()));

        QString apath = export_model_dir.absolutePath() + QDir::separator() + finfo.fileName() + QString("_colocalization_" + _curAnalysis + "_" + _cb_red_element->currentText() + "-" + _cb_green_element->currentText() + "-" + _cb_blue_element->currentText() + "_" + formattedTime);
        QString png_path = QDir::cleanPath(apath + ".png");
        if (false == pixmap.save(png_path, "PNG"))
        {
            logE << "Could not save PNG for " << png_path.toStdString() << "\n";
        }
        else
        {
            if (false == QDesktopServices::openUrl(QUrl::fromLocalFile(export_model_dir.absolutePath())))
            {
                logE << "Failed to open dir " << export_model_dir.absolutePath().toStdString() << "\n";
            }
        }
    }
}

//---------------------------------------------------------------------------
