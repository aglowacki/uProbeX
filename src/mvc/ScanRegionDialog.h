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
#include <QStringListModel>
#include <QListWidgetItem>
#include <QTableView>
#include "gstar/AnnotationProperty.h"
#include "mvc/BlueskyPlan.h"

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

   void setAvailScans(std::unordered_map<QString, BlueskyPlan> * avail_scans);
signals:
	
   void ScanUpdated();

public slots:

	void onUpdate();

   void onUpdateAndQueue();

   void scanChanged(const QString &);

protected:

   void _createLayout();

private:
	QLineEdit *_scan_name;
   
   QComboBox *_scan_type;
   
   QPushButton *_btn_update;

   QPushButton *_btn_update_and_queue;

   QPushButton *_btn_cancel;

   QTableView* _scan_options;

   std::unordered_map<QString, BlueskyPlan> *_avail_scans;
};


//---------------------------------------------------------------------------

#endif /* ImageGridDialog_H_ */

//---------------------------------------------------------------------------

