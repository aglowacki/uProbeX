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
#include <mvc/FileTableModel.h>

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

class FileTabWidget : public QWidget
{
    Q_OBJECT

 public:

    FileTabWidget(QWidget* parent = nullptr);

    ~FileTabWidget(){}

    void set_file_list(const std::map<QString, QFileInfo>& fileinfo_list);

    void set_roi_num_list(const std::map<QString, int>& roi_num_map);

    void set_roi_num(QString name, int val);

    void update_file_list(const std::map<QString, QFileInfo>& fileinfo_list);

    void unload_all();

    void appendFilterHelpAction(QAction * action) { _filterHelpMenu->addAction(action); }

    void addCustomContext(QString Id, QString label);

    void setProcessButtonVisible(bool val) { _process_btn->setVisible(val); }

    void setActionsAndButtonsEnabled(bool val);

signals:
    void onOpenItem(QString);

    void onCloseItem(QString);

    void loadList(QStringList);

    void unloadList(QStringList);

    void processList(const QStringList&);

    void batchRoiList(const QStringList&);

    void customContext(QString, QStringList);

    void onRefresh();

    void selectNewRow(const QString&);

public slots:
    void onDoubleClickElement(const QModelIndex);

    void onLoadFile();

    void onUnloadFile();

    void process_all_visible();

    void batch_roi_visible();

    void filterTextChanged(const QString &);

    void ShowContextMenu(const QPoint &);

    void loaded_file_status_changed(File_Loaded_Status status, const QString& filename);

    void load_all_visible();

    void unload_all_visible();

	void filterBtnClicked();

    void onCustomContext();

    void onFileRowChange(const QModelIndex& current, const QModelIndex& previous);

    void onUpdateFilter();

protected:

    void _gen_visible_list(QStringList *sl);

    QTableView* _file_list_view;

    FileTableModel* _file_list_model;

    QMenu *_contextMenu;

    QMenu *_filterHelpMenu;

    QLineEdit *_filter_line;

    QPushButton *_load_all_btn;

    QPushButton *_unload_all_btn;

    QPushButton* _process_btn;

    QPushButton* _batch_roi_btn;

	QPushButton * _filter_suggest_btn;

    QAction* _action_load;

    QAction* _action_unload;

    QAction* _action_refresh;

    QList<QAction*> _custom_action_list;

};

/*---------------------------------------------------------------------------*/

#endif /* FileTabWidget_H_ */

/*---------------------------------------------------------------------------*/

