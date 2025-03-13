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
#include <QCheckBox>
#include <QJsonArray>
#include "mvc/BlueskyPlan.h"
#include "preferences/Preferences.h"

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

   const std::vector<QString>& property_list() { return _prop_list; }

signals:
   void linkUpdated();

public slots:

   void onScanChanged(const QString &);

   void onSave();

   void onShowAllProps(int);

protected:

   void _createLayout();

   void _clear_args();

   void _add_arg(const QString &scan_name);

private:

    void _loadLinkProfile();

    QCheckBox *_ck_show_all_props;

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
    
    QJsonArray _scan_link_profiles;

    std::vector<QString> _prop_list = {STR_Region_Box_Top_Y,
                                       STR_Region_Box_Left_X,
                                       STR_Region_Box_Right_X,
                                       STR_Region_Box_Bottom_Y,
                                       STR_Region_Box_Center_X,
                                       STR_Region_Box_Center_Y,
                                       STR_Region_Box_Width,
                                       STR_Region_Box_Height
      };
};


//---------------------------------------------------------------------------

#endif /* ImageGridDialog_H_ */

//---------------------------------------------------------------------------

