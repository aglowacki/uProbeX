/*-----------------------------------------------------------------------------
 * Copyright (c) 2025, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SCAN_REGION_LINK_DIALOG_H
#define SCAN_REGION_LINK_DIALOG_H

//---------------------------------------------------------------------------

#include <mvc/ScanRegionDialog.h>

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

signals:
	
public slots:

   void onSave();

protected:

   void _createLayout();

private:
   
    QPushButton *_btn_save;

    QPushButton *_btn_cancel;

    ScanRegionDialog* _scan_region_dialog;
};


//---------------------------------------------------------------------------

#endif /* ImageGridDialog_H_ */

//---------------------------------------------------------------------------

