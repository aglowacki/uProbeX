/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PYTHONSOLVERPROFILEWIDGET_H_
#define PYTHONSOLVERPROFILEWIDGET_H_

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

/*---------------------------------------------------------------------------*/

/**
 * @brief
 */
class PythonSolverProfileWidget
: public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor
    *
    * @param parent - the Qt parent widget
    */
   PythonSolverProfileWidget(QWidget* parent = 0);

   /**
    * Default destructor
    */
   ~PythonSolverProfileWidget();

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

   /**
    * @brief m_pythonSolverTable
    */
   SolverParameterWidget* m_pythonSolverWidget;

   int m_currentProfileIndex;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/

