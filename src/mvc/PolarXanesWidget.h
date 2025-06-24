/*-----------------------------------------------------------------------------
 * Copyright (c) 2025, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef POLAR_XANES_WIDGET_H
#define POLAR_XANES_WIDGET_H

//---------------------------------------------------------------------------

#include <QComboBox>
#include <mvc/MapsH5Model.h>
#include <mvc/SpectraWidget.h>
#include <mvc/PolarXanesWidget.h>

//---------------------------------------------------------------------------

/**
 * @brief When open the acquisition window, the widget is showing for capturing
 * the image from the area detector writer, the window will also be updated to
 * show the image.
 */
class PolarXanesWidget : public QWidget
{

    Q_OBJECT

public:

    /**
    * Constructor.
    */
    PolarXanesWidget(QWidget* parent = nullptr);

    /**
    * Destructor.
    */
    ~PolarXanesWidget();

    void createLayout();

    void setModel(MapsH5Model* model);

protected:

    void _plot_new_element(QString analysis, QString element);

protected slots:

    void onSelecAnalysisOrElement(QString val);

private:

    SpectraWidget* _spectra_widget;

    ArrayDr* _lhcp_int_spec;

    ArrayDr* _rhcp_int_spec;

    MapsH5Model* _model;

    QComboBox* _cb_analysis;

    QComboBox* _cb_elements;
};


//---------------------------------------------------------------------------

#endif /* PolarXanesWidget_H_ */

//---------------------------------------------------------------------------

