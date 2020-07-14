/*-----------------------------------------------------------------------------
 * Copyright (c) 2019, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SPECTRA_FIT_WIDGET_H
#define SPECTRA_FIT_WIDGET_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QThread>
#include <QTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QCheckBox>
#include <QEventLoop>
#include <thread>
#include "fitting//optimizers/lmfit_optimizer.h"
#include "core/GlobalThreadPool.h"

/*---------------------------------------------------------------------------*/

QString STR_LM_FIT = "Levenberg-Marquardt Fit";
QString STR_MP_FIT = "MP Fit";

/*---------------------------------------------------------------------------*/

class SpectraFitWidget : public QWidget
{

   Q_OBJECT

public:

    /**
     * Constructor.
     */
	SpectraFitWidget(QWidget *parent = nullptr);

    /**
     * Destructor.
     */
    ~SpectraFitWidget();

    void updateFitParams(data_struct::Fit_Parameters out_fit_params, data_struct::Fit_Parameters element_fit_params);

    void status_callback(size_t cur_block, size_t total_blocks);

    void SpectraFitWidget::setOptimizer(QString opt);

    bool accepted_fit() { return _accepted; }

    void SpectraFitWidget::setSpectra(data_struct::Spectra* spectra);

    void SpectraFitWidget::setElementsToFit(data_struct::Fit_Element_Map_Dict* elements_to_fit);

signals:

    void processed_list_update();

public slots:
    void runProcessing();
   
protected:

   /**
    * @brief Create layout
    */
   void createLayout();

   
   QTextEdit *_textEdit;

   QProgressBar *_progressBarFiles;

   QProgressBar* _progressBarBlocks;

   QPushButton *_btn_run;

   QPushButton* _btn_accept;

   QPushButton *_btn_cancel;

   fitting::optimizers::LMFit_Optimizer _lmfit_optimizer;

   fitting::optimizers::MPFit_Optimizer _mpfit_optimizer;

   fitting::routines::Param_Optimized_Fit_Routine _fit_routine;

   size_t _total_itr;

   bool _accepted;

   data_struct::Spectra* _int_spec;

   data_struct::Fit_Element_Map_Dict* _elements_to_fit;

   data_struct::Fit_Parameters _out_fit_params;
   data_struct::Fit_Parameters _element_fit_params;
   data_struct::Fit_Parameters _new_out_fit_params;
   data_struct::Fit_Parameters _new_element_fit_params;

};


/*---------------------------------------------------------------------------*/

#endif 

/*---------------------------------------------------------------------------*/
