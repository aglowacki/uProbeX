/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef MDA_MODEL_H
#define MDA_MODEL_H

/*---------------------------------------------------------------------------*/

#include <QString>
#include <QObject>
#include <unordered_map>
#include <vector>
#include "io/file/mda_io.h"

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

    const data_struct::Spectra* getIntegratedSpectra() { return &_integrated_spectra; }

//signals:
//    void model_data_updated();

protected:

    data_struct::Spectra _integrated_spectra;

private:

    io::file::MDA_IO _mda_io;

    QString _filepath;
};


/*---------------------------------------------------------------------------*/

#endif /* MDA_Model_H_ */

/*---------------------------------------------------------------------------*/
