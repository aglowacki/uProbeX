/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsWorkspaceWidget.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QRegExp>
#include <QDebug>

/*---------------------------------------------------------------------------*/

FileTabWidget::FileTabWidget(QWidget* parent) : QWidget(parent)
{

    _contextMenu = new QMenu(("Context menu"), this);
    QAction* action = _contextMenu->addAction("Open");
    connect(action, SIGNAL(triggered()), this, SLOT(onContextMenuTrigger()));

    _file_list_model = new QStandardItemModel();
    _file_list_view = new QListView();
   // _file_list_view->setViewMode(QListView::IconMode);
    _file_list_view->setModel(_file_list_model);
    _file_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _file_list_view->setContextMenuPolicy(Qt::CustomContextMenu);
    _file_list_view->setSelectionMode(QAbstractItemView::ExtendedSelection); //MultiSelection

    connect(_file_list_view, SIGNAL(doubleClicked(const QModelIndex)),
                this, SLOT(onDoubleClickElement(const QModelIndex)));
    connect(_file_list_view, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));

    _filter_line = new QLineEdit();
    connect(_filter_line, SIGNAL(textChanged(const QString &)),
            this, SLOT(filterTextChanged(const QString &)));

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(new QLabel("Filter"));
    hlayout->addWidget(_filter_line);

    QLayout* vlayout = new QVBoxLayout();
    vlayout->addItem(hlayout);
    vlayout->addWidget(_file_list_view);
    setLayout(vlayout);

}

/*---------------------------------------------------------------------------*/

void FileTabWidget::set_file_list(const map<QString, QFileInfo>& fileinfo_list)
{
    for(auto & itr : fileinfo_list)
    {
        _file_list_model->appendRow(new QStandardItem(QIcon(":/not_loaded.png"), itr.first));
    }
}

/*---------------------------------------------------------------------------*/

void FileTabWidget::ShowContextMenu(const QPoint &pos)
{
    _contextMenu->exec(mapToGlobal(pos));
}

/*---------------------------------------------------------------------------*/

void FileTabWidget::onDoubleClickElement(const QModelIndex idx)
{
    QVariant data = idx.data(0);
    emit onOpenItem(data.toString());
}

/*---------------------------------------------------------------------------*/

void FileTabWidget::onContextMenuTrigger()
{
    QModelIndexList list = _file_list_view->selectionModel()->selectedIndexes();
    for(int i =0; i<list.length(); i++)
    {
        QModelIndex idx = list.at(i);
        QVariant data = idx.data(0);
        emit onOpenItem(data.toString());
    }
}

/*---------------------------------------------------------------------------*/

void FileTabWidget::filterTextChanged(const QString &text)
{
    QString filter_text = _filter_line->text();

    if(filter_text.length() > 0)
    {
        QRegExp re (_filter_line->text());
        re.setPatternSyntax(QRegExp::Wildcard);

        for(int i=0; i < _file_list_model->rowCount(); i++)
        {
            _file_list_view->setRowHidden(i, true);
            QStandardItem *val = _file_list_model->item(i, 0);
            if(re.exactMatch(val->text()))
            {
                _file_list_view->setRowHidden(i, false);
            }
        }
    }
    else
    {
        for(int i=0; i < _file_list_model->rowCount(); i++)
        {
            _file_list_view->setRowHidden(i, false);
        }
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

MapsWorkspaceWidget::MapsWorkspaceWidget(QWidget* parent) : QWidget(parent)
{

    createLayout();

}

/*---------------------------------------------------------------------------*/

MapsWorkspaceWidget::~MapsWorkspaceWidget()
{

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceWidget::createLayout()
{
    std::vector<std::string> bound_types {"Not Initialized", "Fixed", "Limited Low High", "Limited Low", "Limited High", "Fit"};
    _lbl_workspace = new QLabel();
    _tab_widget = new QTabWidget();

    _h5_tab_widget = new FileTabWidget();
    connect(_h5_tab_widget, SIGNAL(onOpenItem(QString)),
                this, SLOT(onOpenHDF5(QString)));
    _mda_tab_widget = new FileTabWidget();
    connect(_mda_tab_widget, SIGNAL(onOpenItem(QString)),
                this, SLOT(onOpenMDA(QString)));
    _sws_tab_widget = new FileTabWidget();
    connect(_sws_tab_widget, SIGNAL(onOpenItem(QString)),
                this, SLOT(onOpenSWS(QString)));

    _fit_params_table_model = new FitParamsTableModel();
    ComboBoxDelegate *cbDelegate = new ComboBoxDelegate(bound_types);

    _fit_params_table = new QTableView();
    _fit_params_table->setModel(_fit_params_table_model);
    //_fit_params_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //_fit_params_table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    _fit_params_table->sortByColumn(0, Qt::AscendingOrder);
    _fit_params_table->setItemDelegateForColumn(2, cbDelegate);

    QLayout* vlayout = new QVBoxLayout();

    _tab_widget->insertTab(0, _h5_tab_widget, "Analyized H5");
    _tab_widget->insertTab(1, _mda_tab_widget, "Raw MDA");
    _tab_widget->insertTab(2, _sws_tab_widget, "Light Microscope");

    vlayout->addWidget(_tab_widget);
    setLayout(vlayout);
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceWidget::setModel(MapsWorkspaceModel *model)
{
    _model = model;
    QString path = _model->get_directory_name();
    if(path.length() > 0)
    {
        _lbl_workspace->setText(path);
    }
    connect(_model,
            SIGNAL(doneLoading()),
            this,
            SLOT(model_done_loading()));

    connect(_model,
            SIGNAL(newFitParamsFileLoaded(int)),
            this,
            SLOT(loadedFitParams(int)));


}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceWidget::model_done_loading()
{

    _h5_tab_widget->set_file_list(_model->get_hdf5_file_list());
    _mda_tab_widget->set_file_list(_model->get_mda_file_list());
    _sws_tab_widget->set_file_list(_model->get_sws_file_list());

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceWidget::loadedFitParams(int idx)
{
    if(idx == -1) //avg fit params
    {
        data_struct::Fit_Parameters* fit_params = _model->getFitParameters(idx);
        if(fit_params != nullptr)
        {
            _fit_params_table_model->setFitParams(*fit_params);
        }
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceWidget::onOpenHDF5(QString name)
{
    if(_model != nullptr)
    {
        MapsH5Model* h5Model = _model->getMapsH5Model(name);
        if(h5Model != nullptr)
        {
            emit showFitSpecWindow(h5Model, _model->getFitParameters(-1), _model->getElementToFit(-1));
        }
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceWidget::onOpenMDA(QString name)
{
    if(_model != nullptr)
    {
        MDA_Model* mda_model = _model->get_MDA_Model(name);
        if(mda_model != nullptr)
        {
            emit show_MDA_Window(mda_model);
        }
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceWidget::onOpenSWS(QString name)
{
    if(_model != nullptr)
    {
        SWSModel* sws_model = _model->get_SWS_Model(name);
        if(sws_model != nullptr)
        {
            emit show_SWS_Window(sws_model);
        }
    }
}

/*---------------------------------------------------------------------------*/
