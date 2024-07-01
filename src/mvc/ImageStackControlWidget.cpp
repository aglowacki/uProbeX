/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ImageStackControlWidget.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QSplitter>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

ImageStackControlWidget::ImageStackControlWidget(QWidget* parent) : QWidget(parent)
{
	_model = nullptr;
	createLayout();

}

//---------------------------------------------------------------------------

ImageStackControlWidget::~ImageStackControlWidget()
{
	/* handeled by MapsWorkspaceMdodel
	for(auto &itr : _h5_model_map)
	{
		_model->unload_H5_Model(itr.first);
	}
	_h5_model_map.clear();
	*/
	if (_mapsFilsWidget != nullptr)
	{
		delete _mapsFilsWidget;
		_mapsFilsWidget = nullptr;
	}

}

//---------------------------------------------------------------------------

void ImageStackControlWidget::savePref()
{
	_imageGrid->savePref();
}

//---------------------------------------------------------------------------

void ImageStackControlWidget::createLayout()
{
    QVBoxLayout* vlayout = new QVBoxLayout();
    QHBoxLayout* hlayout1 = new QHBoxLayout();
	//QHBoxLayout* hlayout2 = new QHBoxLayout();
	_imageGrid = new MapsElementsWidget(1,1);
	_vlm_widget = new VLM_Widget();
	connect(_vlm_widget, &VLM_Widget::onLinkRegionToDataset, this, &ImageStackControlWidget::onLinkRegionToDataset);

	_mda_widget = new MDA_Widget();

	_mapsFilsWidget = new MapsWorkspaceFilesWidget();
	connect(_mapsFilsWidget, &MapsWorkspaceFilesWidget::loaded_model, this, &ImageStackControlWidget::onLoad_Model);
	connect(_mapsFilsWidget, &MapsWorkspaceFilesWidget::unloadList_model, this, &ImageStackControlWidget::onUnloadList_Model);
	connect(_mapsFilsWidget, &MapsWorkspaceFilesWidget::datasetSelected, this, &ImageStackControlWidget::onChangeDatasetName);

	connect(_imageGrid, &MapsElementsWidget::new_rois, _mapsFilsWidget, &MapsWorkspaceFilesWidget::update_roi_num);

    _left_btn =  new QPushButton();
    _left_btn->setIcon(QIcon(":/images/previous.png"));
    _left_btn->setMaximumWidth(150);
	connect(_left_btn, &QAbstractButton::pressed, this, &ImageStackControlWidget::onPrevFilePressed);

    _image_name_cb = new QComboBox();
	_image_name_cb->setModel(&_image_name_cb_model);

    _right_btn =  new QPushButton();
    _right_btn->setIcon(QIcon(":/images/next.png"));
    _right_btn->setMaximumWidth(150);
	connect(_right_btn, &QAbstractButton::pressed, this, &ImageStackControlWidget::onNextFilePressed);

    hlayout1->addWidget(_left_btn);
    hlayout1->addWidget(_image_name_cb);
    hlayout1->addWidget(_right_btn);
	hlayout1->setSpacing(0);
	hlayout1->setContentsMargins(0, 0, 0, 0);

	connect(_image_name_cb, &QComboBox::currentTextChanged, this, &ImageStackControlWidget::model_IndexChanged);

	connect(_imageGrid, &MapsElementsWidget::loaded_perc, this, &ImageStackControlWidget::update_progress_bar);
	connect(_mapsFilsWidget, &MapsWorkspaceFilesWidget::loaded_perc, this, &ImageStackControlWidget::update_progress_bar);

	QWidget *navWidget = new QWidget();
	navWidget->setLayout(hlayout1);

	_nav_dock = new QDockWidget("Files", this);
    _nav_dock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	_nav_dock->setWidget(navWidget);

    vlayout->addWidget(_nav_dock);
	vlayout->addWidget(_imageGrid);
	vlayout->addWidget(_vlm_widget);
	vlayout->addWidget(_mda_widget);
	vlayout->setSpacing(0);
	vlayout->setContentsMargins(0, 0, 0, 0);

	_files_dock = new QDockWidget("Files", this);
    _files_dock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	_files_dock->setWidget(_mapsFilsWidget);
	connect(_files_dock, &QDockWidget::topLevelChanged, this, &ImageStackControlWidget::onDockFloatChanged);

	//hlayout2->addWidget(_files_dock);

	_files_anim_widget = new AnnimateSlideWidget();
	//QWidget *leftWidget = new QWidget();
	//leftWidget->setLayout(hlayout2);
	_files_anim_widget->setAnimWidget(_mapsFilsWidget, _files_dock);
	//_files_anim_widget->setAnimWidget(_mapsFilsWidget, ">");
	
	QWidget *rightWidget = new QWidget();
	rightWidget->setLayout(vlayout);

	QSplitter* single_data_splitter = new QSplitter();
	single_data_splitter->setOrientation(Qt::Horizontal);
	single_data_splitter->addWidget(_files_anim_widget);
	single_data_splitter->setStretchFactor(0, 1);
	single_data_splitter->addWidget(rightWidget);
	//createToolBar(m_imageViewWidget);
	//counts_layout->addWidget(m_toolbar);
	//counts_layout->addWidget(splitter);
	single_data_splitter->setCollapsible(0, true);
	single_data_splitter->setCollapsible(1, false);

	_load_progress = new QProgressBar();
	
	QTabWidget* data_tab = new QTabWidget();
	data_tab->addTab(single_data_splitter, "Single Dataset");
	//data_tab->setProperty("padding", QVariant("1px"));
	//data_tab->setSpacing(0);
	data_tab->setContentsMargins(0, 0, 0, 0);
	//data_tab->addTab(single_data_splitter, "Multiple Datasets");

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(data_tab);
	mainLayout->addWidget(_load_progress);
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	//_imageGrid->hide();
	_vlm_widget->hide();
	_mda_widget->hide();

	setLayout(mainLayout);
}

//---------------------------------------------------------------------------

void ImageStackControlWidget::closeEvent(QCloseEvent *event)
{
	emit widgetClosed();
	event->accept();
}

//---------------------------------------------------------------------------

void ImageStackControlWidget::model_IndexChanged(const QString &text)
{
	if (_h5_model_map.count(text) > 0)
	{
		if (_mda_widget->isVisible())
			_mda_widget->hide();
		if (_vlm_widget->isVisible())
			_vlm_widget->hide();
		_imageGrid->setModel(_h5_model_map[text]);
		_imageGrid->show();
	}
	else if(_raw_model_map.count(text) > 0)
	{
		if (_vlm_widget->isVisible())
			_vlm_widget->hide();
		if(_imageGrid->isVisible())
			_imageGrid->hide();
		_mda_widget->setModel(_raw_model_map[text]);
		_mda_widget->show();
	}
	else if (_vlm_model_map.count(text) > 0)
	{
		if (_mda_widget->isVisible())
			_mda_widget->hide();
		if (_imageGrid->isVisible())
			_imageGrid->hide();
		_vlm_widget->setModel(_vlm_model_map[text]);
		_vlm_widget->show();
	}
}

//---------------------------------------------------------------------------

void ImageStackControlWidget::onDockFloatChanged(bool floating)
{
	if(floating)
	{
		_files_anim_widget->setAnimEnabled(false);
	}
	else
	{
		_files_anim_widget->setAnimEnabled(true);
	}
}

//---------------------------------------------------------------------------

void ImageStackControlWidget::onPrevFilePressed()
{
	int idx = _image_name_cb->currentIndex();
	idx--;
	int amt = _image_name_cb->count();
	if (idx < amt)
	{
		_image_name_cb->setCurrentIndex(idx);
	}
}

//---------------------------------------------------------------------------

void ImageStackControlWidget::onNextFilePressed()
{
	int idx = _image_name_cb->currentIndex();
	idx++;
	int amt = _image_name_cb->count();
	if (idx < amt)
	{
		_image_name_cb->setCurrentIndex(idx);
	}
}

//---------------------------------------------------------------------------

void ImageStackControlWidget::setModel(MapsWorkspaceModel *model)
{
	_model = model;
	_mapsFilsWidget->clearLists();
	_mapsFilsWidget->setModel(_model);
	if (_model != nullptr)
	{
		_mapsFilsWidget->setLabelWorkspacePath(_model->get_directory_name());
		this->setWindowTitle(_model->get_directory_name());
	}
}

//---------------------------------------------------------------------------

void ImageStackControlWidget::update_progress_bar(int val, int amt)
{
	if (amt != _load_progress->maximum())
	{
		_load_progress->setRange(0, amt);
	}

	_load_progress->setValue(val);
}

//---------------------------------------------------------------------------

void ImageStackControlWidget::onLoad_Model(const QString name, MODEL_TYPE mt)
{
	QStringList model_list = _image_name_cb_model.stringList();

	//make sure no duplicates
	if (model_list.contains(name) == false)
	{
		switch (mt)
		{
		case MODEL_TYPE::MAPS_H5:
			_h5_model_map[name] = _model->get_MapsH5_Model(name);
			break;
		case MODEL_TYPE::RAW:
			_raw_model_map[name] = _model->get_RAW_Model(name);
			break;
		case MODEL_TYPE::VLM:
			_vlm_model_map[name] = _model->get_VLM_Model(name);
			break;
		}
		
		_image_name_cb->addItem(name);
		if(Preferences::inst()->getValue(STR_PRF_SHOW_DATASET_ON_LOAD).toBool())
		{
			_image_name_cb->setCurrentText(name);
		}
	}
}

//---------------------------------------------------------------------------

void ImageStackControlWidget::onUnloadList_Model(const QStringList sl, MODEL_TYPE mt)
{
	disconnect(_image_name_cb, &QComboBox::currentTextChanged, this, &ImageStackControlWidget::model_IndexChanged);

    _imageGrid->setModel(nullptr);
    foreach (QString s, sl)
    {
		switch (mt)
		{
		case MODEL_TYPE::MAPS_H5:
			_h5_model_map.erase(s);
			_model->unload_H5_Model(s);
			break;
		case MODEL_TYPE::RAW:
			_raw_model_map.erase(s);
			_model->unload_RAW_Model(s);
			break;
		case MODEL_TYPE::VLM:
			_vlm_model_map.erase(s);
			_model->unload_VLM_Model(s);
			break;
		}
       
        for(int i=0; i < _image_name_cb->count(); i++)
        {
            if (s == _image_name_cb->itemText(i))
            {
                _image_name_cb->removeItem(i);
                break;
            }
        }
    }
	connect(_image_name_cb, &QComboBox::currentTextChanged, this, &ImageStackControlWidget::model_IndexChanged);
	emit _image_name_cb->currentTextChanged(_image_name_cb->currentText());
}


//---------------------------------------------------------------------------

void ImageStackControlWidget::update_file_list()
{
	if (_mapsFilsWidget != nullptr) 
	{
		_mapsFilsWidget->updatedMDA(); 
		_mapsFilsWidget->updatedHDF();
		_mapsFilsWidget->updatedVLM();
	} 
}

//---------------------------------------------------------------------------

void ImageStackControlWidget::onLinkRegionToDataset(QString item_name, QString vlm_file_path, QImage image)
{
	if (_model != nullptr)
	{
		QMenu menu(this);

		std::map<QString, QFileInfo> raw_file_map = _model->get_raw_file_list();
		QStringList raw_file_list;
		for (auto& itr : raw_file_map)
		{
			menu.addAction(new QAction(itr.second.baseName()));
		}
		QPoint globalCursorPos = QCursor::pos();
		QAction* result = menu.exec(globalCursorPos);
		if (result != nullptr)
		{
			logI << "Linking VLM region with dataset "<< result->text().toStdString() << "\n"; 
			QDir dir = _model->get_directory();
			if (dir.mkpath("VLM/region_links"))
			{
				dir.cd("VLM/region_links");
				// use @ as delimeter 
				QString filepath = dir.absolutePath() + "/" + result->text() + "@" + item_name + ".tif";
				if (false == image.save(filepath))
				{
					logW << "Error saving region_link " << filepath.toStdString() << "\n";
				}
				else
				{
					_model->reload_region_link();
					//emit reload_dataset();
				}
			}
			else
			{
				QString dir_path_str = dir.absolutePath() + "/VLM/region_links";
				logW << "Error creating directory " << dir_path_str.toStdString() <<"\n";
			}
			logI << "dir " << dir.absolutePath().toStdString() << "\n";
			//result->text();
		}

		//TODO: display dialog with raw dataset names for user to select one to link to.
		/*
		_raw_list_widget->clear();
		_raw_list_widget->addItems(raw_file_list);
		
		_raw_file_dialog.show();
		*/
	}
}

//---------------------------------------------------------------------------

void ImageStackControlWidget::onChangeDatasetName(const QString & name)
{
	int i = _image_name_cb->findText(name);
	if (i > -1)
	{
		_image_name_cb->setCurrentIndex(i);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
