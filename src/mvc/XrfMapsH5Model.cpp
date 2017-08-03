/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/XrfMapsH5Model.h>
#include <QDebug>
#include <hdf5.h>

/*----------------src/mvc/XrfMapsH5Model.cpp \-----------------------------------------------------------*/

XrfMapsH5Model::XrfMapsH5Model()
{

    _is_loaded = false;

}

/*---------------------------------------------------------------------------*/

XrfMapsH5Model::~XrfMapsH5Model()
{

}

/*---------------------------------------------------------------------------*/

bool XrfMapsH5Model::load(QString filepath)
{
    try
    {
        //_is_loaded = ERROR_LOADING;
        //std::chrono::time_point<std::chrono::system_clock> start, end;
        //start = std::chrono::system_clock::now();
/*
        logit<< path <<" detector : "<<detector_num<<std::endl;

        hid_t    file_id, dset_id, dataspace_id, spec_grp_id, memoryspace_id, memoryspace_meta_id, dset_incnt_id, dset_outcnt_id, dset_rt_id, dset_lt_id;
        hid_t    dataspace_lt_id, dataspace_rt_id, dataspace_inct_id, dataspace_outct_id;
        herr_t   error;
        hsize_t dims_in[3] = {0,0,0};
        hsize_t offset[3] = {0,0,0};
        hsize_t count[3] = {1,1,1};
        hsize_t offset_time[2] = {0,0};
        hsize_t count_time[2] = {1,1};

        file_id = H5Fopen(filepath.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
        if(file_id < 0)
        {
            logit<<"Error opening file "<<filepath<<std::endl;
            return false;
        }

        spec_grp_id = H5Gopen(file_id, "/MAPS/Spectra", H5P_DEFAULT);
        if(spec_grp_id < 0)
        {
            H5Fclose(file_id);
            logit<<"Error opening group /MAPS/Spectra"<<std::endl;
            return false;
        }

        logit<<"pre open dset "<<std::endl;
        dset_id = H5Dopen2(spec_grp_id, "mca_arr", H5P_DEFAULT);
        if(dset_id < 0)
        {
            H5Gclose(spec_grp_id);
            H5Fclose(file_id);
            logit<<"Error opening dataset /MAPS/Spectra/mca_arr"<<std::endl;
            return false;
        }
        dataspace_id = H5Dget_space(dset_id);

        dset_lt_id = H5Dopen2(spec_grp_id, "Elapsed_Livetime", H5P_DEFAULT);
        if(dset_lt_id < 0)
        {
            H5Dclose(dset_id);
            H5Sclose(dataspace_id);
            H5Gclose(spec_grp_id);
            H5Fclose(file_id);
            logit<<"Error opening dataset /MAPS/Spectra/Elapsed_Livetime"<<std::endl;
            return false;
        }
        dataspace_lt_id = H5Dget_space(dset_lt_id);

        dset_rt_id = H5Dopen2(spec_grp_id, "Elapsed_Realtime", H5P_DEFAULT);
        if(dset_rt_id < 0)
        {
            H5Dclose(dset_id);
            H5Sclose(dataspace_id);
            H5Dclose(dset_lt_id);
            H5Sclose(dataspace_lt_id);
            H5Gclose(spec_grp_id);
            H5Fclose(file_id);
            logit<<"Error opening dataset /MAPS/Spectra/Elapsed_Realtime"<<std::endl;
            return false;
        }
        dataspace_rt_id = H5Dget_space(dset_rt_id);

        dset_incnt_id = H5Dopen2(spec_grp_id, "Input_Counts", H5P_DEFAULT);
        if(dset_incnt_id < 0)
        {
            H5Dclose(dset_id);
            H5Sclose(dataspace_id);
            H5Dclose(dset_lt_id);
            H5Sclose(dataspace_lt_id);
            H5Dclose(dset_rt_id);
            H5Sclose(dataspace_rt_id);
            H5Gclose(spec_grp_id);
            H5Fclose(file_id);
            logit<<"Error opening dataset /MAPS/Spectra/Input_Counts"<<std::endl;
            return false;
        }
        dataspace_inct_id = H5Dget_space(dset_incnt_id);

        dset_outcnt_id = H5Dopen2(spec_grp_id, "Output_Counts", H5P_DEFAULT);
        if(dset_outcnt_id < 0)
        {
            H5Dclose(dset_id);
            H5Sclose(dataspace_id);
            H5Dclose(dset_lt_id);
            H5Sclose(dataspace_lt_id);
            H5Dclose(dset_rt_id);
            H5Sclose(dataspace_rt_id);
            H5Dclose(dset_incnt_id);
            H5Sclose(dataspace_inct_id);
            H5Gclose(spec_grp_id);
            H5Fclose(file_id);
            logit<<"Error opening dataset /MAPS/Spectra/Ouput_Counts"<<std::endl;
            return false;
        }
        dataspace_outct_id = H5Dget_space(dset_outcnt_id);


        int rank = H5Sget_simple_extent_ndims(dataspace_id);
        if (rank != 3)
        {
            H5Dclose(dset_id);
            H5Sclose(dataspace_id);
            H5Dclose(dset_lt_id);
            H5Sclose(dataspace_lt_id);
            H5Dclose(dset_rt_id);
            H5Sclose(dataspace_rt_id);
            H5Dclose(dset_incnt_id);
            H5Sclose(dataspace_inct_id);
            H5Dclose(dset_outcnt_id);
            H5Sclose(dataspace_outct_id);
            H5Gclose(spec_grp_id);
            H5Fclose(file_id);
            logit<<"Dataset /MAPS/Spectra/mca_arr  rank != 3. rank = "<<rank<<". Can't load dataset. returning"<<std::endl;
            return false;
            //throw exception ("Dataset is not a volume");
        }
        logit<<"rank = "<<rank<< std::endl;
        unsigned int status_n = H5Sget_simple_extent_dims(dataspace_id, &dims_in[0], NULL);
        if(status_n < 0)
        {
            H5Dclose(dset_id);
            H5Sclose(dataspace_id);
            H5Dclose(dset_lt_id);
            H5Sclose(dataspace_lt_id);
            H5Dclose(dset_rt_id);
            H5Sclose(dataspace_rt_id);
            H5Dclose(dset_incnt_id);
            H5Sclose(dataspace_inct_id);
            H5Dclose(dset_outcnt_id);
            H5Sclose(dataspace_outct_id);
            H5Gclose(spec_grp_id);
            H5Fclose(file_id);
            logit<<"Error getting dataset dims for /MAPS/Spectra/mca_arr"<<std::endl;
            return false;
        }

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

        //end = std::chrono::system_clock::now();
        //std::chrono::duration<double> elapsed_seconds = end-start;
        //std::time_t end_time = std::chrono::system_clock::to_time_t(end);

        logit << "elapsed time: " << elapsed_seconds.count() << "s"<<std::endl;
        */
    }
    catch (std::string& s)
    {
        throw s;
    }
    catch (...)
    {
        throw std::string("Failed to open Maps Analyzed dataset!");
    }

    _is_loaded = true;
    return _is_loaded;
}
