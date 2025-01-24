/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SCAN_REGION_DIALOG_H
#define SCAN_REGION_DIALOG_H

//---------------------------------------------------------------------------

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QStringListModel>
#include <QListWidgetItem>
#include <QTableView>
#include "gstar/AnnotationProperty.h"
#include "mvc/BlueskyPlan.h"
#include "mvc/ScanTableModel.h"
#include "mvc/ComboBoxBoolDelegate.h"

//---------------------------------------------------------------------------

class ScanRegionDialog : public QDialog
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
   ScanRegionDialog();

   /**
    * Destructor.
    */
   ~ScanRegionDialog();

   void updateProps(QList<gstar::AnnotationProperty*> &anno_list);

   QString getScanName() { return _scan_name->text(); }

   void setAvailScans(std::map<QString, BlueskyPlan> * avail_scans);

   void setRegionNameVisible(bool val) { _lbl_region_name->setVisible(val); _scan_name->setVisible(false);}

signals:
	
   void ScanUpdated(const BlueskyPlan& plan);

public slots:

   void onUpdateAndQueue();

   void scanChanged(const QString &);

   void onBatchScanChanged(int);

protected:

   void _createLayout();

private:
	QLineEdit *_scan_name;
   
   QComboBox *_scan_type;

   QComboBox *_cb_batch_prop;

   QPushButton *_btn_update_and_queue;

   QPushButton *_btn_cancel;

   QTableView* _scan_options;

   QLineEdit *_batch_start;

   QLineEdit *_batch_end;

   QLineEdit *_batch_num;

   QCheckBox* _chk_batch_scan;

   ScanTableModel *_scan_table_model;
   
   QLabel* _lbl_region_name;
   
   std::map<QString, BlueskyPlan> *_avail_scans;

   ComboBoxBoolDelegate *_cbDelegate;

};


//---------------------------------------------------------------------------

#endif /* ImageGridDialog_H_ */

//---------------------------------------------------------------------------

