/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MAPS_WORKSPACE_WIDGET_H
#define MAPS_WORKSPACE_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <QAction>
#include <QMenu>
#include <QWidget>
#include <QLabel>
#include <mvc/MapsWorkspaceModel.h>
#include <QListView>
#include <QTableView>
#include <QStringListModel>
#include <QListWidgetItem>
#include <QPushButton>
#include "FitParamsTableModel.h"
#include "mvc/ComboBoxDelegate.h"
#include "mvc/FileTabWidget.h"
#include "mvc/PerPixelFitWidget.h"

/*---------------------------------------------------------------------------*/
/**
 * @brief When open the acquisition window, the widget is showing for capturing
 * the image from the area detector writer, the window will also be updated to
 * show the image.
 */
class MapsWorkspaceFilesWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   MapsWorkspaceFilesWidget(QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~MapsWorkspaceFilesWidget();

   void setModel(MapsWorkspaceModel* model);

   void setLabelWorkspacePath(QString path) {_lbl_workspace->setText(path);}

   void clearLists();

signals:

   void selectedAnalyzedH5(MapsH5Model* model);

   void showFitSpecWindow(MapsH5Model* h5_model,
                          data_struct::Fit_Parameters* fit_params,
                          data_struct::Fit_Element_Map_Dict *elements_to_fit);


   void show_MDA_Window(MDA_Model * model);

   void show_SWS_Window(SWSModel* model);

   void status_loaded_hdf5(File_Loaded_Status status, const QString& name);

   void status_loaded_mda(File_Loaded_Status status, const QString& name);

   void status_loaded_sws(File_Loaded_Status status, const QString& name);

   void loadList_H5(QStringList);

   void unloadList_H5(QStringList);

public slots:

   void updateMDA();

   void updateH5();

   void updateSWS();

   void model_done_loading();

   void model_done_unloading();

   void loadedFitParams(int idx);

   void onOpenHDF5(const QStringList& names_list);

   void onCloseHDF5(const QStringList& names_list);

   void onOpenMDA(QString name);

   void onCloseMDA(QString name);

   void onOpenSWS(QString name);

   void onCloseSWS(QString name);

   void onPerPixelProcess(const QString& context_label, const QStringList& file_list);

protected:

   /**
    * @brief Create layout
    */
   void createLayout();

private:

   QTabWidget *_tab_widget;

   FileTabWidget* _h5_tab_widget;

   FileTabWidget* _mda_tab_widget;

   FileTabWidget* _sws_tab_widget;

   QLabel* _lbl_workspace;

   MapsWorkspaceModel* _model;

   QTableView* _fit_params_table;

   FitParamsTableModel* _fit_params_table_model;

   PerPixelFitWidget* _per_pixel_fit_widget;
};


/*---------------------------------------------------------------------------*/

#endif /* MapsWorkspaceFilesWidget_H_ */

/*---------------------------------------------------------------------------*/

