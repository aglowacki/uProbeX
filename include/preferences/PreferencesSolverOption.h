/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PREFERENCESSOLVEROPTION_H_
#define PREFERENCESSOLVEROPTION_H_

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include <QButtonGroup>
#include <QAction>
#include <QPushButton>
#include <QFileDialog>
#include <QRadioButton>

#include <preferences/SolverTable.h>

#include <preferences/SolverParameterWidget.h>
#include <preferences/SolverParameterParse.h>
#include <preferences/PythonSolverProfileWidget.h>
#include <solver/PythonSolver.h>
#include <solver/NelderMeadSolver.h>
#include <core/SWSWidget.h>
#include <mvc/SolverWidget.h>
#include <solver/CoordinateTransformer.h>


class QSpinBox;
class NelderMeadSolver;
class SolverWidget;
class CoordinateTransformer;
class SolverAttributesModel;
class SolverAttributesGroup;

namespace gstar
{

  class Splash;

}

/*---------------------------------------------------------------------------*/

/**
 * @brief Preferences dialog page for handling the switching of the solver. It
 * swiches from different solvers and eable the corresponding widget for running
 * either NMSolver or PythonSolver.
 */
class PreferencesSolverOption
: public QWidget
{

   Q_OBJECT

public:

   /**
    * Constructor
    *
    * @param parent - the Qt parent widget
    */
   PreferencesSolverOption(QList<gstar::AbstractImageWidget*> windowList,
                           Solver* solver,
                           QWidget* parent = 0);

   /**
    * Default destructor
    */
   ~PreferencesSolverOption();

   /**
    * @brief createSolver
    * @param solver
    */
   //static void createSolver(Solver* solver);

   /**
    * @brief getPythonSolverName
    */
   QStringList getPythonSolverName();

   /**
    * @brief getPythonSolverPofile
    */
   QStringList getPythonSolverPofile();

   /**
    * @brief getPythonSolverPath
    */
   QString getPythonSolverPath();

   /**
    * @brief getCheckedID
    */
   int getCheckedID();

   /**
    * @brief getNMCoefficientAttrs
    */
   QStringList getNMCoefficientAttrs();

   /**
    * @brief getNMOptionAttrs
    */
   QStringList getNMOptionAttrs();

   /**
    * @brief getSolverPropertiesFromModel
    * @param widget
    * @param allCoefs
    * @param minCoefs
    * @param options
    */
   void getSolverPropertiesFromModel(SolverParameterWidget* widget,
                                     QMap<QString, double> &allCoefs,
                                     QMap<QString, double> &minCoefs,
                                     QMap<QString, double> &options);

   /**
    * @brief getPythonCoefficientAttrs
    */
   QStringList getCurrentCoefficientAttrs();

   /**
    * @brief getPythonCoefficientAttrs
    */
   int getCurrentIndex();

   /**
    * @brief getCurrentOptionAttrs
    */
   QStringList getCurrentOptionAttrs();

   /**
    * @brief getCurrentFileName
    */
   QString getCurrentFileName();

   /**
    * @brief getPythonCoefficientAttrs
    */
   QStringList getPythonCoefficientAttrs();

   /**
    * @brief getPythonOptionAttrs
    */
   QStringList getPythonOptionAttrs();

   /**
    * @brief createPythonSolver
    */
   void initialState();

   /**
    * @brief setCurrentFileName
    * @param fileName
    */
   void setCurrentFileName(QString fileName);

   /**
    * @brief setCheckedID
    */
   void setCheckedID(int id);

   /**
    * @brief setCurrentCoefficientAttrs
    */
   void setCurrentCoefficientAttrs(QStringList attrs);

   /**
    * @brief setCurrentOptionAttrs
    * @param attrs
    */
   void setCurrentOptionAttrs(QStringList attrs);

   /**
    * @brief setCurrentIndex
    * @param index
    */
   void setCurrentIndex(int index);

   /**
    * @brief setNMCoefficientAttrs
    */
   void setNMCoefficientAttrs(QStringList attrs);

   /**
    * @brief setNMOptionAttrs
    */
   void setNMOptionAttrs(QStringList attrs);

   /**
    * @brief setPythonCoefficientAttrs
    */
   void setPythonCoefficientAttrs(QStringList attrs);

   /**
    * @brief setPythonOptionAttrs
    */
   void setPythonOptionAttrs(QStringList attrs);

   /**
    * @brief setPythonSolverName
    */
   void setPythonSolverName(QStringList fileNameList);

   /**
    * @brief setPythonSolverPofile
    * @param attrs
    */
   void setPythonSolverPofile(QStringList attrs);

private:

   /**
    * @brief createLayOut
    */
   void createLayOut();

   /**
    * @brief createNelMinSolver
    */
   void createNelMinSolver();

   /**
    * @brief createPythonSolver
    */
   void createPythonSolver();

   /**
    * @brief createSolverGroup
    */
   void createSolverGroup();

private slots:

   /**
    * @brief runSolver
    */
   void runSolver();

//   /**
//    * @brief openPythonFile
//    */
//   void openPythonFile();

   /**
    * @brief Solver option
    * @param vals
    */
   void solverOptionSelected(int value);

   /**
    * @brief useUpdatedSolverVariables
    * @param vals
    */
   void useUpdatedSolverVariables(const QMap<QString, double> vals);

private:

   /**
    * @brief m_transformer
    */
   CoordinateTransformer* m_transformer;

   /**
    * @brief m_coordTransformer
    */
   NelderMeadSolver* m_errorMinimizer;

//   /**
//    * @brief m_profileTable
//    */
//   ProfileTable* m_profileTable;

   /**
    * @brief Radio button group.
    */
   PythonSolverProfileWidget* m_pythonSolverProfileWidget;

   /**
    * @brief Radio button group.
    */
   QButtonGroup* m_buttonGroup;

   /**
    * @brief m_windowList
    */
   QList<gstar::AbstractImageWidget*> m_windowList;

   /**
    * @brief m_btnRunSolver
    */
   QPushButton* m_btnRunSolver;

//   /**
//    * @brief m_openPythonAction
//    */
//   QPushButton* m_openPythonButton;

   /**
    * @brief Radio button for simulate camera
    */
   QRadioButton* m_radioGeneral;

   /**
    * @brief Radio button for 32 ID camera
    */
   QRadioButton* m_radioPython;

//   /**
//    * @brief m_lblOpenPthon
//    */
//   QLabel* m_lblOpenPthon;

//   /**
//    * @brief m_lblP
//    */
//   QLabel* m_lblP;

//   /**
//    * @brief m_lblPofileTable
//    */
//   QLabel* m_lblPofileTable;

//   /**
//    * @brief m_lblTitle
//    */
//   QLabel* m_lblTitle;

//   /**
//    * @brief m_pythonFuncName
//    */
//   QLineEdit* m_pythonFuncName;

//   /**
//    * @brief m_lePythonPath
//    */
//   QLineEdit* m_lePythonPath;

//   /**
//    * @brief m_fileName
//    */
//   QString m_fileName;

//   /**
//    * @brief m_filePath
//    */
//   QString m_filePath;

//   /**
//    * @brief m_fileInfo
//    */
//   QFileInfo m_fileInfo;

   /**
    * @brief m_solver
    */
   Solver* m_solver;

   /**
    * @brief m_solverParameterParse
    */
   SolverParameterParse* m_solverParameterParse;

   /**
    * @brief m_NMSolverWidget
    */
   SolverParameterWidget* m_NMSolverWidget;

   /**
    * @brief m_pythonSolverWidget
    */
   SolverParameterWidget* m_pythonSolverWidget;

   /**
    * @brief m_solverWidget
    */
   SolverWidget* m_solverWidget;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/

