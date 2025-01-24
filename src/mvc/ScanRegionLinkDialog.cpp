/*-----------------------------------------------------------------------------
 * Copyright (c) 2025, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ScanRegionLinkDialog.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QGridLayout>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

ScanRegionLinkDialog::ScanRegionLinkDialog() : QDialog()
{

    _createLayout();

}

//---------------------------------------------------------------------------

ScanRegionLinkDialog::~ScanRegionLinkDialog()
{

}

//---------------------------------------------------------------------------

void ScanRegionLinkDialog::_createLayout()
{

	_scan_type = new QComboBox();

	connect(_scan_type, &QComboBox::currentTextChanged, this, &ScanRegionLinkDialog::onScanChanged);

	_cb_link_top_y = new QComboBox();
    _cb_link_left_x = new QComboBox();
    _cb_link_right_x = new QComboBox();
    _cb_link_bottom_y = new QComboBox();
    _cb_link_center_x = new QComboBox();
    _cb_link_center_y = new QComboBox();
    _cb_link_width = new QComboBox();
    _cb_link_height = new QComboBox();


	QGridLayout *gridlayout = new QGridLayout();
	gridlayout->addWidget(new QLabel("Scan Type"), 0, 0);
	gridlayout->addWidget(_scan_type, 0, 1);

	gridlayout->addWidget(new QLabel(" "), 1, 0);

	gridlayout->addWidget(new QLabel("Region Box Top Y"), 2, 0);
	gridlayout->addWidget(_cb_link_top_y, 2, 1);

	gridlayout->addWidget(new QLabel("Region Box Left X"), 3, 0);
	gridlayout->addWidget(_cb_link_left_x, 3, 1);

	gridlayout->addWidget(new QLabel("Region Box Right X"), 4, 0);
	gridlayout->addWidget(_cb_link_right_x, 4, 1);

	gridlayout->addWidget(new QLabel("Region Box Bottom Y"), 5, 0);
	gridlayout->addWidget(_cb_link_bottom_y, 5, 1);

	gridlayout->addWidget(new QLabel("Region Box Center X"), 6, 0);
	gridlayout->addWidget(_cb_link_center_x, 6, 1);

	gridlayout->addWidget(new QLabel("Region Box Center Y"), 7, 0);
	gridlayout->addWidget(_cb_link_center_y, 7, 1);

	gridlayout->addWidget(new QLabel("Region Box Width"), 8, 0);
	gridlayout->addWidget(_cb_link_width, 8, 1);

	gridlayout->addWidget(new QLabel("Region Box Height"), 9, 0);
	gridlayout->addWidget(_cb_link_height, 9, 1);

	_btn_save = new QPushButton("Save");
	_btn_cancel = new QPushButton("Cancel");
	connect(_btn_save, &QPushButton::pressed, this, &ScanRegionLinkDialog::onSave);
	connect(_btn_cancel, &QPushButton::pressed, this, &ScanRegionLinkDialog::close);

	QVBoxLayout* main_layout = new QVBoxLayout();

	QHBoxLayout* button_layout = new QHBoxLayout();
	button_layout->addWidget(_btn_save);
	button_layout->addWidget(_btn_cancel);

	main_layout->addItem(gridlayout);
	main_layout->addItem(button_layout);

	setLayout(main_layout);

	setWindowTitle("Scan Region Link");
}

//---------------------------------------------------------------------------

void ScanRegionLinkDialog::setAvailScans(std::map<QString, BlueskyPlan> * avail_scans) 
{
	 _avail_scans = avail_scans;
	 int i=0;
	 _scan_type->clear();
	 for(auto itr : *avail_scans)
	 {
		_scan_type->insertItem(i, itr.first);
		i++;
	 }
}

//---------------------------------------------------------------------------

void ScanRegionLinkDialog::onScanChanged(const QString &scan_name)
{
	if(_avail_scans != nullptr)
	{
		if(_avail_scans->count(scan_name) > 0)
		{
			const BlueskyPlan plan = _avail_scans->at(scan_name);
			_cb_link_top_y->clear();
			_cb_link_left_x->clear();
			_cb_link_right_x->clear();
			_cb_link_bottom_y->clear();
			_cb_link_center_x->clear();
			_cb_link_center_y->clear();
			_cb_link_width->clear();
			_cb_link_height->clear();

			_cb_link_top_y->addItem(" ");
			_cb_link_left_x->addItem(" ");
			_cb_link_right_x->addItem(" ");
			_cb_link_bottom_y->addItem(" ");
			_cb_link_center_x->addItem(" ");
			_cb_link_center_y->addItem(" ");
			_cb_link_width->addItem(" ");
			_cb_link_height->addItem(" ");

			for(auto itr: plan.parameters)
			{
				if(itr.kind == BlueskyParamType::Double)
				{
					_cb_link_top_y->addItem(itr.name);
					_cb_link_left_x->addItem(itr.name);
					_cb_link_right_x->addItem(itr.name);
					_cb_link_bottom_y->addItem(itr.name);
					_cb_link_center_x->addItem(itr.name);
					_cb_link_center_y->addItem(itr.name);
					_cb_link_width->addItem(itr.name);
					_cb_link_height->addItem(itr.name);
				}
			}
		}
	}
}

//---------------------------------------------------------------------------

void ScanRegionLinkDialog::onSave()
{
	/*
	if(_chk_batch_scan->checkState() == Qt::Checked)
	{
		float start = _batch_start->text().toFloat();
		float end = _batch_end->text().toFloat();
		int num = _batch_num->text().toInt();
		float inc = ( end - start )/ (float)(num - 1); 
		for(int i=0; i<num; i++)
		{
			BlueskyPlan plan;
			plan.type = _scan_type->currentText();
			_scan_table_model->getCurrentParams(plan);
			for(auto &itr : plan.parameters)
			{
				if(itr.name == _cb_batch_prop->currentText())
				{
					itr.default_val = QString::number(start);
				}
			}
			emit ScanUpdated(plan);	
			start += inc;
		}
	}
	else
	{
		BlueskyPlan plan;
		plan.type = _scan_type->currentText();
		_scan_table_model->getCurrentParams(plan);
		emit ScanUpdated(plan);
	}
	*/
	close();
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
