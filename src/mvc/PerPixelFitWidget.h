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

public slots:

   
protected:

   /**
    * @brief Create layout
    */
   void createLayout();

   
   QTextEdit *_textEdit;

   QProgressBar *_progressBar;

   QLineEdit *_qline_ip_addr;

   QLineEdit *_qline_port;

   QPushButton *_btn_update;
};


/*---------------------------------------------------------------------------*/

#endif 

/*---------------------------------------------------------------------------*/
