/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MDA_MODEL_H
#define MDA_MODEL_H

/*---------------------------------------------------------------------------*/

#include <QString>
#include <QObject>
#include <hdf5.h>
#include <unordered_map>
#include <vector>
#include "fitting/routines/param_optimized_fit_routine.h"

/*---------------------------------------------------------------------------*/

//typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> EMatrixF;

/**
 * @brief Model for raw mda files
 */
class MDA_Model : public QObject
{

    Q_OBJECT

public:

    /**
    * Constructor
    */
    MDA_Model();

    /**
    * Destructor
    */
    ~MDA_Model();

    bool load(QString filepath);

    QString getFilePath(){return _filepath;}

//signals:
//    void model_data_updated();

protected:

private:

    QString _filepath;
};


/*---------------------------------------------------------------------------*/

#endif /* MDA_Model_H_ */

/*---------------------------------------------------------------------------*/
