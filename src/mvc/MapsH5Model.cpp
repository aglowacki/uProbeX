/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsH5Model.h>
#include <QDebug>

/*----------------src/mvc/MapsH5Model.cpp \-----------------------------------------------------------*/

MapsH5Model::MapsH5Model()
{

    _filepath = "";
    _is_fully_loaded = false;

    _loaded_scalers = false;
    _loaded_quantification = false;
    _loaded_scan = false;
    _loaded_integrated_spectra = false;
    _loaded_counts = false;

}

/*---------------------------------------------------------------------------*/

MapsH5Model::~MapsH5Model()
{

    for (auto itr : _analyzed_counts)
    {
        delete itr.second;
    }
    _analyzed_counts.clear();

}


/*---------------------------------------------------------------------------*/

XrfAnalyzedCounts* MapsH5Model::getAnalyzedCounts(std::string analysis_type)
{
    if(_analyzed_counts.count(analysis_type) > 0)
    {
        return _analyzed_counts[analysis_type];
    }
    return nullptr;
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

        qDebug()<<" MapsH5Model loading "<< filepath;

        float version = 0.0;
        hid_t    file_id, dset_id, dataspace_id, maps_grp_id, memoryspace_id;
        herr_t   error;
        //hsize_t offset[1] = {0};
        hsize_t count[1] = {1};

        file_id = H5Fopen(filepath.toStdString().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
        if(file_id < 0)
        {
            qDebug()<<"Error opening file "<<filepath;
            return false;
        }

        maps_grp_id = H5Gopen(file_id, "/MAPS", H5P_DEFAULT);
        if(maps_grp_id < 0)
        {
            H5Fclose(file_id);
            qDebug()<<"Error opening group /MAPS";
            return false;
        }

        dset_id = H5Dopen2(maps_grp_id, "version", H5P_DEFAULT);
        if(dset_id < 0)
        {
            H5Gclose(maps_grp_id);
            H5Fclose(file_id);
            qDebug()<<"Error opening dataset /MAPS/version";
            return false;
        }
        dataspace_id = H5Dget_space(dset_id);

        memoryspace_id = H5Screate_simple(1, count, NULL);

        error = H5Dread (dset_id, H5T_NATIVE_FLOAT, memoryspace_id, dataspace_id, H5P_DEFAULT, (void*)&version);

        if(error != 0)
        {
            return false;
        }

        if(version < 10.0)
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

        //logit << "elapsed time: " << elapsed_seconds.count() << "s"<<std::endl;

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



bool MapsH5Model::_load_version_9(hid_t maps_grp_id)
{

    return false;
}

bool MapsH5Model::_load_version_10(hid_t file_id, hid_t maps_grp_id)
{

    _loaded_quantification = _load_quantification_10(maps_grp_id);

    _loaded_scalers = _load_scalers_10(maps_grp_id);

    _loaded_scan = _load_scan_10(maps_grp_id);

    _loaded_integrated_spectra = _load_integrated_spectra_10(file_id);

    _loaded_counts = _load_counts_10(maps_grp_id);

    return (_loaded_quantification && _loaded_scalers && _loaded_scan && _loaded_integrated_spectra &&_loaded_counts);

}


bool MapsH5Model::_load_quantification_10(hid_t maps_grp_id)
{

    return true;
}


bool MapsH5Model::_load_scalers_10(hid_t maps_grp_id)
{

    return true;
}


bool MapsH5Model::_load_scan_10(hid_t maps_grp_id)
{

    return true;
}

bool MapsH5Model::_load_integrated_spectra_10(hid_t file_id)
{

    return io::file::HDF5_IO::inst()->load_integrated_spectra_analyzed_h5(file_id, &_integrated_spectra);

}

bool MapsH5Model::_load_counts_10(hid_t maps_grp_id)
{

    hid_t analyzed_grp_id;

    std::string analyzed_groups[] = {"Fitted", "NNLS", "ROI", "SVD"};

    analyzed_grp_id = H5Gopen(maps_grp_id, "XRF_Analyzed", H5P_DEFAULT);
    if(analyzed_grp_id < 0)
    {
        qDebug()<<"Error opening group /MAPS/XRF_Analyzed";
        return false;
    }

    for( std::string group_name : analyzed_groups )
    {
        _load_analyzed_counts(analyzed_grp_id, group_name);
    }

    H5Gclose(analyzed_grp_id);

    return true;
}


bool MapsH5Model::_load_analyzed_counts(hid_t analyzed_grp_id, std::string group_name)
{

    hid_t sub_grp_id, counts_dset_id, channels_dset_id, counts_dspace_id, channels_dspace_id;
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
        qDebug()<<"Error opening group /MAPS/XRF_Analyzed/"<<group_name.c_str();
        return false;
    }

    counts_dset_id = H5Dopen(sub_grp_id, "Counts_Per_Sec", H5P_DEFAULT);
    if(counts_dset_id < 0)
    {
        H5Gclose(sub_grp_id);
        qDebug()<<"Error opening group /MAPS/XRF_Analyzed/"<<group_name.c_str()<<"/Counts_Per_Sec";
        return false;
    }
    counts_dspace_id = H5Dget_space(counts_dset_id);

    channels_dset_id = H5Dopen(sub_grp_id, "Channel_Names", H5P_DEFAULT);
    if(channels_dset_id < 0)
    {
        H5Sclose(counts_dspace_id);
        H5Dclose(counts_dset_id);
        H5Gclose(sub_grp_id);
        qDebug()<<"Error opening group /MAPS/XRF_Analyzed/"<<group_name.c_str()<<"/Channel_Names";
        return false;
    }
    channels_dspace_id = H5Dget_space(channels_dset_id);

    int rank = H5Sget_simple_extent_ndims(counts_dspace_id);
    if (rank != 3)
    {
        qDebug()<<"Error opening group /MAPS/XRF_Analyzed/"<<group_name.c_str()<<"/Channel_Names";
    }
    hsize_t* dims_out = new hsize_t[rank];
    unsigned int status_n = H5Sget_simple_extent_dims(counts_dspace_id, &dims_out[0], NULL);

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

    XrfAnalyzedCounts* xrf_counts = new XrfAnalyzedCounts(group_name, count[2], count[1]);
    _analyzed_counts.insert( {group_name, xrf_counts} );

    memoryspace_id = H5Screate_simple(3, count, NULL);
    memoryspace_name_id = H5Screate_simple(1, count_name, NULL);
    H5Sselect_hyperslab (memoryspace_id, H5S_SELECT_SET, offset, NULL, count, NULL);
    H5Sselect_hyperslab (memoryspace_name_id, H5S_SELECT_SET, offset_name, NULL, count_name, NULL);

    for(hsize_t el_idx=0; el_idx < dims_out[0]; el_idx++)
    {
        offset[0] = el_idx;
        offset_name[0] = el_idx;
        memset(&tmp_name[0], 0, 254);
        H5Sselect_hyperslab (channels_dspace_id, H5S_SELECT_SET, offset_name, NULL, count_name, NULL);
        error = H5Dread (channels_dset_id, memtype, memoryspace_name_id, channels_dspace_id, H5P_DEFAULT, (void*)&tmp_name[0]);
        std::string el_name = std::string(tmp_name);
        xrf_counts->add_element(el_name);

        H5Sselect_hyperslab (counts_dspace_id, H5S_SELECT_SET, offset, NULL, count, NULL);
        error = H5Dread (counts_dset_id, H5T_NATIVE_FLOAT, memoryspace_id, counts_dspace_id, H5P_DEFAULT, (void*)(xrf_counts->get_counts_ptr(el_name)));
    }

    delete []dims_out;

    H5Sclose(memoryspace_name_id);
    H5Sclose(memoryspace_id);
    H5Sclose(channels_dspace_id);
    H5Dclose(channels_dset_id);
    H5Sclose(counts_dspace_id);
    H5Dclose(counts_dset_id);
    H5Gclose(sub_grp_id);

    return true;
}



data_struct::xrf::Spectra MapsH5Model::fit_integrated_spectra(data_struct::xrf::Fit_Parameters fit_params, data_struct::xrf::Fit_Element_Map_Dict *elements_to_fit)
{
    //fitting::optimizers::LMFit_Optimizer lmfit_optimizer;
    fitting::optimizers::MPFit_Optimizer mpfit_optimizer;
    fitting::models::Gaussian_Model model;

    data_struct::xrf::Fit_Parameters out_fit_params;

    //Range of energy in spectra to fit
    fitting::models::Range energy_range;
    energy_range.min = 0;
    energy_range.max = _integrated_spectra.size() -1;

    //Fitting routines
    fitting::routines::Param_Optimized_Fit_Routine fit_routine;
    fit_routine.set_optimizer(&mpfit_optimizer);

    //reset model fit parameters to defaults
    model.reset_to_default_fit_params();
    //Update fit parameters by override values
    model.update_fit_params_values(fit_params);
    //model.set_fit_params_preset(fitting::models::BATCH_FIT_NO_TAILS);
    //Initialize the fit routine
    fit_routine.initialize(&model, elements_to_fit, energy_range);
    //Fit the spectra saving the element counts in element_fit_count_dict
    out_fit_params = fit_routine.fit_spectra_parameters(&model, &_integrated_spectra, elements_to_fit);
    return model.model_spectrum(&out_fit_params, elements_to_fit, energy_range);
}
