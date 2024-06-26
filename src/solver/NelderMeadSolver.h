/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef NELDER_MEAD_SOLVER_H
#define NELDER_MEAD_SOLVER_H

#include <solver/AbstractSolver.h>

//---------------------------------------------------------------------------

/**
 * @brief NM solver using NM algorithm to do the solver calculation
 *
 */
class NelderMeadSolver : public AbstractSolver
{

public:

   struct LightPoint
   {
      double lx;
      double ly;
      double lz;
      double mx;
      double my;
   };

   /**
    * Constructor.
    */
   NelderMeadSolver();

   /**
    * Destructor.
    */
   ~NelderMeadSolver();

   /**
    * @brief getNumPoints
    * @return
    */
   unsigned int getNumPoints();

   /**
    * @brief getPoints
    * @return
    */
   NelderMeadSolver::LightPoint* getPoints();

   /**
    * @brief getVarIndexes
    * @return
    */
   QMap<QString, int> getVarIndexes();

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
   bool run();

public:

   /**
    * @brief STR_ITER
    */
   static const QString STR_ITER;

   /**
    * @brief STR_STEP_SIZE
    */
   static const QString STR_STEP_SIZE;

private:

   /**
    * @brief m_mins
    */
   double *m_mins;

   /**
    * @brief m_nvars
    */
   int m_nvars;

   /**
    * @brief m_points
    */
   struct LightPoint* m_points;

   /**
    * @brief m_minError
    */
   double m_minError;

   /**
    * @brief m_numPoints
    */
   unsigned int m_numPoints;

   /**
    * @brief m_varIndexMap
    */
   QMap<QString, int> m_varIndexMap;

   /**
    * @brief m_transformer
    */
   gstar::ITransformer *m_transformer;

};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------

