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

signals:
	void onNewGridLayout(int rows, int cols);

public slots:
	void onUpdate();

protected:

   void createLayout();

private:
   QPushButton *_btn_update;
   QPushButton *_btn_cancel;

   QComboBox *_cb_element_selector;
};


//---------------------------------------------------------------------------

#endif /* ElementInfoDialog_H_ */

//---------------------------------------------------------------------------

