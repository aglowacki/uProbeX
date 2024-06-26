/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef AbstractSolver_H
#define AbstractSolver_H

#include <QMap>
#include <QString>

#include <gstar/ITransformer.h>
//---------------------------------------------------------------------------

/**
 * @brief Transformation Interface
 *
 */
class AbstractSolver
{

public:

   /**
    * Constructor.
    */
   AbstractSolver();

   /**
    * Destructor.
    */
   virtual ~AbstractSolver();

   /**
    * @brief getAllCoef
    * @return
    */\
   virtual QMap<QString, double> getAllCoef() = 0;

   /**
    * @brief getLastErrorMessage
    * @return
    */
   QString getLastErrorMessage();

   /**
    * @brief getMinCoef
    * @return
    */
   virtual QMap<QString, double> getMinCoef() = 0;

   /**
    * @brief getOptions
    * @return
    */
   virtual QMap<QString, double> getOptions() = 0;

   /**
    * @brief getTransformer
    * @return
    */
   virtual gstar::ITransformer* getTransformer() = 0;

   /**
    * @brief setAllCoef
    * @param vars
    */
   virtual void setAllCoef(QMap<QString, double> vars) = 0;

   /**
    * @brief setCoordPoints
    * @param vars
    */
   virtual void setCoordPoints(QList < QMap<QString,double> > vars) = 0;

   /**
    * @brief setMinCoef
    * @param vars
    */
   virtual void setMinCoef(QMap<QString, double> vars) = 0;

   /**
    * @brief setOptions
    * @param vars
    */
   virtual void setOptions(QMap<QString, double> vars) = 0;

   /**
    * @brief setTransformer
    * @param transformer
    */
   virtual void setTransformer(gstar::ITransformer* transformer) = 0;

   /**
    * @brief run
    * @return
    */
   virtual bool run() = 0;

protected:

   /**
    * @brief m_lastErrorMsg
    */
   QString m_lastErrorMsg;

   /**
    * m_dict_min_coef.
    */
   QMap<QString, double> m_dict_min_coef;

   /**
    * m_dict_options.
    */
   QMap<QString, double> m_dict_options;

};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
