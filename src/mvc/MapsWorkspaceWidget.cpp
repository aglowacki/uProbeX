/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsWorkspaceWidget.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QDebug>

/*---------------------------------------------------------------------------*/

MapsWorkspaceWidget::MapsWorkspaceWidget(QWidget* parent) : QWidget(parent)
{

    QAction* action;
    createLayout();

    _contextMenu = new QMenu(("Context menu"), this);

    //action = _contextMenu->addAction("View");
    //connect(action, SIGNAL(triggered()), this, SLOT(viewAnalyzedH5()));


    action = _contextMenu->addAction("Open");
    connect(action, SIGNAL(triggered()), this, SLOT(fitIntegratedSpectra()));

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
    std::vector<std::string> bound_types {"Not Initialized", "Fixed", "Limited Low High", "Limited Low", "Limited High", "Fit"};
    _lbl_workspace = new QLabel();
    _tab_widget = new QTabWidget();

    _analyzed_h5_list_model = new QStringListModel();
    _analyzed_h5_list_view = new QListView();
    _analyzed_h5_list_view->setModel(_analyzed_h5_list_model);
    _analyzed_h5_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _analyzed_h5_list_view->setContextMenuPolicy(Qt::CustomContextMenu);
    _analyzed_h5_list_view->setSelectionMode(QAbstractItemView::ExtendedSelection); //MultiSelection
    connect(_analyzed_h5_list_view, SIGNAL(doubleClicked(const QModelIndex)),
                this, SLOT(onOpenHDF5(const QModelIndex)));
    connect(_analyzed_h5_list_view, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));

    _fit_params_table_model = new FitParamsTableModel();
    ComboBoxDelegate *cbDelegate = new ComboBoxDelegate(bound_types);

    _fit_params_table = new QTableView();
    _fit_params_table->setModel(_fit_params_table_model);
    //_fit_params_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //_fit_params_table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    _fit_params_table->sortByColumn(0, Qt::AscendingOrder);
    _fit_params_table->setItemDelegateForColumn(2, cbDelegate);

    QLayout* vlayout = new QVBoxLayout();

    //setup mda
    _mda_list_model = new QStringListModel();
    _mda_list_view = new QListView();
    _mda_list_view->setModel(_mda_list_model);
    _mda_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(_mda_list_view, SIGNAL(doubleClicked(const QModelIndex)),
                this, SLOT(onOpenMDA(const QModelIndex)));

    //setup sws light microscope
    _sws_list_model = new QStringListModel();
    _sws_list_view = new QListView();
    _sws_list_view->setModel(_sws_list_model);
    _sws_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(_sws_list_view, SIGNAL(doubleClicked(const QModelIndex)),
                this, SLOT(onOpenSWS(const QModelIndex)));

    _tab_widget->insertTab(0, _analyzed_h5_list_view, "Analyized H5");
    _tab_widget->insertTab(1, _mda_list_view, "Raw MDA");
    _tab_widget->insertTab(2, _sws_list_view, "Light Microscope");


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

    const map<QString, QFileInfo> h5_fileinfo_list = _model->get_hdf5_file_list();
    const map<QString, QFileInfo> mda_fileinfo_list = _model->get_mda_file_list();
    const map<QString, QFileInfo> sws_fileinfo_list = _model->get_sws_file_list();
    for(auto & itr : h5_fileinfo_list)
    {
        _analyzed_h5_list.append(itr.first);
    }
    _analyzed_h5_list_model->setStringList(_analyzed_h5_list);

    for(auto & itr : mda_fileinfo_list)
    {
        _mda_list.append(itr.first);
    }
    _mda_list_model->setStringList(_mda_list);

    for(auto & itr : sws_fileinfo_list)
    {
        _sws_list.append(itr.first);
    }
    _sws_list_model->setStringList(_sws_list);

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceWidget::ShowContextMenu(const QPoint &pos)
{
    _contextMenu->exec(mapToGlobal(pos));
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

void MapsWorkspaceWidget::viewAnalyzedH5()
{
    QModelIndexList list = _analyzed_h5_list_view->selectionModel()->selectedIndexes();
    for(int i =0; i<list.length(); i++)
    {
        onOpenHDF5(list.at(i));
    }

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceWidget::fitIntegratedSpectra()
{

    QModelIndexList list = _analyzed_h5_list_view->selectionModel()->selectedIndexes();
    for(int i =0; i<list.length(); i++)
    {
        QModelIndex idx = list.at(i);
        QVariant data = idx.data(0);
        if(_model != nullptr)
        {
            MapsH5Model* h5Model = _model->getMapsH5Model(data.toString());
            if(h5Model != nullptr)
            {
                showFitSpecWindow(h5Model, _model->getFitParameters(-1), _model->getElementToFit(-1));
            }
        }
    }

}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceWidget::onOpenHDF5(const QModelIndex idx)
{
    QVariant data = idx.data(0);
    if(_model != nullptr)
    {
        MapsH5Model* h5Model = _model->getMapsH5Model(data.toString());
        if(h5Model != nullptr)
        {
            //selectedAnalyzedH5(h5Model);
            emit showFitSpecWindow(h5Model, _model->getFitParameters(-1), _model->getElementToFit(-1));
        }
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceWidget::onOpenMDA(const QModelIndex idx)
{
    QVariant data = idx.data(0);
    if(_model != nullptr)
    {
        MDA_Model* mda_model = _model->get_MDA_Model(data.toString());
        if(mda_model != nullptr)
        {
            emit show_MDA_Window(mda_model);
        }
    }
}

/*---------------------------------------------------------------------------*/

void MapsWorkspaceWidget::onOpenSWS(const QModelIndex idx)
{
    QVariant data = idx.data(0);
    if(_model != nullptr)
    {
        SWSModel* sws_model = _model->get_SWS_Model(data.toString());
        if(sws_model != nullptr)
        {
            emit show_SWS_Window(sws_model);
        }
    }
}
