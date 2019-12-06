/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PER_PIXEL_FIT_WIDGET_H
#define PER_PIXEL_FIT_WIDGET_H

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

/*---------------------------------------------------------------------------*/


class PerPixelFitWidget : public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor.
    */
	PerPixelFitWidget(QWidget *parent = nullptr);

   /**
    * Destructor.
    */
   ~PerPixelFitWidget();

    void updateFileList(QStringList file_list);

public slots:

   
protected:

   /**
    * @brief Create layout
    */
   void createLayout();

   
   QTextEdit *_textEdit;

   QProgressBar *_progressBar;

   QPushButton *_btn_run;

   QPushButton *_btn_cancel;
};


/*---------------------------------------------------------------------------*/

#endif 

/*---------------------------------------------------------------------------*/
