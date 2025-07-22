/*-----------------------------------------------------------------------------
 * Copyright (c) 2025, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ROI_SELECTOR_DIALOG_H
#define ROI_SELECTOR_DIALOG_H

#include <QDialog>
#include <QComboBox>

//---------------------------------------------------------------------------

class RoiSelectorDialog : public QDialog
{
    Q_OBJECT

public:

   /**
    * Default constructor
    */
   RoiSelectorDialog(QWidget *parent = 0);

   /**
    * Default destructor
    */
   ~RoiSelectorDialog();

   void setROIs(QStringList items);

   void setBackgrounds(QStringList items);

   QString selectedROI() { return _cb_rois->currentText(); }

   QString selectedBackground() { return _cb_background->currentText(); }

private:

   QComboBox* _cb_rois;

   QComboBox* _cb_background;

};

//---------------------------------------------------------------------------

#endif // ROI_SELECTOR_DIALOG_H

//---------------------------------------------------------------------------
