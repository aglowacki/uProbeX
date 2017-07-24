/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef SOLVERPARAMETERPARSE_H
#define SOLVERPARAMETERPARSE_H

/*---------------------------------------------------------------------------*/

#include<QMap>
#include<QString>
#include <QObject>
#include<QMessageBox>

/*---------------------------------------------------------------------------*/

/**
 * @brief SolverParameterParse parse the list of attributes got from the table
 * and parse it into min coef and options. The input would QStringList and the
 * output would be QMap
 */
class SolverParameterParse
: public QObject
{

   Q_OBJECT

public:

   /**
    * Constructor
    */
   SolverParameterParse();

   /**
    * Default destructor
    */
   ~SolverParameterParse();

   /**
    * @brief getCoefficient
    * @param dict_min_coef
    */
   void getCoefficient(QMap<QString, double>& dict_min_coef);
   /**
    * @brief getCoefficientAndTransform
    * @param dict_min_coef
    * @param dict_transform_coef
    */
   void getCoefficientAndTransform(QMap<QString, double>& dict_min_coef,
                                   QMap<QString, double>& dict_transform_coef);
   /**
    * @brief getTransform
    * @param dict_transform_coef
    */
   void getTransform(QMap<QString, double>& dict_transform_coef);

   /**
    * @brief getOptions
    * @param dict_options
    */
   void getOptions(QMap<QString, double>& dict_options);

   /**
    * @brief getCoefficientAndTransform
    *
    * @param coefList
    */
   bool parseSolverCoefList(QStringList& coefList);

   /**
    * @brief parseSolverOptionList
    * @param optionList
    */
   bool parseSolverOptionList(QStringList& optionList);

private:

   /**
    * @brief m_dict_min_coef
    */
   QMap<QString, double> m_dict_min_coef;

   /**
    * @brief m_dict_transform_coef
    */
   QMap<QString, double> m_dict_transform_coef;

   /**
    * @brief m_dict_options
    */
   QMap<QString, double> m_dict_options;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
