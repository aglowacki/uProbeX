/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsElementsWidget.h>

#include <ImageViewWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>


#include <QDebug>


using gstar::AbstractImageWidget;
using gstar::ImageViewWidget;

/*---------------------------------------------------------------------------*/

MapsElementsWidget::MapsElementsWidget(QWidget* parent)
    : AbstractImageWidget(parent)
{

    _model = nullptr;
    _spectra_widget = new SpectraWidget();
    createLayout();

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

void MapsElementsWidget::createLayout()
{

    _cb_analysis = new QComboBox(this);
    _cb_element = new QComboBox(this);

    connect(_cb_analysis, SIGNAL(currentIndexChanged(QString)), this, SLOT(onAnalysisSelect(QString)));
    connect(_cb_element, SIGNAL(currentIndexChanged(QString)), this, SLOT(onElementSelect(QString)));

    QHBoxLayout* hbox = new QHBoxLayout();

    QLayout* layout = generateDefaultLayout();
    layout->addWidget(_spectra_widget);


    hbox->addWidget(_cb_analysis);
    hbox->addWidget(_cb_element);
    layout->addItem(hbox);
    //appendAnnotationTab();
    setLayout(layout);

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::onAnalysisSelect(QString name)
{
    QString elementName = _cb_element->currentText();
    bool found_element = false;

    XrfAnalyzedCounts* analysis_type = _model->getAnalyzedCounts(name.toStdString());
    if (analysis_type != nullptr)
    {
        _cb_element->clear();
        std::vector<std::string> elements = analysis_type->get_count_names();

        for(auto& itr: elements)
        {
            QString val = QString(itr.c_str());
            _cb_element->addItem(val);
            if(elementName == val)
            {
                found_element = true;
            }
        }
        if(found_element)
        {
            _cb_element->setCurrentText(elementName);
            displayCounts(name.toStdString(), elementName.toStdString());
        }
        else
        {
            displayCounts(name.toStdString(), elements[0]);
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

void MapsElementsWidget::setModel(MapsH5Model* model)
{
    _model = model;

    std::vector<std::string> analysis_types = _model->getAnalyzedTypes();

    for(auto& itr: analysis_types)
    {
        _cb_analysis->addItem(QString(itr.c_str()));
    }

    displayCounts("NNLS", "Num_Iter");

    if(_model->is_integrated_spectra_loaded())
    {
        _spectra_widget->append_spectra("Integrated Spectra", _model->getIntegratedSpectra());
    }

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::displayCounts(std::string analysis_type, std::string element)
{
    XrfAnalyzedCounts* nnls = _model->getAnalyzedCounts(analysis_type);
    if (nnls != nullptr)
    {
        int length = nnls->width() * nnls->height();
        // Build a colour table of grayscale
        QByteArray data(length, (char)0);

        float *counts = nnls->get_counts_ptr(element);
        float counts_max = nnls->get_counts_max(element);
        float counts_min = nnls->get_counts_min(element);
        float max_min = counts_max - counts_min;
        for (int i = 0; i < length; ++i)
        {
            data[i] = (unsigned char)( ((counts[i] - counts_min) / max_min) * 255);
        }

        QVector<QRgb> grayscale;

        for (int i = 0; i < 256; ++i)
        {
            grayscale.append(qRgb(i, i, i));
        }

        QImage image((const uchar *)data.constData(), nnls->width(), nnls->height(), nnls->width(), QImage::Format_Indexed8);
        image.setColorTable(grayscale);


        this->updateFrame(&image);
    }
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::windowChanged(Qt::WindowStates oldState,
                                       Qt::WindowStates newState)
{
    Q_UNUSED(oldState);

    if(Qt::WindowMaximized || Qt::WindowActive == newState)
    {
        m_imageViewWidget->resizeEvent(NULL);
    }

}

/*---------------------------------------------------------------------------*/
