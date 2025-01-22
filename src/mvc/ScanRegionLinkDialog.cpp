/*-----------------------------------------------------------------------------
 * Copyright (c) 2025, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ScanRegionLinkDialog.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QGridLayout>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

ScanRegionLinkDialog::ScanRegionLinkDialog() : QDialog()
{
	_scan_region_dialog = nullptr;
    _createLayout();

}

//---------------------------------------------------------------------------

ScanRegionLinkDialog::~ScanRegionLinkDialog()
{

}

//---------------------------------------------------------------------------

void ScanRegionLinkDialog::_createLayout()
{

	_scan_region_dialog = new ScanRegionDialog();

	_btn_save = new QPushButton("Save");
	_btn_cancel = new QPushButton("Cancel");
	connect(_btn_save, &QPushButton::pressed, this, &ScanRegionLinkDialog::onSave);
	connect(_btn_cancel, &QPushButton::pressed, this, &ScanRegionLinkDialog::close);

	QVBoxLayout* main_layout = new QVBoxLayout();

	QHBoxLayout* button_layout = new QHBoxLayout();
	button_layout->addWidget(_btn_save);
	button_layout->addWidget(_btn_cancel);

	main_layout->addWidget(_scan_region_dialog);
	main_layout->addItem(button_layout);

	setLayout(main_layout);

	setWindowTitle("Scan Region");
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
