/*-----------------------------------------------------------------------------
 * Copyright (c) 2025, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef POLAR_XANES_WIDGET_H
#define POLAR_XANES_WIDGET_H

//---------------------------------------------------------------------------

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

    void setIntegratedSpectra(ArrayDr* lhcp_int_spec, ArrayDr* rhcp_int_spec);

    void clearAllSpectra();

private:

    SpectraWidget* _spectra_widget;

    ArrayDr* _lhcp_int_spec;

    ArrayDr* _rhcp_int_spec;

};


//---------------------------------------------------------------------------

#endif /* PolarXanesWidget_H_ */

//---------------------------------------------------------------------------

