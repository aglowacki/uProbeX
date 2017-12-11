/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsElementsWidget.h>

#include <ImageViewWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>

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


    QVBoxLayout* layout = new QVBoxLayout();

    QSplitter* splitter = new QSplitter();
    splitter->setOrientation(Qt::Horizontal);
    splitter->addWidget(m_imageViewWidget);
    splitter->setStretchFactor(0, 1);
    splitter->addWidget(m_tabWidget);
    createToolBar(m_imageViewWidget);
    layout->addWidget(m_toolbar);
    layout->addWidget(splitter);
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

    data_struct::xrf::Fit_Count_Dict* analysis_type = _model->getAnalyzedCounts(name.toStdString());
    if (analysis_type != nullptr)
    {
        _cb_element->clear();
        //std::vector<std::string> elements = analysis_type->get_count_names();
        std::string lastName;
        for(auto& itr: *analysis_type)
        {
            QString val = QString(itr.first.c_str());
            _cb_element->addItem(val);
            if(elementName == val)
            {
                found_element = true;
            }
            lastName = itr.first;
        }
        if(found_element)
        {
            _cb_element->setCurrentText(elementName);
            displayCounts(name.toStdString(), elementName.toStdString());
        }
        else
        {
            displayCounts(name.toStdString(), lastName);
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

    //disconnect(_cb_analysis, SIGNAL(currentIndexChanged(QString)), this, SLOT(onAnalysisSelect(QString)));

    std::vector<std::string> analysis_types = _model->getAnalyzedTypes();

    //m_imageWidgetToolBar->clearImageViewWidget();
    for(auto& itr: analysis_types)
    {
        _cb_analysis->addItem(QString(itr.c_str()));
        //m_imageWidgetToolBar->appendImageViewWidget(imageViewWidget);
    }

    if(_model->is_integrated_spectra_loaded())
    {
        _spectra_widget->append_spectra("Integrated Spectra", _model->getIntegratedSpectra());
    }

    //connect(_cb_analysis, SIGNAL(currentIndexChanged(QString)), this, SLOT(onAnalysisSelect(QString)));

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::redrawCounts()
{
    displayCounts(_cb_analysis->currentText().toStdString(), _cb_element->currentText().toStdString());
}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::displayCounts(std::string analysis_type, std::string element)
{
    data_struct::xrf::Fit_Count_Dict* fit_counts = _model->getAnalyzedCounts(analysis_type);
    if (fit_counts != nullptr)
    {
        if(fit_counts->count(element) > 0)
        {
            int height = fit_counts->at(element).rows();
            int width = fit_counts->at(element).cols();
            unsigned long length = height * width;
            // Build a colour table of grayscale
            QByteArray data(length, (char)0);

            float counts_max = fit_counts->at(element).maxCoeff();
            float counts_min = fit_counts->at(element).minCoeff();
            float max_min = counts_max - counts_min;
            int i=0;
            for(int row = 0; row < height; row++)
            {
                for(int col = 0; col < width; col++)
                {
                    data[i] = (unsigned char)( ((fit_counts->at(element)(row,col) - counts_min) / max_min) * 255);
                    i++;
                }
            }
            QVector<QRgb> grayscale;

            for (int i = 0; i < 255; ++i)
            {
                grayscale.append(qRgb(i, i, i));
            }

            QImage image((const uchar *)data.constData(), width, height, width, QImage::Format_Indexed8);
            image.setColorTable(grayscale);


            m_imageViewWidget->scene()->setPixmap(QPixmap::fromImage(image.convertToFormat(QImage::Format_RGB32)));
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
        m_imageViewWidget->resizeEvent(NULL);
    }

}

/*---------------------------------------------------------------------------*/
