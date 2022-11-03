/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ImageSegRoiDialog.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QGridLayout>

//---------------------------------------------------------------------------
static const QString STR_KMEANS = QString("KMeans");

//---------------------------------------------------------------------------

ImageSegRoiDialog::ImageSegRoiDialog() : QDialog()
{

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

	QLabel* label_nfeatures = new QLabel("Number of Features:");
	_km_nfeatures = new QLineEdit();
	_km_nfeatures->setValidator(new QIntValidator(0, 10, this));
	_km_nfeatures->setText("2");

	QHBoxLayout* hlayout = new QHBoxLayout();
	hlayout->addWidget(label_nfeatures);
	hlayout->addWidget(_km_nfeatures);
	layout->addItem(hlayout);

	QLabel* label_Term = new QLabel("Term Criteria:");
	_km_TermCriteria = new QComboBox();
	_km_TermCriteria->addItem("COUNT");
	_km_TermCriteria->addItem("EPS"); 
	_km_TermCriteria->addItem("COUNT + EPS");
	_km_TermCriteria->setCurrentIndex(2); // default to EPS
//	_km_le_EPS->setText(cv::TermCriteria::EPS);

	hlayout = new QHBoxLayout();
	hlayout->addWidget(label_Term);
	hlayout->addWidget(_km_TermCriteria);
	layout->addItem(hlayout);

	QLabel* label_max_iter = new QLabel("MAX_Iter");
	_km_le_MAX_ITER = new QLineEdit();
	_km_le_MAX_ITER->setValidator(new QIntValidator(0, 10000, this));
	_km_le_MAX_ITER->setText("10");

	hlayout = new QHBoxLayout();
	hlayout->addWidget(label_max_iter);
	hlayout->addWidget(_km_le_MAX_ITER);
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
	if(_cb_tech->currentText() == STR_KMEANS)
	{
		//cv::kmeans(data, nfeatures, 
		//cv::TermCriteria::MAX_ITER
		// 
		//criteria = (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, 10, 1.0)
		// Set flags(Just to avoid line break in the code)
		//flags = cv.KMEANS_RANDOM_CENTERS
		//compactness, labels, centers = cv.kmeans(z, nfeatures, None, criteria, 10, flags)
	}
	// emit list of roi's
	//emit onNewROIs();
	//close();
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

void ImageSegRoiDialog::onImgSelection(QStandardItem* item) 
{
	ArrayXXr<float> int_img;
	bool first = true;

	for (int i = 0; i < _img_list_model->rowCount(); i++)
	{
		QStandardItem* item0 = _img_list_model->item(i);
		if(item0->checkState() == Qt::Checked && _img_data.count(item0->text()) > 0)
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

	if (first == false)
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

		_int_img_widget->setPixMap(QPixmap::fromImage(image.convertToFormat(QImage::Format_RGB32)));
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------