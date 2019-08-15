/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_WORKSPACE_CONTROLLER_H
#define MAPS_WORKSPACE_CONTROLLER_H

/*---------------------------------------------------------------------------*/

#include <QObject>
#include <QMainWindow>
#include <QDockWidget>
#include <mvc/MapsWorkspaceModel.h>
#include <mvc/MapsWorkspaceFilesWidget.h>
#include <mvc/ImageStackControlWidget.h>
//#include <mvc/MapsElementsWidget.h>

class MapsWorkspaceController : public QObject
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
	MapsWorkspaceController(QMainWindow * parentWindow, QObject* parent = nullptr);

   /**
    * Destructor.
    */
   ~MapsWorkspaceController();

   void setWorkingDir(QString path);

public slots:
	void makeSWSWindow(SWSModel* model);

	void makeMDAWindow(MDA_Model *model);

protected:

	MapsWorkspaceFilesWidget* _mapsFilsWidget;

	ImageStackControlWidget* _imgStackControllWidget;

	MapsWorkspaceModel* _mapsWorkspaceModel;

	//MapsElementsWidget* _imageGrid;

	QMainWindow * _parentWindowPtr;

	QDockWidget *_maps_workspace_dock;

	QDockWidget *_image_stack_control_dock;

};


/*---------------------------------------------------------------------------*/

#endif /* MapsWorkspaceController_H_ */

/*---------------------------------------------------------------------------*/

