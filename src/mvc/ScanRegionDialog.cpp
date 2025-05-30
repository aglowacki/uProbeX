/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ScanRegionDialog.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QGridLayout>
#include <QHeaderView>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ScanRegionDialog::ScanRegionDialog() : QDialog()
{
	_emit_on_accept = true;
	_createLayout();
}

//---------------------------------------------------------------------------

ScanRegionDialog::~ScanRegionDialog()
{
	if (_cbDelegate != nullptr)
	{
		delete _cbDelegate;
	}
}

//---------------------------------------------------------------------------

void ScanRegionDialog::_createLayout()
{
	_cbDelegate = new ComboBoxBoolDelegate();
	_cbDelegate->setCustomCol(1);
	_avail_scans = nullptr;

	_scan_name = new QLineEdit(" ");
	_lbl_region_name = new QLabel("Region Name: ");
	_scan_type = new QComboBox();
	connect(_scan_type, &QComboBox::currentTextChanged, this, &ScanRegionDialog::scanChanged);

	_scan_table_model = new ScanTableModel();
	_scan_options = new QTableView();
	_scan_options->setModel(_scan_table_model);
	_scan_options->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	_scan_options->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	_scan_options->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	

	_chk_batch_scan = new QCheckBox("Set Batch Scan");
	connect(_chk_batch_scan, &QCheckBox::stateChanged, this, &ScanRegionDialog::onBatchScanChanged);
	
	QLabel* startLabel = new QLabel("Start Value");
	QLabel* endLabel = new QLabel("End Value");
	QLabel* numItrLabel = new QLabel("Number Iter");

	_cb_batch_prop = new QComboBox();
	_batch_start = new QLineEdit("1");
	_batch_end = new QLineEdit("10");
	_batch_num = new QLineEdit("10");
	
	_cb_batch_prop->setEnabled(false);
	_batch_start->setEnabled(false);
	_batch_end->setEnabled(false);
	_batch_num->setEnabled(false);

	_btn_update_and_queue = new QPushButton("Add To Queue");
	_btn_cancel = new QPushButton("Cancel");
	connect(_btn_update_and_queue, &QPushButton::pressed, this, &ScanRegionDialog::onUpdateAndQueue);
	connect(_btn_cancel, &QPushButton::pressed, this, &ScanRegionDialog::close);

	QVBoxLayout* main_layout = new QVBoxLayout();
	
	QHBoxLayout* name_layout = new QHBoxLayout();
	name_layout->addWidget(_lbl_region_name);
	name_layout->addWidget(_scan_name);

	QHBoxLayout* type_layout = new QHBoxLayout();
	type_layout->addWidget(new QLabel("Scan Type: "));
	type_layout->addWidget(_scan_type);

	QGridLayout* batch_layout = new QGridLayout();
	batch_layout->addWidget(_chk_batch_scan, 0, 0);
	batch_layout->addWidget(startLabel, 0, 1);
	batch_layout->addWidget(endLabel, 0 , 2);
	batch_layout->addWidget(numItrLabel, 0, 3);

	batch_layout->addWidget(_cb_batch_prop, 1, 0);
	batch_layout->addWidget(_batch_start, 1, 1);
	batch_layout->addWidget(_batch_end, 1, 2);
	batch_layout->addWidget(_batch_num, 1, 3);


	QHBoxLayout* button_layout = new QHBoxLayout();
	button_layout->addWidget(_btn_update_and_queue);
	button_layout->addWidget(_btn_cancel);

	main_layout->addItem(name_layout);
	main_layout->addItem(type_layout);
	main_layout->addWidget(_scan_options);
	main_layout->addItem(batch_layout);
	main_layout->addItem(button_layout);
	setLayout(main_layout);

	setWindowTitle("Scan");
}

//---------------------------------------------------------------------------

void ScanRegionDialog::setAvailScans(std::map<QString, BlueskyPlan> * avail_scans) 
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

void ScanRegionDialog::updateProps(QList<gstar::AnnotationProperty*> &anno_list)
{
	for(auto &itr : anno_list)
	{
		if(itr->getName() == DEF_STR_DISPLAY_NAME)
		{
			_scan_name->setText(itr->getValue().toString());
		}
	}
}

//---------------------------------------------------------------------------

bool ScanRegionDialog::execAndReturnPlan(BlueskyPlan& plan)
{
	_emit_on_accept = false;
	if (this->exec() == QDialog::Accepted)
	{
		plan.type = _scan_type->currentText();
		_scan_table_model->getCurrentParams(plan);
		_emit_on_accept = true;
		return true;
	}
	_emit_on_accept = true;
	return false;
}

//---------------------------------------------------------------------------

void ScanRegionDialog::onUpdateAndQueue()
{
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
			if(_emit_on_accept)
			{
				emit ScanUpdated(plan);	
			}
			start += inc;
		}
	}
	else
	{
		BlueskyPlan plan;
		plan.type = _scan_type->currentText();
		_scan_table_model->getCurrentParams(plan);
		if(_emit_on_accept)
		{
			emit ScanUpdated(plan);
		}
	}
	accept();
}

//---------------------------------------------------------------------------

void ScanRegionDialog::scanChanged(const QString &scan_name)
{
	if(_avail_scans != nullptr)
	{
		if(_avail_scans->count(scan_name) > 0)
		{
			_cb_batch_prop->clear();
			const BlueskyPlan plan = _avail_scans->at(scan_name);
			_scan_table_model->setAllData(_avail_scans->at(scan_name));
			int idx = 0;
			for(auto itr: plan.parameters)
			{
				if(itr.default_val == "True" || itr.default_val == "False")
				{
					_scan_options->setItemDelegateForRow(idx, _cbDelegate);
				}
				else
				{
					// only add non strings
					if(itr.kind == BlueskyParamType::Double)
					{
						_cb_batch_prop->addItem(itr.name);
					}
					_scan_options->setItemDelegateForRow(idx, nullptr);
				}
				idx++;
			}
		}
	}
}

//---------------------------------------------------------------------------

void ScanRegionDialog::updatePlanDefaults(BlueskyPlan &plan)
{
	_scan_table_model->updateAllData(plan);
}

//---------------------------------------------------------------------------

void ScanRegionDialog::onBatchScanChanged(int val)
{
	auto state = _chk_batch_scan->checkState();

	if(state == Qt::Checked)
	{
		_cb_batch_prop->setEnabled(true);
		_batch_start->setEnabled(true);
		_batch_end->setEnabled(true);
		_batch_num->setEnabled(true);

	}
	else
	{
		_cb_batch_prop->setEnabled(false);
		_batch_start->setEnabled(false);
		_batch_end->setEnabled(false);
		_batch_num->setEnabled(false);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
