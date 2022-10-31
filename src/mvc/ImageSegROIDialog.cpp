/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ImageSegRoiDialog.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QGridLayout>

//---------------------------------------------------------------------------
static const QString STR_KMEANS = QString("KMeans");

//---------------------------------------------------------------------------

ImageSegRoiDialog::ImageSegRoiDialog() : QDialog()
{

	_techLayout = new QVBoxLayout();
	_layout_map[STR_KMEANS] = _createKMeansLayout();
    createLayout();

}

//---------------------------------------------------------------------------

ImageSegRoiDialog::~ImageSegRoiDialog()
{

}

//---------------------------------------------------------------------------

QLayout* ImageSegRoiDialog::_createKMeansLayout()
{
	QVBoxLayout* layout = new QVBoxLayout();

	return layout;
}

//---------------------------------------------------------------------------

void ImageSegRoiDialog::createLayout()
{
	_techLabel = new QLabel("Auto Image Segment Technique:");
	_cb_tech = new QComboBox();
	_cb_tech->addItem(STR_KMEANS);

	updateBtn = new QPushButton("Run");
	cancelBtn = new QPushButton("Cancel");
	connect(updateBtn, SIGNAL(pressed()), this, SLOT(onUpdate()));
	connect(cancelBtn, SIGNAL(pressed()), this, SLOT(close()));

	QVBoxLayout *mainLayout = new QVBoxLayout;
	QHBoxLayout* optionLayout = new QHBoxLayout;
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	optionLayout->addWidget(_techLabel);
	optionLayout->addWidget(_cb_tech);

	buttonLayout->addWidget(updateBtn);
	buttonLayout->addWidget(cancelBtn);
	
	mainLayout->addItem(optionLayout);
	mainLayout->addItem(_techLayout);
	mainLayout->addItem(buttonLayout);

	_techLayout->addItem(_layout_map[STR_KMEANS]);

	setLayout(mainLayout);

	setWindowTitle("ROI Image Segmentation");
}

//---------------------------------------------------------------------------

void ImageSegRoiDialog::onSetTech(QString name)
{
	if (_layout_map.count(name) > 0)
	{
		_techLayout->removeItem(_techLayout->itemAt(0));
		_techLayout->addItem(_layout_map.at(name));
	}
}

//---------------------------------------------------------------------------

void ImageSegRoiDialog::onUpdate()
{
	// emit list of roi's
	//emit onNewROIs();
	close();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
