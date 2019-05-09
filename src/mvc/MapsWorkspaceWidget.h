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

enum File_Loaded_Status {UNLOADED, LOADED, FAILED_LOADING};

/*---------------------------------------------------------------------------*/

class FileTabWidget : public QWidget
{
    Q_OBJECT

 public:

    FileTabWidget(QWidget* parent = nullptr);

    ~FileTabWidget(){}

    void set_file_list(const map<QString, QFileInfo>& fileinfo_list);

signals:
    void onOpenItem(QString);

public slots:
    void onDoubleClickElement(const QModelIndex);

    void onContextMenuTrigger();

    void filterTextChanged(const QString &);

    void ShowContextMenu(const QPoint &);

    void loaded_file_status_changed(File_Loaded_Status status, const QString& filename);

protected:

    QListView* _file_list_view;

    QStandardItemModel* _file_list_model;

    QMenu *_contextMenu;

    QLineEdit *_filter_line;
};

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

   void loaded_hdf5(File_Loaded_Status status, const QString& name);

   void loaded_mda(File_Loaded_Status status, const QString& name);

   void loaded_sws(File_Loaded_Status status, const QString& name);

public slots:

   void model_done_loading();

   void loadedFitParams(int idx);

   void onOpenHDF5(QString name);

   void onOpenMDA(QString name);

   void onOpenSWS(QString name);

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
};


/*---------------------------------------------------------------------------*/

#endif /* MapsWorkspaceWidget_H_ */

/*---------------------------------------------------------------------------*/

