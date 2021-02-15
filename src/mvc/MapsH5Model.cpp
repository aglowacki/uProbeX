/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsH5Model.h>

/*----------------src/mvc/MapsH5Model.cpp \-----------------------------------------------------------*/

MapsH5Model::MapsH5Model() : QObject()
{

    _filepath = "";
    _is_fully_loaded = false;
    _version = 0.0f;
    _loaded_scalers = false;
    _loaded_quantification = false;
    _loaded_scan = false;
    _loaded_integrated_spectra = false;
    _loaded_counts = false;
    _params_override = nullptr;
    _initialized_by_stream_block = false;

}

/*---------------------------------------------------------------------------*/

MapsH5Model::~MapsH5Model()
{

    clear_analyzed_counts();

}

/*---------------------------------------------------------------------------*/

void MapsH5Model::clear_analyzed_counts()
{

    for (auto itr : _analyzed_counts)
    {
        delete itr.second;
    }
    _analyzed_counts.clear();

}

/*---------------------------------------------------------------------------*/

data_struct::Fit_Count_Dict* MapsH5Model::getAnalyzedCounts(std::string analysis_type)
{
    if(analysis_type.length() > 0 && _analyzed_counts.count(analysis_type) > 0)
    {
        return _analyzed_counts[analysis_type];
    }
    return nullptr;
}

/*---------------------------------------------------------------------------*/

std::vector<std::string> MapsH5Model::count_names()
{
    std::vector<std::string> count_names;

    for(auto& itr1 : _analyzed_counts)
    {
        for(auto& itr2 : *(itr1.second)) //fit counts
        {
            count_names.push_back(itr2.first);
        }
        break;//we only want one iteration
    }

    return count_names;
}

/*---------------------------------------------------------------------------*/

std::vector<std::string> MapsH5Model::getAnalyzedTypes()
{
    std::vector<std::string> keys;
    for(auto &itr : _analyzed_counts)
    {
        keys.push_back(itr.first);
    }
    return keys;
}

/*---------------------------------------------------------------------------*/

std::string MapsH5Model::_analysis_enum_to_str(data_struct::Fitting_Routines val)
{
    if(val == data_struct::Fitting_Routines::ROI)
    {
        return "ROI";
    }
    else if(val == data_struct::Fitting_Routines::GAUSS_TAILS)
    {
        return "Parameter Fitting";
    }
    else if(val == data_struct::Fitting_Routines::GAUSS_MATRIX)
    {
        return "Matrix Fitting";
    }
    else if(val == data_struct::Fitting_Routines::SVD)
    {
        return "SVD";
    }
    else if(val == data_struct::Fitting_Routines::NNLS)
    {
        return "NNLS";
    }
    return "";
}

/*---------------------------------------------------------------------------*/

void MapsH5Model::set_fit_parameters_override(data_struct::Params_Override* override)
{
    _params_override = override;
}

/*---------------------------------------------------------------------------*/

void MapsH5Model::initialize_from_stream_block(data_struct::Stream_Block* block)
{
    clear_analyzed_counts();

    _filepath = QString(block->dataset_directory->c_str());
    _datset_name = QString(block->dataset_name->c_str());

    if(block->spectra != nullptr)
    {
        _integrated_spectra.resize(block->spectra->size());
        _integrated_spectra.setZero(block->spectra->size());
        _integrated_spectra.add( *(block->spectra) );
        _loaded_integrated_spectra = true;
        //emit model_int_spec_updated(false);
    }

    for(auto& itr : block->fitting_blocks)
    {
        std::string group_name = _analysis_enum_to_str(itr.first);

        data_struct::Fit_Count_Dict* xrf_counts = new data_struct::Fit_Count_Dict();
        _analyzed_counts.insert( {group_name, xrf_counts} );

        for(auto& itr2 : itr.second.fit_counts)
        {
            xrf_counts->emplace(std::pair<std::string,EMatrixF>(itr2.first, EMatrixF() ));
            xrf_counts->at(itr2.first).resize(block->height(), block->width());
            xrf_counts->at(itr2.first).setZero(block->height(), block->width());
            xrf_counts->at(itr2.first)(block->row(), block->col()) = itr2.second;
        }
    }
    emit model_data_updated();
}

/*---------------------------------------------------------------------------*/

void MapsH5Model::update_from_stream_block(data_struct::Stream_Block* block)
{
    if(block->spectra != nullptr)
    {
        _integrated_spectra.add( *(block->spectra) );
        //wait until the FitSpectraWidget has a thread to update int spec
        //emit model_int_spec_updated(false);
    }

    if(_analyzed_counts.size() < 1)
    {
        initialize_from_stream_block(block);
    }
    else
    {
        for(auto& itr : block->fitting_blocks)
        {
            std::string group_name = _analysis_enum_to_str(itr.first);
            if(_analyzed_counts.count(group_name) > 0)
            {
                data_struct::Fit_Count_Dict* xrf_counts = _analyzed_counts[group_name];
                for(auto& itr2 : itr.second.fit_counts)
                {
                    if (std::isfinite(itr2.second))
                    {
                        xrf_counts->at(itr2.first)(block->row(), block->col()) = itr2.second;
                    }
                    else
                    {
                        xrf_counts->at(itr2.first)(block->row(), block->col()) = 0.0;
                    }
                }
            }
            else
            {
                initialize_from_stream_block(block);
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::load(QString filepath)
{
    try
    {
        _filepath = filepath;
        //_is_loaded = ERROR_LOADING;
        //std::chrono::time_point<std::chrono::system_clock> start, end;
        //start = std::chrono::system_clock::now();

        //logW<<" MapsH5Model loading "<< filepath.toStdString() << "\n";

        hid_t    file_id, dset_id, dataspace_id, maps_grp_id, memoryspace_id;
        herr_t   error;
        //hsize_t offset[1] = {0};
        hsize_t count[1] = {1};

        file_id = H5Fopen(filepath.toStdString().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
        if(file_id < 0)
        {
            logW<<"Error opening file "<<filepath.toStdString() << "\n";
            return false;
        }

        maps_grp_id = H5Gopen(file_id, "/MAPS", H5P_DEFAULT);
        if(maps_grp_id < 0)
        {
            H5Fclose(file_id);
            logW<<"Error opening group /MAPS";
            return false;
        }

        dset_id = H5Dopen2(maps_grp_id, "version", H5P_DEFAULT);
        if(dset_id < 0)
        {
            H5Gclose(maps_grp_id);
            H5Fclose(file_id);
            logW<<"Error opening dataset /MAPS/version";
            return false;
        }
        dataspace_id = H5Dget_space(dset_id);

        memoryspace_id = H5Screate_simple(1, count, nullptr);

        error = H5Dread (dset_id, H5T_NATIVE_FLOAT, memoryspace_id, dataspace_id, H5P_DEFAULT, (void*)&_version);

        if(error != 0)
        {
            return false;
        }

        if(_version < 10.0)
        {
            _is_fully_loaded = _load_version_9(maps_grp_id);
        }
        else
        {
            _is_fully_loaded = _load_version_10(file_id, maps_grp_id);
        }

        H5Sclose(memoryspace_id);
        H5Sclose(dataspace_id);
        H5Dclose(dset_id);
        H5Gclose(maps_grp_id);
        H5Fclose(file_id);

        //end = std::chrono::system_clock::now();
        //std::chrono::duration<double> elapsed_seconds = end-start;
        //std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    }
    catch (std::string& s)
    {
        throw s;
    }
    catch (...)
    {
        throw std::string("Failed to open Maps Analyzed dataset!");
    }

    return _is_fully_loaded;
}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::load_roi(const std::vector<QPoint> &roi_list, data_struct::Spectra& spec)
{
    if (_version < 10.0)
    {
        return _load_roi_9(roi_list, spec);
    }
    return _load_roi_10(roi_list, spec);
}

//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------Version 10---------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

bool MapsH5Model::_load_version_9(hid_t maps_grp_id)
{

    _loaded_quantification = _load_quantification_9(maps_grp_id);

    _loaded_scalers = _load_scalers_9(maps_grp_id);

    _loaded_scan = _load_scan_9(maps_grp_id);

    _loaded_integrated_spectra = _load_integrated_spectra_9(maps_grp_id);

    _loaded_counts = _load_counts_9(maps_grp_id);

    return (_loaded_quantification && _loaded_scalers && _loaded_scan && _loaded_integrated_spectra &&_loaded_counts);

}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_quantification_9_single(hid_t maps_grp_id, std::string path, std::unordered_map<std::string, Calibration_curve >&quant)
{
    hid_t dset_id, channels_dset_id, dspace_id, channels_dspace_id;
    hid_t memoryspace_id, memoryspace_name_id, error;
    hsize_t offset[3] = { 0,0,0 };
    hsize_t count[3] = { 1,1,1 };
    hsize_t offset_name[1] = { 0 };
    hsize_t count_name[1] = { 1 };
    char tmp_name[255] = { 0 };
    hid_t   filetype, memtype, status;
    real_t values[3] = { 0., 0., 0. };

    dset_id = H5Dopen(maps_grp_id, path.c_str(), H5P_DEFAULT);
    if (dset_id < 0)
    {
        logW << "Error opening group /MAPS/"<<path<<"\n";
        return false;
    }
    dspace_id = H5Dget_space(dset_id);

    channels_dset_id = H5Dopen(maps_grp_id, "channel_names", H5P_DEFAULT);
    if (channels_dset_id < 0)
    {
        logW << "Error opening group /MAPS/channel_names\n";
        return false;
    }
    channels_dspace_id = H5Dget_space(channels_dset_id);

    int rank = H5Sget_simple_extent_ndims(dspace_id);
    if (rank != 3)
    {
        logW << path << " rank is not equal to 3, unknown format!\n";
        return false;
    }
    hsize_t* dims_out = new hsize_t[rank];
    unsigned int status_n = H5Sget_simple_extent_dims(dspace_id, &dims_out[0], nullptr);

    filetype = H5Tcopy(H5T_C_S1);
    H5Tset_size(filetype, 256);
    memtype = H5Tcopy(H5T_C_S1);
    status = H5Tset_size(memtype, 255);

    for (int i = 0; i < 3; i++)
    {
        offset[i] = 0;
        count[i] = dims_out[i];
    }

    count[2] = 1;

    memoryspace_id = H5Screate_simple(3, count, nullptr);
    memoryspace_name_id = H5Screate_simple(1, count_name, nullptr);
    H5Sselect_hyperslab(memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    H5Sselect_hyperslab(memoryspace_name_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);

    Calibration_curve sr_current_curve(STR_SR_CURRENT);
    Calibration_curve us_ic_curve(STR_US_IC);
    Calibration_curve ds_ic_curve(STR_DS_IC);

    for (hsize_t el_idx = 0; el_idx < dims_out[2]; el_idx++)
    {
        offset[2] = el_idx;
        offset_name[0] = el_idx;
        memset(&tmp_name[0], 0, 254);
        H5Sselect_hyperslab(channels_dspace_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);
        error = H5Dread(channels_dset_id, memtype, memoryspace_name_id, channels_dspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
        std::string el_name = std::string(tmp_name);
        H5Sselect_hyperslab(dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
        error = H5Dread(dset_id, H5T_NATIVE_FLOAT, memoryspace_id, dspace_id, H5P_DEFAULT, (void*)&values[0]);
        if (error > -1)
        {
            sr_current_curve.calib_curve[el_name] = values[0];
            us_ic_curve.calib_curve[el_name] = values[1];
            ds_ic_curve.calib_curve[el_name] = values[2];
        }
    }

    quant[STR_SR_CURRENT] = sr_current_curve;
    quant[STR_US_IC] = us_ic_curve;
    quant[STR_DS_IC] = ds_ic_curve;

    delete[]dims_out;

    H5Sclose(memoryspace_name_id);
    H5Sclose(memoryspace_id);
    H5Sclose(channels_dspace_id);
    H5Dclose(channels_dset_id);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);


    return true;
}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_quantification_9(hid_t maps_grp_id)
{
    _load_quantification_9_single(maps_grp_id, QUANT_V9_LOC_MATRIX_STR, _quant_map_matrix);
    _load_quantification_9_single(maps_grp_id, QUANT_V9_LOC_NNLS_STR, _quant_map_nnls);
    _load_quantification_9_single(maps_grp_id, QUANT_V9_LOC_ROI_STR, _quant_map_roi);
    return true;
}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_scalers_9(hid_t maps_grp_id)
{
    hid_t counts_dset_id, channels_dset_id, counts_dspace_id, channels_dspace_id, fit_int_spec_dset_id;
    hid_t memoryspace_id, memoryspace_name_id, error;
    hsize_t offset[3] = { 0,0,0 };
    hsize_t count[3] = { 1,1,1 };
    hsize_t offset_name[1] = { 0 };
    hsize_t count_name[1] = { 1 };
    char tmp_name[255] = { 0 };
    hid_t   filetype, memtype, status;


    counts_dset_id = H5Dopen(maps_grp_id, "scalers", H5P_DEFAULT);
    if (counts_dset_id < 0)
    {
        logW << "Error opening group /MAPS/scalers";
        return false;
    }
    counts_dspace_id = H5Dget_space(counts_dset_id);

    channels_dset_id = H5Dopen(maps_grp_id, "scaler_names", H5P_DEFAULT);
    if (channels_dset_id < 0)
    {
        H5Sclose(counts_dspace_id);
        H5Dclose(counts_dset_id);
        logW << "Error opening group /MAPS/scaler_names";
        return false;
    }
    channels_dspace_id = H5Dget_space(channels_dset_id);


    int rank = H5Sget_simple_extent_ndims(counts_dspace_id);
    if (rank != 3)
    {
        logW << "Error getting rank for /MAPS/scalers";
    }
    hsize_t* dims_out = new hsize_t[rank];
    unsigned int status_n = H5Sget_simple_extent_dims(counts_dspace_id, &dims_out[0], nullptr);

    filetype = H5Tcopy(H5T_C_S1);
    H5Tset_size(filetype, 256);
    memtype = H5Tcopy(H5T_C_S1);
    status = H5Tset_size(memtype, 255);

    for (int i = 0; i < 3; i++)
    {
        offset[i] = 0;
        count[i] = dims_out[i];
    }

    count[0] = 1;

    memoryspace_id = H5Screate_simple(3, count, nullptr);
    memoryspace_name_id = H5Screate_simple(1, count_name, nullptr);
    H5Sselect_hyperslab(memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    H5Sselect_hyperslab(memoryspace_name_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);

    for (hsize_t el_idx = 0; el_idx < dims_out[0]; el_idx++)
    {
        offset[0] = el_idx;
        offset_name[0] = el_idx;
        memset(&tmp_name[0], 0, 254);
        H5Sselect_hyperslab(channels_dspace_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);
        error = H5Dread(channels_dset_id, memtype, memoryspace_name_id, channels_dspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
        std::string el_name = std::string(tmp_name);
        // switch v9 names to v10
        if (el_name == STR_SR_CURRENT_V9)
        {
            el_name = STR_SR_CURRENT;
        }
        else if (el_name == STR_US_IC_V9)
        {
            el_name = STR_US_IC;
        }
        else if (el_name == STR_DS_IC_V9)
        {
            el_name = STR_DS_IC;
        }

        _scalers.emplace(std::pair<std::string, EMatrixF>(el_name, EMatrixF()));
        _scalers.at(el_name).resize(count[1], count[2]);

        H5Sselect_hyperslab(counts_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
        error = H5Dread(counts_dset_id, H5T_NATIVE_FLOAT, memoryspace_id, counts_dspace_id, H5P_DEFAULT, (void*)(_scalers.at(el_name).data()));
    }

    delete[]dims_out;

    H5Sclose(memoryspace_name_id);
    H5Sclose(memoryspace_id);
    H5Sclose(channels_dspace_id);
    H5Dclose(channels_dset_id);
    H5Sclose(counts_dspace_id);
    H5Dclose(counts_dset_id);

    //nan to 0.f
    for (auto& itr : _scalers)
    {
        itr.second = itr.second.unaryExpr([](float v) { return std::isfinite(v) ? v : 0.0f; });
    }

    return true;
}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_scan_9(hid_t maps_grp_id)
{

    return true;
}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_integrated_spectra_9(hid_t maps_grp_id)
{
    //   /MAPS/int_spec
    hid_t counts_dset_id, counts_dspace_id;
    hid_t memoryspace_id, error;
    hsize_t offset[1] = {0};
    hsize_t count[1] = {1};
    hid_t   filetype, memtype, status;

    counts_dset_id = H5Dopen(maps_grp_id, "int_spec", H5P_DEFAULT);
    if(counts_dset_id < 0)
    {
        logW<<"Error opening group /MAPS/int_spec";
        return false;
    }
    counts_dspace_id = H5Dget_space(counts_dset_id);

    int rank = H5Sget_simple_extent_ndims(counts_dspace_id);
    if (rank != 1)
    {
        logW<<"Error opening group /MAPS/int_spec";
    }
    hsize_t* dims_out = new hsize_t[rank];
    unsigned int status_n = H5Sget_simple_extent_dims(counts_dspace_id, &dims_out[0], nullptr);

    for (int i=0; i < rank; i++)
    {
       offset[i] = 0;
       count[i] = dims_out[i];
    }

    _integrated_spectra.resize(count[0]);
    memoryspace_id = H5Screate_simple(1, count, nullptr);
    H5Sselect_hyperslab (memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    H5Sselect_hyperslab (counts_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    error = H5Dread (counts_dset_id, H5T_NATIVE_FLOAT, memoryspace_id, counts_dspace_id, H5P_DEFAULT, (void*)(_integrated_spectra.data()));

    delete []dims_out;

    H5Sclose(memoryspace_id);
    H5Sclose(counts_dspace_id);
    H5Dclose(counts_dset_id);

    return true;

}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_counts_9(hid_t maps_grp_id)
{
    hid_t analyzed_grp_id;

    std::string analyzed_groups[] = {"XRF_roi", "XRF_roi_plus", "XRF_fits"};

    for( std::string group_name : analyzed_groups )
    {
        _load_analyzed_counts_9(maps_grp_id, group_name);
    }

    return true;
}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_analyzed_counts_9(hid_t analyzed_grp_id, std::string group_name)
{

    hid_t counts_dset_id, channels_dset_id, counts_dspace_id, channels_dspace_id;
    hid_t memoryspace_id, memoryspace_name_id, error;
    hsize_t offset[3] = {0,0,0};
    hsize_t count[3] = {1,1,1};
    hsize_t offset_name[1] = {0};
    hsize_t count_name[1] = {1};
    char tmp_name[255] = {0};
    hid_t   filetype, memtype, status;

    counts_dset_id = H5Dopen(analyzed_grp_id, group_name.c_str(), H5P_DEFAULT);
    if(counts_dset_id < 0)
    {
        logW<<"Error opening group /MAPS/"<<group_name.c_str();
        return false;
    }
    counts_dspace_id = H5Dget_space(counts_dset_id);

    channels_dset_id = H5Dopen(analyzed_grp_id, "channel_names", H5P_DEFAULT);
    if(channels_dset_id < 0)
    {
        H5Sclose(counts_dspace_id);
        H5Dclose(counts_dset_id);
        logW<<"Error opening group /MAPS/"<<group_name.c_str();
        return false;
    }
    channels_dspace_id = H5Dget_space(channels_dset_id);

    int rank = H5Sget_simple_extent_ndims(counts_dspace_id);
    if (rank != 3)
    {
        logW<<"Error opening group /MAPS/"<<group_name.c_str();
    }
    hsize_t* dims_out = new hsize_t[rank];
    unsigned int status_n = H5Sget_simple_extent_dims(counts_dspace_id, &dims_out[0], nullptr);

    filetype = H5Tcopy (H5T_C_S1);
    H5Tset_size (filetype, 256);
    memtype = H5Tcopy (H5T_C_S1);
    status = H5Tset_size (memtype, 255);

    for (int i=0; i < 3; i++)
    {
       offset[i] = 0;
       count[i] = dims_out[i];
    }

    count[0] = 1;

    data_struct::Fit_Count_Dict* xrf_counts = new data_struct::Fit_Count_Dict();
    // convert v9 to v10
    if (group_name == STR_FITS_V9)
    {
        group_name = STR_FIT_GAUSS_MATRIX;
    }
    else if (group_name == STR_ROI_PLUS_V9)
    {
        group_name = STR_FIT_NNLS;
    }
    else if (group_name == STR_ROI_V9)
    {
        group_name = STR_FIT_ROI;
    }
    _analyzed_counts.insert( {group_name, xrf_counts} );

    memoryspace_id = H5Screate_simple(3, count, nullptr);
    memoryspace_name_id = H5Screate_simple(1, count_name, nullptr);
    H5Sselect_hyperslab (memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    H5Sselect_hyperslab (memoryspace_name_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);

    for(hsize_t el_idx=0; el_idx < dims_out[0]; el_idx++)
    {
        offset[0] = el_idx;
        offset_name[0] = el_idx;
        memset(&tmp_name[0], 0, 254);
        H5Sselect_hyperslab (channels_dspace_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);
        error = H5Dread (channels_dset_id, memtype, memoryspace_name_id, channels_dspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
        std::string el_name = std::string(tmp_name);
        xrf_counts->emplace(std::pair<std::string,EMatrixF>(el_name, EMatrixF() ));
        xrf_counts->at(el_name).resize(count[1], count[2]);

        H5Sselect_hyperslab (counts_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
        error = H5Dread (counts_dset_id, H5T_NATIVE_FLOAT, memoryspace_id, counts_dspace_id, H5P_DEFAULT, (void*)(xrf_counts->at(el_name).data()));
    }

    delete []dims_out;

    H5Sclose(memoryspace_name_id);
    H5Sclose(memoryspace_id);
    H5Sclose(channels_dspace_id);
    H5Dclose(channels_dset_id);
    H5Sclose(counts_dspace_id);
    H5Dclose(counts_dset_id);

    //nan to 0.f
    for(auto& itr : *xrf_counts)
    {
        itr.second = itr.second.unaryExpr([](float v) { return std::isfinite(v)? v : 0.0f; });
    }

    return true;
}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_roi_9(const std::vector<QPoint>& roi_list, data_struct::Spectra& spec)
{
    return false;

}


//------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------Version 10---------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------

bool MapsH5Model::_load_version_10(hid_t file_id, hid_t maps_grp_id)
{

    _loaded_quantification = _load_quantification_10(maps_grp_id);

    _loaded_scalers = _load_scalers_10(maps_grp_id);

    _loaded_scan = _load_scan_10(maps_grp_id);

    _loaded_integrated_spectra = _load_integrated_spectra_10(file_id);

    _loaded_counts = _load_counts_10(maps_grp_id);

    return (_loaded_quantification && _loaded_scalers && _loaded_scan && _loaded_integrated_spectra &&_loaded_counts);

}

/*---------------------------------------------------------------------------*/


bool MapsH5Model::_load_quantification_10_single(hid_t maps_grp_id, std::string path, std::unordered_map<std::string, Calibration_curve >& quant)
{
    hid_t channels_dset_id, channels_dspace_id;
    hid_t sr_dset_id, sr_dspace_id;
    hid_t us_dset_id, us_dspace_id;
    hid_t ds_dset_id, ds_dspace_id;
    hid_t grp_id;
    hid_t memoryspace_id, memoryspace_name_id;
    hid_t error;
    hid_t sr_error;
    hid_t us_error;
    hid_t ds_error;
    hsize_t offset[2] = { 0,0 };
    hsize_t count[2] = { 1,1 };
    hsize_t offset_name[2] = { 0, 0 };
    hsize_t count_name[2] = { 1, 1 };
    char tmp_name[255] = { 0 };
    hid_t   filetype, memtype, status;
    real_t sr_values[3] = { 0., 0., 0. };
    real_t us_values[3] = { 0., 0., 0. };
    real_t ds_values[3] = { 0., 0., 0. };

    grp_id = H5Gopen(maps_grp_id, path.c_str(), H5P_DEFAULT);
    if (grp_id < 0)
    {
        logW << "Error opening group /MAPS/" << path << "\n";
        return false;
    }
    

    sr_dset_id = H5Dopen(grp_id, STR_CALIB_SR_CURRENT.c_str(), H5P_DEFAULT);
    if (sr_dset_id < 0)
    {
        logW << "Error opening group /MAPS/" << STR_CALIB_SR_CURRENT << "\n";
        return false;
    }
    sr_dspace_id = H5Dget_space(sr_dset_id);

    us_dset_id = H5Dopen(grp_id, STR_CALIB_US_IC.c_str(), H5P_DEFAULT);
    if (us_dset_id < 0)
    {
        logW << "Error opening group /MAPS/" << STR_CALIB_US_IC << "\n";
        return false;
    }
    us_dspace_id = H5Dget_space(us_dset_id);

    ds_dset_id = H5Dopen(grp_id, STR_CALIB_DS_IC.c_str(), H5P_DEFAULT);
    if (ds_dset_id < 0)
    {
        logW << "Error opening group /MAPS/" << STR_CALIB_DS_IC << "\n";
        return false;
    }
    ds_dspace_id = H5Dget_space(ds_dset_id);

    channels_dset_id = H5Dopen(grp_id, STR_CALIB_Labels.c_str(), H5P_DEFAULT);
    if (channels_dset_id < 0)
    {
        logW << "Error opening group /MAPS/"<< STR_CALIB_Labels<<"\n";
        return false;
    }
    channels_dspace_id = H5Dget_space(channels_dset_id);

    int rank = H5Sget_simple_extent_ndims(sr_dspace_id);
    if (rank != 2)
    {
        logW << path << " rank is not equal to 3, unknown format!\n";
        return false;
    }
    hsize_t* dims_out = new hsize_t[rank];
    unsigned int status_n = H5Sget_simple_extent_dims(sr_dspace_id, &dims_out[0], nullptr);

    filetype = H5Tcopy(H5T_C_S1);
    H5Tset_size(filetype, 256);
    memtype = H5Tcopy(H5T_C_S1);
    status = H5Tset_size(memtype, 255);

    for (int i = 0; i < rank; i++)
    {
        offset[i] = 0;
        count[i] = dims_out[i];
    }

    count[1] = 1;

    memoryspace_id = H5Screate_simple(rank, count, nullptr);
    memoryspace_name_id = H5Screate_simple(rank, count_name, nullptr);
    H5Sselect_hyperslab(memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    H5Sselect_hyperslab(memoryspace_name_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);

    Calibration_curve sr_current_curve(STR_SR_CURRENT);
    Calibration_curve us_ic_curve(STR_US_IC);
    Calibration_curve ds_ic_curve(STR_DS_IC);

    for (hsize_t el_idx = 0; el_idx < dims_out[1]; el_idx++)
    {
        offset[1] = el_idx;
        offset_name[1] = el_idx;
        memset(&tmp_name[0], 0, 254);
        H5Sselect_hyperslab(channels_dspace_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);
        error = H5Dread(channels_dset_id, memtype, memoryspace_name_id, channels_dspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
        std::string el_name = std::string(tmp_name);
        H5Sselect_hyperslab(sr_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
        sr_error = H5Dread(sr_dset_id, H5T_NATIVE_FLOAT, memoryspace_id, sr_dspace_id, H5P_DEFAULT, (void*)&sr_values[0]);

        H5Sselect_hyperslab(us_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
        us_error = H5Dread(us_dset_id, H5T_NATIVE_FLOAT, memoryspace_id, us_dspace_id, H5P_DEFAULT, (void*)&us_values[0]);

        H5Sselect_hyperslab(ds_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
        ds_error = H5Dread(ds_dset_id, H5T_NATIVE_FLOAT, memoryspace_id, ds_dspace_id, H5P_DEFAULT, (void*)&ds_values[0]);
        if (error > -1 && sr_error > -1)
        {
            sr_current_curve.calib_curve[el_name] = sr_values[0];
            sr_current_curve.calib_curve[el_name + "_L"] = sr_values[1];
            sr_current_curve.calib_curve[el_name + "_M"] = sr_values[2];
        }
        if (error > -1 && us_error > -1)
        {
            us_ic_curve.calib_curve[el_name] = us_values[0];
            us_ic_curve.calib_curve[el_name + "_L"] = us_values[1];
            us_ic_curve.calib_curve[el_name + "_M"] = us_values[2];
        }
        if (error > -1 && ds_error > -1)
        {
            ds_ic_curve.calib_curve[el_name] = ds_values[0];
            ds_ic_curve.calib_curve[el_name + "_L"] = ds_values[1];
            ds_ic_curve.calib_curve[el_name + "_M"] = ds_values[2];
        }
    }

    quant[STR_SR_CURRENT] = sr_current_curve;
    quant[STR_US_IC] = us_ic_curve;
    quant[STR_DS_IC] = ds_ic_curve;

    delete[]dims_out;

    H5Sclose(memoryspace_name_id);
    H5Sclose(memoryspace_id);
    H5Sclose(channels_dspace_id);
    H5Dclose(channels_dset_id);
    H5Sclose(us_dspace_id);
    H5Dclose(us_dset_id);
    H5Sclose(ds_dspace_id);
    H5Dclose(ds_dset_id);
    H5Sclose(sr_dspace_id);
    H5Dclose(sr_dset_id);
    H5Gclose(grp_id);


    return true;
}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_quantification_10(hid_t maps_grp_id)
{
    _load_quantification_10_single(maps_grp_id, QUANT_V10_LOC_MATRIX_STR, _quant_map_matrix);
    _load_quantification_10_single(maps_grp_id, QUANT_V10_LOC_NNLS_STR, _quant_map_nnls);
    _load_quantification_10_single(maps_grp_id, QUANT_V10_LOC_ROI_STR, _quant_map_roi);
    return true;
}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_scalers_10(hid_t maps_grp_id)
{
    hid_t sub_grp_id, counts_dset_id, channels_dset_id, counts_dspace_id, channels_dspace_id, fit_int_spec_dset_id;
    hid_t memoryspace_id, memoryspace_name_id, error;
    hsize_t offset[3] = { 0,0,0 };
    hsize_t count[3] = { 1,1,1 };
    hsize_t offset_name[1] = { 0 };
    hsize_t count_name[1] = { 1 };
    char tmp_name[255] = { 0 };
    hid_t   filetype, memtype, status;

    sub_grp_id = H5Gopen(maps_grp_id, "Scalers", H5P_DEFAULT);
    if (sub_grp_id < 0)
    {
        logW << "Error opening group /MAPS/Scalers";
        return false;
    }

    counts_dset_id = H5Dopen(sub_grp_id, "Values", H5P_DEFAULT);
    if (counts_dset_id < 0)
    {
        H5Gclose(sub_grp_id);
        logW << "Error opening group /MAPS/Scalers/Values";
        return false;
    }
    counts_dspace_id = H5Dget_space(counts_dset_id);

    channels_dset_id = H5Dopen(sub_grp_id, "Names", H5P_DEFAULT);
    if (channels_dset_id < 0)
    {
        H5Sclose(counts_dspace_id);
        H5Dclose(counts_dset_id);
        H5Gclose(sub_grp_id);
        logW << "Error opening group /MAPS/Scalers/Names";
        return false;
    }
    channels_dspace_id = H5Dget_space(channels_dset_id);


    int rank = H5Sget_simple_extent_ndims(counts_dspace_id);
    if (rank != 3)
    {
        logW << "Error getting rank for /MAPS/Scalers/Values";
    }
    hsize_t* dims_out = new hsize_t[rank];
    unsigned int status_n = H5Sget_simple_extent_dims(counts_dspace_id, &dims_out[0], nullptr);

    filetype = H5Tcopy(H5T_C_S1);
    H5Tset_size(filetype, 256);
    memtype = H5Tcopy(H5T_C_S1);
    status = H5Tset_size(memtype, 255);

    for (int i = 0; i < 3; i++)
    {
        offset[i] = 0;
        count[i] = dims_out[i];
    }

    count[0] = 1;

    memoryspace_id = H5Screate_simple(3, count, nullptr);
    memoryspace_name_id = H5Screate_simple(1, count_name, nullptr);
    H5Sselect_hyperslab(memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    H5Sselect_hyperslab(memoryspace_name_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);

    for (hsize_t el_idx = 0; el_idx < dims_out[0]; el_idx++)
    {
        offset[0] = el_idx;
        offset_name[0] = el_idx;
        memset(&tmp_name[0], 0, 254);
        H5Sselect_hyperslab(channels_dspace_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);
        error = H5Dread(channels_dset_id, memtype, memoryspace_name_id, channels_dspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
        std::string el_name = std::string(tmp_name);
        _scalers.emplace(std::pair<std::string, EMatrixF>(el_name, EMatrixF()));
        _scalers.at(el_name).resize(count[1], count[2]);

        H5Sselect_hyperslab(counts_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
        error = H5Dread(counts_dset_id, H5T_NATIVE_FLOAT, memoryspace_id, counts_dspace_id, H5P_DEFAULT, (void*)(_scalers.at(el_name).data()));
    }

    delete[]dims_out;

    H5Sclose(memoryspace_name_id);
    H5Sclose(memoryspace_id);
    H5Sclose(channels_dspace_id);
    H5Dclose(channels_dset_id);
    H5Sclose(counts_dspace_id);
    H5Dclose(counts_dset_id);
    H5Gclose(sub_grp_id);

    //nan to 0.f
    for (auto& itr : _scalers)
    {
        itr.second = itr.second.unaryExpr([](float v) { return std::isfinite(v) ? v : 0.0f; });
    }

    return true;
}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_scan_10(hid_t maps_grp_id)
{

    return true;
}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_integrated_spectra_10(hid_t file_id)
{

    return io::file::HDF5_IO::inst()->load_integrated_spectra_analyzed_h5(_filepath.toStdString(), &_integrated_spectra);

}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_counts_10(hid_t maps_grp_id)
{

    hid_t analyzed_grp_id;

    std::string analyzed_groups[] = {"Fitted", "NNLS", "ROI", "SVD"};

    analyzed_grp_id = H5Gopen(maps_grp_id, "XRF_Analyzed", H5P_DEFAULT);
    if(analyzed_grp_id < 0)
    {
        logW<<"Error opening group /MAPS/XRF_Analyzed";
        return false;
    }

    for( std::string group_name : analyzed_groups )
    {
        _load_analyzed_counts_10(analyzed_grp_id, group_name);
    }

    H5Gclose(analyzed_grp_id);

    return true;
}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_analyzed_counts_10(hid_t analyzed_grp_id, std::string group_name)
{

    hid_t sub_grp_id, counts_dset_id, channels_dset_id, counts_dspace_id, channels_dspace_id, fit_int_spec_dset_id;
    hid_t memoryspace_id, memoryspace_name_id, error;
    hsize_t offset[3] = {0,0,0};
    hsize_t count[3] = {1,1,1};
    hsize_t offset_name[1] = {0};
    hsize_t count_name[1] = {1};
    char tmp_name[255] = {0};
    hid_t   filetype, memtype, status;

    sub_grp_id = H5Gopen(analyzed_grp_id, group_name.c_str(), H5P_DEFAULT);
    if(sub_grp_id < 0)
    {
        logW<<"Error opening group /MAPS/XRF_Analyzed/"<<group_name.c_str();
        return false;
    }

    counts_dset_id = H5Dopen(sub_grp_id, "Counts_Per_Sec", H5P_DEFAULT);
    if(counts_dset_id < 0)
    {
        H5Gclose(sub_grp_id);
        logW<<"Error opening group /MAPS/XRF_Analyzed/"<<group_name.c_str()<<"/Counts_Per_Sec";
        return false;
    }
    counts_dspace_id = H5Dget_space(counts_dset_id);

    channels_dset_id = H5Dopen(sub_grp_id, "Channel_Names", H5P_DEFAULT);
    if(channels_dset_id < 0)
    {
        H5Sclose(counts_dspace_id);
        H5Dclose(counts_dset_id);
        H5Gclose(sub_grp_id);
        logW<<"Error opening group /MAPS/XRF_Analyzed/"<<group_name.c_str()<<"/Channel_Names";
        return false;
    }
    channels_dspace_id = H5Dget_space(channels_dset_id);


	fit_int_spec_dset_id = H5Dopen(sub_grp_id, "Fitted_Integrated_Spectra", H5P_DEFAULT);
	if (fit_int_spec_dset_id > -1)
	{
		hid_t dataspace_id = H5Dget_space(fit_int_spec_dset_id);
		int rank = H5Sget_simple_extent_ndims(dataspace_id);
		if (rank == 1)
		{
			hsize_t dims_in[1] = { 0 };
			int status_n = H5Sget_simple_extent_dims(dataspace_id, &dims_in[0], nullptr);
			if (status_n > -1)
			{
				for (int i = 0; i < rank; i++)
				{
					offset[i] = 0;
					count[i] = dims_in[i];
				}

				data_struct::ArrayXr* spectra = new data_struct::ArrayXr(dims_in[0]);

				count[0] = dims_in[0];

				memoryspace_id = H5Screate_simple(1, count, nullptr);
				H5Sselect_hyperslab(memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
				H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);

				error = H5Dread(fit_int_spec_dset_id, H5T_NATIVE_REAL, memoryspace_id, dataspace_id, H5P_DEFAULT, (void*)&(*spectra)[0]);
				_fit_int_spec_dict.insert({ group_name , spectra });
			}
		}
	}
	



    int rank = H5Sget_simple_extent_ndims(counts_dspace_id);
    if (rank != 3)
    {
        logW<<"Error getting rank for  /MAPS/XRF_Analyzed/"<<group_name.c_str()<<"/Channel_Names";
    }
    hsize_t* dims_out = new hsize_t[rank];
    unsigned int status_n = H5Sget_simple_extent_dims(counts_dspace_id, &dims_out[0], nullptr);

    filetype = H5Tcopy (H5T_C_S1);
    H5Tset_size (filetype, 256);
    memtype = H5Tcopy (H5T_C_S1);
    status = H5Tset_size (memtype, 255);

    for (int i=0; i < 3; i++)
    {
       offset[i] = 0;
       count[i] = dims_out[i];
    }

    count[0] = 1;

    data_struct::Fit_Count_Dict* xrf_counts = new data_struct::Fit_Count_Dict();
    _analyzed_counts.insert( {group_name, xrf_counts} );

    memoryspace_id = H5Screate_simple(3, count, nullptr);
    memoryspace_name_id = H5Screate_simple(1, count_name, nullptr);
    H5Sselect_hyperslab (memoryspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
    H5Sselect_hyperslab (memoryspace_name_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);

    for(hsize_t el_idx=0; el_idx < dims_out[0]; el_idx++)
    {
        offset[0] = el_idx;
        offset_name[0] = el_idx;
        memset(&tmp_name[0], 0, 254);
        H5Sselect_hyperslab (channels_dspace_id, H5S_SELECT_SET, offset_name, nullptr, count_name, nullptr);
        error = H5Dread (channels_dset_id, memtype, memoryspace_name_id, channels_dspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
        std::string el_name = std::string(tmp_name);
        xrf_counts->emplace(std::pair<std::string,EMatrixF>(el_name, EMatrixF() ));
        xrf_counts->at(el_name).resize(count[1], count[2]);

        H5Sselect_hyperslab (counts_dspace_id, H5S_SELECT_SET, offset, nullptr, count, nullptr);
        error = H5Dread (counts_dset_id, H5T_NATIVE_FLOAT, memoryspace_id, counts_dspace_id, H5P_DEFAULT, (void*)(xrf_counts->at(el_name).data()));
    }

    delete []dims_out;

    H5Sclose(memoryspace_name_id);
    H5Sclose(memoryspace_id);
    H5Sclose(channels_dspace_id);
    H5Dclose(channels_dset_id);
    H5Sclose(counts_dspace_id);
    H5Dclose(counts_dset_id);
    H5Gclose(sub_grp_id);

    //nan to 0.f
    for(auto& itr : *xrf_counts)
    {
        itr.second = itr.second.unaryExpr([](float v) { return std::isfinite(v)? v : 0.0f; });
    }

    return true;
}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::_load_roi_10(const std::vector<QPoint>& roi_list, data_struct::Spectra& spec)
{

    return false;
}

/*---------------------------------------------------------------------------*/

Calibration_curve* MapsH5Model::get_calibration_curve(string analysis_type, string scaler_name)
{
    if (analysis_type == STR_FIT_GAUSS_MATRIX)
    {
        if (_quant_map_matrix.count(scaler_name) > 0)
        {
            return &_quant_map_matrix[scaler_name];
        }
    }
    else if (analysis_type == STR_FIT_NNLS)
    {
        if (_quant_map_nnls.count(scaler_name) > 0)
        {
            return &_quant_map_nnls[scaler_name];
        }
    }
    else if (analysis_type == STR_FIT_ROI)
    {
        if (_quant_map_roi.count(scaler_name) > 0)
        {
            return &_quant_map_roi[scaler_name];
        }
    }

    return nullptr;
}

/*---------------------------------------------------------------------------*/
