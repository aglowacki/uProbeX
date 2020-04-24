/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/PythonSolverProfileWidget.h>
#include <solver/NelderMeadSolver.h>

#include <QSpinBox>
#include <QLabel>
#include <QFormLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QMessageBox>

#include <core/PythonLoader.h>

/*---------------------------------------------------------------------------*/

PythonSolverProfileWidget::PythonSolverProfileWidget(QWidget* parent) : QDialog(parent)
{

   m_currentProfileIndex = 0;
    m_coordPoints = nullptr;
    m_solverWidget = nullptr;
   createCompontent();

   createLayOut();

}

/*---------------------------------------------------------------------------*/

PythonSolverProfileWidget::~PythonSolverProfileWidget()
{

}

/*---------------------------------------------------------------------------*/

void PythonSolverProfileWidget::addProfileItem(QString name, QString desc)
{

   m_currentProfileIndex = m_profiles.size();

   Profile profile;
   profile.setName(name);
   profile.setDescription(desc);

   m_profiles.append(profile);


   m_pythonSolverWidget -> removeCoefficientItems();
   m_pythonSolverWidget -> removeOptionItems();

}

/*---------------------------------------------------------------------------*/

void PythonSolverProfileWidget::coefficientItemChanged()
{
    if(m_profiles.size() > 0)
    {
        m_profiles[m_currentProfileIndex].setCoefficientAttrs(
                m_pythonSolverWidget -> getCoefficientAttrsList());
    }
}

/*---------------------------------------------------------------------------*/

void PythonSolverProfileWidget::createCompontent()
{

   m_profileTable = new ProfileTable();

   connect(m_profileTable,
           SIGNAL(addItem(QString, QString)),
           this,
           SLOT(addProfileItem(QString, QString)));
   connect(m_profileTable,
           SIGNAL(removeItem(int)),
           this,
           SLOT(removeProfileItem(int)));
   connect(m_profileTable,
           SIGNAL(editItem(int, QString)),
           this,
           SLOT(editProfileItem(int, QString)));
   connect(m_profileTable,
           SIGNAL(switchItem(const QItemSelection&, const QItemSelection& )),
           this,
           SLOT(switchProfileItem(const QItemSelection&, const QItemSelection&)));

   m_pythonSolverWidget = new SolverParameterWidget();
   connect(m_pythonSolverWidget,
           SIGNAL(coefficientItemChanged()),
           this,
           SLOT(coefficientItemChanged()));
   connect(m_pythonSolverWidget,
           SIGNAL(optionItemChanged()),
           this,
           SLOT(optionItemChanged()));

   QFont font;
   font.setBold(true);
   font.setWeight(75);

   m_lblPofileTable = new QLabel("Profile:");
   m_lblPofileTable->setFont(font);

   m_lblOpenPthon = new QLabel("Python Script Path:");
   m_lblOpenPthon->setFont(font);

   m_openPythonButton = new QPushButton(tr("Open Python File..."), this);
   connect(m_openPythonButton,
          SIGNAL(clicked()),
          this,
          SLOT(openPythonFile()));

   m_lblP = new QLabel("");
   m_lePythonPath = new QLineEdit("");

   m_lblTitle = new QLabel("Python Function Name:");
   m_lblTitle->setFont(font);

   m_pythonFuncName = new QLineEdit("");

}

/*---------------------------------------------------------------------------*/

void PythonSolverProfileWidget::createLayOut()
{

   QFormLayout* mainLayout = new QFormLayout();
   mainLayout->addRow(m_lblPofileTable);
   mainLayout->addRow(m_profileTable);

   mainLayout->addRow(m_lblOpenPthon);
   QHBoxLayout* hLayout = new QHBoxLayout();
   hLayout->addWidget(m_openPythonButton);
   hLayout->addWidget(m_lePythonPath);
   mainLayout->addRow(hLayout);
   //mainLayout->addRow(m_openPythonButton,m_lePythonPath);
   //mainLayout->addRow(m_lblTitle);
   // mainLayout->addRow(m_pythonFuncName);
   mainLayout->addRow(
         new QLabel("The python solver must have solver and transform functions !"));
   mainLayout->addRow(m_pythonSolverWidget);

   m_btnRunSolver = new QPushButton("Run Solver");
   connect(m_btnRunSolver,
           SIGNAL(pressed()),
           this,
           SLOT(runSolver()));
   mainLayout->addRow(m_btnRunSolver);

   QHBoxLayout* hLayout2 = new QHBoxLayout();
   m_btnSave = new QPushButton("Save");
   connect(m_btnSave,
           SIGNAL(pressed()),
           this,
           SLOT(accept()));
   m_btnCancel = new QPushButton("Cancel");
   connect(m_btnCancel,
           SIGNAL(pressed()),
           this,
           SLOT(reject()));
   hLayout2->addWidget(m_btnSave);
   hLayout2->addWidget(m_btnCancel);

   mainLayout->addRow(hLayout2);


   setLayout(mainLayout);

}

/*---------------------------------------------------------------------------*/

void PythonSolverProfileWidget::editProfileItem(int position, QString editValue)
{

   if(position == 0)
   {
      m_profiles[m_currentProfileIndex].setName(editValue);
   }

   if(position == 2)
   {
      m_profiles[m_currentProfileIndex].setDescription(editValue);
   }

}

/*---------------------------------------------------------------------------*/

int PythonSolverProfileWidget::getCurrentIndex()
{

   return m_currentProfileIndex;

}

/*---------------------------------------------------------------------------*/

QStringList PythonSolverProfileWidget::getPythonSolverName()
{

   QStringList str;

   for (int i = 0 ; i < m_profiles.size() ; i++)
   {
      Profile prof = m_profiles.at(i);
      str.append(QString("%1,%2").arg(prof.getName())
                                 .arg(prof.getFilePath()));
   }

   return str;

}

/*---------------------------------------------------------------------------*/

QString PythonSolverProfileWidget::getCurrentFileName()
{

   return m_filePath;

}

/*---------------------------------------------------------------------------*/

QStringList PythonSolverProfileWidget::getProfileAttrs()
{

   return m_profileTable->toStringList();

}

/*---------------------------------------------------------------------------*/

QStringList PythonSolverProfileWidget::getProfileCoefficientAttrs()
{

   QStringList sList;

   for (int i = 0 ; i < m_profiles.size() ; i++)
   {
      sList.append(m_profiles[i].attrsCoefficienttoString());
   }

   return sList;

}

/*---------------------------------------------------------------------------*/

QStringList PythonSolverProfileWidget::getProfileOptionAttrs()
{

   QStringList sList;

   for (int i = 0 ; i < m_profiles.size() ; i++)
   {
      sList.append(m_profiles[i].attrsOptiontoString());
   }

   return sList;

}

/*---------------------------------------------------------------------------*/

SolverParameterWidget* PythonSolverProfileWidget::getPythonSolverProfileWiget()
{

   return m_pythonSolverWidget;
}

/*---------------------------------------------------------------------------*/

void PythonSolverProfileWidget::openPythonFile()
{

   m_fileName = QFileDialog::getOpenFileName(this,
      "Open python script", ".",
         tr("py (*.py *.PY)"));

   // Dialog returns a nullptr string if user press cancel.
   if (m_fileName.isNull() || m_fileName.isEmpty()) return;

   m_fileInfo = QFileInfo(m_fileName);

   m_filePath = m_fileInfo.canonicalFilePath();

   m_lePythonPath->setText(m_filePath);

   if(m_profiles.size() < 1)
   {
        addProfileItem(m_fileName, "New Python");
   }

   m_profiles[m_currentProfileIndex].setFilePath(m_filePath);

   try
    {
        if(false == PythonLoader::inst()->isLoaded())
        {
            PythonLoader::inst()->init();
        }
        QMap<QString, QString> variables;
        //Coefs
        if(!PythonLoader::inst()-> loadFunction(m_fileInfo.path(), m_fileInfo.baseName(), "getCoefDict"))
        {
            QMessageBox::warning(nullptr, "Error loading variables", "Error calling getCoefDict in the script. You will have to manually enter them.");
            return;
        }
        PythonLoader::inst()->setRetCnt(m_fileInfo.baseName(), "getCoefDict", 1);
        PythonLoader::inst()->callFunc(m_fileInfo.baseName(), "getCoefDict", PythonLoader::RET_STR_DICT);
        QList<Attribute> coefList;
        if(PythonLoader::inst()->getRetStrDict(m_fileInfo.baseName(), "getCoefDict", &variables))
        {
            foreach (QString key, variables.keys())
            {
                logW<< key.toStdString() <<" : "<< variables[key].toStdString()<<"\n";
                ////play_attributes->addAttr(key, variables[key], "float", true);
                Attribute attr(key, variables[key], "", true);
                coefList.append(attr);
            }
            m_profiles[m_currentProfileIndex].setCoefficientAttrs(coefList);
            m_profileTable->setRow(m_currentProfileIndex);
            m_pythonSolverWidget->removeCoefficientItems();
            m_pythonSolverWidget->addCoefficientItems(m_profiles[m_currentProfileIndex].getCoefficientAttrs());
        }


        variables.clear();
        //Options
        if(!PythonLoader::inst()-> loadFunction(m_fileInfo.path(), m_fileInfo.baseName(), "getOptionsDict"))
        {
            QMessageBox::warning(nullptr, "Error loading options", "Error calling getOptionsDict in the script. You will have to manually enter them.");
            return;
        }
        PythonLoader::inst()->setRetCnt(m_fileInfo.baseName(), "getOptionsDict", 1);
        PythonLoader::inst()->callFunc(m_fileInfo.baseName(), "getOptionsDict", PythonLoader::RET_STR_DICT);
        QList<Attribute> optionsList;
        if(PythonLoader::inst()->getRetStrDict(m_fileInfo.baseName(), "getOptionsDict", &variables))
        {
            foreach (QString key, variables.keys())
            {
                logW<< key.toStdString() <<" : "<< variables[key].toStdString()<<"\n";
                Attribute attr(key, variables[key], "", true);
                optionsList.append(attr);
            }
            m_profiles[m_currentProfileIndex].setOptionAttrs(optionsList);
            m_pythonSolverWidget->removeOptionItems();
            m_pythonSolverWidget->addOptionItems(m_profiles[m_currentProfileIndex].getOptionAttrs());
        }

        m_profileTable->setRow(m_currentProfileIndex);

    }
    catch(PythonLoader::pyException ex)
    {
        logW<<ex.what();
        QMessageBox::warning(nullptr, "Error loading variables", "Error loading variables. You will have to manually enter them.");
    }
}

/*---------------------------------------------------------------------------*/

void PythonSolverProfileWidget::optionItemChanged()
{
    if(m_profiles.size() > 0)
    {
       m_profiles[m_currentProfileIndex].setOptionAttrs(
                m_pythonSolverWidget -> getOptionAttrsList());
    }
}

/*---------------------------------------------------------------------------*/

void PythonSolverProfileWidget::removeProfileItem(int selectRow)
{

   m_pythonSolverWidget -> removeCoefficientItems();
   m_pythonSolverWidget -> removeOptionItems();

   m_profiles.removeAt(selectRow);

   // Check if current profile index needs to be updated.
   if (selectRow == m_profiles.size())
   {
      m_currentProfileIndex = selectRow - 1;
   }
   else
   {
      m_currentProfileIndex = selectRow;
   }

   if (m_currentProfileIndex < 0)
   {
      return;
   }

   setPythonSolverName(m_profiles[m_currentProfileIndex].getFilePath());

   m_pythonSolverWidget ->
         addCoefficientItems(m_profiles[m_currentProfileIndex].getCoefficientAttrs());

   m_pythonSolverWidget ->
         addOptionItems(m_profiles[m_currentProfileIndex].getOptionAttrs());


}

/*---------------------------------------------------------------------------*/

void PythonSolverProfileWidget::setCoefficientAttrs(QStringList attrs)
{

   for (int i = 0 ; i < attrs.length() ; i++)
   {
       if(m_profiles.size() > 0)
        {
            m_profiles[i].coefficientfromString(attrs[i]);
        }
   }
}

/*---------------------------------------------------------------------------*/

void PythonSolverProfileWidget::setCurrentIndex(int index)
{

   m_currentProfileIndex = index;
   m_profileTable->setRow(m_currentProfileIndex);

}

/*---------------------------------------------------------------------------*/

void PythonSolverProfileWidget::setPythonOptionAttrs(QStringList attrs)
{

   for (int i = 0 ; i < attrs.length() ; i++)
   {
       if(m_profiles.size() > 0)
        {
            m_profiles[i].optionfromString(attrs[i]);
       }
   }

}

/*---------------------------------------------------------------------------*/

void PythonSolverProfileWidget::setPythonSolverNameList(QStringList fileNameList)
{

   for (int i = 0 ; i < fileNameList.length() ; i++)
   {
      QString attr = fileNameList.at(i);
      QStringList l = attr.split(",");
      if (l.size() != 2)  continue;

      if(m_profiles.size() > i)
      {
          if(l.at(0) == m_profiles[i].getName())
          {
             m_profiles[i].setFilePath(l.at(1));
          }
      }
   }

}

/*---------------------------------------------------------------------------*/

void PythonSolverProfileWidget::setPythonSolverPofileList(QStringList list)
{

   m_profileTable->fromStringList(list);

   for (int i = 0 ; i < list.length() ; i++)
   {
      QString attr = list.at(i);
      QStringList l = attr.split(",");
      if (l.size() != 4)  continue;

      addProfileItem(l.at(0), l.at(2));
   }

}

/*---------------------------------------------------------------------------*/

void PythonSolverProfileWidget::setPythonSolverName(QString fileName)
{

   m_fileName = fileName;

//   if (m_fileName.isNull() || m_fileName.isEmpty()) return;

   m_fileInfo = QFileInfo(m_fileName);

   m_filePath = m_fileInfo.canonicalFilePath();

   m_lePythonPath->setText(m_filePath);

}

/*---------------------------------------------------------------------------*/

void PythonSolverProfileWidget::switchProfileItem(const QItemSelection& selected,
                                                  const QItemSelection& deselected)
{

   QModelIndexList list = selected.indexes();

   if(list.empty())
   {
//      logW<<"the index list is empty";
      return;
   }

   Q_UNUSED(deselected);

   m_pythonSolverWidget -> removeCoefficientItems();
   m_pythonSolverWidget -> removeOptionItems();


   QModelIndex index = list.at(0);

   m_currentProfileIndex = index.row();

   setPythonSolverName(m_profiles[m_currentProfileIndex].getFilePath());

   m_pythonSolverWidget ->
         addCoefficientItems(m_profiles[m_currentProfileIndex].getCoefficientAttrs());

   m_pythonSolverWidget ->
         addOptionItems(m_profiles[m_currentProfileIndex].getOptionAttrs());

}

/*---------------------------------------------------------------------------*/

void PythonSolverProfileWidget::runSolver()
{
    if(m_coordPoints == nullptr)
    {
     return;
    }

   emit solverStart();

    if(m_solver == nullptr)
    {
        m_solver = new PythonSolver();
        if(false == m_solver->initialPythonSolver(m_filePath, m_fileInfo.baseName(), "my_solver"))
        {
         QMessageBox::warning(nullptr, "Error loading python solver", "Could not load function my_solver() in python script");
         return;
        }
    }

   m_solver->setCoordPoints(*m_coordPoints);

   QMap<QString, double> newMinCoefs;
   QMap<QString, double> minCoefs = m_pythonSolverWidget->getSelectedCoefficientAttrsMap();

   m_solver->setAllCoef(m_pythonSolverWidget->getCoefficientAttrsMap());
   m_solver->setOptions(m_pythonSolverWidget->getOptionAttrsMap());
   m_solver->setMinCoef(minCoefs);



   QApplication::setOverrideCursor(Qt::WaitCursor);
   bool retVal = m_solver->run();
   QApplication::restoreOverrideCursor();

   if(m_solverWidget != nullptr)
      delete m_solverWidget;
   m_solverWidget = nullptr;

   m_solverWidget = new SolverWidget();
   connect(m_solverWidget,
     SIGNAL(useUpdatedVariables(const QMap<QString, double>)),
     this,
     SLOT(useUpdatedSolverVariables(const QMap<QString, double> )));

   connect(m_solverWidget,
     SIGNAL(cancelUpdatedVariables()),
     this,
     SLOT(cancelUpdatedSolverVariables()));

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


