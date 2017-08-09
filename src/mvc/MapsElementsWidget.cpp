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

    QLayout* layout = generateDefaultLayout();
    layout->addWidget(_spectra_widget);
    //appendAnnotationTab();
    setLayout(layout);

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::setModel(MapsH5Model* model)
{
    _model = model;

    XrfAnalyzedCounts* nnls = _model->getAnalyzedCounts("NNLS");
    if (nnls != nullptr)
    {
        int length = nnls->width() * nnls->height();
        // Build a colour table of grayscale
        QByteArray data(length, (char)0);

        float *counts = nnls->get_counts_ptr("Num_Iter");
        float counts_max = nnls->get_counts_max("Num_Iter");
        float counts_min = nnls->get_counts_min("Num_Iter");
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
    _spectra_widget->append_spectra("Integrated Spectra", _model->getIntegratedSpectra());
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
