/*-----------------------------------------------------------------------------
 * Copyright (c) 2014, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef COUNTS_LOOKUP_TRANSFORMER_H
#define COUNTS_LOOKUP_TRANSFORMER_H

/*---------------------------------------------------------------------------*/

#include "gstar/ITransformer.h"
#include "mvc/MapsH5Model.h"
#include <QString>
#include <QMap>

/*---------------------------------------------------------------------------*/

namespace gstar
{

/**
 * @brief
 *
 */
class CountsLookupTransformer : public ITransformer
{

public:

   /**
    * Constructor.
    */
   CountsLookupTransformer();

   /**
    * Destructor.
    */
   virtual ~CountsLookupTransformer();

   /**
    * Init variables
    */ 
   virtual bool Init(QMap<QString, double> globalVars); 

   /**
    * @brief getAllCoef
    * @return
    */
   virtual QMap<QString, double> getAllCoef();

   /**
    * Get scale X
    */
   double getCounts();

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

   void setCounts(double x);

   void setModel(MapsH5Model* model, data_struct::ArrayXXr* norm) { _model = model; _normalizer = norm; }

   void setAnalyaisElement(std::string analysis_type, std::string element);

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

protected:

   double _counts;

   double _min_counts;

   double _max_counts;

   MapsH5Model* _model;

   std::string _analysis_type;

   std::string _element;

   int _rows;

   int _cols;

   data_struct::ArrayXXr* _counts_ptr;

   data_struct::ArrayXXr* _normalizer;

   data_struct::ArrayXXr _normalized_counts;
};

}
/*---------------------------------------------------------------------------*/

#endif

/*---------------------------------------------------------------------------*/
