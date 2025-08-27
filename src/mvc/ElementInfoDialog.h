/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ELEMENT_INFO_DIALOG_H
#define ELEMENT_INFO_DIALOG_H

//---------------------------------------------------------------------------

#include <QDialog>
#include <QPushButton>
#include <QComboBox>
#include "data_struct/params_override.h"

//---------------------------------------------------------------------------

/**
 * @brief When open the acquisition window, the widget is showing for capturing
 * the image from the area detector writer, the window will also be updated to
 * show the image.
 */
class ElementInfoDialog : public QDialog
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   ElementInfoDialog();

   /**
    * Destructor.
    */
   ~ElementInfoDialog();

   void set_param_override(data_struct::Params_Override<double>* param_override) {_param_override = param_override;}

   void set_selected_element(const QString& el_name);

signals:
	void onNewGridLayout(int rows, int cols);

public slots:
	void onUpdate();

protected:

   void createLayout();

private:
   data_struct::Params_Override<double>* _param_override;

   QPushButton *_btn_update;
   QPushButton *_btn_cancel;

   QComboBox *_cb_element_selector;
};


//---------------------------------------------------------------------------

#endif /* ElementInfoDialog_H_ */

//---------------------------------------------------------------------------

