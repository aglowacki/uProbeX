/*-----------------------------------------------------------------------------
 * Copyright (c) 2025, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SCAN_REGION_LINK_DIALOG_H
#define SCAN_REGION_LINK_DIALOG_H

//---------------------------------------------------------------------------

#include <QDialog>
#include <QPushButton>
#include <QComboBox>
#include "mvc/BlueskyPlan.h"

//---------------------------------------------------------------------------

class ScanRegionLinkDialog : public QDialog
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
    ScanRegionLinkDialog();

   /**
    * Destructor.
    */
    ~ScanRegionLinkDialog();

    void setAvailScans(std::map<QString, BlueskyPlan> * avail_scans);

public slots:

   void onScanChanged(const QString &);

   void onSave();

protected:

   void _createLayout();

private:
   
    QComboBox *_scan_type;

    QComboBox *_cb_link_top_y;
    
    QComboBox *_cb_link_left_x;
    
    QComboBox *_cb_link_right_x;
    
    QComboBox *_cb_link_bottom_y;

    QComboBox *_cb_link_center_x;

    QComboBox *_cb_link_center_y;

    QComboBox *_cb_link_width;

    QComboBox *_cb_link_height;

    std::map<QString, BlueskyPlan> *_avail_scans;

    QPushButton *_btn_save;

    QPushButton *_btn_cancel;

};


//---------------------------------------------------------------------------

#endif /* ImageGridDialog_H_ */

//---------------------------------------------------------------------------

