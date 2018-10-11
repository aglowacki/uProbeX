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
#include <QDebug>
#include <solver/Solver.h>

const static int NM_SELECTED = 0;
const static int PY_SELECTED = 1;

/*---------------------------------------------------------------------------*/

PreferencesSolverOption::PreferencesSolverOption(
      QList<gstar::AbstractImageWidget*> windowList,
      Solver* solver,
      QWidget* parent) : QWidget(parent)
{

   m_windowList = windowList;
   m_solverWidget = NULL;
   m_transformer = new CoordinateTransformer();
   m_solverParameterParse = new SolverParameterParse();
   m_solver = solver;

   createSolverGroup();

   createLayOut();

   //initialState();

}

/*---------------------------------------------------------------------------*/

PreferencesSolverOption::~PreferencesSolverOption()
{

   if(m_solverParameterParse != NULL)
   {
      delete m_solverParameterParse;
   }
   m_solverParameterParse = NULL;

   if(m_solverWidget != NULL)
   {
      delete m_solverWidget;
   }
   m_solverWidget = NULL;

   if(m_transformer != NULL)
   {
      delete m_transformer;
   }
   m_transformer = NULL;

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
//   mainLayout->addRow(
//         new QLabel("The python solver must have solver and transform functions !"));
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

   return m_NMSolverWidget->getCoefficientAttrs();

}

/*---------------------------------------------------------------------------*/

QStringList PreferencesSolverOption::getNMOptionAttrs()
{

   return m_NMSolverWidget->getOptionAttrs();

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
      qDebug()<<"The coefList has invalid parameters";
      return;
   }
   m_solverParameterParse -> getCoefficientAndTransform(minCoefs, allCoefs);

   QStringList optionList = widget -> getOptionAttrs();
   if(false == m_solverParameterParse -> parseSolverOptionList(optionList))
   {
      qDebug()<<"The optionList has invalid parameters";
      return;
   }
   m_solverParameterParse -> getOptions(options);

}

/*---------------------------------------------------------------------------*/

void PreferencesSolverOption::runSolver()
{

   QMap<QString, double> minCoefs;
   QMap<QString, double> newMinCoefs;
   QMap<QString, double> allCoefs;
   QMap<QString, double> options;
   QList< QMap<QString,double> > coordPoints;

   AbstractSolver *impl = m_solver->getImpl();
   SolverParameterWidget* widget;

   if(impl != NULL)
   {
      m_solver->setImpl(NULL);
      delete impl;
      impl = NULL;
   }

   if (m_buttonGroup->checkedId() == NM_SELECTED)
   {
      widget = m_NMSolverWidget;

      m_solver->setImpl(new NelderMeadSolver());

   }
   else if(m_buttonGroup->checkedId() == PY_SELECTED)
   {
      widget = m_pythonSolverWidget;

      impl = new PythonSolver();
/*      QFileInfo fileInfo = QFileInfo(getPythonSolverPath());
      if(false ==((PythonSolver*)impl)->initialPythonSolver(fileInfo.path(),
                                                            fileInfo.baseName(),
                                                            "my_solver"))
      {
         delete impl;
         qDebug()<<"Error initializing python solver";
         QMessageBox::critical(NULL,"Error", "Error Initializing Python Solver!");
         return;
      }
*/
      m_solver->setImpl(impl);
   }

   getSolverPropertiesFromModel(widget,
                                allCoefs,
                                minCoefs,
                                options);

   m_transformer->Init(allCoefs);
   m_solver->setTransformer(m_transformer);
   m_solver->setAllCoef(allCoefs);
   m_solver->setOptions(options);
   m_solver->setMinCoef(minCoefs);

   if(m_windowList.size() > 0)
   {
      SWSWidget* sws = (SWSWidget*)m_windowList.at(0);
      sws->getMarkerCoordinatePoints(coordPoints);
   }
   else
   {
      QMessageBox::critical(NULL, "Solver Error", "Could not find active SWS workspace!");
      return;
   }

   m_solver->setCoordPoints(coordPoints);

   QApplication::setOverrideCursor(Qt::WaitCursor);
   bool retVal = m_solver->run();
   QApplication::restoreOverrideCursor();

   if(m_solverWidget != NULL)
      delete m_solverWidget;
   m_solverWidget = NULL;

   m_solverWidget = new SolverWidget();
   connect(m_solverWidget,
     SIGNAL(useUpdatedVariables(const QMap<QString, double>)),
     this,
     SLOT(useUpdatedSolverVariables(const QMap<QString, double> )));

   newMinCoefs = m_solver->getMinCoef();
   m_solverWidget->setCoefs(minCoefs, newMinCoefs);
   m_solverWidget->setStatusString(m_solver->getLastErrorMessage());

   if(retVal)
   {
      m_solverWidget->setUseBtnEnabled(true);
   }
   else
   {
      m_solverWidget->setUseBtnEnabled(false);
   }

   m_solverWidget->show();

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
