/*-----------------------------------------------------------------------------
 * Copyright (c) 2023, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef ScanCorrCoefDialog_DIALOG_H
#define ScanCorrCoefDialog_DIALOG_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QTextEdit>
#include <QDir>
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
#include <QLineEdit>
#include <QApplication>
#include <QGroupBox>
#include <QScrollArea>
#include <QMessageBox>
#include "mvc/MapsH5Model.h"

/*---------------------------------------------------------------------------*/


class ScanCorrCoefDialog : public QWidget
{

    Q_OBJECT

public:

    /**
    * Constructor.
    */
    ScanCorrCoefDialog(QWidget *parent = nullptr);

    /**
    * Destructor.
    */
    ~ScanCorrCoefDialog();

    void status_callback(size_t cur, size_t total);

    void setViewProps(bool blog10, bool bdark, bool bgrid);

    void setRunEnabled(bool val);

    void setModel(std::string analysis_type, MapsH5Model* model);

public slots:

    void onRun();
    
    void onClose();

protected:

    /**
    * @brief Create layout
    */
    void _createLayout();

    QGroupBox* _processing_grp;

    QProgressBar *_progressBarFiles;

    QPushButton *_btn_run;

    QPushButton *_btn_cancel;

    QLineEdit* _le_restrict_coef;

    QCheckBox* _ck_use_elements;

    QCheckBox* _ck_use_scalers;

    QCheckBox* _ck_use_rois;

    MapsH5Model* _model;

    size_t _total_blocks;

    std::string _analysis_type;

    bool _blog10;

    bool _bdark;

    bool _bgrid;

    bool _running;

};


/*---------------------------------------------------------------------------*/

#endif 

/*---------------------------------------------------------------------------*/
