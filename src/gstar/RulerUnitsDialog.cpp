/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include "gstar/RulerUnitsDialog.h"

//---------------------------------------------------------------------------

RulerUnitsDialog::RulerUnitsDialog(QWidget* parent, Qt::WindowFlags f)
   : QDialog(parent, f)
{

   // Set dialog title
   setWindowTitle(tr("Ruler Units"));

   QLabel* labelStartValue = new QLabel(tr("Units Label"));
   m_unitLabel = new QLineEdit();

   QLabel* labelEndValue = new QLabel(tr("Units Per Pixel X"));
   m_unitsPerPixelX = new QDoubleSpinBox();
   m_unitsPerPixelX -> setMinimum(-9999999);
   m_unitsPerPixelX -> setMaximum(9999999);

   QLabel* labelStepValue = new QLabel(tr("Units Per Pixel Y"));
   m_unitsPerPixelY = new QDoubleSpinBox();
   m_unitsPerPixelY -> setMinimum(-9999999);
   m_unitsPerPixelY -> setMaximum(9999999);

   // Layout
   QGridLayout* layout = new QGridLayout();
   layout -> setAlignment(Qt::AlignTop);
   layout -> addWidget(labelStartValue, 0, 0, Qt::AlignRight);
   layout -> addWidget(m_unitLabel, 0, 1);
   layout -> addWidget(labelEndValue, 1, 0, Qt::AlignRight);
   layout -> addWidget(m_unitsPerPixelX, 1, 1);
   layout -> addWidget(labelStepValue, 2, 0, Qt::AlignRight);
   layout -> addWidget(m_unitsPerPixelY, 2, 1);

   // Dialog buttons
   m_btnOK = new QPushButton(tr("OK"));
   m_btnCancel = new QPushButton(tr("Cancel"));

   // Button layout
   QHBoxLayout* btnLayout = new QHBoxLayout();
   btnLayout -> setContentsMargins(0, 0, 0, 0);
   btnLayout -> addStretch();
   btnLayout -> addWidget(m_btnOK);
   btnLayout -> addWidget(m_btnCancel);

   // Connect button signals/slots
   connect(m_btnOK, &QPushButton::clicked, this, &RulerUnitsDialog::accept);
   connect(m_btnCancel, &QPushButton::clicked, this, &RulerUnitsDialog::reject);

   // Main layout
   QVBoxLayout* mainLayout = new QVBoxLayout();
   mainLayout -> addLayout(layout, Qt::AlignLeft);
   mainLayout -> addLayout(btnLayout);
   setLayout(mainLayout);

   // Fix dialog size
   this -> layout() -> setSizeConstraint(QLayout::SetFixedSize);

}

//---------------------------------------------------------------------------

void RulerUnitsDialog::accept()
{

   // Message box
   QMessageBox msgBox;

   // Check step size
   if (m_unitsPerPixelY -> value() >= 0.0 && m_unitsPerPixelY -> value() <= 0.0) {
      QMessageBox::critical(this, tr("Error"),
                            tr("Delta can not be zero."));
      return;
   }

   // Close with accept
   QDialog::accept();

}

//---------------------------------------------------------------------------

QString RulerUnitsDialog::getUnitLabel()
{

   return m_unitLabel->text();

}

//---------------------------------------------------------------------------

double RulerUnitsDialog::getUnitsPerPixelX()
{

   return m_unitsPerPixelX->value();

}

//---------------------------------------------------------------------------

double RulerUnitsDialog::getUnitsPerPixelY()
{

   return m_unitsPerPixelY->value();

}

//---------------------------------------------------------------------------

void RulerUnitsDialog::reject()
{

   // Close with reject
   QDialog::reject();

}

//---------------------------------------------------------------------------

void RulerUnitsDialog::setUnitLabel(QString label)
{

   m_unitLabel->setText(label);

}

//---------------------------------------------------------------------------

void RulerUnitsDialog::setUnitsPerPixelX(double end)
{

   m_unitsPerPixelX-> setValue(end);

}

//---------------------------------------------------------------------------

void RulerUnitsDialog::setUnitsPerPixelY(double delta)
{

   m_unitsPerPixelY-> setValue(delta);

}

//---------------------------------------------------------------------------
