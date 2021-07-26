/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_SUB_IMAGE_WIDGET_H
#define GSTAR_SUB_IMAGE_WIDGET_H

/*---------------------------------------------------------------------------*/

#include "gstar/CoordinateWidget.h"
#include "gstar/ImageViewScene.h"
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QListView>

#include <gstar/CountsLookupTransformer.h>
#include "gstar/ContrastDialog.h"


/*---------------------------------------------------------------------------*/

namespace gstar
{
    class SubImageWindow : public QObject
    {

        Q_OBJECT

    public:

        SubImageWindow();

        SubImageWindow(const SubImageWindow&);

        ~SubImageWindow();

        QGraphicsView* view;
        ImageViewScene* scene;
        QComboBox* cb_image_label;
        gstar::CountsLookupTransformer* counts_lookup;
        gstar::CoordinateModel* counts_coord_model;
        gstar::CoordinateWidget* counts_coord_widget;
        QPushButton* btn_contrast;

        QVBoxLayout* layout;

    public slots:
        void on_contrast_show();

    protected:
        ContrastDialog* _contrast_dialog;
        float _contrast_min;
        float _contrast_max;
    };

}

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
