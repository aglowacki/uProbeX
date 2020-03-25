/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsWorkspaceController.h>

MapsWorkspaceController::MapsWorkspaceController(ThreadPool* thread_pool, QObject* parent) : QObject(parent)
{
	_tp = thread_pool;
    //_load_maps_workspace_thread = nullptr;
	_mapsWorkspaceModel = new MapsWorkspaceModel();
	_imgStackControllWidget = new ImageStackControlWidget();
	
	_imgStackControllWidget->setModel(_mapsWorkspaceModel);

//	connect(_mapsFilsWidget, SIGNAL(loadList_H5(QStringList)), _imgStackControllWidget, SLOT(loadList_H5(QStringList)));
//	connect(_mapsFilsWidget, SIGNAL(unloadList_H5(QStringList)), _imgStackControllWidget, SLOT(unloadList_H5(QStringList)));

//	connect(_mapsFilsWidget, SIGNAL(show_MDA_Window(MDA_Model*)), this, SLOT(makeMDAWindow(MDA_Model*)));
//	connect(_mapsFilsWidget, SIGNAL(show_SWS_Window(SWSModel*)), this, SLOT(makeSWSWindow(SWSModel*)));
	_imgStackControllWidget->setAttribute(Qt::WA_DeleteOnClose);
	connect(_imgStackControllWidget, SIGNAL(widgetClosed()), this, SLOT(imgWidgetClosed()));

	_imgStackControllWidget->show();
}

/*---------------------------------------------------------------------------*/

MapsWorkspaceController::~MapsWorkspaceController()
{

    //if(_load_maps_workspace_thread != nullptr)
    //{
    //    _load_maps_workspace_thread->join();
    //    delete _load_maps_workspace_thread;
    //}


	if(_imgStackControllWidget != nullptr)
	{
		_imgStackControllWidget->setModel(nullptr);
		delete _imgStackControllWidget;
	}

	if (_mapsWorkspaceModel != nullptr)
	{
		delete _mapsWorkspaceModel;
	}
}

/*---------------------------------------------------------------------------*/


void MapsWorkspaceController::imgWidgetClosed()
{
	_imgStackControllWidget = nullptr;
	emit controllerClosed(this);

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceController::makeSWSWindow(SWSModel* model)
{

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceController::makeMDAWindow(MDA_Model *model)
{

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceController::setWorkingDir(QString path)
{

    _mapsWorkspaceModel->unload();

    _mapsWorkspaceModel->load(path, _tp);
/*
    if(_load_maps_workspace_thread != nullptr)
    {
        _load_maps_workspace_thread->join();
        delete _load_maps_workspace_thread;
    }
    _load_maps_workspace_thread = new std::thread(&MapsWorkspaceModel::load, _mapsWorkspaceModel, path);
*/
}
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
