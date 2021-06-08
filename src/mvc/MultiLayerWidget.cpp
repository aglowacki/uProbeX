/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MultiLayerWidget.h>

#include <gstar/ImageViewWidget.h>

#include <QSplitter>
#include <QMessageBox>

/*---------------------------------------------------------------------------*/

MultiLayerWidget::MultiLayerWidget(QWidget* parent) : AbstractImageWidget(1, 1, parent)
{

    createLayout();

}

/*---------------------------------------------------------------------------*/

MultiLayerWidget::~MultiLayerWidget()
{

}

/*---------------------------------------------------------------------------*/

void MultiLayerWidget::createLayout()
{
    QVBoxLayout* counts_layout = new QVBoxLayout();

    QSplitter* splitter = new QSplitter();
    splitter->setOrientation(Qt::Horizontal);
    splitter->addWidget(m_imageViewWidget);
    splitter->setStretchFactor(0, 1);
    splitter->addWidget(m_tabWidget);

    createToolBar(m_imageViewWidget);
    counts_layout->addWidget(m_toolbar);
    counts_layout->addWidget(splitter);

    //don't erase counts when mouse is off scene
//    m_imageViewWidget->set_null_mouse_pos = false;
//    connect(m_imageViewWidget, SIGNAL(cbLabelChanged(QString, int)), this, SLOT(onElementSelect(QString, int)));

    appendAnnotationTab();

//    createActions();
    
    setLayout(counts_layout);
    
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/