/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef PYTHON_TRANSFORMER_H
#define PYTHON_TRANSFORMER_H

/*---------------------------------------------------------------------------*/

#include <ITransformer.h>
//#include <python2.7/Python.h>

#include <QString>

/*---------------------------------------------------------------------------*/

/**
 * @brief The transformer that use python script
 *
 */
class PythonTransformer : public gstar::ITransformer
{

public:

   /**
    * Constructor.
    */
   PythonTransformer(QString path, QString filename, QString functionnName);

   /**
    * Destructor.
    */
   ~PythonTransformer();

   /**
    * @brief foundFunctions
    * @return
    */
   bool foundFunctions();

   /**
    * @brief getAllCoef
    * @return
    */
   virtual QMap<QString, double> getAllCoef();

   /**
    * @brief Init
    * @param globalVars
    * @return
    */
   virtual bool Init(QMap<QString, double> globalVars);

   /**
    * @brief getVariable
    * @param name
    * @param val
    * @return
    */
   virtual bool getVariable(QString name, double *val);

   /**
    * @brief setVariable
    * @param name
    * @param val
    * @return
    */
   virtual bool setVariable(QString name, double val);

   /**
    * @brief transformCommand
    * @param inX
    * @param inY
    * @param inZ
    * @param outX
    * @param outY
    */
   virtual void transformCommand(double inX,
                                 double inY,
                                 double inZ,
                                 double *outX,
                                 double *outY,
                                 double *outZ);

private:

   /**
    * @brief m_foundFuncs
    */
   bool m_foundFuncs;

   /**
    * @brief m_module
    */
   QString m_module;

   /**
    * @brief m_funcName
    */
   QString m_funcName;

   /**
    * @brief m_globalVars
    */
   QMap<QString, double> m_globalVars;
};

/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
