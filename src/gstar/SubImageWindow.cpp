/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gstar/SubImageWindow.h>

using namespace gstar;

/*---------------------------------------------------------------------------*/

SubImageWindow::SubImageWindow() : QObject()
{
    _contrast_dialog = nullptr;
	_contrast_updated = false;
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
    counts_coord_widget = new gstar::CoordinateWidget();
    counts_coord_widget->setModel(counts_coord_model);
    counts_coord_widget->setLabel("Counts:", "Min:", "Max:");
    counts_coord_widget->setUnitsLabel("cts/s");

    btn_contrast = new QPushButton("C");
    connect(btn_contrast, &QPushButton::pressed, this, &SubImageWindow::on_contrast_show);
    btn_contrast->setVisible(false);

    QHBoxLayout* hbox = new QHBoxLayout();
    hbox->addWidget(counts_coord_widget);
    hbox->addWidget(btn_contrast);

    layout = new QVBoxLayout();

    layout->addItem(hbox);
    layout->addWidget(view);
    layout->addWidget(cb_image_label);

}

/*---------------------------------------------------------------------------*/

SubImageWindow::SubImageWindow(const SubImageWindow& win)
{

    scene = win.scene;
    view = win.view;
    cb_image_label = win.cb_image_label;
    counts_coord_model = win.counts_coord_model;
    counts_coord_widget = win.counts_coord_widget;
    btn_contrast = win.btn_contrast;
    counts_lookup = win.counts_lookup;
    layout = win.layout;
    _contrast_dialog = win._contrast_dialog;
}

/*---------------------------------------------------------------------------*/

SubImageWindow::~SubImageWindow()
{
    delete scene;
    delete view;
    delete cb_image_label;
    delete counts_coord_model;
    delete counts_coord_widget;
    delete btn_contrast;
}
/*---------------------------------------------------------------------------*/

void SubImageWindow::on_contrast_show()
{
    if (_contrast_dialog == nullptr)
    {
        _contrast_dialog = new ContrastDialog();
		_contrast_dialog->setWindowTitle("Contrast");
		connect(_contrast_dialog, &ContrastDialog::on_min_max_update, this, &SubImageWindow::on_update_min_max);
		connect(_contrast_dialog, &ContrastDialog::accepted, this, &SubImageWindow::on_accept_contrast);
		connect(_contrast_dialog, &ContrastDialog::rejected, this, &SubImageWindow::on_cancel_contrast);
    }
	_contrast_dialog->show();
	_contrast_dialog->raise();
	_contrast_dialog->activateWindow();
    _contrast_dialog->set_array(counts_lookup->get_count_array());
    _contrast_dialog->min_max_updated();
}

/*---------------------------------------------------------------------------*/

void SubImageWindow::on_update_min_max(float minCoef, float maxCoef)
{
	_contrast_updated = true;
	_contrast_min = minCoef;
	_contrast_max = maxCoef;
	emit redraw_event();
}

/*---------------------------------------------------------------------------*/

void SubImageWindow::on_accept_contrast()
{
	_contrast_updated = true;
}

/*---------------------------------------------------------------------------*/

void SubImageWindow::on_cancel_contrast()
{
	_contrast_updated = false;
	emit redraw_event();
}

/*---------------------------------------------------------------------------*/
