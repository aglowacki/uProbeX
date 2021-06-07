/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsWorkspaceController.h>

MapsWorkspaceController::MapsWorkspaceController(QObject* parent) : QObject(parent)
{
	_mapsWorkspaceModel = std::make_shared< MapsWorkspaceModel>();
	_imgStackControllWidget = new ImageStackControlWidget();
	
	_imgStackControllWidget->setModel(_mapsWorkspaceModel);

	_imgStackControllWidget->setAttribute(Qt::WA_DeleteOnClose);
	connect(_imgStackControllWidget, SIGNAL(widgetClosed()), this, SLOT(imgWidgetClosed()));

	_imgStackControllWidget->show();
}

/*---------------------------------------------------------------------------*/

MapsWorkspaceController::~MapsWorkspaceController()
{

	if(_imgStackControllWidget != nullptr)
	{
		_imgStackControllWidget->setModel(nullptr);
		delete _imgStackControllWidget;
	}

	if (_mapsWorkspaceModel)
	{
		_mapsWorkspaceModel.reset();
	}
}

/*---------------------------------------------------------------------------*/


void MapsWorkspaceController::imgWidgetClosed()
{
	_imgStackControllWidget = nullptr;
	emit controllerClosed(this);

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceController::setWorkingDir(QString path)
{

    _mapsWorkspaceModel->unload();

    _mapsWorkspaceModel->load(path);

	_imgStackControllWidget->setWindowTitle(path);

}
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
