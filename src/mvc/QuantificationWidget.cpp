/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/QuantificationWidget.h>

//---------------------------------------------------------------------------

QuantificationWidget::QuantificationWidget(QWidget* parent) : gstar::AbstractImageWidget(1, 1, parent)
{
    _model = nullptr;
    setAnnotationsEnabled(false);
    _createLayout();
}

//---------------------------------------------------------------------------

QuantificationWidget::~QuantificationWidget()
{

}

//---------------------------------------------------------------------------

void QuantificationWidget::_createLayout()
{
    
    QVBoxLayout* layout = new QVBoxLayout();
    

    setLayout(layout);

}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void QuantificationWidget::setModel(MapsH5Model* model)
{
    if (_model != model)
    {
        _model = model;
        //model_updated();
        if (_model != nullptr)
        {
            
        }
    }
}

//---------------------------------------------------------------------------

void QuantificationWidget::windowChanged(Qt::WindowStates oldState,
                                       Qt::WindowStates newState)
{
    Q_UNUSED(oldState);

    if(Qt::WindowMaximized || Qt::WindowActive == newState)
    {
        m_imageViewWidget->resizeEvent(nullptr);
    }

}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
