/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsWorkspaceWidget.h>


#include <QVBoxLayout>
#include <QHBoxLayout>



#include <QDebug>

/*---------------------------------------------------------------------------*/

MapsWorkspaceWidget::MapsWorkspaceWidget(QWidget* parent) : QWidget(parent)
{

    createLayout();

    _action_view_analyzed_h5 = new QAction("View", this);
    //_action_check_log10->setCheckable(true);
    //_action_check_log10->setChecked(false);
    connect(_action_view_analyzed_h5, SIGNAL(triggered()), this, SLOT(viewAnalyzedH5()));

    _contextMenu = new QMenu(("Context menu"), this);
    _contextMenu->addAction(_action_view_analyzed_h5);

    /*
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));
            */
}

/*---------------------------------------------------------------------------*/

MapsWorkspaceWidget::~MapsWorkspaceWidget()
{

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceWidget::createLayout()
{
    _lbl_workspace = new QLabel();
    _analyzed_h5_list_view = new QListView();

    _analyzed_h5_list_model = new QStringListModel();

    _analyzed_h5_list_view->setModel(_analyzed_h5_list_model);
    _analyzed_h5_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _analyzed_h5_list_view->setContextMenuPolicy(Qt::CustomContextMenu);
    _analyzed_h5_list_view->setSelectionMode(QAbstractItemView::ExtendedSelection); //MultiSelection
    connect(_analyzed_h5_list_view, SIGNAL(doubleClicked(const QModelIndex)),
                this, SLOT(onListItemClicked(const QModelIndex)));
    connect(_analyzed_h5_list_view, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));


    _fit_params_table_model = new FitParamsTableModel();

    _fit_params_table = new QTableView();
    _fit_params_table->setModel(_fit_params_table_model);
    _fit_params_table->sortByColumn(0, Qt::AscendingOrder);

    QLayout* vlayout = new QVBoxLayout();

    QLayout* hlayout = new QHBoxLayout();

    hlayout->addWidget(new QLabel("Analyzed H5"));
    //hlayout->addWidget(_lbl_workspace);

    vlayout->addItem(hlayout);
    vlayout->addWidget(_analyzed_h5_list_view);
    vlayout->addWidget(_fit_params_table);
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
            SIGNAL(newAnalyzedH5FileLoaded(QString)),
            this,
            SLOT(loadedAnalysedH5(QString)));

    connect(_model,
            SIGNAL(newFitParamsFileLoaded(int)),
            this,
            SLOT(loadedFitParams(int)));


}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceWidget::ShowContextMenu(const QPoint &pos)
{
    _contextMenu->exec(mapToGlobal(pos));
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceWidget::loadedAnalysedH5(QString filename)
{
    _analyzed_h5_list.append(filename);
    _analyzed_h5_list_model->setStringList(_analyzed_h5_list);
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceWidget::loadedFitParams(int idx)
{
    if(idx == -1) //avg fit params
    {
        _fit_params_table_model->setFitParams(_model->getFitParameters(idx));
        //_fit_params_table->repaint();
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceWidget::viewAnalyzedH5()
{
    QModelIndexList list = _analyzed_h5_list_view->selectionModel()->selectedIndexes();
    for(int i =0; i<list.length(); i++)
    {
        onListItemClicked(list.at(i));
    }

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceWidget::onListItemClicked(const QModelIndex idx)
{
    QVariant data = idx.data(0);
    if(_model != nullptr)
    {
        MapsH5Model* h5Model = _model->getMapsH5Model(data.toString());
        if(h5Model != nullptr)
        {
            selectedAnalyzedH5(h5Model);
        }
    }
}

/*---------------------------------------------------------------------------*/
