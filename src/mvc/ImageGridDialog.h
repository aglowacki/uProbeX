/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef IMAGE_GRID_DIALOG_H
#define IMAGE_GRID_DIALOG_H

/*---------------------------------------------------------------------------*/

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QStringListModel>
#include <QListWidgetItem>
#include <QSpinBox>
#include <QDialogButtonBox>

/*---------------------------------------------------------------------------*/

/**
 * @brief When open the acquisition window, the widget is showing for capturing
 * the image from the area detector writer, the window will also be updated to
 * show the image.
 */
class ImageGridDialog : public QDialog
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   ImageGridDialog();

   /**
    * Destructor.
    */
   ~ImageGridDialog();

signals:
	void onNewGridLayout(int rows, int cols);

public slots:
	void onUpdate();

protected:

   void createLayout();

private:
	QLabel *rowLabel;
	QLabel *colLabel;

   //QDialogButtonBox *buttonBox;

   QPushButton *updateBtn;
   QPushButton *cancelBtn;

   QSpinBox *sbRow;
   QSpinBox *sbCol;
   
};


/*---------------------------------------------------------------------------*/

#endif /* ImageGridDialog_H_ */

/*---------------------------------------------------------------------------*/

