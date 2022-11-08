/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ImageSegRoiDialog.h>


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
	for (auto& itr : img_data)
	{
		_img_data[QString(itr.first.c_str())] = itr.second;
		QStandardItem* item0 = new QStandardItem(false);
		item0->setCheckable(true);
		item0->setCheckState(Qt::Unchecked);
		item0->setText(QString(itr.first.c_str()));
		_img_list_model->appendRow(item0);
	}
	
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

	runBtn = new QPushButton("Run");
	runBtn->setEnabled(false);
	acceptBtn = new QPushButton("Accept");
	acceptBtn->setEnabled(false);
	cancelBtn = new QPushButton("Cancel");
	connect(runBtn, SIGNAL(pressed()), this, SLOT(onRun()));
	connect(acceptBtn, SIGNAL(pressed()), this, SLOT(onAccept()));
	connect(cancelBtn, SIGNAL(pressed()), this, SLOT(close()));

	_img_list_model = new QStandardItemModel();
	_img_names_view = new QListView();
	_img_names_view->setModel(_img_list_model);
	_img_names_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_img_names_view->setSelectionMode(QAbstractItemView::ExtendedSelection);

	_roi_list_model = new QStandardItemModel();
	_roi_names_view = new QListView();
	_roi_names_view->setModel(_roi_list_model);
	//_roi_names_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_roi_names_view->setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(_img_list_model, &QStandardItemModel::itemChanged, this, &ImageSegRoiDialog::onImgSelection);

	_int_img_widget = new ImageSegWidget();

	QHBoxLayout* mainLayout = new QHBoxLayout;
	QVBoxLayout *leftLayout = new QVBoxLayout;
	QHBoxLayout* optionLayout = new QHBoxLayout;
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	QVBoxLayout* rightLayout = new QVBoxLayout;

	optionLayout->addWidget(_techLabel);
	optionLayout->addWidget(_cb_tech);

	buttonLayout->addWidget(runBtn);
	buttonLayout->addWidget(acceptBtn);
	buttonLayout->addWidget(cancelBtn);
	
	leftLayout->addWidget(_img_names_view);
	leftLayout->addItem(optionLayout);
	leftLayout->addItem(_techLayout);
	leftLayout->addItem(buttonLayout);

	rightLayout->addWidget(_roi_names_view);

	mainLayout->addItem(leftLayout);
	mainLayout->addWidget(_int_img_widget);
	mainLayout->addItem(rightLayout);

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
		

		_int_img_widget->setPixMap(QPixmap::fromImage(_generate_sum_image(new_labels, int_img)));
		/*
		std::vector<QImage> images = _generate_images(clusterCount, new_labels);
		if (images.size() > 0)
		{
			_int_img_widget->setPixMap(QPixmap::fromImage(images[0]));
		}
		*/
	}

}

//---------------------------------------------------------------------------

void ImageSegRoiDialog::onAccept()
{
	// emit list of roi's
	//emit onNewROIs();
	close();
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

	QImage background = _generate_img(bg_img);
	background.convertTo(QImage::Format_ARGB32_Premultiplied);

	QImage overlay(mat.cols, mat.rows, QImage::Format_RGB32);

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

	// Overlay the two 
	//QImage surface(mat.cols, mat.rows, QImage::Format_ARGB32_Premultiplied);
	//QPainter p(&surface);
//	p.setCompositionMode(QPainter::CompositionMode_SourceOver);
//	p.drawImage(0, 0, background);

	QPainter p(&background);
	/*
	if (alpha < 255)
	{
		p.setCompositionMode(QPainter::CompositionMode_Overlay);
	}
	else
	{
		p.setCompositionMode(QPainter::CompositionMode_SourceOver);
	}*/
	p.setCompositionMode(QPainter::CompositionMode_SourceOver);
	p.drawImage(0, 0, overlay);
	p.end();
	
	return background;
}

//---------------------------------------------------------------------------

void ImageSegRoiDialog::onImgSelection(QStandardItem* item) 
{
	ArrayXXr<float> int_img;
	bool is_checked = _get_img(int_img);

	runBtn->setEnabled(is_checked);

	if (is_checked)
	{
		QImage image = _generate_img(int_img);

		_int_img_widget->setPixMap(QPixmap::fromImage(image.convertToFormat(QImage::Format_RGB32)));
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------