/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/RoiStatisticsWidget.h>
#include <QDesktopServices>
#include <QUrl>
#include <QGuiApplication>
#include <QScreen>

//---------------------------------------------------------------------------
enum HEADER_NAME { ROI_Name=0, MapName=1, SumCnts=2, MeanCts=3, MedianCts=4, StdDevCts=5, SumUgcm2=6, MeanUg=7, MedianUg=8, StdDevUg=9, Area=10, MinCnts=11, MinPixel=12, MaxCnts=13, MaxPixel=14, Min_ugcm2=15, Max_ugcm2=16, Num_Spectra=17, TotalConcentrationCts=18, TotalConcentrationUg=19, TotalContentUg=20 };
//---------------------------------------------------------------------------

RoiStatisticsWidget::RoiStatisticsWidget() : QWidget()
{
	_base_header = { "ROI Name", "Map Name", "Sum Cnt/s",  "Mean Cts/s", "Median Cts/s", "Std Dev Cts/s", "Sum ug/cm2", "Mean ug/cm2", "Median ug/cm2", "Std Dev ug/cm2", "Area", "Min Cnts/s", "Mix Pixel(x|y)", "Max Cnts/s", "Max Pixel (x|y)", "Min ug/cm2", "Max ug/cm2", "Num Spectra", "Total Concentration Cts/s (mean x # of pixels)", "Total Concentration ug/cm2", "Total Content ug/cm2 (mean x scan area with unit of femtogram 10^-15)" }; 

	gen_insert_order_lists(_element_lines_list, _scalers_to_add_first_list, _final_counts_to_add_before_scalers);

    _createLayout();

}

//---------------------------------------------------------------------------

RoiStatisticsWidget::~RoiStatisticsWidget()
{

}

//---------------------------------------------------------------------------

void RoiStatisticsWidget::_insert_item(QString roiName, 
										QString imgName,
										float sq_area,
										const data_struct::ArrayXXr<float>& img,
										const std::vector<std::pair<int, int>>& roi_pixels,
										int i,
										const data_struct::ArrayXXr<float>* normalizer,
										const Calibration_curve<double>* calib_curve)
{
	_table_widget->setItem(i, ROI_Name, new QTableWidgetItem(roiName));
	_table_widget->setItem(i, MapName, new QTableWidgetItem(imgName));
	double sum_cts = 0.0;
	double min_cts = std::numeric_limits<float>::max();
	double max_cts = std::numeric_limits<float>::min();
	double std_dev_cts = 0.0;
	double median_cts = 0.0;
	double sum_ugcm2 = 0.0;
	double min_ug = std::numeric_limits<float>::max();
	double max_ug = std::numeric_limits<float>::min();
	double std_dev_ug = 0.0;
	double median_ug = 0.0;
	double area = sq_area * roi_pixels.size();
	std::pair<int, int> min_pixel;
	std::pair<int, int> max_pixel;
	Eigen::ArrayXf cts_arr(roi_pixels.size());
	Eigen::ArrayXf ug_arr(roi_pixels.size());
	int j = 0;
	double calib_val = 1.0;
	bool hasNorm = false;
	if (calib_curve != nullptr && normalizer != nullptr)
    {
		if( calib_curve->calib_curve.count(imgName.toStdString()) > 0 )
		{
    		calib_val = static_cast<float>(calib_curve->calib_curve.at(imgName.toStdString()));
			hasNorm = true;
		}
	}

	for(auto pitr : roi_pixels)
	{
		double val = img(pitr.second, pitr.first);
		sum_cts += val;
		min_cts = std::min(val,min_cts);
		if(min_cts == val)
		{
			min_pixel = pitr;
		}
		max_cts = std::max(val,max_cts);
		if(max_cts == val)
		{
			max_pixel = pitr;
		}
		cts_arr[j] = val;
		if(hasNorm)
		{
			double ug = (val / (*normalizer)(pitr.second, pitr.first) / calib_val);
			sum_ugcm2 += ug;
			min_ug = std::min(ug,min_ug);
			max_ug = std::max(ug,max_ug);
			ug_arr[j] = ug;
		}
		j++;
	}
	double mean_cts = sum_cts / (double)roi_pixels.size();
	double mean_ug = sum_ugcm2 / (double)roi_pixels.size();
	for(auto pitr : roi_pixels)
	{
		double val = img(pitr.second, pitr.first);
		std_dev_cts += pow((val - mean_cts), 2);
		if(hasNorm)
		{
			double ug = (val / (*normalizer)(pitr.second, pitr.first) / calib_val);
			std_dev_ug +=  pow((ug - mean_ug), 2);
		}
	}
	std_dev_cts = sqrtf(std_dev_cts / (double)roi_pixels.size());
	if(hasNorm)
	{
		std_dev_ug = sqrtf(std_dev_ug / (double)roi_pixels.size());
		std::sort(ug_arr.begin(), ug_arr.end(), [](float const& t1, float const& t2) { return t1 < t2; });
	    int idx = ug_arr.size() / 2;
   		median_ug = ug_arr(idx);
	}
	
	std::sort(cts_arr.begin(), cts_arr.end(), [](float const& t1, float const& t2) { return t1 < t2; });
    int idx = cts_arr.size() / 2;
    median_cts = cts_arr(idx);
	
	QString str_min_pixel = QString::number(min_pixel.second) + " | " + QString::number(min_pixel.first);
	QString str_max_pixel = QString::number(max_pixel.second) + " | " + QString::number(max_pixel.first);
	_table_widget->setItem(i, SumCnts, new QTableWidgetItem(QString::number(sum_cts)));
	_table_widget->setItem(i, MinCnts, new QTableWidgetItem(QString::number(min_cts)));
	_table_widget->setItem(i, MinPixel, new QTableWidgetItem(str_min_pixel));
	_table_widget->setItem(i, MaxCnts, new QTableWidgetItem(QString::number(max_cts)));
	_table_widget->setItem(i, MaxPixel, new QTableWidgetItem(str_max_pixel));
	_table_widget->setItem(i, MeanCts, new QTableWidgetItem(QString::number(mean_cts)));
	_table_widget->setItem(i, MedianCts, new QTableWidgetItem(QString::number(median_cts)));
	_table_widget->setItem(i, StdDevCts, new QTableWidgetItem(QString::number(std_dev_cts)));
	_table_widget->setItem(i, Num_Spectra, new QTableWidgetItem(QString::number(roi_pixels.size())));

	_table_widget->setItem(i, Area, new QTableWidgetItem(QString::number(area)));
	_table_widget->setItem(i, TotalConcentrationCts, new QTableWidgetItem(QString::number(mean_cts * (double)roi_pixels.size())));

	if(hasNorm)
	{
		_table_widget->setItem(i, SumUgcm2, new QTableWidgetItem(QString::number(sum_ugcm2)));
		_table_widget->setItem(i, Min_ugcm2, new QTableWidgetItem(QString::number(min_ug)));
		_table_widget->setItem(i, Max_ugcm2, new QTableWidgetItem(QString::number(max_ug)));
		_table_widget->setItem(i, MeanUg, new QTableWidgetItem(QString::number(mean_ug)));
		_table_widget->setItem(i, MedianUg, new QTableWidgetItem(QString::number(median_ug)));
		_table_widget->setItem(i, StdDevUg, new QTableWidgetItem(QString::number(std_dev_ug)));
		_table_widget->setItem(i, TotalContentUg, new QTableWidgetItem(QString::number(area*mean_ug)));
		_table_widget->setItem(i, TotalConcentrationUg, new QTableWidgetItem(QString::number(mean_ug * (double)roi_pixels.size())));
	}
	else
	{
		_table_widget->setItem(i, SumUgcm2, new QTableWidgetItem(" "));
		_table_widget->setItem(i, Min_ugcm2, new QTableWidgetItem(" "));
		_table_widget->setItem(i, Max_ugcm2, new QTableWidgetItem(" "));
		_table_widget->setItem(i, MeanUg, new QTableWidgetItem(" "));
		_table_widget->setItem(i, MedianUg, new QTableWidgetItem(" "));
		_table_widget->setItem(i, StdDevUg, new QTableWidgetItem(" "));
	}
}

//---------------------------------------------------------------------------

void RoiStatisticsWidget::setData(QDir model_dir,
								QString dataset_name,
								QString fitting_name, 
								QString normalizer_name,
								float sq_area,
								std::unordered_map<std::string, data_struct::ArrayXXr<float>>& img_data, 
								std::vector<gstar::RoiMaskGraphicsItem*>& roi_list,
                     			const data_struct::ArrayXXr<float>* normalizer,
                     			const Calibration_curve<double>* calib_curve)
{

	int total = img_data.size() * roi_list.size();
	_table_widget->setRowCount(total);

	std::unordered_map<std::string, int> done_map;

	_export_dir = model_dir;
    _export_dir.cdUp();
    _export_dir.cdUp();
    _export_dir.cd("output");


	QString filename = "roi_export_" + dataset_name + "_" + fitting_name + "_";
	if(normalizer != nullptr)
	{
		filename += normalizer_name + "_";
	}
	
	QDateTime date = QDateTime::currentDateTime();
    QString formattedTime = date.toString("yyyy_MM_dd_hh_mm_ss");
	filename += formattedTime + ".csv";

	_str_export_filename = _export_dir.absolutePath() + QDir::separator() + filename;

	int i = 0;
	for (auto roi_itr : roi_list)
	{
		std::vector<std::pair<int, int>> roi_pixels;
		roi_itr->to_roi_vec(roi_pixels);
		// insert in z order
		for (auto itr : _element_lines_list)
		{
			if (img_data.count(itr) > 0)
			{
				_insert_item(roi_itr->getName(), QString::fromLatin1(itr.c_str(), itr.length()), sq_area, img_data.at(itr), roi_pixels, i, normalizer, calib_curve);				
				done_map[itr] = 1;
				i++;
			}

		}
		// add end of element list that are not elements
		for (auto itr : _final_counts_to_add_before_scalers)
		{
			if (img_data.count(itr) > 0)
			{
				_insert_item(roi_itr->getName(), QString::fromLatin1(itr.c_str(), itr.length()), sq_area, img_data.at(itr), roi_pixels, i, normalizer, calib_curve);
				done_map[itr] = 1;
				i++;
			}
		}
		// add scalers in certain order
		for (auto itr : _scalers_to_add_first_list)
		{
			if (img_data.count(itr) > 0)
			{
				_insert_item(roi_itr->getName(), QString::fromLatin1(itr.c_str(), itr.length()),sq_area, img_data.at(itr), roi_pixels, i, normalizer, calib_curve);
				done_map[itr] = 1;
				i++;
			}
		}
		// add rest of scalers
		for (auto itr : img_data)
		{
			if(done_map.count(itr.first) == 0)
			{
				_insert_item(roi_itr->getName(), QString::fromLatin1(itr.first.c_str(), itr.first.length()), sq_area, itr.second, roi_pixels, i, normalizer, calib_curve);
				i++;
			}
		}
	}
}

//---------------------------------------------------------------------------

void RoiStatisticsWidget::_createLayout()
{

	_table_widget = new QTableWidget(1, _base_header.count());
	_table_widget->setHorizontalHeaderLabels(_base_header);

	_btn_export = new QPushButton("Export");
	_btn_close = new QPushButton("Close");
	connect(_btn_export, &QPushButton::pressed, this, &RoiStatisticsWidget::onExport);
	connect(_btn_close, &QPushButton::pressed, this, &RoiStatisticsWidget::onClose);
	
	QVBoxLayout* mainLayout = new QVBoxLayout;
	QHBoxLayout* buttonLayout = new QHBoxLayout;

	buttonLayout->addWidget(_btn_export);
	buttonLayout->addWidget(_btn_close);

	mainLayout->addWidget(_table_widget);
	mainLayout->addItem(buttonLayout);

	setLayout(mainLayout);

	setWindowTitle("ROI Stats");

	QRect rec = QGuiApplication::screens()[0]->geometry();
	int height1 = rec.height() - 20;
	int width1 = rec.width() - 20;
	resize(width1,height1);
	
}

//---------------------------------------------------------------------------

void RoiStatisticsWidget::onExport()
{
	QFile file(_str_export_filename);
    if (!file.open(QIODevice::WriteOnly)) 
	{
        logE<< "Could not open file for writing:"<< _str_export_filename.toStdString()<<"\n";
        return;
    }

    QTextStream out(&file);

    // Write header row
    QStringList headers = _base_header;
    out << headers.join(",") << "\n";

    // Write data rows
    for (int i = 0; i < _table_widget->rowCount(); ++i) 
	{
        QStringList rowData;
        for (int j = 0; j < _table_widget->columnCount(); ++j) 
		{
            QTableWidgetItem* item = _table_widget->item(i, j);
            if (item) 
			{
                rowData.append(item->text());
            }
			else 
			{
                rowData.append(""); 
            }
        }
        out << rowData.join(",") << "\n";
    }

    file.close();

	if (false == QDesktopServices::openUrl(QUrl::fromLocalFile(_export_dir.absolutePath())))
	{
		logW << "Failed to open dir " << _export_dir.absolutePath().toStdString() << "\n";
	}
	close();
}

//---------------------------------------------------------------------------

void RoiStatisticsWidget::onClose()
{
	clear_all();
	close();
}

//---------------------------------------------------------------------------

void RoiStatisticsWidget::clear_all()
{
	_table_widget->setRowCount(0);
	_str_export_filename = "";
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------