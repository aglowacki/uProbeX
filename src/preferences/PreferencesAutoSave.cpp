/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/PreferencesAutoSave.h>

#include <QWidget>
#include <QLabel>
#include <QFormLayout>
#include <QSpacerItem>

//---------------------------------------------------------------------------

PreferencesAutoSave::PreferencesAutoSave(QWidget* parent) : QWidget(parent)
{

   QFormLayout* mainLayout = new QFormLayout();

   m_autoSaveTemporaryDataEverySeconds = new QSpinBox;
   m_autoSaveTemporaryDataEverySeconds->setRange(1, 9999);
   m_autoSaveTemporaryDataEverySeconds->setSingleStep(1);

   m_enableRecoveryAutoSaveFunctionality = new QCheckBox();

   m_enableSaveOnExit = new QCheckBox();   

   QLabel* lblEnableAutoSave = new QLabel("Recovery auto save enabled");
   QLabel* lblAutoSaveTime = new QLabel("Perform every (s)");
   QLabel* lblSaveAutomaticallyOnExit = new QLabel("Automcatically save all on app exit");

   mainLayout->addRow(lblEnableAutoSave, m_enableRecoveryAutoSaveFunctionality);
   mainLayout->addRow(lblAutoSaveTime, m_autoSaveTemporaryDataEverySeconds);
   mainLayout->addRow(lblSaveAutomaticallyOnExit, m_enableSaveOnExit);

   setLayout(mainLayout);

   connect(m_enableRecoveryAutoSaveFunctionality, SIGNAL(stateChanged(int)), this, SLOT(enableRecoveryAutoSafeChanged(int)));
}

//---------------------------------------------------------------------------

PreferencesAutoSave::~PreferencesAutoSave()
{

}

//---------------------------------------------------------------------------

void PreferencesAutoSave::enableRecoveryAutoSafeChanged(int state)
{
   if (state == 0)
   {
      m_autoSaveTemporaryDataEverySeconds->setEnabled(false);
   }  else if (state == 2 || state == 1) {
      m_autoSaveTemporaryDataEverySeconds->setEnabled(true);
   }
}

//---------------------------------------------------------------------------

bool PreferencesAutoSave::getEnableRecoveryAutoSaveFunctionality()
{
   return m_enableRecoveryAutoSaveFunctionality->isChecked();
}

//---------------------------------------------------------------------------

void PreferencesAutoSave::setEnableRecoveryAutoSaveFunctionality(bool enableRecoveryAutoSave)
{
   m_enableRecoveryAutoSaveFunctionality->setChecked(enableRecoveryAutoSave);
   enableRecoveryAutoSafeChanged(m_enableRecoveryAutoSaveFunctionality->isChecked());
}

//---------------------------------------------------------------------------

int PreferencesAutoSave::getRecoveryAutoSaveEveryMiliseconds()
{
   int seconds = m_autoSaveTemporaryDataEverySeconds->value();
   int miliseconds = seconds * 1000;
   return miliseconds;
}

//---------------------------------------------------------------------------

void PreferencesAutoSave::setRecoveryAutoSaveEveryMiliseconds(int recoveryAutoSaveMilisecods)
{
   int seconds = recoveryAutoSaveMilisecods / 1000;
   m_autoSaveTemporaryDataEverySeconds->setValue(seconds);
}

//---------------------------------------------------------------------------

bool PreferencesAutoSave::getEnableSaveOnExit()
{
   return m_enableSaveOnExit->isChecked();
}

//---------------------------------------------------------------------------

void PreferencesAutoSave::setEnableSaveOnExit(bool saveOnExitAutomatically)
{
   m_enableSaveOnExit->setChecked(saveOnExitAutomatically);
}

//---------------------------------------------------------------------------
