/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsH5Model.h>
#include <QDebug>

/*----------------src/mvc/MapsH5Model.cpp \-----------------------------------------------------------*/

MapsH5Model::MapsH5Model()
{

    _is_loaded = false;

}

/*---------------------------------------------------------------------------*/

MapsH5Model::~MapsH5Model()
{

}

/*---------------------------------------------------------------------------*/

bool MapsH5Model::load(QString filepath)
{
    try
    {
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
            _is_loaded = _load_version_9(maps_grp_id);
        }
        else
        {
            _is_loaded = _load_version_10(maps_grp_id);
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

    return _is_loaded;
}



bool MapsH5Model::_load_version_9(hid_t maps_grp_id)
{
    /*
    for (int i=0; i < rank; i++)
    {
        logit<<"dims ["<<i<<"] ="<<dims_in[i]<< std::endl;
        offset[i] = 0;
        count[i] = dims_in[i];
    }

    if(row_idx_end < row_idx_start)
    {
        row_idx_end = dims_in[1];
    }

    if(col_idx_end < col_idx_start)
    {
        col_idx_end = dims_in[2];
    }

    spectra_volume->resize(dims_in[1], dims_in[2], dims_in[0]);

    //buffer = new real_t [dims_in[0] * dims_in[2]]; // cols x spectra_size
    count[0] = dims_in[0];
    count[1] = 1;
    count[2] = 1;

    memoryspace_id = H5Screate_simple(3, count, NULL);
    memoryspace_meta_id = H5Screate_simple(2, count_time, NULL);
    H5Sselect_hyperslab (memoryspace_id, H5S_SELECT_SET, offset, NULL, count, NULL);
    H5Sselect_hyperslab (memoryspace_meta_id, H5S_SELECT_SET, offset_time, NULL, count_time, NULL);

    real_t live_time = 1.0;
    real_t real_time = 1.0;
    real_t in_cnt = 1.0;
    real_t out_cnt = 1.0;

    //offset[1] = row;

    for(size_t row=row_idx_start; row < row_idx_end; row++)
    {
        offset[1] = row;
        offset_time[0] = row;
        for(size_t col=col_idx_start; col < col_idx_end; col++)
        {
            data_struct::xrf::Spectra *spectra = &((*spectra_volume)[row][col]);
            offset[2] = col;
            offset_time[1] = col;
            H5Sselect_hyperslab (dataspace_id, H5S_SELECT_SET, offset, NULL, count, NULL);

            error = H5Dread (dset_id, H5T_NATIVE_REAL, memoryspace_id, dataspace_id, H5P_DEFAULT, (void*)&(*spectra)[0]);

            H5Sselect_hyperslab (dataspace_lt_id, H5S_SELECT_SET, offset_time, NULL, count_time, NULL);
            H5Sselect_hyperslab (dataspace_rt_id, H5S_SELECT_SET, offset_time, NULL, count_time, NULL);
            H5Sselect_hyperslab (dataspace_inct_id, H5S_SELECT_SET, offset_time, NULL, count_time, NULL);
            H5Sselect_hyperslab (dataspace_outct_id, H5S_SELECT_SET, offset_time, NULL, count_time, NULL);

            error = H5Dread (dset_rt_id, H5T_NATIVE_REAL, memoryspace_meta_id, dataspace_rt_id, H5P_DEFAULT, (void*)&real_time);
            error = H5Dread (dset_lt_id, H5T_NATIVE_REAL, memoryspace_meta_id, dataspace_lt_id, H5P_DEFAULT, (void*)&live_time);
            error = H5Dread (dset_incnt_id, H5T_NATIVE_REAL, memoryspace_meta_id, dataspace_inct_id, H5P_DEFAULT, (void*)&in_cnt);
            error = H5Dread (dset_outcnt_id, H5T_NATIVE_REAL, memoryspace_meta_id, dataspace_outct_id, H5P_DEFAULT, (void*)&out_cnt);

            spectra->elapsed_lifetime(live_time);
            spectra->elapsed_realtime(real_time);
            spectra->input_counts(in_cnt);
            spectra->output_counts(out_cnt);
        }
    }

    H5Dclose(dset_id);
    H5Dclose(dset_incnt_id);
    H5Dclose(dset_outcnt_id);
    H5Dclose(dset_rt_id);
    H5Dclose(dset_lt_id);
    H5Sclose(memoryspace_meta_id);
    H5Sclose(memoryspace_id);
    H5Sclose(dataspace_lt_id);
    H5Sclose(dataspace_rt_id);
    H5Sclose(dataspace_inct_id);
    H5Sclose(dataspace_outct_id);
    H5Sclose(dataspace_id);
    H5Gclose(spec_grp_id);
    H5Fclose(file_id);
    */
    return true;
}

bool MapsH5Model::_load_version_10(hid_t maps_grp_id)
{
    /*
    if (!_load_quantification_10(maps_grp_id))
        return false;

    if (!_load_scalers_10(maps_grp_id))
        return false;

    if (!_load_scan_10(maps_grp_id))
        return false;

    if (!_load_spectra_10(maps_grp_id))
        return false;

    if (!_load_counts_10(maps_grp_id))
        return false;
    */
    return true;
    /*
    for (int i=0; i < rank; i++)
    {
        logit<<"dims ["<<i<<"] ="<<dims_in[i]<< std::endl;
        offset[i] = 0;
        count[i] = dims_in[i];
    }

    if(row_idx_end < row_idx_start)
    {
        row_idx_end = dims_in[1];
    }

    if(col_idx_end < col_idx_start)
    {
        col_idx_end = dims_in[2];
    }

    spectra_volume->resize(dims_in[1], dims_in[2], dims_in[0]);

    //buffer = new real_t [dims_in[0] * dims_in[2]]; // cols x spectra_size
    count[0] = dims_in[0];
    count[1] = 1;
    count[2] = 1;

    memoryspace_id = H5Screate_simple(3, count, NULL);
    memoryspace_meta_id = H5Screate_simple(2, count_time, NULL);
    H5Sselect_hyperslab (memoryspace_id, H5S_SELECT_SET, offset, NULL, count, NULL);
    H5Sselect_hyperslab (memoryspace_meta_id, H5S_SELECT_SET, offset_time, NULL, count_time, NULL);

    real_t live_time = 1.0;
    real_t real_time = 1.0;
    real_t in_cnt = 1.0;
    real_t out_cnt = 1.0;

    //offset[1] = row;

    for(size_t row=row_idx_start; row < row_idx_end; row++)
    {
        offset[1] = row;
        offset_time[0] = row;
        for(size_t col=col_idx_start; col < col_idx_end; col++)
        {
            data_struct::xrf::Spectra *spectra = &((*spectra_volume)[row][col]);
            offset[2] = col;
            offset_time[1] = col;
            H5Sselect_hyperslab (dataspace_id, H5S_SELECT_SET, offset, NULL, count, NULL);

            error = H5Dread (dset_id, H5T_NATIVE_REAL, memoryspace_id, dataspace_id, H5P_DEFAULT, (void*)&(*spectra)[0]);

            H5Sselect_hyperslab (dataspace_lt_id, H5S_SELECT_SET, offset_time, NULL, count_time, NULL);
            H5Sselect_hyperslab (dataspace_rt_id, H5S_SELECT_SET, offset_time, NULL, count_time, NULL);
            H5Sselect_hyperslab (dataspace_inct_id, H5S_SELECT_SET, offset_time, NULL, count_time, NULL);
            H5Sselect_hyperslab (dataspace_outct_id, H5S_SELECT_SET, offset_time, NULL, count_time, NULL);

            error = H5Dread (dset_rt_id, H5T_NATIVE_REAL, memoryspace_meta_id, dataspace_rt_id, H5P_DEFAULT, (void*)&real_time);
            error = H5Dread (dset_lt_id, H5T_NATIVE_REAL, memoryspace_meta_id, dataspace_lt_id, H5P_DEFAULT, (void*)&live_time);
            error = H5Dread (dset_incnt_id, H5T_NATIVE_REAL, memoryspace_meta_id, dataspace_inct_id, H5P_DEFAULT, (void*)&in_cnt);
            error = H5Dread (dset_outcnt_id, H5T_NATIVE_REAL, memoryspace_meta_id, dataspace_outct_id, H5P_DEFAULT, (void*)&out_cnt);

            spectra->elapsed_lifetime(live_time);
            spectra->elapsed_realtime(real_time);
            spectra->input_counts(in_cnt);
            spectra->output_counts(out_cnt);
        }
    }

    H5Dclose(dset_id);
    H5Dclose(dset_incnt_id);
    H5Dclose(dset_outcnt_id);
    H5Dclose(dset_rt_id);
    H5Dclose(dset_lt_id);
    H5Sclose(memoryspace_meta_id);
    H5Sclose(memoryspace_id);
    H5Sclose(dataspace_lt_id);
    H5Sclose(dataspace_rt_id);
    H5Sclose(dataspace_inct_id);
    H5Sclose(dataspace_outct_id);
    H5Sclose(dataspace_id);
    H5Gclose(spec_grp_id);
    H5Fclose(file_id);
    */
}
