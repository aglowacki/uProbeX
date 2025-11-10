/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/SubImageWindow.h>

using namespace gstar;

//---------------------------------------------------------------------------

SubImageWindow::SubImageWindow() : QObject()
{
	_contrast_updated = false;
    _contrast_min = 0.;
    _contrast_max = 1.;
    // Initialize scene
    scene = new ImageViewScene();
    //// scene->setSceneRect(scene->itemsBoundingRect());

    // Initialize view
    view = new QGraphicsView();
    view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    view->setScene(scene);

    cb_image_label = new QComboBox();

    counts_lookup = new gstar::CountsLookupTransformer();
    counts_coord_model = new gstar::CoordinateModel(counts_lookup);
    counts_coord_widget = new gstar::CoordinateWidget(true);
    counts_coord_widget->setModel(counts_coord_model);
    counts_coord_widget->setLabel("Counts: ", "Min:  ", "Max:  ");
    counts_coord_widget->setUnitsLabel("cts/s");
    counts_coord_widget->setContentsMargins(0, 0, 0, 0);

    counts_stats = new gstar::CountsStatsTransformer();
    counts_stats_model = new gstar::CoordinateModel(counts_stats);
    counts_stats_widget = new gstar::CoordinateWidget(true);
    counts_stats_widget->setModel(counts_stats_model);
    counts_stats_widget->setLabel("Avg: ", "Median:  ", "StDev:  ");
    counts_stats_widget->setUnitsLabel("");
    counts_stats_widget->setContentsMargins(0, 0, 0, 0);

    btn_contrast = new QPushButton("C");
    connect(btn_contrast, &QPushButton::pressed, this, &SubImageWindow::on_contrast_show);
    btn_contrast->setVisible(false);

    QHBoxLayout* hbox = new QHBoxLayout();
    hbox->addWidget(counts_coord_widget);
    hbox->addWidget(counts_stats_widget);
    hbox->addWidget(btn_contrast);
    hbox->setContentsMargins(0, 0, 0, 0);

    layout = new QVBoxLayout();
    layout->addItem(hbox);
    layout->addWidget(view);
    layout->addWidget(cb_image_label);
    layout->setContentsMargins(0, 0, 0, 0);
}

//---------------------------------------------------------------------------

SubImageWindow::SubImageWindow(const SubImageWindow& win)
{

    scene = win.scene;
    view = win.view;
    cb_image_label = win.cb_image_label;
    counts_coord_model = win.counts_coord_model;
    counts_coord_widget = win.counts_coord_widget;
    counts_stats_model = win.counts_stats_model;
    counts_stats_widget = win.counts_stats_widget;
    btn_contrast = win.btn_contrast;
    counts_lookup = win.counts_lookup;
    counts_stats = win.counts_stats;
    layout = win.layout;
}

//---------------------------------------------------------------------------

SubImageWindow::~SubImageWindow()
{
    delete view;
    delete cb_image_label;
    delete counts_coord_model;
    delete counts_coord_widget;
    delete counts_stats_model;
    delete counts_stats_widget;
    delete btn_contrast;
}

//---------------------------------------------------------------------------

void SubImageWindow::setCountsVisible(bool val)
{
    counts_coord_widget->setVisible(val, val, val); 
    counts_stats_widget->setVisible(val, val, val);
}

//---------------------------------------------------------------------------

void SubImageWindow::on_contrast_show()
{
    
    _contrast_updated = true;
    
    _contrast_dialog.setWindowTitle("Contrast");
    _contrast_dialog.set_array(counts_lookup->get_count_array());
    connect(&_contrast_dialog, &ContrastDialog::on_min_max_update, this, &SubImageWindow::on_update_min_max);
    connect(&_contrast_dialog, &ContrastDialog::accepted, this, &SubImageWindow::on_accept_contrast);
    connect(&_contrast_dialog, &ContrastDialog::rejected, this, &SubImageWindow::on_cancel_contrast);
    _contrast_dialog.min_max_updated();

	_contrast_dialog.show();
	_contrast_dialog.raise();
	_contrast_dialog.activateWindow();
}

//---------------------------------------------------------------------------

void SubImageWindow::on_update_min_max(float minCoef, float maxCoef, bool send_draw_event)
{
	_contrast_min = minCoef;
	_contrast_max = maxCoef;
    if(send_draw_event)
    {
	    emit redraw_event(this);
    }
}

//---------------------------------------------------------------------------

void SubImageWindow::on_accept_contrast()
{
	_contrast_updated = true;
}

//---------------------------------------------------------------------------

void SubImageWindow::on_cancel_contrast()
{
	_contrast_updated = false;
	emit redraw_event(this);
}

//---------------------------------------------------------------------------
