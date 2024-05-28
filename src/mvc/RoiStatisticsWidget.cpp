/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/RoiStatisticsWidget.h>

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

RoiStatisticsWidget::RoiStatisticsWidget() : QWidget()
{
	_base_header = { "ROI Name", "Element", "Cnt/s", "Min Cnts/s", "Max Cnts/s", "ug/cm2", "Min ug/cm2", "Max ug/cm2", "Mean", "Median", "Std Dev", "Area", "Num Spectra" }; 

	//create save ordered vector by element Z number with K , L, M lines
	for (std::string el_name : data_struct::Element_Symbols)
	{
		_element_lines_list.push_back(el_name);
	}
	for (std::string el_name : data_struct::Element_Symbols)
	{
		_element_lines_list.push_back(el_name + "_L");
	}
	for (std::string el_name : data_struct::Element_Symbols)
	{
		_element_lines_list.push_back(el_name + "_M");
	}

	_final_counts_to_add_before_scalers.push_back(STR_COHERENT_SCT_AMPLITUDE);
	_final_counts_to_add_before_scalers.push_back(STR_COMPTON_AMPLITUDE);
	_final_counts_to_add_before_scalers.push_back(STR_SUM_ELASTIC_INELASTIC_AMP);
	_final_counts_to_add_before_scalers.push_back(STR_TOTAL_FLUORESCENCE_YIELD);
	_final_counts_to_add_before_scalers.push_back(STR_NUM_ITR);
	_final_counts_to_add_before_scalers.push_back(STR_RESIDUAL);

	_scalers_to_add_first_list.push_back(STR_SR_CURRENT);
	_scalers_to_add_first_list.push_back(STR_US_IC);
	_scalers_to_add_first_list.push_back(STR_DS_IC);
	_scalers_to_add_first_list.push_back(STR_ELT);
	_scalers_to_add_first_list.push_back(STR_ELAPSED_LIVE_TIME);
	_scalers_to_add_first_list.push_back(STR_ERT);
	_scalers_to_add_first_list.push_back(STR_ELAPSED_REAL_TIME);
	_scalers_to_add_first_list.push_back(STR_INPUT_COUNTS);
	_scalers_to_add_first_list.push_back(STR_ICR);
	_scalers_to_add_first_list.push_back("INCNT");
	_scalers_to_add_first_list.push_back(STR_OUTPUT_COUNTS);
	_scalers_to_add_first_list.push_back(STR_OCR);
	_scalers_to_add_first_list.push_back("OUTCNT");
	_scalers_to_add_first_list.push_back(STR_DEAD_TIME);
	_scalers_to_add_first_list.push_back("abs_cfg");
	_scalers_to_add_first_list.push_back("abs_ic");
	_scalers_to_add_first_list.push_back("H_dpc_cfg");
	_scalers_to_add_first_list.push_back("V_dpc_cfg");
	_scalers_to_add_first_list.push_back("DPC1_IC");
	_scalers_to_add_first_list.push_back("DPC2_IC");
	_scalers_to_add_first_list.push_back("dia1_dpc_cfg");
	_scalers_to_add_first_list.push_back("dia2_dpc_cfg");
	_scalers_to_add_first_list.push_back("CFG_1");
	_scalers_to_add_first_list.push_back(STR_CFG_2);
	_scalers_to_add_first_list.push_back(STR_CFG_3);
	_scalers_to_add_first_list.push_back(STR_CFG_4);
	_scalers_to_add_first_list.push_back(STR_CFG_5);
	_scalers_to_add_first_list.push_back("CFG_6");
	_scalers_to_add_first_list.push_back("CFG_7");
	_scalers_to_add_first_list.push_back("CFG_8");
	_scalers_to_add_first_list.push_back("CFG_9");


    _createLayout();

}

//---------------------------------------------------------------------------

RoiStatisticsWidget::~RoiStatisticsWidget()
{

}

//---------------------------------------------------------------------------

void RoiStatisticsWidget::setData(std::unordered_map<std::string, data_struct::ArrayXXr<float>>& img_data, std::vector<gstar::RoiMaskGraphicsItem*>& roi_list)
{

	int total = img_data.size() * roi_list.size();
	_table_widget->setRowCount(total);

	std::unordered_map<std::string, int> done_map;

	int i = 0;
	for (auto roi_itr : roi_list)
	{
		/*
		_table_widget->setItem(i, 0, new QTableWidgetItem(QString::fromLatin1(itr.name.c_str(), itr.name.length())));
		_table_widget->setItem(i, 1, new QTableWidgetItem());
		_table_widget->setItem(i, 2, new QTableWidgetItem(QString::fromLatin1(itr.unit.c_str(), itr.unit.length())));
		_table_widget->setItem(i, 3, new QTableWidgetItem(QString::fromLatin1(itr.description.c_str(), itr.description.length())));
		*/

		// insert in z order
		for (auto itr : _element_lines_list)
		{
			if (img_data.count(itr) > 0)
			{
				_table_widget->setItem(i, 0, new QTableWidgetItem(roi_itr->getName()));
				_table_widget->setItem(i, 1, new QTableWidgetItem(QString::fromLatin1(itr.c_str(), itr.length())));
		
				/*
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
				*/
				done_map[itr] = 1;
				i++;
			}

		}
		
	
		// add end of element list that are not elements
		for (auto itr : _final_counts_to_add_before_scalers)
		{
			if (img_data.count(itr) > 0)
			{
				_table_widget->setItem(i, 0, new QTableWidgetItem(roi_itr->getName()));
				_table_widget->setItem(i, 1, new QTableWidgetItem(QString::fromLatin1(itr.c_str(), itr.length())));
		
				/*
				_img_data[QString(itr.c_str())] = img_data.at(itr);
				QStandardItem* item0 = new QStandardItem(false);
				item0->setCheckable(true);
				item0->setCheckState(Qt::Unchecked);
				item0->setText(QString(itr.c_str()));
				_img_list_model->appendRow(item0);

				img_data.erase(itr);
				*/
				done_map[itr] = 1;
				i++;
			}
		}

		// add scalers in certain order
		for (auto itr : _scalers_to_add_first_list)
		{
			if (img_data.count(itr) > 0)
			{
				_table_widget->setItem(i, 0, new QTableWidgetItem(roi_itr->getName()));
				_table_widget->setItem(i, 1, new QTableWidgetItem(QString::fromLatin1(itr.c_str(), itr.length())));
		
				/*
				_img_data[QString(itr.c_str())] = img_data.at(itr);
				QStandardItem* item0 = new QStandardItem(false);
				item0->setCheckable(true);
				item0->setCheckState(Qt::Unchecked);
				item0->setText(QString(itr.c_str()));
				_img_list_model->appendRow(item0);

				img_data.erase(itr);
				*/
				done_map[itr] = 1;
				i++;
			}
		}

		// add rest of scalers
		for (auto itr : img_data)
		{
			if(done_map.count(itr.first) == 0)
			{
				_table_widget->setItem(i, 0, new QTableWidgetItem(roi_itr->getName()));
				_table_widget->setItem(i, 1, new QTableWidgetItem(QString::fromLatin1(itr.first.c_str(), itr.first.length())));
		
				/*
				_img_data[QString(itr.first.c_str())] = img_data.at(itr.first);
				QStandardItem* item0 = new QStandardItem(false);
				item0->setCheckable(true);
				item0->setCheckState(Qt::Unchecked);
				item0->setText(QString(itr.first.c_str()));
				_img_list_model->appendRow(item0);
				*/
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
}

//---------------------------------------------------------------------------

void RoiStatisticsWidget::onExport()
{

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
	//_table_widget->clear();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------