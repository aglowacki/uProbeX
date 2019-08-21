/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ImageStackControlWidget.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QSplitter>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ImageStackControlWidget::ImageStackControlWidget(QWidget* parent) : QWidget(parent)
{
	createLayout();

}

/*---------------------------------------------------------------------------*/

ImageStackControlWidget::~ImageStackControlWidget()
{
	/* handeled by MapsWorkspaceMdodel
	for(auto &itr : _h5_model_map)
	{
		_model->unload_H5_Model(itr.first);
	}
	_h5_model_map.clear();
	*/
	if (_mapsFilsWidget != nullptr)
	{
		delete _mapsFilsWidget;
		_mapsFilsWidget = nullptr;
	}

}

/*---------------------------------------------------------------------------*/

void ImageStackControlWidget::createLayout()
{
    QVBoxLayout* vlayout = new QVBoxLayout();
    QHBoxLayout* hlayout1 = new QHBoxLayout();
	QHBoxLayout* hlayout2 = new QHBoxLayout();
	_imageGrid = new MapsElementsWidget(1,1);

	_mapsFilsWidget = new MapsWorkspaceFilesWidget();
	connect(_mapsFilsWidget, SIGNAL(loadList_H5(QStringList)), this, SLOT(loadList_H5(QStringList)));
	connect(_mapsFilsWidget, SIGNAL(unloadList_H5(QStringList)), this, SLOT(unloadList_H5(QStringList)));

    _left_btn =  new QPushButton();
    _left_btn->setIcon(QIcon(":/images/previous.png"));
    _left_btn->setMaximumWidth(150);
    _image_name_cb = new QComboBox();
    _right_btn =  new QPushButton();
    _right_btn->setIcon(QIcon(":/images/next.png"));
    _right_btn->setMaximumWidth(150);

    hlayout1->addWidget(_left_btn);
    hlayout1->addWidget(_image_name_cb);
    hlayout1->addWidget(_right_btn);

	connect(_image_name_cb, SIGNAL(currentIndexChanged(QString)), this, SLOT(h5IndexChanged(QString)));

	connect(this, SIGNAL(newH5ModelSelected(MapsH5Model*)), this, SLOT(onNewH5ModelSelected(MapsH5Model*)));

    vlayout->addItem(hlayout1);
	vlayout->addWidget(_imageGrid);

	hlayout2->addWidget(_mapsFilsWidget);

	QWidget *leftWidget = new QWidget();
	leftWidget->setLayout(hlayout2);
	QWidget *rightWidget = new QWidget();
	rightWidget->setLayout(vlayout);

	QSplitter* splitter = new QSplitter();
	splitter->setOrientation(Qt::Horizontal);
	splitter->addWidget(leftWidget);
	splitter->setStretchFactor(0, 1);
	splitter->addWidget(rightWidget);
	//createToolBar(m_imageViewWidget);
	//counts_layout->addWidget(m_toolbar);
	//counts_layout->addWidget(splitter);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(splitter);


	setLayout(mainLayout);
}

/*---------------------------------------------------------------------------*/

void ImageStackControlWidget::closeEvent(QCloseEvent *event)
{
	emit widgetClosed();
	event->accept();
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

void ImageStackControlWidget::onNewH5ModelSelected(MapsH5Model* model)
{

	//_imageGrid->setModel(model, _fit_params, _elements_to_fit);
	_imageGrid->setModel(model, nullptr, nullptr);
}

/*---------------------------------------------------------------------------*/

void ImageStackControlWidget::setModel(MapsWorkspaceModel *model)
{
	_model = model;
	_mapsFilsWidget->clearLists();
	_mapsFilsWidget->setModel(_model);
	if (_model != nullptr)
	{
		_mapsFilsWidget->setLabelWorkspacePath(_model->get_directory_name());
		this->setWindowTitle(_model->get_directory_name());
	}
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
	disconnect(_image_name_cb, SIGNAL(currentIndexChanged(QString)), this, SLOT(h5IndexChanged(QString)));
	disconnect(this, SIGNAL(newH5ModelSelected(MapsH5Model*)), this, SLOT(onNewH5ModelSelected(MapsH5Model*)));

	_imageGrid->setModel(nullptr, nullptr, nullptr);
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


	connect(_image_name_cb, SIGNAL(currentIndexChanged(QString)), this, SLOT(h5IndexChanged(QString)));
	connect(this, SIGNAL(newH5ModelSelected(MapsH5Model*)), this, SLOT(onNewH5ModelSelected(MapsH5Model*)));

	if (_h5_model_map.count(_image_name_cb->currentText()) > 0)
	{
		emit newH5ModelSelected(_h5_model_map[_image_name_cb->currentText()]);
	}



}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
