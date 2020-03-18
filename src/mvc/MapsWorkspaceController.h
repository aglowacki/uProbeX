/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_WORKSPACE_CONTROLLER_H
#define MAPS_WORKSPACE_CONTROLLER_H

/*---------------------------------------------------------------------------*/
#include <future>
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
	MapsWorkspaceController(QObject* parent = nullptr);

   /**
    * Destructor.
    */
   ~MapsWorkspaceController();

   void setWorkingDir(QString path);

   void update_file_list() {if(_imgStackControllWidget!= nullptr){_imgStackControllWidget->update_file_list();}}

signals:
	void controllerClosed(MapsWorkspaceController*);

public slots:

	void imgWidgetClosed();

	void makeSWSWindow(SWSModel* model);

	void makeMDAWindow(MDA_Model *model);

protected:

	ImageStackControlWidget* _imgStackControllWidget;

	MapsWorkspaceModel* _mapsWorkspaceModel;

	QDockWidget *_maps_workspace_dock;

	QDockWidget *_image_stack_control_dock;

    std::thread* _load_maps_workspace_thread;

};


/*---------------------------------------------------------------------------*/

#endif /* MapsWorkspaceController_H_ */

/*---------------------------------------------------------------------------*/

