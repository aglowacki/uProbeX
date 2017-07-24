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
#include <QDebug>


/*---------------------------------------------------------------------------*/

PythonSolverProfileWidget::PythonSolverProfileWidget(QWidget* parent) : QWidget(parent)
{

   m_currentProfileIndex = 0;

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

   m_profiles[m_currentProfileIndex].setCoefficientAttrs(
            m_pythonSolverWidget -> getCoefficientAttrsList());

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

   // Dialog returns a null string if user press cancel.
   if (m_fileName.isNull() || m_fileName.isEmpty()) return;

   m_fileInfo = QFileInfo(m_fileName);

   m_filePath = m_fileInfo.canonicalFilePath();

   m_lePythonPath->setText(m_filePath);

   m_profiles[m_currentProfileIndex].setFilePath(m_filePath);

}

/*---------------------------------------------------------------------------*/

void PythonSolverProfileWidget::optionItemChanged()
{

   m_profiles[m_currentProfileIndex].setOptionAttrs(
            m_pythonSolverWidget -> getOptionAttrsList());

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
      m_profiles[i].coefficientfromString(attrs[i]);
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
      m_profiles[i].optionfromString(attrs[i]);
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

      if(l.at(0) == m_profiles[i].getName())
      {
         m_profiles[i].setFilePath(l.at(1));
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
//      qDebug()<<"the index list is empty";
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




