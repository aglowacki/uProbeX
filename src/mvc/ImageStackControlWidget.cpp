/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ImageStackControlWidget.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ImageStackControlWidget::ImageStackControlWidget(QWidget* parent) : QWidget(parent)
{

    createLayout();

}

/*---------------------------------------------------------------------------*/

ImageStackControlWidget::~ImageStackControlWidget()
{

}

/*---------------------------------------------------------------------------*/

void ImageStackControlWidget::createLayout()
{
    QVBoxLayout* vlayout = new QVBoxLayout();
    QHBoxLayout* hlayout = new QHBoxLayout();
	_imageGrid = new MapsElementsWidget(1,1);

    _left_btn =  new QPushButton();
    _left_btn->setIcon(QIcon(":/images/previous.png"));
    _left_btn->setMaximumWidth(150);
    _image_name_cb = new QComboBox();
    _right_btn =  new QPushButton();
    _right_btn->setIcon(QIcon(":/images/next.png"));
    _right_btn->setMaximumWidth(150);

    hlayout->addWidget(_left_btn);
    hlayout->addWidget(_image_name_cb);
    hlayout->addWidget(_right_btn);

	connect(_image_name_cb, SIGNAL(currentIndexChanged(const QString &text)), this, SLOT(h5IndexChanged(const QString &text)));

	connect(this, SIGNAL(newH5ModelSelected), _imageGrid, SLOT(setModel));

    vlayout->addItem(hlayout);
	vlayout->addWidget(_imageGrid);
	setLayout(vlayout);
}

/*---------------------------------------------------------------------------*/

void ImageStackControlWidget::h5IndexChanged(const QString &text)
{
	if (_h5_model_map.count(text) > 0)
	{
		emit newH5ModelSelected(_h5_model_map[text]);
	}
}

/*---------------------------------------------------------------------------*/

void ImageStackControlWidget::setModel(MapsWorkspaceModel *model)
{
    _model = model;
}

/*---------------------------------------------------------------------------*/

void ImageStackControlWidget::loadList_H5(QStringList sl)
{
    foreach (QString s, sl)
    {
        _h5_model_map[s] = _model->getMapsH5Model(s);
        _image_name_cb->addItem(s);
    }
}

/*---------------------------------------------------------------------------*/

void ImageStackControlWidget::unloadList_H5(QStringList sl)
{
    foreach (QString s, sl)
    {
        _h5_model_map.erase(s);
        _model->unload_H5_Model(s);
        for(int i=0; i < _image_name_cb->count(); i++)
        {
            if (s == _image_name_cb->itemText(i))
            {
                _image_name_cb->removeItem(i);
                break;
            }
        }
    }

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
