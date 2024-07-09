/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef LinearSolver_H
#define LinearSolver_H

#include <solver/AbstractSolver.h>
//---------------------------------------------------------------------------

/**
 * @brief Transformation Interface
 *
 */
class LinearSolver : public AbstractSolver
{

public:

   /**
    * Constructor.
    */
   LinearSolver();

   /**
    * Destructor.
    */
   virtual ~LinearSolver();

   /**
    * @brief getAllCoef
    * @return
    */
   virtual QMap<QString, double> getAllCoef();

   /**
    * @brief getLastErrorMessage
    * @return
    */
   QString getLastErrorMessage();

   /**
    * @brief getMinCoef
    * @return
    */
   virtual QMap<QString, double> getMinCoef();

   /**
    * @brief getOptions
    * @return
    */
   virtual QMap<QString, double> getOptions();

   /**
    * @brief getTransformer
    * @return
    */
   virtual gstar::ITransformer* getTransformer();

   /**
    * @brief setAllCoef
    * @param vars
    */
   virtual void setAllCoef(QMap<QString, double> vars);

   /**
    * @brief setCoordPoints
    * @param vars
    */
   virtual void setCoordPoints(QList < QMap<QString,double> > vars);

   /**
    * @brief setMinCoef
    * @param vars
    */
   virtual void setMinCoef(QMap<QString, double> vars);

   /**
    * @brief setOptions
    * @param vars
    */
   virtual void setOptions(QMap<QString, double> vars);

   /**
    * @brief setTransformer
    * @param transformer
    */
   virtual void setTransformer(gstar::ITransformer* transformer);

   /**
    * @brief run
    * @return
    */
   virtual bool run();

protected:

   /**
    * m_dict_min_coef.
    */
   QMap<QString, double> _dict_min_coef;

   /**
    * m_dict_options.
    */
   QMap<QString, double> _dict_options;

   gstar::ITransformer *_transformer;

};

//---------------------------------------------------------------------------

#endif //LinearSolver_H

//---------------------------------------------------------------------------
