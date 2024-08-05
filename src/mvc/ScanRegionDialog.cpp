/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ScanRegionDialog.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QGridLayout>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

ScanRegionDialog::ScanRegionDialog() : QDialog()
{
	_avail_scans = nullptr;
    _createLayout();

}

//---------------------------------------------------------------------------

ScanRegionDialog::~ScanRegionDialog()
{

}

//---------------------------------------------------------------------------

void ScanRegionDialog::_createLayout()
{

	_scan_name = new QLineEdit(" ");
	
	_scan_type = new QComboBox();
	connect(_scan_type, &QComboBox::currentTextChanged, this, &ScanRegionDialog::scanChanged);

	_scan_table_model = new ScanTableModel();
	_scan_options = new QTableView();
	_scan_options->setModel(_scan_table_model);

	_btn_update = new QPushButton("Update");
	_btn_update_and_queue = new QPushButton("Update and Queue");
	_btn_cancel = new QPushButton("Cancel");
	connect(_btn_update, &QPushButton::pressed, this, &ScanRegionDialog::onUpdate);
	connect(_btn_update_and_queue, &QPushButton::pressed, this, &ScanRegionDialog::onUpdateAndQueue);
	connect(_btn_cancel, &QPushButton::pressed, this, &ScanRegionDialog::close);

	QVBoxLayout* main_layout = new QVBoxLayout();
	
	QHBoxLayout* name_layout = new QHBoxLayout();
	name_layout->addWidget(new QLabel("Scan Name: "));
	name_layout->addWidget(_scan_name);

	QHBoxLayout* type_layout = new QHBoxLayout();
	type_layout->addWidget(new QLabel("Scan Type: "));
	type_layout->addWidget(_scan_type);

	QHBoxLayout* button_layout = new QHBoxLayout();
	button_layout->addWidget(_btn_update);
	button_layout->addWidget(_btn_update_and_queue);
	button_layout->addWidget(_btn_cancel);


	main_layout->addItem(name_layout);
	main_layout->addItem(type_layout);
	main_layout->addWidget(_scan_options);
	main_layout->addItem(button_layout);

	setLayout(main_layout);

	setWindowTitle("Scan Region");
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

void ScanRegionDialog::onUpdate()
{
	emit ScanUpdated();
	close();
}

//---------------------------------------------------------------------------

void ScanRegionDialog::onUpdateAndQueue()
{
	emit ScanUpdated();
	close();
}

//---------------------------------------------------------------------------

void ScanRegionDialog::scanChanged(const QString &scan_name)
{
	if(_avail_scans != nullptr)
	{
		if(_avail_scans->count(scan_name) > 0)
		{
			_scan_table_model->setAllData(_avail_scans->at(scan_name));
		}
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
