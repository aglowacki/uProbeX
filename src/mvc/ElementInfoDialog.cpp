/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ElementInfoDialog.h>

#include <QLabel>
#include <QGridLayout>
#include "data_struct/element_info.h"
#include "data_struct/fit_element_map.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

ElementInfoDialog::ElementInfoDialog() : QDialog()
{

	_param_override = nullptr;
    createLayout();

}

//---------------------------------------------------------------------------

ElementInfoDialog::~ElementInfoDialog()
{

}

//---------------------------------------------------------------------------

void ElementInfoDialog::createLayout()
{
	QLabel *lbl_element = new QLabel("Element");
	//QLabel *lbl_shell = new QLabel("Shell");
	_cb_element_selector = new QComboBox();
	
	for(const std::string& e : data_struct::Element_Symbols)
    {
        _cb_element_selector->addItem(QString::fromStdString(e));
    }

	_btn_update = new QPushButton("Update");
	_btn_cancel = new QPushButton("Cancel");
	connect(_btn_update, &QPushButton::pressed, this, &ElementInfoDialog::onUpdate);
	connect(_btn_cancel, &QPushButton::pressed, this, &ElementInfoDialog::close);

	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->setSizeConstraint(QLayout::SetFixedSize);
	mainLayout->addWidget(lbl_element, 0, 0);
	mainLayout->addWidget(_cb_element_selector, 0, 1);
	//mainLayout->addWidget(lbl_shell, 1, 0);
	//mainLayout->addWidget(, 1, 1);
	mainLayout->addWidget(_btn_update, 2, 0);
	mainLayout->addWidget(_btn_cancel, 2, 1);
	mainLayout->setRowStretch(3, 1);

	setLayout(mainLayout);

	setWindowTitle("Element Info");
}

//---------------------------------------------------------------------------

void ElementInfoDialog::set_selected_element(const QString& el_name)
{
	_cb_element_selector->setCurrentText(el_name);
	std::string _detector_element = "Si";
	
	//data_struct::Element_Info_Map<T_real>* element_map = data_struct::Element_Info_Map<T_real>::inst();
	data_struct::Fit_Element_Map<double>* fit_element = data_struct::gen_element_map<double>(el_name.toStdString());
    if(fit_element != nullptr)
    {
		
		if (_param_override != nullptr)
		{
            std::map<int, double> ratios = _param_override->get_custom_factor(el_name.toStdString());
			for (const auto &itr : ratios)
			{
				fit_element->multiply_custom_multiply_ratio(itr.first, itr.second);
			}
		}
		/*
        if(_chk_is_pileup->checkState() == Qt::CheckState::Checked)
        {
            QString pileup_name = _cb_pileup_elements->currentText();
            data_struct::Element_Info<double>* pileup_element = data_struct::Element_Info_Map<double>::inst()->get_element(pileup_name.toStdString());
            if(pileup_element != nullptr)
            {
                el_name += "_" + pileup_name;
                fit_element->set_as_pileup(pileup_name.toStdString(), pileup_element);
            }
        }
		*/
        fit_element->init_energy_ratio_for_detector_element(data_struct::Element_Info_Map<double>::inst()->get_element(_detector_element));
		const std::vector<data_struct::Element_Energy_Ratio<double>>& ratios = fit_element->energy_ratios();
    	const  std::vector<double>& energy_ratio_multipliers = fit_element->energy_ratio_multipliers();
    }
	
}

//---------------------------------------------------------------------------

void ElementInfoDialog::onUpdate()
{
	//emit onNewGridLayout(sbRow->value(), sbCol->value());
	close();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
