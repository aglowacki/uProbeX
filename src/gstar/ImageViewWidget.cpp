/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/ImageViewWidget.h>

using namespace gstar;

//---------------------------------------------------------------------------

ImageViewWidget::ImageViewWidget(int rows, int cols , QWidget* parent)
: QWidget(parent), m_fillState(false)
{

   m_coordWidget = nullptr;

   // Zoom in/out cursors
   m_zoomInCursor = QCursor(QPixmap(":/images/zoomin.png"));
   m_zoomOutCursor = QCursor(QPixmap(":/images/zoomout.png"));

   m_zoomPercent = nullptr;
   
   set_null_mouse_pos = true;

}

//---------------------------------------------------------------------------

ImageViewWidget::~ImageViewWidget()
{

}

//---------------------------------------------------------------------------

void ImageViewWidget::setCoordsVisible(bool val)
{
    m_coordWidget->setVisible(val, val, val);
}

//---------------------------------------------------------------------------

QImage ImageViewWidget::generate_img(ArrayXXr<float>& int_img, QVector<QRgb>& colormap)
{
    float counts_max = int_img.maxCoeff();
    float counts_min = int_img.minCoeff();
    int width = static_cast<unsigned int>(int_img.cols());
    int height = static_cast<unsigned int>(int_img.rows());
    QImage image(width, height, QImage::Format_Indexed8);
    image.setColorTable(colormap);

    float max_min = counts_max - counts_min;
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            //first clamp the data to max min
            float cnts = int_img(row, col);
            cnts = std::min(counts_max, cnts);
            cnts = std::max(counts_min, cnts);
            //convert to pixel
            unsigned char data = static_cast<unsigned char>(((cnts - counts_min) / max_min) * 255.0);
            image.setPixel(col, row, (uint)data);
        }
    }
    return image;
}

//---------------------------------------------------------------------------

CoordinateWidget* ImageViewWidget::coordinateWidget()
{

   return m_coordWidget;

}

//---------------------------------------------------------------------------

void ImageViewWidget::enterEvent(QEvent * event)
{

   QWidget::enterEvent(static_cast<QEnterEvent*>(event));

}

//---------------------------------------------------------------------------

void ImageViewWidget::leaveEvent(QEvent * event)
{

   if(set_null_mouse_pos)
        m_coordWidget -> setnullptr();

   QLayout* lout = layout();
   QWidget* w = lout->itemAt(lout->count()-1)->widget();
   CoordinateWidget* lightToMicroCoordWidget = dynamic_cast<CoordinateWidget*>(w);
   if(set_null_mouse_pos)
        lightToMicroCoordWidget->setnullptr();

   QWidget::leaveEvent(event);

}

//---------------------------------------------------------------------------

void ImageViewWidget::setCoordinateModel(CoordinateModel* model)
{

   m_coordWidget->setModel(model);

}

//---------------------------------------------------------------------------

void ImageViewWidget::sceneRectUpdated(const QRectF& rect)
{

   Q_UNUSED(rect)

   // Force a resize
   resizeEvent(nullptr);

}

//---------------------------------------------------------------------------

void ImageViewWidget::setZoomPercentWidget(QComboBox* zoomPercent)
{

    m_zoomPercent  = zoomPercent;
    connect(m_zoomPercent, &QComboBox::currentIndexChanged, this, &ImageViewWidget::zoomValueChanged);
    updateZoomPercentage();

}

//---------------------------------------------------------------------------

void ImageViewWidget::updateZoomPercentage()
{

    if (m_zoomPercent == nullptr) return;

    qreal wp = getCurrentZoomPercent();

    disconnect(m_zoomPercent, &QComboBox::currentIndexChanged, this, &ImageViewWidget::zoomValueChanged);

    m_zoomPercent->setEditText(QString("%1").arg(wp, 0, 'f',  0));

    connect(m_zoomPercent, &QComboBox::currentIndexChanged, this, &ImageViewWidget::zoomValueChanged);

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------