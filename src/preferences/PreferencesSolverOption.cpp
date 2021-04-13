/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/PreferencesSolverOption.h>

#include <QSpinBox>
#include <QLabel>
#include <QFormLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QMessageBox>
#include <solver/Solver.h>

#include "core/defines.h"

const static int NM_SELECTED = 0;
const static int PY_SELECTED = 1;

/*---------------------------------------------------------------------------*/

PreferencesSolverOption::PreferencesSolverOption(
      QList<gstar::AbstractImageWidget*> windowList,
      QWidget* parent) : QWidget(parent)
{

   m_windowList = windowList;
   m_solverWidget = nullptr;
   m_transformer = new CoordinateTransformer();
   m_solverParameterParse = new SolverParameterParse();
   //m_solver = solver;

   createSolverGroup();

   createLayOut();

   //initialState();

}

/*---------------------------------------------------------------------------*/

PreferencesSolverOption::~PreferencesSolverOption()
{

   if(m_solverParameterParse != nullptr)
   {
      delete m_solverParameterParse;
   }
   m_solverParameterParse = nullptr;

   if(m_solverWidget != nullptr)
   {
      delete m_solverWidget;
   }
   m_solverWidget = nullptr;

   if(m_transformer != nullptr)
   {
      delete m_transformer;
   }
   m_transformer = nullptr;

}

/*---------------------------------------------------------------------------*/

void PreferencesSolverOption::createLayOut()
{

   QFormLayout* mainLayout = new QFormLayout();
   mainLayout->setSizeConstraint(QLayout::SetFixedSize);

   // General Solver
   mainLayout->addRow(m_radioGeneral);
   mainLayout->addRow(m_NMSolverWidget);

   // Python Solver
   mainLayout->addRow(m_radioPython);

//   mainLayout->addRow(m_lblPofileTable);
//   mainLayout->addRow(m_profileTable);

//   mainLayout->addRow(m_lblOpenPthon);

//   QHBoxLayout* hLayout = new QHBoxLayout();
//   hLayout->addWidget(m_openPythonButton);
//   hLayout->addWidget(m_lePythonPath);
//   mainLayout->addRow(hLayout);
//   //mainLayout->addRow(m_openPythonButton,m_lePythonPath);
//   //mainLayout->addRow(m_lblTitle);
//   // mainLayout->addRow(m_pythonFuncName);
//   mainLayout->addRow(new QLabel("The python solver must have solver and transform functions !"));
//   mainLayout->addRow(m_pythonSolverProfileWidget);

   mainLayout->addRow(m_btnRunSolver);

   setLayout(mainLayout);

}

/*---------------------------------------------------------------------------*/

void PreferencesSolverOption::createNelMinSolver()
{

   m_NMSolverWidget = new SolverParameterWidget();
   m_NMSolverWidget->setButtionVisible(false);

   m_NMSolverWidget->setDefaultCoefficient();
   m_NMSolverWidget->setDefaultOption();
   m_NMSolverWidget->setNMModel();

}

/*---------------------------------------------------------------------------*/

void PreferencesSolverOption::createPythonSolver()
{

   //m_pythonSolverProfileWidget = new PythonSolverProfileWidget();
   //m_pythonSolverWidget = m_pythonSolverProfileWidget -> getPythonSolverProfileWiget();

}

/*---------------------------------------------------------------------------*/

void PreferencesSolverOption::createSolverGroup()
{

   m_buttonGroup = new QButtonGroup();
   m_radioGeneral = new QRadioButton(tr("General Solver"));
   m_radioPython = new QRadioButton(tr("Python Solver"));

   m_buttonGroup->addButton(m_radioGeneral, NM_SELECTED);
   m_buttonGroup->addButton(m_radioPython, PY_SELECTED);
   m_buttonGroup->setExclusive(true);
   m_radioGeneral->setChecked(true);

   connect(m_buttonGroup,
           SIGNAL(buttonClicked(int)),
           this,
           SLOT(solverOptionSelected(int)));

   createNelMinSolver();
   createPythonSolver();

    m_btnRunSolver = new QPushButton("Run Solver");

     connect(m_btnRunSolver,
             SIGNAL(clicked()),
             this,
             SLOT(runSolver()));


     if(m_windowList.count() > 0)
     {
        m_btnRunSolver->setEnabled(true);
     }
     else
     {
        m_btnRunSolver->setEnabled(false);
     }

}

/*---------------------------------------------------------------------------*/

int PreferencesSolverOption::getCheckedID()
{

    return m_buttonGroup->checkedId();

}

/*---------------------------------------------------------------------------*/

QStringList PreferencesSolverOption::getNMCoefficientAttrs()
{
    if (m_NMSolverWidget != nullptr)
    {
        return m_NMSolverWidget->getCoefficientAttrs();
    }
    return QStringList();

}

/*---------------------------------------------------------------------------*/

QStringList PreferencesSolverOption::getNMOptionAttrs()
{
    if (m_NMSolverWidget != nullptr)
    {
         return m_NMSolverWidget->getOptionAttrs();
    }
    return QStringList();
}

/*---------------------------------------------------------------------------*/

QStringList PreferencesSolverOption::getCurrentCoefficientAttrs()
{

   return m_pythonSolverWidget->getCoefficientAttrs();

}

/*---------------------------------------------------------------------------*/

QStringList PreferencesSolverOption::getCurrentOptionAttrs()
{

   return m_pythonSolverWidget->getOptionAttrs();

}

/*---------------------------------------------------------------------------*/

void PreferencesSolverOption::getSolverPropertiesFromModel(
      SolverParameterWidget* widget,
      QMap<QString, double> &allCoefs,
      QMap<QString, double> &minCoefs,
      QMap<QString, double> &options)
{

   QStringList coefList = widget -> getCoefficientAttrs();

   if(false == m_solverParameterParse -> parseSolverCoefList(coefList))
   {
      logW<<"The coefList has invalid parameters";
      return;
   }
   m_solverParameterParse -> getCoefficientAndTransform(minCoefs, allCoefs);

   QStringList optionList = widget -> getOptionAttrs();
   if(false == m_solverParameterParse -> parseSolverOptionList(optionList))
   {
      logW<<"The optionList has invalid parameters";
      return;
   }
   m_solverParameterParse -> getOptions(options);

}

/*---------------------------------------------------------------------------*/

void PreferencesSolverOption::setCheckedID(int id)
{

    if(id == NM_SELECTED)
    {
        m_radioGeneral->setChecked(true);
    }
    else
    {
        m_radioPython->setChecked(true);
    }

    //solverOptionSelected(id);

}

/*---------------------------------------------------------------------------*/


void PreferencesSolverOption::setNMCoefficientAttrs(QStringList attrs)
{

   if(!attrs.empty())
   {
      m_NMSolverWidget->setCoefficientAttrs(attrs);
   }

}

/*---------------------------------------------------------------------------*/

void PreferencesSolverOption::setNMOptionAttrs(QStringList attrs)
{

   if(!attrs.empty())
   {
      m_NMSolverWidget->setOptionAttrs(attrs);
   }

}

/*---------------------------------------------------------------------------*/

void PreferencesSolverOption::useUpdatedSolverVariables(const QMap<QString, double> vals)
{

   QList<Attribute> attrList;
   SolverParameterWidget *widget;


   QMap<QString, double> oldMinCoefs;
   QMap<QString, double> allCoefs;
   QMap<QString, double> options;

   if (m_buttonGroup->checkedId() == NM_SELECTED)
   {
      widget = m_NMSolverWidget;
   }
   else if(m_buttonGroup->checkedId() == PY_SELECTED)
   {
      widget = m_pythonSolverWidget;
   }

   getSolverPropertiesFromModel(widget,
                                allCoefs,
                                oldMinCoefs,
                                options);


   for(const QString &key : allCoefs.keys())
   {
      Attribute atr(key, QString::number(allCoefs[key]), key, false);
      if(vals.contains(key))
      {
         atr.setIsEnable(true);
         atr.setValue(QString::number(vals[key]));
      }
      attrList.append(atr);
   }

   widget->setCoefficientAttrsFromList(attrList);

}

/*---------------------------------------------------------------------------*/
