/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/SolverParameterWidget.h>
#include <solver/NelderMeadSolver.h>

#include <QSpinBox>
#include <QLabel>
#include <QFormLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QMessageBox>
#include <QDebug>


/*---------------------------------------------------------------------------*/

SolverParameterWidget::SolverParameterWidget(QWidget* parent) : QWidget(parent)
{

   m_coefficientTable = new SolverTable();
   m_optionTable = new SolverTable();

   connect(m_coefficientTable,
           SIGNAL(itemChanged()),
           this,
           SIGNAL(coefficientItemChanged()));

   connect(m_optionTable,
           SIGNAL(itemChanged()),
           this,
           SIGNAL(optionItemChanged()));

   QFormLayout* mainLayout = new QFormLayout();
   mainLayout->setContentsMargins(0, 0, 0, 0);
   mainLayout->addRow(new QLabel("Coefficient"), new QLabel("Parameters"));
   mainLayout->addRow(m_coefficientTable, m_optionTable);

   setLayout(mainLayout);

}

/*---------------------------------------------------------------------------*/

SolverParameterWidget::~SolverParameterWidget()
{

}

/*---------------------------------------------------------------------------*/

void SolverParameterWidget::addCoefficientItem(Attribute attr)
{

   m_coefficientTable -> addItem(attr);

}

/*---------------------------------------------------------------------------*/

void SolverParameterWidget::addCoefficientItems(QList<Attribute> solverAttrs)
{

   for(int i = 0; i < solverAttrs.size(); i++)
   {
      addCoefficientItem(solverAttrs[i]);
   }

}

/*---------------------------------------------------------------------------*/

void SolverParameterWidget::addOptionItem(Attribute attr)
{

   m_optionTable -> addItem(attr);

}

/*---------------------------------------------------------------------------*/

void SolverParameterWidget::addOptionItems(QList<Attribute> solverAttrs)
{

   for(int i = 0; i < solverAttrs.size(); i++)
   {
      addOptionItem(solverAttrs[i]);
   }

}

/*---------------------------------------------------------------------------*/

void SolverParameterWidget::removeCoefficientItems()
{

   m_coefficientTable -> removeItems();

}

/*---------------------------------------------------------------------------*/

void SolverParameterWidget::removeOptionItems()
{

   m_optionTable -> removeItems();

}

/*---------------------------------------------------------------------------*/

QStringList SolverParameterWidget::getCoefficientAttrs()
{

   return m_coefficientTable -> toStringList();

}

/*---------------------------------------------------------------------------*/

QList<Attribute> SolverParameterWidget::getCoefficientAttrsList()
{

   return m_coefficientTable -> getSolverAttrs();

}

/*---------------------------------------------------------------------------*/

QMap<QString, double> SolverParameterWidget::getCoefficientAttrsMap()
{
   return m_coefficientTable->toMap();
}

/*---------------------------------------------------------------------------*/

QMap<QString, double> SolverParameterWidget::getSelectedCoefficientAttrsMap()
{
   return m_coefficientTable->toSelectedMap();
}

/*---------------------------------------------------------------------------*/

QStringList SolverParameterWidget::getOptionAttrs()
{

   return m_optionTable -> toStringList();
}

/*---------------------------------------------------------------------------*/

QList<Attribute> SolverParameterWidget::getOptionAttrsList()
{

   return m_optionTable->getSolverAttrs();

}

/*---------------------------------------------------------------------------*/

QMap<QString, double> SolverParameterWidget::getOptionAttrsMap()
{
   return m_optionTable->toMap();
}

/*---------------------------------------------------------------------------*/

QMap<QString, double> SolverParameterWidget::getSelectedOptionAttrsMap()
{
   return m_optionTable->toSelectedMap();
}


/*---------------------------------------------------------------------------*/

void SolverParameterWidget::setButtionVisible(bool visible)
{

   m_coefficientTable->setAddButtionVisible(visible);
   m_optionTable->setAddButtionVisible(visible);

   m_coefficientTable->setDeleteButtionVisible(visible);
   m_optionTable->setDeleteButtionVisible(visible);

}

/*---------------------------------------------------------------------------*/

void SolverParameterWidget::setCoefficientAttrs(QStringList attrs)
{

   m_coefficientTable->fromStringList(attrs);

}

/*---------------------------------------------------------------------------*/

void SolverParameterWidget::setCoefficientAttrsFromList(
      QList<Attribute> attrList)
{

   m_coefficientTable->fromAttributeList(attrList);

}

/*---------------------------------------------------------------------------*/

void SolverParameterWidget::setDefaultCoefficient()
{

   addCoefficientItem(Attribute("m2xfm_x", "0.0", "m2xfm_x"));
   addCoefficientItem(Attribute("m2xfm_y", "0.0", "m2xfm_y"));
   addCoefficientItem(Attribute("angle_alpha", "0.0", "angle_alpha"));
   addCoefficientItem(Attribute("offset_a", "0.0", "offset_a"));
   addCoefficientItem(Attribute("offset_b", "0.0", "offset_b"));
   addCoefficientItem(Attribute("offset_c", "0.0", "offset_c"));
   addCoefficientItem(Attribute("offset_d", "0.0", "offset_d"));
   addCoefficientItem(Attribute("omega", "0.0", "omega"));
   addCoefficientItem(Attribute("omega_prime", "0.0", "omega_prime"));
   addCoefficientItem(Attribute("scaling_XFM_X", "0.0", "scaling_XFM_X"));
   addCoefficientItem(Attribute("scaling_XFM_Y", "0.0", "scaling_XFM_Y"));
   addCoefficientItem(Attribute("z_offset", "0.0", "z_offset"));
   addCoefficientItem(Attribute("z_lin_x", "0.0", "z_lin_x"));
   addCoefficientItem(Attribute("z_lin_y", "0.0", "z_lin_y"));

}

/*---------------------------------------------------------------------------*/

void SolverParameterWidget::setDefaultOption()
{

   addOptionItem(Attribute(NelderMeadSolver::STR_ITER,
                           "100000.0",
                           "Num Iterations"));
   addOptionItem(Attribute(NelderMeadSolver::STR_STEP_SIZE,
                           "0.01",
                           "Step Size"));

}

/*---------------------------------------------------------------------------*/

void SolverParameterWidget::setNMModel()
{

   m_coefficientTable->setNMModel();
   m_optionTable->setNMModel();

}

/*---------------------------------------------------------------------------*/

void SolverParameterWidget::setOptionAttrs(QStringList attrs)
{

   m_optionTable->fromStringList(attrs);

}

/*---------------------------------------------------------------------------*/

void SolverParameterWidget::setOptionAttrsFromList(
      QList<Attribute> attrList)
{

   m_optionTable->fromAttributeList(attrList);

}
