/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/RoiStatisticsWidget.h>

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

RoiStatisticsWidget::RoiStatisticsWidget() : QWidget()
{

    _createLayout();

}

//---------------------------------------------------------------------------

RoiStatisticsWidget::~RoiStatisticsWidget()
{

}

//---------------------------------------------------------------------------

void append_roi(gstar::RoiMaskGraphicsItem* roi)
{

}

//---------------------------------------------------------------------------

void RoiStatisticsWidget::setImageData(std::unordered_map<std::string, data_struct::ArrayXXr<float>>& img_data)
{
/*
	_img_list_model->clear();
	_img_data.clear();

	//create save ordered vector by element Z number with K , L, M lines
	std::vector<std::string> element_lines;
	for (std::string el_name : data_struct::Element_Symbols)
	{
		element_lines.push_back(el_name);
	}
	for (std::string el_name : data_struct::Element_Symbols)
	{
		element_lines.push_back(el_name + "_L");
	}
	for (std::string el_name : data_struct::Element_Symbols)
	{
		element_lines.push_back(el_name + "_M");
	}

	std::vector<std::string> final_counts_to_add_before_scalers;
	final_counts_to_add_before_scalers.push_back(STR_COHERENT_SCT_AMPLITUDE);
	final_counts_to_add_before_scalers.push_back(STR_COMPTON_AMPLITUDE);
	final_counts_to_add_before_scalers.push_back(STR_SUM_ELASTIC_INELASTIC_AMP);
	final_counts_to_add_before_scalers.push_back(STR_TOTAL_FLUORESCENCE_YIELD);
	final_counts_to_add_before_scalers.push_back(STR_NUM_ITR);
	final_counts_to_add_before_scalers.push_back(STR_RESIDUAL);

	std::vector<std::string> scalers_to_add_first;
	scalers_to_add_first.push_back(STR_SR_CURRENT);
	scalers_to_add_first.push_back(STR_US_IC);
	scalers_to_add_first.push_back(STR_DS_IC);
	scalers_to_add_first.push_back(STR_ELT);
	scalers_to_add_first.push_back(STR_ELAPSED_LIVE_TIME);
	scalers_to_add_first.push_back(STR_ERT);
	scalers_to_add_first.push_back(STR_ELAPSED_REAL_TIME);
	scalers_to_add_first.push_back(STR_INPUT_COUNTS);
	scalers_to_add_first.push_back(STR_ICR);
	scalers_to_add_first.push_back("INCNT");
	scalers_to_add_first.push_back(STR_OUTPUT_COUNTS);
	scalers_to_add_first.push_back(STR_OCR);
	scalers_to_add_first.push_back("OUTCNT");
	scalers_to_add_first.push_back(STR_DEAD_TIME);
	scalers_to_add_first.push_back("abs_cfg");
	scalers_to_add_first.push_back("abs_ic");
	scalers_to_add_first.push_back("H_dpc_cfg");
	scalers_to_add_first.push_back("V_dpc_cfg");
	scalers_to_add_first.push_back("DPC1_IC");
	scalers_to_add_first.push_back("DPC2_IC");
	scalers_to_add_first.push_back("dia1_dpc_cfg");
	scalers_to_add_first.push_back("dia2_dpc_cfg");
	scalers_to_add_first.push_back("CFG_1");
	scalers_to_add_first.push_back(STR_CFG_2);
	scalers_to_add_first.push_back(STR_CFG_3);
	scalers_to_add_first.push_back(STR_CFG_4);
	scalers_to_add_first.push_back(STR_CFG_5);
	scalers_to_add_first.push_back("CFG_6");
	scalers_to_add_first.push_back("CFG_7");
	scalers_to_add_first.push_back("CFG_8");
	scalers_to_add_first.push_back("CFG_9");

	// insert in z order
	for (std::string el_name : element_lines)
	{
		if (img_data.count(el_name) > 0)
		{
			_img_data[QString(el_name.c_str())] = img_data.at(el_name);
			QStandardItem* item0 = new QStandardItem(false);
			item0->setCheckable(true);
			item0->setCheckState(Qt::Unchecked);
			item0->setText(QString(el_name.c_str()));
			_img_list_model->appendRow(item0);

			if (_image_size.isEmpty())
			{
				_image_size = QSize(img_data.at(el_name).cols(), img_data.at(el_name).rows());
			}
			img_data.erase(el_name);
		}

	}
	
	// add end of element list that are not elements
	for (auto& itr : final_counts_to_add_before_scalers)
	{
		if (img_data.count(itr) > 0)
		{
			_img_data[QString(itr.c_str())] = img_data.at(itr);
			QStandardItem* item0 = new QStandardItem(false);
			item0->setCheckable(true);
			item0->setCheckState(Qt::Unchecked);
			item0->setText(QString(itr.c_str()));
			_img_list_model->appendRow(item0);

			img_data.erase(itr);
		}
	}

	// add scalers in certain order
	for (auto& itr : scalers_to_add_first)
	{
		if (img_data.count(itr) > 0)
		{
			_img_data[QString(itr.c_str())] = img_data.at(itr);
			QStandardItem* item0 = new QStandardItem(false);
			item0->setCheckable(true);
			item0->setCheckState(Qt::Unchecked);
			item0->setText(QString(itr.c_str()));
			_img_list_model->appendRow(item0);

			img_data.erase(itr);
		}
	}

	// add rest of scalers
	for (auto& itr : img_data)
	{
		_img_data[QString(itr.first.c_str())] = img_data.at(itr.first);
		QStandardItem* item0 = new QStandardItem(false);
		item0->setCheckable(true);
		item0->setCheckState(Qt::Unchecked);
		item0->setText(QString(itr.first.c_str()));
		_img_list_model->appendRow(item0);
	}


	*/
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void ImageSegRoiDialog::_createLayout()
{
	/*
	_techTabs = new QTabWidget();
	_techTabs->addTab(_layout_map[STR_KMEANS], STR_KMEANS);
	_techTabs->addTab(_layout_map[STR_DBSCAN], STR_DBSCAN);
	_techTabs->addTab(_layout_map[STR_MANUAL], STR_MANUAL);
	_techTabs->setEnabled(false);
	connect(_techTabs, &QTabWidget::currentChanged, this, &ImageSegRoiDialog::onTabChanged);

	_acceptBtn = new QPushButton("Accept");
	//_acceptBtn->setEnabled(false);
	_cancelBtn = new QPushButton("Cancel");
	connect(_acceptBtn, SIGNAL(pressed()), this, SLOT(onAccept()));
	connect(_cancelBtn, SIGNAL(pressed()), this, SLOT(onClose()));

	_img_list_model = new QStandardItemModel();
	_img_names_view = new QListView();
	_img_names_view->setModel(_img_list_model);
	_img_names_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_img_names_view->setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(_img_list_model, &QStandardItemModel::itemChanged, this, &ImageSegRoiDialog::onImgSelection);

	_int_img_widget = new ImageSegWidget();

	QHBoxLayout* mainLayout = new QHBoxLayout;
	QVBoxLayout *leftLayout = new QVBoxLayout;
	QHBoxLayout* optionLayout = new QHBoxLayout;
	QHBoxLayout* buttonLayout = new QHBoxLayout;

	_chk_normalize_sum = new QCheckBox("Normalize before summing");
	connect(_chk_normalize_sum, &QCheckBox::stateChanged, this, &ImageSegRoiDialog::onNormalizeChanged);

	optionLayout->addWidget(_chk_normalize_sum);

	buttonLayout->addWidget(_acceptBtn);
	buttonLayout->addWidget(_cancelBtn);
	
	leftLayout->addWidget(_img_names_view);
	leftLayout->addItem(optionLayout);
	leftLayout->addWidget(_techTabs);
	leftLayout->addItem(buttonLayout);

	mainLayout->addItem(leftLayout);
	mainLayout->addWidget(_int_img_widget);

	setLayout(mainLayout);

	setWindowTitle("ROI Image Segmentation");
	*/
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void RoiStatisticsWidget::onClose()
{
	clear_all();
	close();
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void RoiStatisticsWidget::clear_all()
{
	//_int_img_widget->clearAllRoiMasks();
	_int_img_widget->deleteAllItems();
	//_cb_selected_roi->clear();
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/*
std::vector<QImage> ImageSegRoiDialog::_generate_images(int num_images, cv::Mat& mat)
{
	std::vector<QImage> images;
	for (int i = 0; i < num_images; i++)
	{
		QImage image(mat.cols, mat.rows, QImage::Format_ARGB32);
		image.setColorTable(*_selected_colormap);
		images.emplace_back(image);
	}

	for (int row = 0; row < mat.rows; row++)
	{
		for (int col = 0; col < mat.cols; col++)
		{
			//first clamp the data to max min
			int idx = mat.at<int>(row, col);
			byte data = 255;
			images[idx].setPixel(col, row, (uint)data);
		}
	}
	return images;
}

//---------------------------------------------------------------------------

QImage ImageSegRoiDialog::_generate_sum_image(cv::Mat& mat, ArrayXXr<float>& bg_img, uchar alpha)
{

	QImage background = _generate_img(bg_img).convertToFormat(QImage::Format_ARGB32_Premultiplied);
	
	QImage overlay(mat.cols, mat.rows, QImage::Format_ARGB32);

	for (int row = 0; row < mat.rows; row++)
	{
		for (int col = 0; col < mat.cols; col++)
		{
			//first clamp the data to max min
			int color_idx = mat.at<int>(row, col);
			if (color_idx > _color_map.size())
			{
				color_idx = color_idx % _color_map.size();
			}
			QColor color_data = _color_map[color_idx];
			color_data.setAlpha(alpha);

			//QRgb val = background.pixel(col, row);
			//val = color_data.rgba();
			overlay.setPixel(col, row, color_data.rgba());
		}
	}

	QPainter p(&background);
	//p.setCompositionMode(QPainter::CompositionMode_SourceOver);
	p.setCompositionMode(QPainter::CompositionMode_Plus);
	//p.setCompositionMode(QPainter::CompositionMode_Exclusion);
	//p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
	p.drawImage(0, 0, overlay);
	p.end();
	
	return background;
	
}
*/
//---------------------------------------------------------------------------

void RoiStatisticsWidget::onNormalizeChanged(int a)
{
	onImgSelection(nullptr);
}

//---------------------------------------------------------------------------

void RoiStatisticsWidget::onImgSelection(QStandardItem* item) 
{
	ArrayXXr<float> int_img;
	bool is_checked = _get_img(int_img, _chk_normalize_sum->isChecked());

	_techTabs->setEnabled(is_checked);

	if (is_checked)
	{
		_int_img_widget->setImageFromArray(int_img, *_selected_colormap);
	}
	else if (false == _image_size.isEmpty())
	{
		clear_image();
	}
}

//---------------------------------------------------------------------------

void RoiStatisticsWidget::clear_image()
{
	QImage image(_image_size, QImage::Format_ARGB32_Premultiplied);
	image.fill(QColor(Qt::gray));
	_int_img_widget->setPixMap(QPixmap::fromImage(image.convertToFormat(QImage::Format_RGB32)));
}

//---------------------------------------------------------------------------