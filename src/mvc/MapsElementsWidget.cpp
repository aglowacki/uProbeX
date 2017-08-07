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
    createLayout();

}

/*---------------------------------------------------------------------------*/

MapsElementsWidget::~MapsElementsWidget()
{

    if(_model != nullptr)
    {
        delete _model;
    }
    _model = nullptr;

}

/*---------------------------------------------------------------------------*/

void MapsElementsWidget::createLayout()
{

    QLayout* layout = generateDefaultLayout();
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
        QByteArray data(length, 0);

        float *counts = nnls->get_counts_ptr("Si");
        for (int i = 0; i < length; ++i)
        {
            data[i] = (unsigned char)(counts[i] * 255);
        }

        QVector<QRgb> grayscale;

        for (int i = 0; i < 256; ++i)
        {
            grayscale.append(qRgb(i, i, i));
        }

        QImage image((const uchar *)data.constData(), nnls->width(), nnls->height(), QImage::Format_Indexed8);
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
