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

    unsigned int getNumIntegratedSpectra() { return _mda_io.get_num_integreated_spectra(); }

    data_struct::ArrayXr* getIntegratedSpectra(unsigned int det) { return _mda_io.get_integrated_spectra(det); }

    void getDims(int& rows, int& cols) { rows = _rows; cols = _cols; }

    data_struct::Scan_Info* getScanInfo() { return _mda_io.get_scan_info(); }

    void setParamOverride(int idx, data_struct::Params_Override* params) { if (params != nullptr) { _fit_params_override_dict[idx] = params; } }

    data_struct::Params_Override* getParamOverride(int idx);

protected:

    data_struct::Params_Override* _param_override;

private:

    std::map<int, data_struct::Params_Override*> _fit_params_override_dict;

    int _rows;

    int _cols;

    io::file::MDA_IO _mda_io;

    QString _filepath;
};


/*---------------------------------------------------------------------------*/

#endif /* MDA_Model_H_ */

/*---------------------------------------------------------------------------*/
