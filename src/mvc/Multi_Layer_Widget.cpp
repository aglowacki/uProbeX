/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/Multi_Layer_Widget.h>

#include <gstar/ImageViewWidget.h>

#include <QSplitter>
#include <QMessageBox>

/*---------------------------------------------------------------------------*/

Multi_Layer_Widget::Multi_Layer_Widget(QWidget* parent) : QWidget(parent)
{

    createLayout();

}

/*---------------------------------------------------------------------------*/

Multi_Layer_Widget::~Multi_Layer_Widget()
{

}

/*---------------------------------------------------------------------------*/

void Multi_Layer_Widget::createLayout()
{
    QVBoxLayout* layout = new QVBoxLayout();
    m_imageViewWidget = new gstar::ImageViewWidget(1,1);

    QSplitter* splitter = new QSplitter();
    splitter->setOrientation(Qt::Horizontal);
    splitter->addWidget(m_imageViewWidget);
    //splitter->setStretchFactor(0, 1);
    //splitter->addWidget(m_tabWidget);

    //createToolBar(m_imageViewWidget);
    //layout->addWidget(m_toolbar);
    layout->addWidget(splitter);

/*
    _tab_widget = new QTabWidget();
    _spectra_widget = new FitSpectraWidget();
    connect(_spectra_widget, &FitSpectraWidget::export_fit_paramters, this, &Multi_Layer_Widget::on_export_fit_params);
    connect(_spectra_widget, &FitSpectraWidget::export_csv_and_png, this, &Multi_Layer_Widget::on_export_csv);

    _cb_detector = new QComboBox(this);
    connect(_cb_detector, qOverload<const QString&>(&QComboBox::currentIndexChanged), this, &Multi_Layer_Widget::onDetectorSelect);

    _scaler_widget = new Scaler_Widget(this);

    QVBoxLayout* vbox = new QVBoxLayout();
    QHBoxLayout* hbox2 = new QHBoxLayout();
    QVBoxLayout* layout = new QVBoxLayout();

    hbox2->addWidget(new QLabel("Detector:"));
    hbox2->addWidget(_cb_detector);
    vbox->addItem(hbox2);
    vbox->addWidget(_spectra_widget);

    QStringList extra_pv_header = { "Name", "Value", "Unit", "Description" };

    _extra_pvs_table_widget = new QTableWidget(1, 4);
    _extra_pvs_table_widget->setHorizontalHeaderLabels(extra_pv_header);

    QWidget* window2 = new QWidget();
    window2->setLayout(vbox);

    _tab_widget->addTab(window2, "Integrated Spectra");
    _tab_widget->addTab(_scaler_widget, "Scalers");
    _tab_widget->addTab(_extra_pvs_table_widget, "Extra PV's");

    layout->addWidget(_tab_widget);
    */
    setLayout(layout);
    
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/