/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef Solver_H
#define Solver_H

#include <solver/AbstractSolver.h>

/*---------------------------------------------------------------------------*/

/**
 * @brief The solver class has the parameters for all the solver, it is the
 * wrapper for the NMSolver and Python Solver, has all the parameters the solver
 * need
 *
 */
class Solver
{

public:

   /**
    * Constructor.
    */
   Solver();

   /**
    * Destructor.
    */
   ~Solver();

   /**
    * @brief getAllCoef
    * @return
    */\
   QMap<QString, double> getAllCoef();

   /**
    * @brief getImpl
    * @return
    */
   AbstractSolver* getImpl();

   /**
    * @brief getLastErrorMessage
    * @return
    */
   QString getLastErrorMessage();

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
    * @brief setAllCoef
    * @param vars
    */
   void setAllCoef(QMap<QString, double> vars);

   /**
    * @brief setCoordPoints
    * @param vars
    */
   void setCoordPoints(QList < QMap<QString, double> > vars);

   /**
    * @brief setImpl
    * @param impl
    */
   void setImpl(AbstractSolver* impl);

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
   bool run();

private:

   /**
    * @brief m_impl
    */
   AbstractSolver* m_impl;

};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
