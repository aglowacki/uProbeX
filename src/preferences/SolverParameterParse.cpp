/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <preferences/SolverParameterParse.h>
#include "core/defines.h"
//---------------------------------------------------------------------------

SolverParameterParse::SolverParameterParse()
: QObject(0)
{



}

//---------------------------------------------------------------------------

SolverParameterParse::~SolverParameterParse()
{


}

//---------------------------------------------------------------------------

void SolverParameterParse::getCoefficient(QMap<QString, double>& dict_min_coef)
{

   if(!dict_min_coef.empty())
      dict_min_coef.clear();

   dict_min_coef = m_dict_min_coef;

}

//---------------------------------------------------------------------------

void SolverParameterParse::getCoefficientAndTransform(
      QMap<QString, double>& dict_min_coef,
      QMap<QString, double>& dict_transform_coef)
{

   if(!dict_min_coef.empty())
      dict_min_coef.clear();

   if(!dict_transform_coef.empty())
      dict_transform_coef.clear();

   dict_min_coef = m_dict_min_coef;
   dict_transform_coef = m_dict_transform_coef;

}


//---------------------------------------------------------------------------

void SolverParameterParse::getTransform(QMap<QString, double>& dict_transform_coef)
{

   if(!dict_transform_coef.empty())
      dict_transform_coef.clear();

   dict_transform_coef = m_dict_transform_coef;

}

//---------------------------------------------------------------------------

void SolverParameterParse::getOptions(QMap<QString, double>& dict_options)
{

   if(!dict_options.empty())
      dict_options.clear();

   dict_options = m_dict_options;

}

//---------------------------------------------------------------------------

bool SolverParameterParse::parseSolverCoefList(QStringList& coefList)
{

   if(!m_dict_min_coef.empty())
      m_dict_min_coef.clear();

   if(!m_dict_transform_coef.empty())
      m_dict_transform_coef.clear();

   if(coefList.length() == 0)
   {
       logW << "Python Solver : No input for Coefficient. \n";
      return false;
   }

   bool xnameCheck = false;
   bool ynameCheck = false;
   for (int i = 0 ; i < coefList.length() ; i++)
   {
      QString attr = coefList.at(i);
      QStringList l = attr.split(",");
      if (l.size() != 4)  continue;

      for(int j = 0 ; j < i ; j++)
      {
         QString preAttr = coefList.at(j);
         QStringList m = preAttr.split(",");
         if (m.size() != 4)  continue;
         if(m.at(0) == l.at(0))
         {
            QMessageBox::critical(0, "Python Solver",
                                 "Multiple rows of coefficient have same name.");
            return false;
         }
      }

      if(l.at(0) == "m2xfm_x" )
      {
         xnameCheck = true;
      }
      if(l.at(0) == "m2xfm_y")
      {
         ynameCheck = true;
      }

      if(l.at(3).toInt() == 1)
      {
         m_dict_min_coef[l.at(0)] = l.at(1).toDouble();
      }

    m_dict_transform_coef[l.at(0)] = l.at(1).toDouble();
   }

   /*
   if(m_dict_min_coef.empty())
   {
      QMessageBox::critical(0, "Python Solver",
                           "No checked coefficient.");
      return false;
   }
   */

   if(m_dict_transform_coef.empty())
   {
      QMessageBox::critical(0, "Python Solver",
                           "No coefficient in input.");
      return false;
   }

   if (xnameCheck == false || ynameCheck ==false)
   {
       QMessageBox::critical(0, "Error", "Must have two conefficients m2xfm_x and m2xfm_y");
       return false;
   }

   return true;

}

//---------------------------------------------------------------------------

bool SolverParameterParse::parseSolverOptionList(QStringList& optionList)
{

   if(!m_dict_options.empty())
      m_dict_options.clear();

   if(optionList.length() == 0)
   {
       logW << "Python Solver : No input for Options.\n";
      return false;
   }

   for (int i = 0 ; i < optionList.length() ; i++)
   {
     QString attr = optionList.at(i);
     QStringList l = attr.split(",");
     if (l.size() != 4)  continue;

     for(int j = 0 ; j < i ; j++)
     {
        QString preAttr = optionList.at(j);
        QStringList m = preAttr.split(",");
        if (m.size() != 4)  continue;
        if(m.at(0) == l.at(0))
        {
           QMessageBox::critical(0, "Python Solver",
                                "Multiple rows of parameters have same name.");
           return false;
        }
     }

     if(l.at(3).toInt() == 1)
     {
        m_dict_options[l.at(0)] = l.at(1).toDouble();
     }
   }
   /*
   if(m_dict_options.empty())
   {
      QMessageBox::critical(0, "Python Solver",
                           "No parameters is checked or input.");
      return false;
   }
   */
   return true;

}

//---------------------------------------------------------------------------
