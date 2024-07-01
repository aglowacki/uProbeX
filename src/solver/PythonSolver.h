/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PYTHONSOLVER_H_
#define PYTHONSOLVER_H_

#include <solver/AbstractSolver.h>
#include <core/PythonLoader.h>

//---------------------------------------------------------------------------

/**
 * @brief PythonSolver the solver that use python script it will call the Python
 * function listed in the python script
 *
 */
class PythonSolver : public AbstractSolver
{

public:

   /**
    * Constructor.
    */
   PythonSolver();

   /**
    * Destructor.
    */
   ~PythonSolver();

   /**
    * @brief getAllCoef
    * @return
    */\
   QMap<QString, double> getAllCoef();

   /**
    * @brief getMinCoef
    * @return
    */
   QMap<QString, double> getMinCoef();

   /**
    * @brief getOptions
    * @return
    */
   QMap<QString, double> getOptions();

   /**
    * @brief getTransformer
    * @return
    */
   gstar::ITransformer* getTransformer();

   /**
    * initialPythonSolver.
    */
   bool initialPythonSolver(QString path, QString module, QString functionnName);

   /**
    * @brief setAllCoef
    * @param vars
    */
   void setAllCoef(QMap<QString, double> vars);

   /**
    * @brief setCoordPoints
    * @param vars
    */
   void setCoordPoints(QList < QMap<QString,double> > vars);

   /**
    * @brief setMinCoef
    * @param vars
    */
   void setMinCoef(QMap<QString, double> vars);

   /**
    * @brief setOptions
    * @param vars
    */
   void setOptions(QMap<QString, double> vars);

   /**
    * @brief setTransformer
    * @param transformer
    */
   void setTransformer(gstar::ITransformer* transformer);

   /**
    * @brief run
    * @return
    */
   virtual bool run();

private:

   /**
    * @brief m_module.
    */
   QString m_module;

   /**
    * @brief m_funcName.
    */
   QString m_funcName;

   /**
    * @brief m_dict_transform_coef.
    */
   QMap<QString, double> m_dict_transform_coef;

   /**
    * @brief m_list_coord_points.
    */
   QList < QList<double> >  m_list_coord_points;

};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------

