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
#include <QJsonObject>
#include "preferences/Preferences.h"

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

	_ck_show_all_props = new QCheckBox("Show All Properties");
	connect(_ck_show_all_props, &QCheckBox::stateChanged, this, &ScanRegionLinkDialog::onShowAllProps);

	_cb_link_top_y = new QComboBox();
    _cb_link_left_x = new QComboBox();
    _cb_link_right_x = new QComboBox();
    _cb_link_bottom_y = new QComboBox();
    _cb_link_center_x = new QComboBox();
    _cb_link_center_y = new QComboBox();
    _cb_link_width = new QComboBox();
    _cb_link_height = new QComboBox();


	QGridLayout *gridlayout = new QGridLayout();
	gridlayout->addWidget(new QLabel(STR_SCAN_TYPE), 0, 0);
	gridlayout->addWidget(_scan_type, 0, 1);

	gridlayout->addWidget(_ck_show_all_props, 1, 0);
	gridlayout->addWidget(new QLabel("   not just ones that are type Double"), 1, 1);

	gridlayout->addWidget(new QLabel(STR_Region_Box_Top_Y), 2, 0);
	gridlayout->addWidget(_cb_link_top_y, 2, 1);

	gridlayout->addWidget(new QLabel(STR_Region_Box_Left_X), 3, 0);
	gridlayout->addWidget(_cb_link_left_x, 3, 1);

	gridlayout->addWidget(new QLabel(STR_Region_Box_Right_X), 4, 0);
	gridlayout->addWidget(_cb_link_right_x, 4, 1);

	gridlayout->addWidget(new QLabel(STR_Region_Box_Bottom_Y), 5, 0);
	gridlayout->addWidget(_cb_link_bottom_y, 5, 1);

	gridlayout->addWidget(new QLabel(STR_Region_Box_Center_X), 6, 0);
	gridlayout->addWidget(_cb_link_center_x, 6, 1);

	gridlayout->addWidget(new QLabel(STR_Region_Box_Center_Y), 7, 0);
	gridlayout->addWidget(_cb_link_center_y, 7, 1);

	gridlayout->addWidget(new QLabel(STR_Region_Box_Width), 8, 0);
	gridlayout->addWidget(_cb_link_width, 8, 1);

	gridlayout->addWidget(new QLabel(STR_Region_Box_Height), 9, 0);
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
	 _loadLinkProfile();
}
//---------------------------------------------------------------------------

void ScanRegionLinkDialog::_loadLinkProfile() 
{
	QString scan_name = Preferences::inst()->getValue(STR_PREF_LAST_SCAN_LINK_SELECTED).toString();
	_scan_link_profiles = Preferences::inst()->getValue(STR_PREF_SCAN_LINK_PROFILES).toJsonArray();

	if(scan_name.length() > 0)
	{
		_scan_type->setCurrentText(scan_name);
	}
}

//---------------------------------------------------------------------------

void ScanRegionLinkDialog::onShowAllProps(int)
{
	onScanChanged(_scan_type->currentText());
}

//---------------------------------------------------------------------------

void ScanRegionLinkDialog::_clear_args()
{
	_cb_link_top_y->clear();
	_cb_link_left_x->clear();
	_cb_link_right_x->clear();
	_cb_link_bottom_y->clear();
	_cb_link_center_x->clear();
	_cb_link_center_y->clear();
	_cb_link_width->clear();
	_cb_link_height->clear();
}

//---------------------------------------------------------------------------

void ScanRegionLinkDialog::_add_arg(const QString &name)
{
	_cb_link_top_y->addItem(name);
	_cb_link_left_x->addItem(name);
	_cb_link_right_x->addItem(name);
	_cb_link_bottom_y->addItem(name);
	_cb_link_center_x->addItem(name);
	_cb_link_center_y->addItem(name);
	_cb_link_width->addItem(name);
	_cb_link_height->addItem(name);
}

//---------------------------------------------------------------------------

void ScanRegionLinkDialog::onScanChanged(const QString &scan_name)
{
	if(_avail_scans != nullptr)
	{
		if(_avail_scans->count(scan_name) > 0)
		{
			const BlueskyPlan plan = _avail_scans->at(scan_name);
			_clear_args();

			_add_arg(" ");
			
			for(auto itr: plan.parameters)
			{
				if(_ck_show_all_props->isChecked())
				{
					_add_arg(itr.name);
				}
				else
				{
					if( itr.kind == BlueskyParamType::Double)
					{
						_add_arg(itr.name);
					}
				}
			}

			for (const auto &scan_link_ref : _scan_link_profiles)
			{
				QJsonObject scan_link  = scan_link_ref.toObject();
				if (scan_link.contains(STR_SCAN_TYPE))
				{
					QString l_scan_name = scan_link[STR_SCAN_TYPE].toString();
					if(scan_name == l_scan_name)
					{
						if(scan_link.contains(STR_Region_Box_Top_Y))
						{
							QString value = scan_link.value(STR_Region_Box_Top_Y).toString();
							int idx = _cb_link_top_y->findText(value);
							if(idx > -1)
							{
								_cb_link_top_y->setCurrentIndex(idx);
							}
						}
						if(scan_link.contains(STR_Region_Box_Left_X))
						{
							QString value = scan_link.value(STR_Region_Box_Left_X).toString();
							int idx = _cb_link_left_x->findText(value);
							if(idx > -1)
							{
								_cb_link_left_x->setCurrentIndex(idx);
							}
						}
						if(scan_link.contains(STR_Region_Box_Right_X))
						{
							QString value = scan_link.value(STR_Region_Box_Right_X).toString();
							int idx = _cb_link_right_x->findText(value);
							if(idx > -1)
							{
								_cb_link_right_x->setCurrentIndex(idx);
							}
						}
						if(scan_link.contains(STR_Region_Box_Bottom_Y))
						{
							QString value = scan_link.value(STR_Region_Box_Bottom_Y).toString();
							int idx = _cb_link_bottom_y->findText(value);
							if(idx > -1)
							{
								_cb_link_bottom_y->setCurrentIndex(idx);
							}
						}
						if(scan_link.contains(STR_Region_Box_Center_X))
						{
							QString value = scan_link.value(STR_Region_Box_Center_X).toString();
							int idx = _cb_link_center_x->findText(value);
							if(idx > -1)
							{
								_cb_link_center_x->setCurrentIndex(idx);
							}
						}
						if(scan_link.contains(STR_Region_Box_Center_Y))
						{
							QString value = scan_link.value(STR_Region_Box_Center_Y).toString();
							int idx = _cb_link_center_y->findText(value);
							if(idx > -1)
							{
								_cb_link_center_y->setCurrentIndex(idx);
							}
						}
						if(scan_link.contains(STR_Region_Box_Width))
						{
							QString value = scan_link.value(STR_Region_Box_Width).toString();
							int idx = _cb_link_width->findText(value);
							if(idx > -1)
							{
								_cb_link_width->setCurrentIndex(idx);
							}
						}
						if(scan_link.contains(STR_Region_Box_Height))
						{
							QString value = scan_link.value(STR_Region_Box_Height).toString();
							int idx = _cb_link_height->findText(value);
							if(idx > -1)
							{
								_cb_link_height->setCurrentIndex(idx);
							}
						}
						
						break;
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------------

void ScanRegionLinkDialog::onSave()
{
	
	QJsonArray new_scan_link_profiles;
	bool found = false;
	for (const auto &scan_link_ref : _scan_link_profiles)
	{
		QJsonObject scan_link  = scan_link_ref.toObject();
		if (scan_link.contains(STR_SCAN_TYPE))
        {
            QString scan_name = scan_link[STR_SCAN_TYPE].toString();
			if(_scan_type->currentText() == scan_name)
			{
				QJsonObject scan_profile;
				scan_profile[STR_SCAN_TYPE] = _scan_type->currentText();
				scan_profile[STR_Region_Box_Top_Y] = _cb_link_top_y->currentText();
				scan_profile[STR_Region_Box_Left_X] = _cb_link_left_x->currentText();
				scan_profile[STR_Region_Box_Right_X] = _cb_link_right_x->currentText();
				scan_profile[STR_Region_Box_Bottom_Y] = _cb_link_bottom_y->currentText();
				scan_profile[STR_Region_Box_Center_X] = _cb_link_center_x->currentText();
				scan_profile[STR_Region_Box_Center_Y] = _cb_link_center_y->currentText();
				scan_profile[STR_Region_Box_Width] = _cb_link_width->currentText();
				scan_profile[STR_Region_Box_Height] = _cb_link_height->currentText();
				new_scan_link_profiles.append(scan_profile);
				found = true;
			}
			else
			{
				new_scan_link_profiles.append(scan_link);
			}
		}
	}
	
	if(found == false)
	{
		QJsonObject scan_profile;
		scan_profile[STR_SCAN_TYPE] = _scan_type->currentText();
		scan_profile[STR_Region_Box_Top_Y] = _cb_link_top_y->currentText();
		scan_profile[STR_Region_Box_Left_X] = _cb_link_left_x->currentText();
		scan_profile[STR_Region_Box_Right_X] = _cb_link_right_x->currentText();
		scan_profile[STR_Region_Box_Bottom_Y] = _cb_link_bottom_y->currentText();
		scan_profile[STR_Region_Box_Center_X] = _cb_link_center_x->currentText();
		scan_profile[STR_Region_Box_Center_Y] = _cb_link_center_y->currentText();
		scan_profile[STR_Region_Box_Width] = _cb_link_width->currentText();
		scan_profile[STR_Region_Box_Height] = _cb_link_height->currentText();
		new_scan_link_profiles.append(scan_profile);
	}

	_scan_link_profiles = new_scan_link_profiles;
	Preferences::inst()->setValue(STR_PREF_LAST_SCAN_LINK_SELECTED, QVariant(_scan_type->currentText()));
	Preferences::inst()->setValue(STR_PREF_SCAN_LINK_PROFILES, QVariant(_scan_link_profiles));

	emit linkUpdated();
	close();
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
