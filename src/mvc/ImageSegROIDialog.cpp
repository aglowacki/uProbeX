/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ImageSegROIDialog.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QGridLayout>
#include <opencv2/core/eigen.hpp>

//---------------------------------------------------------------------------
static const QString STR_KMEANS = QString("KMeans");

//---------------------------------------------------------------------------

ImageSegRoiDialog::ImageSegRoiDialog() : QDialog()
{

	_color_map.insert({ 0, QColor(Qt::red) });
	_color_map.insert({ 1, QColor(Qt::green) });
	_color_map.insert({ 2, QColor(Qt::blue) });
	_color_map.insert({ 3, QColor(Qt::yellow) });
	_color_map.insert({ 4, QColor(Qt::magenta) });
	_color_map.insert({ 5, QColor(Qt::white) });
	_color_map.insert({ 6, QColor(Qt::gray) });
	_color_map.insert({ 7, QColor(Qt::cyan) });
	_color_map.insert({ 8, QColor(Qt::darkGray) });
	_color_map.insert({ 9, QColor(Qt::darkGreen) });
	_color_map.insert({ 10, QColor(Qt::darkMagenta) });
	_color_map.insert({ 11, QColor(Qt::darkBlue) });
	_color_map.insert({ 12, QColor(Qt::darkRed) });
	_color_map.insert({ 13, QColor(Qt::darkYellow) });
	_color_map.insert({ 14, QColor(Qt::darkCyan) });
	_color_map.insert({ 15, QColor(Qt::black) });
	

	_techLayout = new QVBoxLayout();
	_layout_map[STR_KMEANS] = _createKMeansLayout();
    createLayout();

}

//---------------------------------------------------------------------------

ImageSegRoiDialog::~ImageSegRoiDialog()
{

}

//---------------------------------------------------------------------------

void ImageSegRoiDialog::setImageData(std::unordered_map<std::string, data_struct::ArrayXXr<float>>& img_data)
{

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
	/*
	//add leftovers ( pile ups )
	for (auto& itr : element_counts_not_added)
	{
		// if it is not in the final add then add it
		if (std::find(final_counts_to_add_before_scalers.begin(), final_counts_to_add_before_scalers.end(), itr.first) == final_counts_to_add_before_scalers.end())
		{
			QString val = QString(itr.first.c_str());
			m_imageViewWidget->addLabel(val);
		}
	}
	*/
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


	/* // any order
	for (auto& itr : img_data)
	{
		//_img_data[QString(itr.first.c_str())] = itr.second;
		QStandardItem* item0 = new QStandardItem(false);
		item0->setCheckable(true);
		item0->setCheckState(Qt::Unchecked);
		item0->setText(QString(itr.first.c_str()));
		_img_list_model->appendRow(item0);
	}
	*/
}

//---------------------------------------------------------------------------

QLayout* ImageSegRoiDialog::_createKMeansLayout()
{
	QVBoxLayout* layout = new QVBoxLayout();

	QLabel* label = new QLabel("Number of Features:");
	_km_nfeatures = new QLineEdit();
	_km_nfeatures->setValidator(new QIntValidator(0, 10, this));
	_km_nfeatures->setText("2");

	QHBoxLayout* hlayout = new QHBoxLayout();
	hlayout->addWidget(label);
	hlayout->addWidget(_km_nfeatures);
	layout->addItem(hlayout);

	label = new QLabel("Term Criteria:");
	_km_TermCriteria = new QComboBox();
	_km_TermCriteria->addItem("COUNT");
	_km_TermCriteria->addItem("EPS"); 
	_km_TermCriteria->addItem("COUNT + EPS");
	_km_TermCriteria->setCurrentIndex(2); // default to EPS

	hlayout = new QHBoxLayout();
	hlayout->addWidget(label);
	hlayout->addWidget(_km_TermCriteria);
	layout->addItem(hlayout);

	label = new QLabel("Centers:");
	_km_Centers = new QComboBox();
	_km_Centers->addItem("Random");
	_km_Centers->addItem("PP");
	_km_Centers->setCurrentIndex(0); // default to Random

	hlayout = new QHBoxLayout();
	hlayout->addWidget(label);
	hlayout->addWidget(_km_Centers);
	layout->addItem(hlayout);
	
	label = new QLabel("Attempts:");
	_km_attempts = new QLineEdit();
	_km_attempts->setValidator(new QIntValidator(0, 100, this));
	_km_attempts->setText("3");

	hlayout = new QHBoxLayout();
	hlayout->addWidget(label);
	hlayout->addWidget(_km_attempts);
	layout->addItem(hlayout);

	label = new QLabel("MAX_Iter");
	_km_le_MAX_ITER = new QLineEdit();
	_km_le_MAX_ITER->setValidator(new QIntValidator(0, 10000, this));
	_km_le_MAX_ITER->setText("10");

	hlayout = new QHBoxLayout();
	hlayout->addWidget(label);
	hlayout->addWidget(_km_le_MAX_ITER);
	layout->addItem(hlayout);

	label = new QLabel("Epsilon");
	_km_le_epsilon = new QLineEdit();
	_km_le_epsilon->setValidator(new QDoubleValidator());
	_km_le_epsilon->setText("1.0");

	hlayout = new QHBoxLayout();
	hlayout->addWidget(label);
	hlayout->addWidget(_km_le_epsilon);
	layout->addItem(hlayout);
	
	return layout;
}

//---------------------------------------------------------------------------

void ImageSegRoiDialog::createLayout()
{
	_techLabel = new QLabel("Auto Image Segment Technique:");
	_cb_tech = new QComboBox();
	_cb_tech->addItem(STR_KMEANS);

	_runBtn = new QPushButton("Run");
	_runBtn->setEnabled(false);
	_acceptBtn = new QPushButton("Accept");
	_acceptBtn->setEnabled(false);
	_cancelBtn = new QPushButton("Cancel");
	connect(_runBtn, SIGNAL(pressed()), this, SLOT(onRun()));
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

	optionLayout->addWidget(_techLabel);
	optionLayout->addWidget(_cb_tech);

	buttonLayout->addWidget(_runBtn);
	buttonLayout->addWidget(_acceptBtn);
	buttonLayout->addWidget(_cancelBtn);
	
	leftLayout->addWidget(_img_names_view);
	leftLayout->addItem(optionLayout);
	leftLayout->addItem(_techLayout);
	leftLayout->addItem(buttonLayout);

	mainLayout->addItem(leftLayout);
	mainLayout->addWidget(_int_img_widget);
	
	_techLayout->addItem(_layout_map[STR_KMEANS]);

	setLayout(mainLayout);

	setWindowTitle("ROI Image Segmentation");
}

//---------------------------------------------------------------------------

void ImageSegRoiDialog::onSetTech(QString name)
{
	if (_layout_map.count(name) > 0)
	{
		_techLayout->removeItem(_techLayout->itemAt(0));
		_techLayout->addItem(_layout_map.at(name));
	}
}

//---------------------------------------------------------------------------

void ImageSegRoiDialog::onRun()
{
	// check if we have roi masks and ask user if we should clear them before running.

	ArrayXXr<float> int_img;
	_get_img(int_img);

	if(_cb_tech->currentText() == STR_KMEANS)
	{
		cv::KmeansFlags flags;
		cv::TermCriteria crit;
		if (_km_TermCriteria->currentIndex() == 0)
		{
			crit.type = cv::TermCriteria::COUNT;
		}
		else if (_km_TermCriteria->currentIndex() == 1)
		{
			crit.type = cv::TermCriteria::EPS;
		}
		else if (_km_TermCriteria->currentIndex() == 2)
		{
			crit.type = cv::TermCriteria::COUNT | cv::TermCriteria::EPS;
		}
		crit.maxCount = _km_le_MAX_ITER->text().toInt();
		crit.epsilon = _km_le_epsilon->text().toDouble();
		int clusterCount = _km_nfeatures->text().toInt();
		int attempts = _km_attempts->text().toInt();

		if (_km_Centers->currentIndex() == 0)
		{
			flags = cv::KMEANS_RANDOM_CENTERS;
		}
		else if (_km_Centers->currentIndex() == 1)
		{
			flags = cv::KMEANS_PP_CENTERS;
		}

		cv::Mat labels, centers;
		
		cv::Mat data(int_img.rows() * int_img.cols(), 1, CV_32FC1, int_img.data());
		
		double compactness = cv::kmeans(data, clusterCount, labels, crit, attempts, flags, centers);

		cv::Mat new_labels = labels.reshape(1, int_img.rows());
		

		for (int i = 0; i < clusterCount; i++)
		{
			int color_idx = i;
			if (color_idx > _color_map.size())
			{
				color_idx = color_idx % _color_map.size();
			}
			QColor color_data = _color_map[color_idx];

			gstar::RoiMaskGraphicsItem* roi = new gstar::RoiMaskGraphicsItem(new_labels, i, color_data);
			_int_img_widget->addRoiMask(roi);
		}
	}
	_acceptBtn->setEnabled(true);
}

//---------------------------------------------------------------------------

void ImageSegRoiDialog::onAccept()
{
	// emit list of roi's
	emit onNewROIs(_int_img_widget->getAllROIs());
	close();
}

//---------------------------------------------------------------------------

void ImageSegRoiDialog::onClose()
{
	clear_all_rois();
	clear_image();
	close();
}

//---------------------------------------------------------------------------

void ImageSegRoiDialog::clear_all_rois()
{
	//_int_img_widget->clearAllRoiMasks();
	_int_img_widget->deleteAllItems();
}

//---------------------------------------------------------------------------

void ImageSegRoiDialog::setColorMap(QVector<QRgb>* selected_colormap)
{
	_selected_colormap = selected_colormap;
}

//---------------------------------------------------------------------------

bool ImageSegRoiDialog::_get_img(ArrayXXr<float> &int_img)
{
	bool first = true;

	for (int i = 0; i < _img_list_model->rowCount(); i++)
	{
		QStandardItem* item0 = _img_list_model->item(i);
		if (item0->checkState() == Qt::Checked && _img_data.count(item0->text()) > 0)
		{
			if (first)
			{
				int_img = _img_data.at(item0->text());
				first = false;
			}
			else
			{
				int_img += _img_data.at(item0->text());
			}
		}
	}
	return !first; 
}

//---------------------------------------------------------------------------

QImage ImageSegRoiDialog::_generate_img(ArrayXXr<float>& int_img)
{
	float counts_max = int_img.maxCoeff();
	float counts_min = int_img.minCoeff();
	int width = static_cast<int>(int_img.cols());
	int height = static_cast<int>(int_img.rows());
	QImage image(width, height, QImage::Format_Indexed8);
	image.setColorTable(*_selected_colormap);

	float max_min = counts_max - counts_min;
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			//first clamp the data to max min
			float cnts = int_img(row, col);
			cnts = std::min(counts_max, cnts);
			cnts = std::max(counts_min, cnts);
			//convert to pixel
			byte data = static_cast<byte>(((cnts - counts_min) / max_min) * 255.0);
			image.setPixel(col, row, (uint)data);
		}
	}
	return image;
}

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

void ImageSegRoiDialog::onImgSelection(QStandardItem* item) 
{
	ArrayXXr<float> int_img;
	bool is_checked = _get_img(int_img);

	_runBtn->setEnabled(is_checked);

	if (is_checked)
	{
		QImage image = _generate_img(int_img);

		_int_img_widget->setPixMap(QPixmap::fromImage(image.convertToFormat(QImage::Format_RGB32)));
	}
	else if (false == _image_size.isEmpty())
	{
		clear_image();
	}
}

//---------------------------------------------------------------------------

void ImageSegRoiDialog::clear_image()
{
	QImage image(_image_size, QImage::Format_ARGB32_Premultiplied);
	image.fill(QColor(Qt::gray));
	_int_img_widget->setPixMap(QPixmap::fromImage(image.convertToFormat(QImage::Format_RGB32)));
}

//---------------------------------------------------------------------------