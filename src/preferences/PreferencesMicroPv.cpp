/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/PreferencesMicroPv.h>

#include <QSpinBox>
#include <QLabel>
#include <QFormLayout>

//---------------------------------------------------------------------------

PreferencesMicroPv::PreferencesMicroPv(QWidget* parent) : QWidget(parent)
{


   m_microProbeXPv = new QLineEdit("");
   m_microProbeYPv = new QLineEdit("");

   QFont font;
   font.setBold(true);
   font.setWeight(QFont::Thin);

   QLabel* lblX = new QLabel("MicroProbe X motor PV:");
   QLabel* lblY = new QLabel("MicroProbe Y motor PV:");

   QFormLayout* mainLayout = new QFormLayout();
   mainLayout->addRow(lblX, m_microProbeXPv);
   mainLayout->addRow(lblY, m_microProbeYPv);

   setLayout(mainLayout);

}

//---------------------------------------------------------------------------

PreferencesMicroPv::~PreferencesMicroPv()
{

}

//---------------------------------------------------------------------------

void PreferencesMicroPv::acceptChanges()
{

}

//---------------------------------------------------------------------------

QString PreferencesMicroPv::getMicroProbeXPv()
{

   return m_microProbeXPv->text();
   
}

//---------------------------------------------------------------------------

QString PreferencesMicroPv::getMicroProbeYPv()
{

   return m_microProbeYPv->text();

}

//---------------------------------------------------------------------------

void PreferencesMicroPv::setMicroProbeXPv(QString val)
{

   m_microProbeXPv->setText(val);

}

//---------------------------------------------------------------------------

void PreferencesMicroPv::setMicroProbeYPv(QString val)
{

   m_microProbeYPv->setText(val);

}

//---------------------------------------------------------------------------

