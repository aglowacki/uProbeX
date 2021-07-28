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

		bool contrast_updated() { return _contrast_updated; }

		float contrast_min() { return _contrast_min; }
		
		float contrast_max() { return _contrast_max; }

        QGraphicsView* view;
        ImageViewScene* scene;
        QComboBox* cb_image_label;
        gstar::CountsLookupTransformer* counts_lookup;
        gstar::CoordinateModel* counts_coord_model;
        gstar::CoordinateWidget* counts_coord_widget;
        QPushButton* btn_contrast;

        QVBoxLayout* layout;

	signals:
		void redraw_event();

    public slots:
        void on_contrast_show();
		
		void on_update_min_max(float minCoef, float maxCoef);

		void on_accept_contrast();

		void on_cancel_contrast();

	protected:
        ContrastDialog* _contrast_dialog;

		bool _contrast_updated;

        float _contrast_min;
        
		float _contrast_max;
    };

}

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
