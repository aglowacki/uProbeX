/*-----------------------------------------------------------------------------
 * Copyright (c) 2026, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef CONTRAST_WIDGET_H
#define CONTRAST_WIDGET_H

//---------------------------------------------------------------------------

#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include "gstar/MinMaxSlider.h"

class ContrastWidget : public QWidget
{

    Q_OBJECT

    public:
    
    ContrastWidget(QWidget* parent = nullptr);

    ~ContrastWidget();

    void set_contrast_changed(QString val);

    QString get_contrast_limits() { return _cb_contrast->currentText(); }
    
    bool is_global_contrast_checked() { return _global_contrast_chk->isChecked(); }

    float max_contrast_perc() { return _max_contrast_perc; }
    
    float min_contrast_perc() { return _min_contrast_perc; }

    signals:

    void global_contrast_change(bool);

    void call_redraw();

    public slots:

    void on_contrast_changed(QString val);

    void on_min_max_contrast_changed();

    void on_global_contrast_changed(Qt::CheckState);

    private:


    gstar::MinMaxSlider* _contrast_widget;

    QComboBox* _cb_contrast;

    QCheckBox* _global_contrast_chk;

   float _min_contrast_perc;

   float _max_contrast_perc;
};

//---------------------------------------------------------------------------

#endif /* Contrast Widget */

//---------------------------------------------------------------------------

