/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PREFERENCES_DIALOG_H
#define PREFERENCES_DIALOG_H

/*---------------------------------------------------------------------------*/

#include <QDialog>


class Preferences;
class PreferencesDisplay;
class PreferencesMicroPv;
class PreferencesPythonFunc;
class PreferencesSolverOption;
class PreferencesAutoSave;
class PreferencesExport;

class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QVBoxLayout;
class Solver;

namespace gstar
{
   class AbstractImageWidget;
}

/*---------------------------------------------------------------------------*/

/**
 * @brief The preference dialog for setting the preference, after check the
 * corresponding PV attribute box in the preference dialog, the PV attributes
 * are recorded in the setting file.
 */
class PreferencesDialog
: public QDialog
{

   Q_OBJECT

public:

   /**
    * Constructor
    *
    * @param parent - the Qt parent widget
    * @param f - Qt window flags
    */

   PreferencesDialog(QList<gstar::AbstractImageWidget*> windowList,
                     Solver* solver,
                     QWidget* parent = 0,
                     Qt::WindowFlags f = 0);

   /**
    * Set preferences object
    *
    * @param preferences - preferences object
    */
   void setPreferences(bool passMode);

protected slots:

   /**
    * This internal slot is called when the item in the category list is
    * changed.
    *
    * @param current - the current preferences item selected in the category
    *                  list on the left of the dialog
    * @param previous - the previously selected item in the category list
    */
   void changePage(QListWidgetItem* current, QListWidgetItem* previous);

private slots:

   /**
    * Overridden accept() method.
    *
    * Process changes when user clicks 'OK' button.
    */
   void accept();

   /**
    * Overridden reject() method.
    *
    * Process when user clicks 'Cancel' button.
    */
   void reject();

private:

   /**
    * @brief m_pythonFuncWidget
    */
   PreferencesPythonFunc* m_pythonFuncWidget;

   /**
    * OK button
    */
   QPushButton* m_btnOK;

   /**
    * Cancel button
    */
   QPushButton* m_btnCancel;

   /**
    * Main layout
    */
   QVBoxLayout* m_layout;

   /**
    * Contents
    */
   QListWidget* m_contentsWidget;

   /**
    * Page for individual categories
    */
   QStackedWidget* m_pagesWidget;

   /**
    * Preferences
    */
   Preferences* m_preferences;

  /**
    * Display settings e.g font, color etc.
    */
   PreferencesDisplay* m_displaySettings;

   /**
    * @brief m_microPvSettings
    */
   PreferencesMicroPv* m_microPvSettings;

   /**
     * m_solverOptionWidget.
     */
   PreferencesSolverOption* m_solverOptionWidget;

   /**
     * m_autosaveOptionWidget
     */
   PreferencesAutoSave* m_autoSaveOptionsWidget;

   /**
    * m_exportOptionsWidget
    */
   PreferencesExport* m_exportOptionsWidget;

   /**
    * Display settings
    */
   QListWidgetItem* m_displayButton;

   /**
    * SolverTableButton list widget
    */
   QListWidgetItem* m_solverTableButton;

   /**
    * @brief m_regionTableButton
    */
   QListWidgetItem* m_regionTableButton;

   /**
    * @brief m_microPvButton
    */
   QListWidgetItem* m_microPvButton;

   /**
    * @brief m_regionTableButton
    */
   QListWidgetItem* m_solverOptionButton;

   /**
    * @brief m_autoSafeOptionsButton
    */
   QListWidgetItem* m_autoSaveOptionsButton;

   QListWidgetItem* m_exportOptionsButton;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/

