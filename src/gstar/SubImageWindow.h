/*-----------------------------------------------------------------------------
 * Copyright (c) 2021, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef GSTAR_SUB_IMAGE_WIDGET_H
#define GSTAR_SUB_IMAGE_WIDGET_H

//---------------------------------------------------------------------------

#include "gstar/CoordinateWidget.h"
#include "gstar/ImageViewScene.h"
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QListView>

#include <gstar/CountsLookupTransformer.h>
#include <gstar/CountsStatsTransformer.h>
#include "gstar/ContrastDialog.h"


//---------------------------------------------------------------------------

namespace gstar
{
    class SubImageWindow : public QObject
    {

        Q_OBJECT

    public:

        SubImageWindow();

        SubImageWindow(const SubImageWindow&);

        virtual ~SubImageWindow();

		bool contrast_updated() { return _contrast_updated; }

		float contrast_min() { return _contrast_min; }
		
		float contrast_max() { return _contrast_max; }

        void setImageLabelVisible(bool val) { cb_image_label->setVisible(val); }

        void setCountsVisible(bool val);

        void redraw() { emit redraw_event(); }

        QGraphicsView* view;
        ImageViewScene* scene;
        QComboBox* cb_image_label;

        gstar::CountsLookupTransformer* counts_lookup;
        gstar::CoordinateModel* counts_coord_model;
        gstar::CoordinateWidget* counts_coord_widget;

        gstar::CountsStatsTransformer* counts_stats;
        gstar::CoordinateModel* counts_stats_model; 
        gstar::CoordinateWidget* counts_stats_widget;
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

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
