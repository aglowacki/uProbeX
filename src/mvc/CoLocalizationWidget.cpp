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
    
    _cb_red_element = new QComboBox();
    connect(_cb_red_element, SIGNAL(currentIndexChanged(QString)), this, SLOT(onColorSelected(QString)));
    _cb_green_element = new QComboBox();
    connect(_cb_green_element, SIGNAL(currentIndexChanged(QString)), this, SLOT(onColorSelected(QString)));
    _cb_blue_element = new QComboBox();
    connect(_cb_blue_element, SIGNAL(currentIndexChanged(QString)), this, SLOT(onColorSelected(QString)));

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

    setLayout(layout);

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

    bool first = true;

    ArrayXXr<float> int_img;
    
    if (_fit_counts.count(_cb_red_element->currentText().toStdString()) > 0)
    {
        red_img = m_imageViewWidget->generate_img(_fit_counts.at(_cb_red_element->currentText().toStdString()), _red_colormap);
        sum_img = red_img.convertToFormat(QImage::Format_RGB32);
        first = false;
    }
    if (_fit_counts.count(_cb_green_element->currentText().toStdString()) > 0)
    {
        green_img = m_imageViewWidget->generate_img(_fit_counts.at(_cb_green_element->currentText().toStdString()), _green_colormap);
        if (first)
        {
            sum_img = green_img.convertToFormat(QImage::Format_RGB32);
            first = false;
        }
        else
        {
            QPainter p(&sum_img);
            p.setCompositionMode(QPainter::CompositionMode_Plus);
            p.drawImage(0, 0, green_img.convertToFormat(QImage::Format_RGB32));
            p.end();
        }
    }
    if (_fit_counts.count(_cb_blue_element->currentText().toStdString()) > 0)
    {
        blue_img = m_imageViewWidget->generate_img(_fit_counts.at(_cb_blue_element->currentText().toStdString()), _blue_colormap);
        if (first)
        {
            sum_img = blue_img.convertToFormat(QImage::Format_RGB32);;
            first = false;
        }
        else
        {
            QPainter p(&sum_img);
            p.setCompositionMode(QPainter::CompositionMode_Plus);
            p.drawImage(0, 0, blue_img.convertToFormat(QImage::Format_RGB32));
            p.end();
        }
    }

    if (_ck_quad_view->isChecked())
    {
        if (Preferences::inst()->getValue(STR_INVERT_Y_AXIS).toBool())
        {
            red_img = red_img.mirrored(false, true);
            green_img = green_img.mirrored(false, true);
            blue_img = blue_img.mirrored(false, true);
            sum_img = sum_img.mirrored(false, true);
        }
        m_imageViewWidget->scene(0)->setPixmap(QPixmap::fromImage(red_img.convertToFormat(QImage::Format_RGB32)));
        m_imageViewWidget->scene(1)->setPixmap(QPixmap::fromImage(green_img.convertToFormat(QImage::Format_RGB32)));
        m_imageViewWidget->scene(2)->setPixmap(QPixmap::fromImage(blue_img.convertToFormat(QImage::Format_RGB32)));
        m_imageViewWidget->scene(3)->setPixmap(QPixmap::fromImage(sum_img.convertToFormat(QImage::Format_RGB32)));
    }
    else
    {
        if (Preferences::inst()->getValue(STR_INVERT_Y_AXIS).toBool())
        {
            sum_img = sum_img.mirrored(false, true);
        }
        m_imageViewWidget->scene(0)->setPixmap(QPixmap::fromImage(sum_img.convertToFormat(QImage::Format_RGB32)));
    }
    if (_first_pixmap_set)
    {
        m_imageViewWidget->clickFill(true);
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
    //model_updated();
    //m_imageViewWidget->restoreLabels(element_view_list);
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
        std::map<std::string, data_struct::ArrayXXr<float>>* scalers = _model->getScalers();
        if (scalers != nullptr)
        {
            for (auto& itr : *scalers)
            {
                fit_counts.insert(itr);
            }
        }

        //copy
        _fit_counts = fit_counts;

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

        std::vector<std::string> final_counts_to_add_before_scalers;
        final_counts_to_add_before_scalers.push_back(STR_COHERENT_SCT_AMPLITUDE);
        final_counts_to_add_before_scalers.push_back(STR_COMPTON_AMPLITUDE);
        final_counts_to_add_before_scalers.push_back(STR_SUM_ELASTIC_INELASTIC_AMP);
        final_counts_to_add_before_scalers.push_back(STR_TOTAL_FLUORESCENCE_YIELD);
        final_counts_to_add_before_scalers.push_back(STR_NUM_ITR);
        final_counts_to_add_before_scalers.push_back(STR_RESIDUAL);

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
        QString formattedTime = date.toString("yyyy.MM.dd_hh.mm.ss");
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
