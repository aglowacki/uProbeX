/*-----------------------------------------------------------------------------
 * Copyright (c) 2020, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SpectraWidgetSettingsDialog_H
#define SpectraWidgetSettingsDialog_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QEventLoop>
#include <QDialog>
#include <QTableView>
#include "mvc/ComboBoxDelegate.h"

/*---------------------------------------------------------------------------*/

class SpectraWidgetSettingsDialog : public QDialog
{

   Q_OBJECT

public:

    /**
     * Constructor.
     */
	SpectraWidgetSettingsDialog(QWidget *parent = nullptr);

    /**
     * Destructor.
     */
    ~SpectraWidgetSettingsDialog();

    bool isAccepted() { return _accepted; }

public slots:
    
	void onAccepted();

    void onCancel();
   
protected:

   /**
    * @brief Create layout
    */
   void createLayout();

   QCheckBox* _chkLog10;

   QCheckBox* _chkDetailedFit;

   //QTextEdit *_textEdit;

   QPushButton *_btn_run;

   QPushButton* _btn_accept;

   QPushButton *_btn_cancel;

   bool _accepted;

};


/*---------------------------------------------------------------------------*/

#endif 

/*---------------------------------------------------------------------------*/
