/*-----------------------------------------------------------------------------
 * Copyright (c) 2025, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ELEMENT_SELECT_DIALOG
#define ELEMENT_SELECT_DIALOG

//---------------------------------------------------------------------------

#include <QDialog>
#include <QPushButton>
#include <QStringListModel>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include "data_struct/fit_parameters.h"

//---------------------------------------------------------------------------

class ElementSelectDialog : public QDialog
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   ElementSelectDialog();

   /**
    * Destructor.
    */
   ~ElementSelectDialog();

   void setListData(std::unordered_map<std::string, data_struct::ArrayXXr<float>>& elements,
						const std::map<std::string, data_struct::ArrayXXr<float>>* scalers);

	QStringList getSelection();

	void clear() {_img_list_model->clear();}
	
public slots:
	void onAccept();

	void onClose();

protected:

   void createLayout();

private:

	QPushButton* _acceptBtn;

	QPushButton* _cancelBtn;
  
	QListView* _img_names_view;

	QStandardItemModel* _img_list_model;

};


//---------------------------------------------------------------------------

#endif /* ElementSelectDialog_H_ */

//---------------------------------------------------------------------------

