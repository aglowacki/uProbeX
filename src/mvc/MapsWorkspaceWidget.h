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
#include "FitParamsTableModel.h"
#include "mvc/ComboBoxDelegate.h"

/*---------------------------------------------------------------------------*/

/**
 * @brief When open the acquisition window, the widget is showing for capturing
 * the image from the area detector writer, the window will also be updated to
 * show the image.
 */
class MapsWorkspaceWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   MapsWorkspaceWidget(QWidget* parent = nullptr);

   /**
    * Destructor.
    */
   ~MapsWorkspaceWidget();

   void setModel(MapsWorkspaceModel* model);

   void setLabelWorkspacePath(QString path) {_lbl_workspace->setText(path);}

signals:

   void selectedAnalyzedH5(MapsH5Model* model);

   void showFitSpecWindow(MapsH5Model* h5_model,
                          data_struct::Fit_Parameters* fit_params,
                          data_struct::Fit_Element_Map_Dict *elements_to_fit);


   void show_MDA_Window(MDA_Model * model);

   void show_SWS_Window(SWSModel* model);

public slots:

   void ShowContextMenu(const QPoint &);

   void model_done_loading();

   void loadedFitParams(int idx);

   void viewAnalyzedH5();

   void fitIntegratedSpectra();

   void onOpenHDF5(const QModelIndex);

   void onOpenMDA(const QModelIndex);

   void onOpenSWS(const QModelIndex);

protected:

   /**
    * @brief Create layout
    */
   void createLayout();

private:

   QTabWidget *_tab_widget;

   QStringList _analyzed_h5_list;

   QStringList _mda_list;

   QStringList _sws_list;

   QListView* _analyzed_h5_list_view;

   QListView* _mda_list_view;

   QListView* _sws_list_view;

   QStringListModel* _analyzed_h5_list_model;

   QStringListModel* _mda_list_model;

   QStringListModel* _sws_list_model;

   QLabel* _lbl_workspace;

   MapsWorkspaceModel* _model;

   QMenu *_contextMenu;

   QTableView* _fit_params_table;

   FitParamsTableModel* _fit_params_table_model;
};


/*---------------------------------------------------------------------------*/

#endif /* MapsWorkspaceWidget_H_ */

/*---------------------------------------------------------------------------*/

