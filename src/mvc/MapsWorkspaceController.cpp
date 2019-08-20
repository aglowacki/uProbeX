/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsWorkspaceController.h>

MapsWorkspaceController::MapsWorkspaceController(QObject* parent) : QObject(parent)
{

	_mapsWorkspaceModel = new MapsWorkspaceModel();
	_imgStackControllWidget = new ImageStackControlWidget();
	
	_imgStackControllWidget->setModel(_mapsWorkspaceModel);

//	connect(_mapsFilsWidget, SIGNAL(loadList_H5(QStringList)), _imgStackControllWidget, SLOT(loadList_H5(QStringList)));
//	connect(_mapsFilsWidget, SIGNAL(unloadList_H5(QStringList)), _imgStackControllWidget, SLOT(unloadList_H5(QStringList)));

//	connect(_mapsFilsWidget, SIGNAL(show_MDA_Window(MDA_Model*)), this, SLOT(makeMDAWindow(MDA_Model*)));
//	connect(_mapsFilsWidget, SIGNAL(show_SWS_Window(SWSModel*)), this, SLOT(makeSWSWindow(SWSModel*)));

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

	if (_mapsWorkspaceModel != nullptr)
	{
		delete _mapsWorkspaceModel;
	}
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
