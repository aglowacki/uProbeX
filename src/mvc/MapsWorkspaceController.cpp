/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsWorkspaceController.h>

MapsWorkspaceController::MapsWorkspaceController(QMainWindow * parentWindow, QObject* parent) : QObject(parent)
{

	_parentWindowPtr = parentWindow;
	_mapsWorkspaceModel = new MapsWorkspaceModel();
	_mapsFilsWidget = new MapsWorkspaceFilesWidget();
	_imgStackControllWidget = new ImageStackControlWidget();
	

	connect(_mapsFilsWidget, SIGNAL(loadList_H5(QStringList)), _imgStackControllWidget, SLOT(loadList_H5(QStringList)));
	connect(_mapsFilsWidget, SIGNAL(unloadList_H5(QStringList)), _imgStackControllWidget, SLOT(unloadList_H5(QStringList)));

	connect(_mapsFilsWidget, SIGNAL(show_MDA_Window(MDA_Model*)), this, SLOT(makeMDAWindow(MDA_Model*)));
	connect(_mapsFilsWidget, SIGNAL(show_SWS_Window(SWSModel*)), this, SLOT(makeSWSWindow(SWSModel*)));

	if (_parentWindowPtr != nullptr)
	{
		_maps_workspace_dock = new QDockWidget("Files", _parentWindowPtr);
		_maps_workspace_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		_maps_workspace_dock->setWidget(_mapsFilsWidget);
		_parentWindowPtr->addDockWidget(Qt::LeftDockWidgetArea, _maps_workspace_dock);

		/*
		_image_stack_control_dock = new QDockWidget("", _parentWindowPtr);
		_image_stack_control_dock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
		_image_stack_control_dock->setWidget(_imgStackControllWidget);
		_parentWindowPtr->addDockWidget(Qt::TopDockWidgetArea, _image_stack_control_dock);
		*/
		_imgStackControllWidget->show();
	}

}

/*---------------------------------------------------------------------------*/

MapsWorkspaceController::~MapsWorkspaceController()
{

}

void MapsWorkspaceController::setWorkingDir(QString path)
{
	_mapsWorkspaceModel->unload();

	_mapsFilsWidget->setLabelWorkspacePath(path);

	_mapsFilsWidget->clearLists();
	_mapsFilsWidget->setModel(_mapsWorkspaceModel);
	_imgStackControllWidget->setModel(_mapsWorkspaceModel);

	_mapsWorkspaceModel->load(path);

	//    if(_load_maps_workspace_thread != nullptr)
//    {
//        _load_maps_workspace_thread->join();
//        delete _load_maps_workspace_thread;
//    }
//    _load_maps_workspace_thread = new std::thread( [model, path]()
//    {
//        try
//        {
//            model->load(path);
//        }
//        catch(std::string& s)
//        {
//            qDebug()<<"Failed to open maps workspace.\n\n"<<QString(s.c_str());
//        }
//    });

}
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
