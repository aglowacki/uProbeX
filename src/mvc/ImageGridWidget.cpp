/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ImageGridWidget.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QGridlayout>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ImageGridWidget::ImageGridWidget(QWidget* parent) : QWidget(parent)
{

    createLayout();

}

/*---------------------------------------------------------------------------*/

ImageGridWidget::~ImageGridWidget()
{

}

/*---------------------------------------------------------------------------*/

void ImageGridWidget::createLayout()
{
	QGridLayout * glayout = new QGridLayout();
//    QVBoxLayout* vlayout = new QVBoxLayout();
 //   QHBoxLayout* hlayout = new QHBoxLayout();
	_imageViewArray.resize(16);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			gstar::ImageViewWidget * iw = new gstar::ImageViewWidget();
			_imageViewArray[(i*4) + j] = iw;
			glayout->addWidget(iw, i, j, 1, 1);
		}
	}

	setLayout(glayout);
}

/*---------------------------------------------------------------------------*/

void ImageGridWidget::setModel(MapsH5Model *model)
{
    _model = model;

	//data_struct::Fit_Count_Dict* fit_counts = _model->getAnalyzedCounts(analysis_type);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			
		}
	}
}

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
