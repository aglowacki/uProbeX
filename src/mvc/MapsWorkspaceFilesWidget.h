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

enum class MODEL_TYPE { MAPS_H5, RAW, VLM };

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

   MapsWorkspaceFilesWidget(QWidget* parent = nullptr);

   ~MapsWorkspaceFilesWidget();

   void setModel(std::shared_ptr<MapsWorkspaceModel> model);

   void setLabelWorkspacePath(QString path) {_lbl_workspace->setText(path);}

signals:

   void loaded_model(const QString, MODEL_TYPE);

   void unloadList_model(const QStringList, MODEL_TYPE);

   void loaded_perc(int, int);

public slots:

    void updatedMDA();

    void updatedVLM();

    void updatedHDF();

    void clearLists();

    void loadedFitParams(int idx);
 
    void onOpenModel(const QStringList& names_list, MODEL_TYPE mt);

    void onCloseModel(const QStringList& names_list, MODEL_TYPE mt);

    void onPerPixelProcess(const QString& context_label, const QStringList& file_list);

    void onPerPixelProcessList(const QStringList& file_list);

    void onProcessed_list_update();

protected:

   /**
    * @brief Create layout
    */
   void createLayout();

private:

   QTabWidget *_tab_widget;

   FileTabWidget* _h5_tab_widget;

   FileTabWidget* _mda_tab_widget;

   FileTabWidget* _vlm_tab_widget;

   QLabel* _lbl_workspace;

   std::shared_ptr<MapsWorkspaceModel> _model;

   QTableView* _fit_params_table;

   FitParamsTableModel* _fit_params_table_model;

   PerPixelFitWidget* _per_pixel_fit_widget;
};


/*---------------------------------------------------------------------------*/

#endif /* MapsWorkspaceFilesWidget_H_ */

/*---------------------------------------------------------------------------*/

