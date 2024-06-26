/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SCAN_REGION_DIALOG_H
#define SCAN_REGION_DIALOG_H

//---------------------------------------------------------------------------

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QStringListModel>
#include <QListWidgetItem>
#include <QSpinBox>
#include <QDialogButtonBox>

//---------------------------------------------------------------------------

/**
 * @brief When open the acquisition window, the widget is showing for capturing
 * the image from the area detector writer, the window will also be updated to
 * show the image.
 */
class ScanRegionDialog : public QDialog
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   ScanRegionDialog();

   /**
    * Destructor.
    */
   ~ScanRegionDialog();

signals:
	void NewScan();

public slots:
	void onUpdate();

protected:

   void _createLayout();

private:
	QLabel *rowLabel;
	QLabel *colLabel;

   //QDialogButtonBox *buttonBox;

   QPushButton *updateBtn;
   QPushButton *cancelBtn;

   QSpinBox *sbRow;
   QSpinBox *sbCol;
   
};


//---------------------------------------------------------------------------

#endif /* ImageGridDialog_H_ */

//---------------------------------------------------------------------------

