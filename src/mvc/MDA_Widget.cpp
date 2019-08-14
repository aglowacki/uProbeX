/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MDA_Widget.h>

#include <gstar/ImageViewWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>

#include <QDebug>


using gstar::AbstractImageWidget;
using gstar::ImageViewWidget;

/*---------------------------------------------------------------------------*/

MDA_Widget::MDA_Widget(QWidget* parent)
    : AbstractImageWidget(1,1,parent)
{

    _model = nullptr;
    createLayout();

}

/*---------------------------------------------------------------------------*/

MDA_Widget::~MDA_Widget()
{

}

/*---------------------------------------------------------------------------*/

void MDA_Widget::createLayout()
{

//    _tab_widget = new QTabWidget();
//    _spectra_widget = new FitSpectraWidget();

//    _cb_analysis = new QComboBox(this);
//    _cb_element = new QComboBox(this);
//    connect(_cb_analysis, SIGNAL(currentIndexChanged(QString)), this, SLOT(onAnalysisSelect(QString)));
//    connect(_cb_element, SIGNAL(currentIndexChanged(QString)), this, SLOT(onElementSelect(QString)));

//    QHBoxLayout* hbox = new QHBoxLayout();
//    QVBoxLayout* counts_layout = new QVBoxLayout();
//    QVBoxLayout* layout = new QVBoxLayout();

//    QSplitter* splitter = new QSplitter();
//    splitter->setOrientation(Qt::Horizontal);
//    splitter->addWidget(m_imageViewWidget);
//    splitter->setStretchFactor(0, 1);
//    splitter->addWidget(m_tabWidget);
//    createToolBar(m_imageViewWidget);
//    counts_layout->addWidget(m_toolbar);
//    counts_layout->addWidget(splitter);

//	_cb_colormap = new QComboBox();
//	_cb_colormap->addItem("Grayscale");
//	_cb_colormap->addItem("Heatmap");
//    connect(_cb_colormap, SIGNAL(currentIndexChanged(QString)), this, SLOT(onColormapSelect(QString)));

//    m_toolbar -> addWidget(new QLabel(" ColorMap :"));
//    m_toolbar -> addWidget(_cb_colormap);

//    hbox->addWidget(_cb_analysis);
//    hbox->addWidget(_cb_element);
//    counts_layout->addItem(hbox);

//	_pb_perpixel_fitting = new QPushButton("Per Pixel Fitting");
//	counts_layout->addWidget(_pb_perpixel_fitting);


//    QWidget *window = new QWidget();
//    window->setLayout(counts_layout);

//    _tab_widget->addTab(window, "Counts");
//    _tab_widget->addTab(_spectra_widget, "Integrated Spectra");

//    layout->addWidget(_tab_widget);

//    setLayout(layout);

}


 /*---------------------------------------------------------------------------*/

void MDA_Widget::setModel(MDA_Model* model)
{
    if(_model == model)
    {
        return;
    }
    _model = model;
    model_updated();
}

/*---------------------------------------------------------------------------*/

void MDA_Widget::model_updated()
{
    if(_model == nullptr)
    {
        return;
    }

}

/*---------------------------------------------------------------------------*/

void MDA_Widget::windowChanged(Qt::WindowStates oldState,
                                       Qt::WindowStates newState)
{
    Q_UNUSED(oldState);

    if(Qt::WindowMaximized || Qt::WindowActive == newState)
    {
        m_imageViewWidget->resizeEvent(nullptr);
    }

}

/*---------------------------------------------------------------------------*/
