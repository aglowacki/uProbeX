/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/CoLocalizationWidget.h>


constexpr unsigned int RED_WINDOW = 0;
constexpr unsigned int BLUE_WINDOW = 1;
constexpr unsigned int GREEN_WINDOW = 2;
//---------------------------------------------------------------------------

CoLocalizationWidget::CoLocalizationWidget(QWidget* parent) : gstar::AbstractImageWidget(1, 1, parent)
{
    _model = nullptr;
    _firstRed = true;
    _firstGreen = true;
    _firstBlue = true;
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
    
    connect(_contrast_widget, &ContrastWidget::call_redraw, this, &CoLocalizationWidget::redraw);
    connect(_contrast_widget, &ContrastWidget::on_global_contrast_changed, this, &CoLocalizationWidget::globalContrastChanged);
    connect(m_imageViewWidget, &gstar::ImageViewWidget::parent_redraw, this, &CoLocalizationWidget::singleRedrawCounts);

    _cb_red_element = new QComboBox();
    connect(_cb_red_element, &QComboBox::currentTextChanged, this, &CoLocalizationWidget::onRedColorSelected);
    _cb_green_element = new QComboBox();
    connect(_cb_green_element, &QComboBox::currentTextChanged, this, &CoLocalizationWidget::onGreenColorSelected);
    _cb_blue_element = new QComboBox();
    connect(_cb_blue_element, &QComboBox::currentTextChanged, this, &CoLocalizationWidget::onBlueColorSelected);

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

void CoLocalizationWidget::globalContrastChanged(bool value)
{
    displayCounts(!value);
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::redraw()
{
    displayCounts(false);
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
    
    displayCounts(false);
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::singleRedrawCounts(int window)
{
    displayCounts(false);
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::onRedColorSelected(QString name)
{
    displayCounts(_firstRed);
    if( _firstRed && false == _contrast_widget->is_global_contrast_checked())
    {
        _firstRed = false;
    }
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::onGreenColorSelected(QString name)
{
    displayCounts(_firstGreen);
    if( _firstGreen && false == _contrast_widget->is_global_contrast_checked())
    {
        _firstGreen = false;
    }
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::onBlueColorSelected(QString name)
{
    displayCounts(_firstBlue);
    if( _firstBlue && false == _contrast_widget->is_global_contrast_checked())
    {
        _firstBlue = false;
    }
}

//---------------------------------------------------------------------------

void CoLocalizationWidget::displayCounts(bool update_Min_MAx)
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

    ArrayXXr<float> red_normalized;
    ArrayXXr<float> green_normalized;
    ArrayXXr<float> blue_normalized;
    ArrayXXr<float> sum_normalized;
    GenerateImageProp props;
    props.analysis_type = _curAnalysis.toStdString();
    props.log_color = false;
    props.normalizer = nullptr;
    props.calib_curve = nullptr;
    props.contrast_limits = _contrast_widget->get_contrast_limits();
    props.show_legend = false;
    props.invert_y = Preferences::inst()->getValue(STR_INVERT_Y_AXIS).toBool();
    props.global_contrast = _contrast_widget->is_global_contrast_checked();
    /*
    if (props.global_contrast)
    {
        props.contrast_max = _contrast_widget->max_contrast_perc();
        props.contrast_min =  _contrast_widget->min_contrast_perc();
    }
    else
    {
        m_imageViewWidget->getMinMaxAt(0, props.contrast_min, props.contrast_max);        
    }*/
    
    if (_fit_counts.count(_cb_red_element->currentText().toStdString()) > 0)
    {
        if (props.global_contrast)
        {
            props.contrast_max = _contrast_widget->max_contrast_perc();
            props.contrast_min =  _contrast_widget->min_contrast_perc();
        }
        else
        {
            if(!_ck_quad_view->isChecked())
            {
                m_imageViewWidget->getMinMaxAt(0, props.contrast_min, props.contrast_max);
            }
            else
            {
                m_imageViewWidget->getMinMaxAt(RED_WINDOW, props.contrast_min, props.contrast_max);
            }
        }

        props.element = _cb_red_element->currentText().toStdString();
        props.selected_colormap = &_red_colormap;
        red_pixmap = _model->gen_pixmap(props, red_normalized);
        sum_pixmap = red_pixmap;
        sum_normalized = red_normalized;
        first = false;
    }
    if (_fit_counts.count(_cb_green_element->currentText().toStdString()) > 0)
    {
        if (props.global_contrast)
        {
            props.contrast_max = _contrast_widget->max_contrast_perc();
            props.contrast_min =  _contrast_widget->min_contrast_perc();
        }
        else 
        {
            if(!_ck_quad_view->isChecked())
            {
                m_imageViewWidget->getMinMaxAt(0, props.contrast_min, props.contrast_max);
            }
            else 
            {
                m_imageViewWidget->getMinMaxAt(GREEN_WINDOW, props.contrast_min, props.contrast_max);
            }
        }
        props.element = _cb_green_element->currentText().toStdString();
        props.selected_colormap = &_green_colormap;
        green_pixmap = _model->gen_pixmap(props, green_normalized);
        if (first)
        {
            sum_pixmap = green_pixmap;
            sum_normalized = green_normalized;
            first = false;
        }
        else
        {
            sum_normalized += green_normalized;
            QPainter p(&sum_pixmap);
            p.setCompositionMode(QPainter::CompositionMode_Plus);
            p.drawPixmap(0, 0, green_pixmap);
            p.end();
        }
    }
    if (_fit_counts.count(_cb_blue_element->currentText().toStdString()) > 0)
    {
        if (props.global_contrast)
        {
            props.contrast_max = _contrast_widget->max_contrast_perc();
            props.contrast_min =  _contrast_widget->min_contrast_perc();
        }
        else
        {
            if(!_ck_quad_view->isChecked())
            {
                m_imageViewWidget->getMinMaxAt(0, props.contrast_min, props.contrast_max);
            }
            else
            {
                m_imageViewWidget->getMinMaxAt(BLUE_WINDOW, props.contrast_min, props.contrast_max);
            }
        }
        props.element = _cb_blue_element->currentText().toStdString();
        props.selected_colormap = &_blue_colormap;
        blue_pixmap = _model->gen_pixmap(props, blue_normalized);
        if (first)
        {
            sum_pixmap = blue_pixmap;
            sum_normalized = blue_normalized;
            first = false;
        }
        else
        {
            sum_normalized += blue_normalized;
            QPainter p(&sum_pixmap);
            p.setCompositionMode(QPainter::CompositionMode_Plus);
            p.drawPixmap(0, 0, blue_pixmap);
            p.end();
        }
    }

    if (_ck_quad_view->isChecked())
    {
        m_imageViewWidget->setSubScenePixmap(RED_WINDOW, red_pixmap);
        m_imageViewWidget->setSubScenePixmap(GREEN_WINDOW, green_pixmap);
        m_imageViewWidget->setSubScenePixmap(BLUE_WINDOW, blue_pixmap);
        m_imageViewWidget->setSubScenePixmap(3, sum_pixmap);
        
        if(red_normalized.size() > 0)
        {
            m_imageViewWidget->setCountsTrasnformAt(RED_WINDOW, red_normalized);
            if (update_Min_MAx)
            {
                m_imageViewWidget->updateMinMax(RED_WINDOW, red_normalized);
            }
        }
        if(green_normalized.size() > 0)
        {
            m_imageViewWidget->setCountsTrasnformAt(GREEN_WINDOW, green_normalized);
            if (update_Min_MAx)
            {
                m_imageViewWidget->updateMinMax(GREEN_WINDOW, green_normalized);
            }
        }
        if(blue_normalized.size() > 0)
        {
            m_imageViewWidget->setCountsTrasnformAt(BLUE_WINDOW, blue_normalized);
            if (update_Min_MAx)
            {
                m_imageViewWidget->updateMinMax(BLUE_WINDOW, blue_normalized);
            }
        }
        if(sum_normalized.size() > 0)
        {
            m_imageViewWidget->setCountsTrasnformAt(3, sum_normalized);
            if (update_Min_MAx)
            {
                m_imageViewWidget->updateMinMax(3, sum_normalized);
            }
        }
    }
    else
    {
        m_imageViewWidget->setScenePixmap(sum_pixmap);
        if(sum_normalized.size() > 0)
        {
            m_imageViewWidget->setCountsTrasnformAt(0, sum_normalized);
            if (update_Min_MAx)
            {
                m_imageViewWidget->updateMinMax(0, sum_normalized);
            }
        }
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

    QString saved_red = _cb_red_element->currentText();
    QString saved_green = _cb_green_element->currentText();
    QString saved_blue = _cb_blue_element->currentText();

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

    if(_cb_red_element->findText(saved_red) > -1)
    {
        _cb_red_element->setCurrentText(saved_red);
    }
    if(_cb_green_element->findText(saved_green) > -1)
    {
        _cb_green_element->setCurrentText(saved_green);
    }
    if(_cb_blue_element->findText(saved_blue) > -1)
    {
        _cb_blue_element->setCurrentText(saved_blue);
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
