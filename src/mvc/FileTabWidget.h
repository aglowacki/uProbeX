/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef FILE_TAB_WIDGET_H
#define FILE_TAB_WIDGET_H

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

    void unload_all();

signals:
    void onOpenItem(QString);

    void onCloseItem(QString);

    void loadList(QStringList);

    void unloadList(QStringList);

public slots:
    void onDoubleClickElement(const QModelIndex);

    void onLoadFile();

    void onUnloadFile();

    void filterTextChanged(const QString &);

    void ShowContextMenu(const QPoint &);

    void loaded_file_status_changed(File_Loaded_Status status, const QString& filename);

    void load_all_visible();

    void unload_all_visible();

protected:

    void _gen_visible_list(QStringList *sl);

    QListView* _file_list_view;

    QStandardItemModel* _file_list_model;

    QMenu *_contextMenu;

    QLineEdit *_filter_line;

    QPushButton *_load_all_btn;

    QPushButton *_unload_all_btn;
};

/*---------------------------------------------------------------------------*/

#endif /* FileTabWidget_H_ */

/*---------------------------------------------------------------------------*/

