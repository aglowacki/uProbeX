//---------------------------------------------------------------------------

#ifndef PREFERENCESAUTOSAVE_H
#define PREFERENCESAUTOSAVE_H

//---------------------------------------------------------------------------

#include <QWidget>
#include <QSpinBox>
#include <QCheckBox>

/**
 * @brief Preferences dialog page for configuring autosave functionality
 */
class PreferencesAutoSave
: public QWidget
{

   Q_OBJECT

public:
   /**
    * Constructor
    *
    * @param parent - the Qt parent widget
    */
   PreferencesAutoSave(QWidget* parent = 0);

   /**
    * Default destructor
    */
   ~PreferencesAutoSave();

   /**
    * @brief getEnableRecoveryAutoSafeFunctionality
    */
   bool getEnableRecoveryAutoSaveFunctionality();

   /**
    * @brief setEnableRecoveryAutoSafeFunctionality
    */
   void setEnableRecoveryAutoSaveFunctionality(bool);

   /**
    * @brief getRecoveryAutoSaveEveryMiliseconds
    */
   int getRecoveryAutoSaveEveryMiliseconds();

   /**
    * @brief setRecoveryAutoSaveEveryMiliseconds
    */
   void setRecoveryAutoSaveEveryMiliseconds(int);

   /**
    * @brief getSaveOnExitAutomatically
    */
   bool getEnableSaveOnExit();

   /**
    * @brief setSaveOnExitAutomatically
    */
   void setEnableSaveOnExit(bool);

private:

   /**
    * Describes the duration between every auto safe in application.
    *
    * @brief m_autoSafeTemporaryDataEverySeconds
    */
   QSpinBox* m_autoSaveTemporaryDataEverySeconds;

   /**
    * @brief m_enableAutoSaveFunctionality
    */
   QCheckBox* m_enableRecoveryAutoSaveFunctionality;

   /**
    * @brief m_saveOnExit
    */
   QCheckBox* m_enableSaveOnExit;

private slots:

   void enableRecoveryAutoSafeChanged(int);

};

//---------------------------------------------------------------------------

#endif // PREFERENCESAUTOSAVE_H

//---------------------------------------------------------------------------
