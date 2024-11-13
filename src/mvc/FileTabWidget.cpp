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
#include <QRegularExpression>
#include <preferences/Preferences.h>
#include "core/defines.h"
//---------------------------------------------------------------------------

FileTabWidget::FileTabWidget(QWidget* parent) : QWidget(parent)
{

    _filterHelpMenu = new QMenu("Help", this);

    _contextMenu = new QMenu(("Context menu"), this);
    _action_load = _contextMenu->addAction("Load");
    connect(_action_load, SIGNAL(triggered()), this, SLOT(onLoadFile()));
    _action_unload = _contextMenu->addAction("UnLoad");
    connect(_action_unload, SIGNAL(triggered()), this, SLOT(onUnloadFile()));
    _action_refresh = _contextMenu->addAction("Refresh");
    connect(_action_refresh, SIGNAL(triggered()), this, SIGNAL(onRefresh()));

    _file_list_model = new FileTableModel();
    _file_list_view = new QTableView();
   // _file_list_view->setViewMode(QListView::IconMode);
    _file_list_view->setModel(_file_list_model);
    _file_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _file_list_view->setContextMenuPolicy(Qt::CustomContextMenu);
    _file_list_view->setSelectionMode(QAbstractItemView::ExtendedSelection); //MultiSelection
    _file_list_view->setSortingEnabled(true);

    // if preferences saves on select changes loaded dataset
    connect(_file_list_view->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &FileTabWidget::onFileRowChange);

    connect(_file_list_view, SIGNAL(doubleClicked(const QModelIndex)),
                this, SLOT(onDoubleClickElement(const QModelIndex)));
    connect(_file_list_view, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));

    _filter_line = new QLineEdit();
    //connect(_filter_line, SIGNAL(textChanged(const QString &)), this, SLOT(filterTextChanged(const QString &)));

    connect(_filter_line, &QLineEdit::returnPressed, this, &FileTabWidget::onUpdateFilter);

	_filter_suggest_btn = new QPushButton();
	_filter_suggest_btn->setIcon(QIcon(":/images/question.png"));
	_filter_suggest_btn->setIconSize(QSize(15, 15));
    connect(_filter_suggest_btn, SIGNAL(released()),
		this, SLOT(filterBtnClicked()));



    QHBoxLayout* hlayout1 = new QHBoxLayout();
    hlayout1->addWidget(new QLabel("Filter"));
    hlayout1->addWidget(_filter_line);
	hlayout1->addWidget(_filter_suggest_btn);

    bool rad_opt = Preferences::inst()->getValue(STR_PREF_RADIO_LOAD_SELECTED_OPTION).toBool();

    _radio_all_files = new QRadioButton("All Visible Files");
    _radio_all_files->setChecked(!rad_opt);
    _radio_all_files->setAutoExclusive(true);
    connect(_radio_all_files, &QAbstractButton::toggled, this, &FileTabWidget::onRadioAllChanged);
    _selected_all_files = new QRadioButton("Selected Files");
    _selected_all_files->setChecked(rad_opt);
    _selected_all_files->setAutoExclusive(true);
    connect(_selected_all_files, &QAbstractButton::toggled, this, &FileTabWidget::onRadioSelectChanged);
    QHBoxLayout* hbox_radios = new QHBoxLayout();
    hbox_radios->addWidget(_radio_all_files);
    hbox_radios->addWidget(_selected_all_files);

    _load_all_btn = new QPushButton("Load");
    connect(_load_all_btn, SIGNAL(released()), this, SLOT(load_all_visible()));
    _unload_all_btn = new QPushButton("Unload");
    connect(_unload_all_btn, SIGNAL(released()), this, SLOT(unload_all_visible()));

    QHBoxLayout* hlayout2 = new QHBoxLayout();
    hlayout2->addWidget(_load_all_btn);
    hlayout2->addWidget(_unload_all_btn);

    _custom_btn_box = new QVBoxLayout();

    QLayout* vlayout = new QVBoxLayout();
    vlayout->addItem(hlayout1);
    vlayout->addItem(hbox_radios);
    vlayout->addItem(hlayout2);
    vlayout->addItem(_custom_btn_box);
    vlayout->addWidget(_file_list_view);
    vlayout->setSpacing(0);
	vlayout->setContentsMargins(0, 0, 0, 0);
    setLayout(vlayout);

}

//---------------------------------------------------------------------------

void FileTabWidget::_gen_visible_list(QStringList *sl)
{
    for(int i=0; i < _file_list_model->rowCount(); i++)
    {
        if(false == _file_list_view->isRowHidden(i))
        {
            auto val = _file_list_model->item(i);
            sl->append(val.text);
        }
    }
}

//---------------------------------------------------------------------------

void FileTabWidget::onRadioAllChanged(bool val)
{
    Preferences::inst()->setValue(STR_PREF_RADIO_LOAD_SELECTED_OPTION, !val);
}

//---------------------------------------------------------------------------

void FileTabWidget::onRadioSelectChanged(bool val)
{
    Preferences::inst()->setValue(STR_PREF_RADIO_LOAD_SELECTED_OPTION, val);
}

//---------------------------------------------------------------------------

void FileTabWidget::onUpdateFilter()
{
    filterTextChanged(_filter_line->text());
}

//---------------------------------------------------------------------------

void FileTabWidget::load_all_visible()
{
    QStringList sl;
    if(_radio_all_files->isChecked())
    {
        _gen_visible_list(&sl);
    }
    else
    {
        QModelIndexList list = _file_list_view->selectionModel()->selectedIndexes();
        for(int i =0; i<list.length(); i++)
        {
            QModelIndex idx = list.at(i);
            QString val;
            if( _file_list_model->getNameAtRow(idx.row(), val) )
            {
                sl.append(val);
            }
        }
    }
    emit loadList(sl);
}

//---------------------------------------------------------------------------

void FileTabWidget::unload_all_visible()
{
    QStringList sl;
    if(_radio_all_files->isChecked())
    {
        _gen_visible_list(&sl);
    }
    else
    {
        QModelIndexList list = _file_list_view->selectionModel()->selectedIndexes();
        for(int i =0; i<list.length(); i++)
        {
            QModelIndex idx = list.at(i);
            QString val;
            if( _file_list_model->getNameAtRow(idx.row(), val) )
            {
                sl.append(val);
            }
        }
    }
    emit unloadList(sl);
}

//---------------------------------------------------------------------------

void FileTabWidget::unload_all()
{
    _file_list_model->clear();
}

//---------------------------------------------------------------------------

void FileTabWidget::set_file_list(const std::map<QString, QFileInfo>& fileinfo_list)
{
    double divisor = 1;
    switch(Preferences::inst()->getValue(STR_PRF_FILE_SIZE).toInt())
    {
        case 0:
            break;
        case 1: 
            divisor = 1024;
            break;
        case 2: 
            divisor = 1024*1024;
            break;
        case 3: 
            divisor = 1024*1024*1024;
            break;
        default:
            break;
    }

	_file_list_model->clear();
    for(auto & itr : fileinfo_list)
    {
        _file_list_model->appendRow( RowData(QIcon(":/images/circle_gray.png"), itr.first, (double)itr.second.size()/divisor));       
    }

    _file_list_view->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    _file_list_view->horizontalHeader()->resizeSections(QHeaderView::Interactive);
}

//---------------------------------------------------------------------------

void FileTabWidget::set_roi_num_list(const std::map<QString, int>& roi_num_map)
{
    _file_list_model->update_roi_num(roi_num_map);
}

//---------------------------------------------------------------------------

void FileTabWidget::set_roi_num(QString name, int val)
{
    _file_list_model->update_single_roi_num(name, val);
}

//---------------------------------------------------------------------------

void FileTabWidget::update_file_list(const std::map<QString, QFileInfo>& fileinfo_list)
{
    double divisor = 1;
    switch(Preferences::inst()->getValue(STR_PRF_FILE_SIZE).toInt())
    {
        case 0:
            break;
        case 1: 
            divisor = 1024;
            break;
        case 2: 
            divisor = 1024*1024;
            break;
        case 3: 
            divisor = 1024*1024*1024;
            break;
        default:
            break;
    }

    int rows = _file_list_model->rowCount();
    for (const auto& itr : fileinfo_list)
    {
        bool found = false;
        for(int i=0; i<rows; i++)
        {
            auto item = _file_list_model->item(i);
            if (item.text == itr.first)
            {
                found = true;
                break;
            }
        }

        if (false == found)
        {
            _file_list_model->appendRow(RowData(QIcon(":/images/circle_gray.png"), itr.first, (double)itr.second.size()/divisor));
        }
    }

    _file_list_view->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    _file_list_view->horizontalHeader()->resizeSections(QHeaderView::Interactive);
}

//---------------------------------------------------------------------------

void FileTabWidget::ShowContextMenu(const QPoint &pos)
{
    _contextMenu->exec(mapToGlobal(pos));
}

//---------------------------------------------------------------------------

void FileTabWidget::onDoubleClickElement(const QModelIndex idx)
{
    if (_load_all_btn->isEnabled())
    {
        QStringList sl;
        QString val;
        if( _file_list_model->getNameAtRow(idx.row(), val) )
        {
            sl.append(val);
        }
        emit loadList(sl);
    }
}

//---------------------------------------------------------------------------

void FileTabWidget::setActionsAndButtonsEnabled(bool val)
{
    _load_all_btn->setEnabled(val);
    _file_list_view->setEnabled(val);
    _unload_all_btn->setEnabled(val);
    _action_load->setEnabled(val);
    _action_unload->setEnabled(val);
    _action_refresh->setEnabled(val);
    for (QAction* act : _custom_action_list)
    {
        act->setEnabled(val);
    }
    for (QPushButton* btn : _custom_button_list)
    {
        btn->setEnabled(val);
    }
}

//---------------------------------------------------------------------------

void FileTabWidget::onLoadFile()
{
    QStringList sl;
    QModelIndexList list = _file_list_view->selectionModel()->selectedIndexes();
    for(int i =0; i<list.length(); i++)
    {
        QModelIndex idx = list.at(i);
        QString val;
        if( _file_list_model->getNameAtRow(idx.row(), val) )
        {
            sl.append(val);
        }
    }
    emit loadList(sl);
}

//---------------------------------------------------------------------------

void FileTabWidget::onUnloadFile()
{
    QStringList sl;
    QModelIndexList list = _file_list_view->selectionModel()->selectedIndexes();
    for(int i =0; i<list.length(); i++)
    {
        QModelIndex idx = list.at(i);
        QString val;
        if( _file_list_model->getNameAtRow(idx.row(), val) )
        {
            sl.append(val);
        }
    }
    emit unloadList(sl);

}

//---------------------------------------------------------------------------

void FileTabWidget::filterTextChanged(const QString &filter_text)
{
    _filter_line->setText(filter_text);
    if(filter_text.length() > 0)
    {
        // if not strict then prepend and append '*'
        QString filter_t = filter_text;
        if(false == Preferences::inst()->getValue(STR_PRF_STRICT_REGEX).toBool())
        {
            filter_t.prepend('*');
            filter_t.append('*');
        }
        QRegularExpression re (QRegularExpression::wildcardToRegularExpression(filter_t));
     
        for(int i=0; i < _file_list_model->rowCount(); i++)
        {
            _file_list_view->setRowHidden(i, true);
            auto val = _file_list_model->item(i);
            QRegularExpressionMatchIterator j = re.globalMatch(val.text);
            if (j.hasNext())
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

//---------------------------------------------------------------------------

void FileTabWidget::loaded_file_status_changed(File_Loaded_Status status, const QString& filename)
{
    _file_list_model->updateStatus(status, filename);
}

//---------------------------------------------------------------------------

void FileTabWidget::loaded_file_status_changed_all(File_Loaded_Status status)
{
    _file_list_model->updateAllStatus(status);
}

//---------------------------------------------------------------------------

void FileTabWidget::filterBtnClicked()
{
    QAction* result = _filterHelpMenu->exec(QCursor::pos());
    if(result != nullptr)
    {
      result->trigger();
    }
}

//---------------------------------------------------------------------------

void FileTabWidget::addCustomContext(QString label)
{
    QAction* action = _contextMenu->addAction(label);
    _custom_action_list.append(action);
    connect(action, SIGNAL(triggered()), this, SLOT(onCustomContext()));
}
//---------------------------------------------------------------------------

void FileTabWidget::onCustomContext()
{
    QStringList sl;
    QModelIndexList list = _file_list_view->selectionModel()->selectedIndexes();
    for(int i =0; i<list.length(); i++)
    {
        QModelIndex idx = list.at(i);
        QString val;
        if( _file_list_model->getNameAtRow(idx.row(), val) )
        {
            sl.append(val);
        }
    }
    QAction *act = qobject_cast<QAction *>(sender());
    emit customContext(act->text(), sl);
}

//---------------------------------------------------------------------------

void FileTabWidget::addCustomButtonRow(QString label)
{
    QPushButton* btn = new QPushButton(label, this);
    _custom_btn_box->addWidget(btn);
    _custom_button_list.append(btn);
    connect(btn, &QPushButton::pressed, this, &FileTabWidget::onCustomButton);
}

//---------------------------------------------------------------------------

void FileTabWidget::addCustomButtonRow(QString label, QString label2)
{
    QPushButton* btn = new QPushButton(label, this);
    QPushButton* btn2 = new QPushButton(label2, this);
    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(btn);
    hbox->addWidget(btn2);
    _custom_button_list.append(btn);
    _custom_button_list.append(btn2);
    _custom_btn_box->addItem(hbox);
    connect(btn, &QPushButton::pressed, this, &FileTabWidget::onCustomButton);
    connect(btn2, &QPushButton::pressed, this, &FileTabWidget::onCustomButton);
}

//---------------------------------------------------------------------------

void FileTabWidget::onCustomButton()
{
    QStringList sl;
    if(_radio_all_files->isChecked())
    {
        _gen_visible_list(&sl);
    }
    else
    {
        QModelIndexList list = _file_list_view->selectionModel()->selectedIndexes();
        for(int i =0; i<list.length(); i++)
        {
            QModelIndex idx = list.at(i);
            QString val;
            if( _file_list_model->getNameAtRow(idx.row(), val) )
            {
                sl.append(val);
            }
        }
    }
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    emit customButton(btn->text(), sl);
}

//---------------------------------------------------------------------------

void FileTabWidget::onFileRowChange(const QModelIndex& current, const QModelIndex& previous)
{
    if (current.isValid())
    {
        emit selectNewRow(current.data(0).toString());
    }
}

//---------------------------------------------------------------------------