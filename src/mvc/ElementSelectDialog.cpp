/*-----------------------------------------------------------------------------
 * Copyright (c) 2025, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/ElementSelectDialog.h>
#include <mvc/MapsH5Model.h>
#include "preferences/Preferences.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

ElementSelectDialog::ElementSelectDialog() : QDialog()
{

	createLayout();

}

//---------------------------------------------------------------------------

ElementSelectDialog::~ElementSelectDialog()
{

}

//---------------------------------------------------------------------------

void ElementSelectDialog::setListData(std::unordered_map<std::string, data_struct::ArrayXXr<float>>& elements,
									  const std::map<std::string, data_struct::ArrayXXr<float>>* scalers)
{
	clear();
	int rows = Preferences::inst()->getValue(STR_GRID_ROWS).toInt();
    int cols = Preferences::inst()->getValue(STR_GRID_COLS).toInt();
	QStringList selected_elemenets = Preferences::inst()->getValue(STR_PFR_SELECTED_ELEMENTS).toStringList();
	int total = rows * cols;
	std::vector<std::string> element_lines;
	std::vector<std::string> scalers_to_add_first;
	std::vector<std::string> final_counts_to_add_before_scalers;
	gen_insert_order_lists(element_lines, scalers_to_add_first, final_counts_to_add_before_scalers);

	// insert in z order
	for (std::string el_name : element_lines)
	{
		if (elements.count(el_name) > 0)
		{
			QStandardItem* item0 = new QStandardItem(false);
			item0->setCheckable(true);
			QString name = QString(el_name.c_str());
			if(selected_elemenets.size() == 0 && total > 0)
			{
				item0->setCheckState(Qt::Checked);
				total --;
			}
			else if(selected_elemenets.count(name) > 0)
			{
				item0->setCheckState(Qt::Checked);
			}
			else
			{
				item0->setCheckState(Qt::Unchecked);
			}
			item0->setText(name);
			_img_list_model->appendRow(item0);

			elements.erase(el_name);
		}

	}
	/*
	//add leftovers ( pile ups )
	for (auto& itr : element_counts_not_added)
	{
		// if it is not in the final add then add it
		if (std::find(final_counts_to_add_before_scalers.begin(), final_counts_to_add_before_scalers.end(), itr.first) == final_counts_to_add_before_scalers.end())
		{
			QString val = QString(itr.first.c_str());
			m_imageViewWidget->addLabel(val);
		}
	}
	*/
	// add end of element list that are not elements
	for (auto& itr : final_counts_to_add_before_scalers)
	{
		if (elements.count(itr) > 0)
		{
			QStandardItem* item0 = new QStandardItem(false);
			item0->setCheckable(true);
			QString name = QString(itr.c_str());
			if(selected_elemenets.size() == 0 && total > 0)
			{
				item0->setCheckState(Qt::Checked);
				total --;
			}
			else if(selected_elemenets.count(name) > 0)
			{
				item0->setCheckState(Qt::Checked);
			}
			else
			{
				item0->setCheckState(Qt::Unchecked);
			}
			item0->setText(name);
			_img_list_model->appendRow(item0);

			elements.erase(itr);
		}
	}

	std::map<std::string, data_struct::ArrayXXr<float>> left_over_scalers = *scalers;
	// add scalers in certain order
	for (auto& itr : scalers_to_add_first)
	{
		if (left_over_scalers.count(itr) > 0)
		{
			QStandardItem* item0 = new QStandardItem(false);
			item0->setCheckable(true);
			QString name = QString(itr.c_str());
			if(selected_elemenets.size() == 0 && total > 0)
			{
				item0->setCheckState(Qt::Checked);
				total --;
			}
			else if(selected_elemenets.count(name) > 0)
			{
				item0->setCheckState(Qt::Checked);
			}
			else
			{
				item0->setCheckState(Qt::Unchecked);
			}
			item0->setText(name);
			_img_list_model->appendRow(item0);

			left_over_scalers.erase(itr);
		}
	}

	// add rest of scalers
	for (auto& itr : elements)
	{
		QStandardItem* item0 = new QStandardItem(false);
		item0->setCheckable(true);
		QString name = QString(itr.first.c_str());
		if(selected_elemenets.size() == 0 && total > 0)
		{
			item0->setCheckState(Qt::Checked);
			total --;
		}
		else if(selected_elemenets.count(name) > 0)
		{
			item0->setCheckState(Qt::Checked);
		}
		else
		{
			item0->setCheckState(Qt::Unchecked);
		}
		item0->setText(name);
		_img_list_model->appendRow(item0);
	}
}
//---------------------------------------------------------------------------

QStringList ElementSelectDialog::getSelection()
{

	QStringList itemList;

	for (int i = 0; i < _img_list_model->rowCount(); i++)
	{
		QStandardItem* item0 = _img_list_model->item(i);
		if (item0->checkState() == Qt::Checked)
		{
			itemList.append(item0->text());
		}
	}

	return itemList;

}

//---------------------------------------------------------------------------

void ElementSelectDialog::createLayout()
{
	_acceptBtn = new QPushButton("Accept");
	_cancelBtn = new QPushButton("Cancel");
	connect(_acceptBtn, &QPushButton::pressed, this, &ElementSelectDialog::onAccept);
	connect(_cancelBtn, &QPushButton::pressed, this, &ElementSelectDialog::onClose);

	_img_list_model = new QStandardItemModel();
	_img_names_view = new QListView();
	_img_names_view->setModel(_img_list_model);
	_img_names_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_img_names_view->setSelectionMode(QAbstractItemView::ExtendedSelection);


	QVBoxLayout* mainLayout = new QVBoxLayout;
	QVBoxLayout *leftLayout = new QVBoxLayout;
	QHBoxLayout* buttonLayout = new QHBoxLayout;

	buttonLayout->addWidget(_acceptBtn);
	buttonLayout->addWidget(_cancelBtn);
	
	leftLayout->addWidget(_img_names_view);
	leftLayout->addItem(buttonLayout);
	
	mainLayout->addItem(leftLayout);
	setLayout(mainLayout);

	setWindowTitle("Select Elements To View");
}

//---------------------------------------------------------------------------

void ElementSelectDialog::onAccept()
{
	// emit list of roi's
	//emit onNewROIs(_int_img_widget->getAllROIs());
	close();
}

//---------------------------------------------------------------------------

void ElementSelectDialog::onClose()
{
	close();
}

//---------------------------------------------------------------------------