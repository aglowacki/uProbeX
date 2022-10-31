/*-----------------------------------------------------------------------------
 * Copyright (c) 2022, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef IMAGE_SEG_ROI_DIALOG_H
#define IMAGE_SEG_ROI_DIALOG_H

/*---------------------------------------------------------------------------*/

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QStringListModel>
#include <QListWidgetItem>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QDialogButtonBox>

/*---------------------------------------------------------------------------*/

class ImageSegRoiDialog : public QDialog
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   ImageSegRoiDialog();

   /**
    * Destructor.
    */
   ~ImageSegRoiDialog();

signals:
	void onNewROIs(std::vector<QPoint> roi_list);

public slots:
	void onUpdate();

	void onSetTech(QString name);

protected:

   void createLayout();

   QLayout* _createKMeansLayout();

private:
	QLabel* _techLabel;
	QComboBox* _cb_tech;
	
	QVBoxLayout* _techLayout;

	QPushButton* updateBtn;
	QPushButton* cancelBtn;
  
	std::map<QString, QLayout*> _layout_map;

};


/*---------------------------------------------------------------------------*/

#endif /* ImageSegRoiDialog_H_ */

/*---------------------------------------------------------------------------*/

