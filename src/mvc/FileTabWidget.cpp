/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/FileTabWidget.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QRegExp>

/*---------------------------------------------------------------------------*/

FileTabWidget::FileTabWidget(QWidget* parent) : QWidget(parent)
{

    _filterHelpMenu = new QMenu("Help", this);

    _contextMenu = new QMenu(("Context menu"), this);
    QAction* action = _contextMenu->addAction("Load");
    connect(action, SIGNAL(triggered()), this, SLOT(onLoadFile()));
    action = _contextMenu->addAction("UnLoad");
    connect(action, SIGNAL(triggered()), this, SLOT(onUnloadFile()));
    action = _contextMenu->addAction("Refresh");
    connect(action, SIGNAL(triggered()), this, SIGNAL(onRefresh()));

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

	_filter_suggest_btn = new QPushButton();
	_filter_suggest_btn->setIcon(QIcon(":/images/question.png"));
	_filter_suggest_btn->setIconSize(QSize(15, 15));
    connect(_filter_suggest_btn, SIGNAL(released()),
		this, SLOT(filterBtnClicked()));



    QHBoxLayout* hlayout1 = new QHBoxLayout();
    hlayout1->addWidget(new QLabel("Filter"));
    hlayout1->addWidget(_filter_line);
	hlayout1->addWidget(_filter_suggest_btn);


    _process_btn = new QPushButton("Process All");
    connect(_process_btn, SIGNAL(released()), this, SLOT(process_all_visible()));
    _load_all_btn = new QPushButton("Load All");
    connect(_load_all_btn, SIGNAL(released()), this, SLOT(load_all_visible()));
    _unload_all_btn = new QPushButton("Unload All");
    connect(_unload_all_btn, SIGNAL(released()), this, SLOT(unload_all_visible()));

    QHBoxLayout* hlayout2 = new QHBoxLayout();
    hlayout2->addWidget(_load_all_btn);
    hlayout2->addWidget(_unload_all_btn);

    QLayout* vlayout = new QVBoxLayout();
    vlayout->addItem(hlayout1);
    vlayout->addItem(hlayout2);
    vlayout->addWidget(_process_btn);
    vlayout->addWidget(_file_list_view);
    setLayout(vlayout);

}

/*---------------------------------------------------------------------------*/

void FileTabWidget::_gen_visible_list(QStringList *sl)
{
    for(int i=0; i < _file_list_model->rowCount(); i++)
    {
        if(false == _file_list_view->isRowHidden(i))
        {
            QStandardItem *val = _file_list_model->item(i, 0);
            sl->append(val->text());
        }
    }
}

/*---------------------------------------------------------------------------*/

void FileTabWidget::load_all_visible()
{
    QStringList sl;
    _gen_visible_list(&sl);
    emit loadList(sl);
}

/*---------------------------------------------------------------------------*/

void FileTabWidget::unload_all_visible()
{
    QStringList sl;
    _gen_visible_list(&sl);
    emit unloadList(sl);
}

/*---------------------------------------------------------------------------*/

void FileTabWidget::process_all_visible()
{
    QStringList sl;
    _gen_visible_list(&sl);
    emit processList(sl);
}

/*---------------------------------------------------------------------------*/

void FileTabWidget::unload_all()
{
    _file_list_model->clear();
}

/*---------------------------------------------------------------------------*/

void FileTabWidget::set_file_list(const map<QString, QFileInfo>& fileinfo_list)
{
	_file_list_model->clear();
    for(auto & itr : fileinfo_list)
    {
        _file_list_model->appendRow(new QStandardItem(QIcon(":/images/circle_gray.png"), itr.first));
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
    QStringList sl;
    sl.append(idx.data(0).toString());
    emit loadList(sl);
}

/*---------------------------------------------------------------------------*/

void FileTabWidget::onLoadFile()
{
    QStringList sl;
    QModelIndexList list = _file_list_view->selectionModel()->selectedIndexes();
    for(int i =0; i<list.length(); i++)
    {
        QModelIndex idx = list.at(i);
        sl.append(idx.data(0).toString());
    }
    emit loadList(sl);
}

/*---------------------------------------------------------------------------*/

void FileTabWidget::onUnloadFile()
{
    QStringList sl;
    QModelIndexList list = _file_list_view->selectionModel()->selectedIndexes();
    for(int i =0; i<list.length(); i++)
    {
        QModelIndex idx = list.at(i);
        sl.append(idx.data(0).toString());
    }
    emit unloadList(sl);
}

/*---------------------------------------------------------------------------*/

void FileTabWidget::filterTextChanged(const QString &filter_text)
{
    _filter_line->setText(filter_text);
    if(filter_text.length() > 0)
    {
        QRegExp re (filter_text);
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

void FileTabWidget::loaded_file_status_changed(File_Loaded_Status status, const QString& filename)
{
    for(int i=0; i < _file_list_model->rowCount(); i++)
    {
        QStandardItem *val = _file_list_model->item(i, 0);
        if(filename == val->text())
        {
            switch(status)
            {
            case UNLOADED:
                val->setIcon(QIcon(":/images/circle_gray.png"));
                break;
            case LOADED:
                val->setIcon(QIcon(":/images/circle_green.png"));
                break;
            case FAILED_LOADING:
                val->setIcon(QIcon(":/images/circle_red.png"));
                break;
            }
            break;
        }
    }

}

/*---------------------------------------------------------------------------*/

void FileTabWidget::filterBtnClicked()
{
    QAction* result = _filterHelpMenu->exec(QCursor::pos());
    if(result != nullptr)
    {
      result->trigger();
    }

}

/*---------------------------------------------------------------------------*/

void FileTabWidget::addCustomContext(QString Id, QString label)
{
    QAction* action = _contextMenu->addAction(label);
    action->setData(Id);
    connect(action, SIGNAL(triggered()), this, SLOT(onCustomContext()));
}

/*---------------------------------------------------------------------------*/

void FileTabWidget::onCustomContext()
{
    QStringList sl;
    QModelIndexList list = _file_list_view->selectionModel()->selectedIndexes();
    for(int i =0; i<list.length(); i++)
    {
        QModelIndex idx = list.at(i);
        sl.append(idx.data(0).toString());
    }
    QAction *act = qobject_cast<QAction *>(sender());
    QVariant v = act->data();
    emit customContext(v.toString(), sl);
}

/*---------------------------------------------------------------------------*/
