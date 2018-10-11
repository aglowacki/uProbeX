/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <solver/PythonSolver.h>
#include <string>
#include <QDebug>

using gstar::ITransformer;

/*---------------------------------------------------------------------------*/

PythonSolver::PythonSolver() : AbstractSolver()
{



}

/*---------------------------------------------------------------------------*/

PythonSolver::~PythonSolver()
{



}

/*---------------------------------------------------------------------------*/

QMap<QString, double> PythonSolver::getAllCoef()
{

   return m_dict_transform_coef;

}

/*---------------------------------------------------------------------------*/

QMap<QString, double> PythonSolver::getMinCoef()
{

   return m_dict_min_coef;

}

/*---------------------------------------------------------------------------*/

QMap<QString, double> PythonSolver::getOptions()
{

   return m_dict_options;

}

/*---------------------------------------------------------------------------*/

gstar::ITransformer* PythonSolver::getTransformer()
{

   return NULL;

}

/*---------------------------------------------------------------------------*/

bool PythonSolver::initialPythonSolver(QString path,
                                       QString module,
                                       QString functionnName)
{

   try
   {
      if(false == PythonLoader::inst()->isLoaded())
      {
         PythonLoader::inst()->init();
      }

      m_module = module;
      m_funcName = functionnName;

      if(false == PythonLoader::inst()->loadFunction(
               path, module, functionnName))
      {
         qDebug()<<"Failed to load function: "<<module<<" "<<functionnName;
         return false;
      }

      if(false == PythonLoader::inst()->setNumArgs(
               m_module, m_funcName, 4))
      {
         qDebug()<<"Failed to set argument count";
         return false;
      }

   }
   catch(PythonLoader::pyException px)
   {
      QString er = QString(px.what());
      er += QString("\r\n Path: "+path
                    +"\r\nModule: "+module+
                    "\r\nFunction Name: "+functionnName);
      QMessageBox::critical(NULL, "PythonSolver Error", er);
      return false;
   }

   return true;
}

/*---------------------------------------------------------------------------*/

bool PythonSolver::run()
{

   try
   {
      PythonLoader::inst()->callFunc(m_module, m_funcName, PythonLoader::RET_DOUBLE_DICT);
      PythonLoader::inst()->getRetDict(m_module, m_funcName, &m_dict_min_coef);
   }
   catch(PythonLoader::pyException ex)
   {
      this->m_lastErrorMsg = ex.what();
      //QMessageBox::critical(NULL, "Error", ex.what());
      qDebug()<<ex.what();
      return false;
   }

   this->m_lastErrorMsg = "Finished";

   return true;

}

/*---------------------------------------------------------------------------*/

void PythonSolver::setAllCoef(QMap<QString, double> vars)
{

   m_dict_transform_coef = vars;

   if(false == PythonLoader::inst()->setArgDict(
            m_module, m_funcName, 2, m_dict_transform_coef))
   {
      qDebug()<<"PythonSolver::setAllCoef Failed to set dict argument count";
   }

}

/*---------------------------------------------------------------------------*/

void PythonSolver::setCoordPoints(QList < QMap<QString,double> > vars)
{

   m_list_coord_points.clear();
   for(auto &point : vars)
   {
      QList<double> cPoint;
      cPoint.append(point["Lx"]);
      cPoint.append(point["Ly"]);
      cPoint.append(point["Lz"]);
      cPoint.append(point["Mx"]);
      cPoint.append(point["My"]);

      m_list_coord_points.append(cPoint);
   }

   if(false == PythonLoader::inst()->setArgTupleTuples(
            m_module, m_funcName, 3, m_list_coord_points))
   {
      qDebug()<<"PythonSolver::setCoordPoints: Failed to set tuple of tuples argument count";
   }

}

/*---------------------------------------------------------------------------*/

void PythonSolver::setMinCoef(QMap<QString, double> vars)
{

   m_dict_min_coef = vars;

   if(false == PythonLoader::inst()->setArgDict(
            m_module, m_funcName, 0, m_dict_min_coef))
   {
      qDebug()<<"PythonSolver::setMinCoef: Failed to set dict argument count";
   }

   if(false == PythonLoader::inst()->setRetCnt(
         m_module, m_funcName, m_dict_min_coef.size()))
   {
      qDebug()<<"PythonSolver::setMinCoef: Failed to set argument count";
   }

}

/*---------------------------------------------------------------------------*/

void PythonSolver::setOptions(QMap<QString, double> vars)
{

   m_dict_options = vars;

   if(false == PythonLoader::inst()->setArgDict(
            m_module, m_funcName, 1, m_dict_options))
   {
      qDebug()<<"PythonSolver::setOptions: Failed to set dict argument count";
   }

}

/*---------------------------------------------------------------------------*/

void PythonSolver::setTransformer(gstar::ITransformer* transformer)
{

   Q_UNUSED(transformer);

}
