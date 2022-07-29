/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/SolverWidget.h>


#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QApplication>
#include "core/defines.h"
#define BUTTON_SIZE 32

/*---------------------------------------------------------------------------*/

SolverWidget::SolverWidget(QWidget *parent) : QWidget(parent)
{

   setWindowModality(Qt::WindowModal);
   setWindowTitle("Solver Results");

   //Set background white
   //QPalette * p = new QPalette;
   //p->setColor(QPalette::Background, Qt::white);
   //setAutoFillBackground(true);
   //setPalette(*p);

   QVBoxLayout* vboxLayout = new QVBoxLayout();
   m_gridLayout = new QGridLayout();
   m_gridLayout->setSpacing(10);

   QHBoxLayout* statusLayout = new QHBoxLayout();
   QLabel* lblStatus = new QLabel("Status: ");
   m_lblStatus = new QLabel("");


   //Create Prepend plus minus buttons
   m_btnCancel = new QPushButton("Cancel");
   m_btnUse = new QPushButton("Use New Values");

   connect(m_btnCancel,
           SIGNAL(clicked()),
           this,
           SLOT(cancel()));
   

   connect(m_btnUse,
           SIGNAL(clicked()),
           this,
           SLOT(useNewValues()));


   QHBoxLayout* buttonLayout = new QHBoxLayout();
   buttonLayout->setSpacing(5);
   buttonLayout->addWidget(m_btnUse);
   buttonLayout->addWidget(m_btnCancel);

   statusLayout->setSpacing(5);
   statusLayout->addWidget(lblStatus, Qt::AlignLeft);
   statusLayout->addWidget(m_lblStatus, Qt::AlignLeft);

   vboxLayout->addLayout(statusLayout);
   vboxLayout->addLayout(m_gridLayout);
   vboxLayout->addLayout(buttonLayout);

   setLayout(vboxLayout);

}

/*---------------------------------------------------------------------------*/

SolverWidget::~SolverWidget()
{

}

/*---------------------------------------------------------------------------*/

void SolverWidget::appendVariable(int row,
                                  QString name,
                                  double original,
                                  double newVal)
{

   int col = 0;
   QLabel* varLabel = new QLabel("Variable: ");
   m_gridLayout->addWidget(varLabel, row, col);
   col++;

   QLabel* nameLabel = new QLabel(name);
   m_gridLayout->addWidget(nameLabel, row, col);
   col++;

   QLabel* startLabel = new QLabel("Original Value: ");
   m_gridLayout->addWidget(startLabel, row, col);
   col++;

   QLabel* stepLabel = new QLabel(QString::number(original));
   m_gridLayout->addWidget(stepLabel, row, col);
   col++;

   QLabel* minLabel = new QLabel("New Value: ");
   m_gridLayout->addWidget(minLabel, row, col);
   col++;

   QLabel* minValueLabel = new QLabel(QString::number(newVal));
   m_gridLayout->addWidget(minValueLabel, row, col);
   col++;

}

/*---------------------------------------------------------------------------*/

void SolverWidget::cancel()
{

   emit cancelUpdatedVariables();
   this->close();

}

/*---------------------------------------------------------------------------*/

void SolverWidget::setCoefs(QMap<QString, double> oldVals, QMap<QString, double> newVals)
{

   m_oldCoefs = oldVals;
   m_newCoefs = newVals;
   if(oldVals.keys().size() != newVals.keys().size())
   {
      logE<<"Error: SolverWidget::setCoefs(), oldVals dictionary size does not match newVals dictionary!";
      return;
   }
   int i=0;
   for(const QString &key : oldVals.keys())
   {
      if(false == newVals.contains(key))
      {
         logE<<"Error: SolverWidget::setCoefs(), oldVals dictionary keys do not match newVals dictionary keys!";
         return;
      }
      this->appendVariable(i, key, oldVals[key], newVals[key]);
      i++;
   }

}

/*---------------------------------------------------------------------------*/

void SolverWidget::setStatusString(QString val)
{

   m_lblStatus->setText(val);

}

/*---------------------------------------------------------------------------*/

void SolverWidget::setUseBtnEnabled(bool val)
{

   m_btnUse->setEnabled(val);

}

/*---------------------------------------------------------------------------*/

void SolverWidget::useNewValues()
{

   emit useUpdatedVariables(m_newCoefs);
   this->close();

}

/*---------------------------------------------------------------------------*/
