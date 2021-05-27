/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SOLVER_PROFILE_WIDGET_H_
#define SOLVER_PROFILE_WIDGET_H_

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include <QAction>
#include <QPushButton>
#include <QFileDialog>
#include <QList>


#include <preferences/SolverTable.h>
#include <preferences/ProfileTable.h>
#include <preferences/Profile.h>
#include <preferences/SolverParameterWidget.h>
#include <solver/PythonSolver.h>
#include <mvc/SolverWidget.h>

/*---------------------------------------------------------------------------*/

/**
 * @brief
 */
class SolverProfileWidget
: public QDialog
{

   Q_OBJECT

public:

   /**
    * Constructor
    *
    * @param parent - the Qt parent widget
    */
   SolverProfileWidget(QWidget* parent = 0);

   /**
    * Default destructor
    */
   ~SolverProfileWidget();

   /**
    * @brief getCurrentIndex
    */
   int getCurrentIndex();

   QStringList getProfileAttrs();

   /**
    * @brief getPythonSolverName
    */
   QStringList getProfileCoefficientAttrs();

   /**
    * @brief getProfileOptionAttrs
    */

   QStringList getProfileOptionAttrs();

   /**
    * @brief getPythonSolverName
    */
   QStringList getPythonSolverName();

   /**
    * @brief getCurrentFileName
    */
   QString getCurrentFileName();

   /**
    * @brief getPythonSolverProfileWiget
    */
   SolverParameterWidget* getPythonSolverProfileWiget();

   /**
    * @brief setCoefficientAttrs
    * @param attrs
    */
   void setCoefficientAttrs(QStringList attrs);

   /**
    * @brief setCurrentIndex
    * @param index
    */
   void setCurrentIndex(int index);

   /**
    * @brief setPythonOptionAttrs
    * @param attrs
    */
   void setPythonOptionAttrs(QStringList attrs);

   /**
    * @brief setPythonSolverNameList
    * @param fileNameList
    */
   void setPythonSolverNameList(QStringList fileNameList);

   /**
    * @brief setPythonSolverName
    * @param fileName
    */
   void setPythonSolverName(QString fileName);

   /**
    * @brief setPythonSolverPofileList
    * @param list
    */
   void setPythonSolverPofileList(QStringList list);

   void setCoordinatePoints( QList< QMap<QString, double> > * points){m_coordPoints = points;}

signals:

   /**
    * @brief SolverStart
    */
   void solverStart();

   /**
    * @brief SolverVariableUpdate
    * @param valX
    * @param valY
    */
   void solverVariableUpdate(double valX, double valY);

   /**
    * @brief CancelSolverVariableUpdate
    */
   void cancelSolverVariableUpdate();

private:

   /**
    * @brief createCompontent
    */
   void createCompontent();

   /**
    * @brief createLayOut
    */
   void createLayOut();

private slots:

   /**
    * @brief addProfileItem
    */
   void addProfileItem(QString name, QString desc);

   /**
    * @brief coefficientItemChanged
    */
   void coefficientItemChanged();

   /**
    * @brief addProfileItem
    */
   void editProfileItem(int, QString);

   /**
    * @brief optionItemChanged
    */
   void optionItemChanged();

   /**
    * @brief openPythonFile
    */
   void openPythonFile();

   /**
    * @brief removeProfileItem
    */
   void removeProfileItem(int);

   /**
    * @brief switchProfileItem
    */
   void switchProfileItem(const QItemSelection &, const QItemSelection &);

   void runSolver();

private:

   /**
    * @brief m_profileTable
    */
   ProfileTable* m_profileTable;

   /**
    * @brief m_openPythonAction
    */
   QPushButton* m_openPythonButton;

   /**
    * @brief m_lblOpenPthon
    */
   QLabel* m_lblOpenPthon;

   /**
    * @brief m_lblP
    */
   QLabel* m_lblP;

   /**
    * @brief m_lblPofileTable
    */
   QLabel* m_lblPofileTable;

   /**
    * @brief m_lblTitle
    */
   QLabel* m_lblTitle;

   /**
    * @brief m_pythonFuncName
    */
   QLineEdit* m_pythonFuncName;

   /**
    * @brief m_lePythonPath
    */
   QLineEdit* m_lePythonPath;

   /**
    * @brief m_profiles
    */
   QList<Profile> m_profiles;

   /**
    * @brief m_fileName
    */
   QString m_fileName;

   /**
    * @brief m_filePath
    */
   QString m_filePath;

   /**
    * @brief m_fileInfo
    */
   QFileInfo m_fileInfo;

   QPushButton *m_btnRunSolver;

   QPushButton *m_btnSave;

   QPushButton *m_btnCancel;

   /**
    * @brief m_pythonSolverTable
    */
   SolverParameterWidget* m_pythonSolverWidget;

   PythonSolver *m_solver;

   QList< QMap<QString, double> > *m_coordPoints;

   SolverWidget *m_solverWidget;

   int m_currentProfileIndex;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/

