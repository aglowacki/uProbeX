/*-----------------------------------------------------------------------------
 * Copyright (c) 2012, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <mvc/MapsH5Model.h>
#include <QDebug>

/*----------------src/mvc/MapsH5Model.cpp \-----------------------------------------------------------*/

MapsH5Model::MapsH5Model() : QObject()
{

    _filepath = "";
    _is_fully_loaded = false;

    _loaded_scalers = false;
    _loaded_quantification = false;
    _loaded_scan = false;
    _loaded_integrated_spectra = false;
    _loaded_counts = false;

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

std::string MapsH5Model::_analysis_enum_to_str(int val)
{
    if(val == data_struct::ROI)
    {
        return "ROI";
    }
    else if(val == data_struct::GAUSS_TAILS)
    {
        return "Parameter Fitting";
    }
    else if(val == data_struct::GAUSS_MATRIX)
    {
        return "Matrix Fitting";
    }
    else if(val == data_struct::SVD)
    {
        return "SVD";
    }
    else if(val == data_struct::NNLS)
    {
        return "NNLS";
    }
    return "";
}

/*---------------------------------------------------------------------------*/

void MapsH5Model::initialize_from_stream_block(data_struct::Stream_Block* block)
{
    clear_analyzed_counts();

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
                    xrf_counts->at(itr2.first)(block->row(), block->col()) = itr2.second;
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

    //return io::file::HDF5_IO::inst()->_load_integrated_spectra_analyzed_h5(file_id, &_integrated_spectra);
    return io::file::HDF5_IO::inst()->load_integrated_spectra_analyzed_h5(_filepath.toStdString(), -1, &_integrated_spectra);

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

    data_struct::Fit_Count_Dict* xrf_counts = new data_struct::Fit_Count_Dict();
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
        xrf_counts->emplace(std::pair<std::string,EMatrixF>(el_name, EMatrixF() ));
        xrf_counts->at(el_name).resize(count[1], count[2]);

        H5Sselect_hyperslab (counts_dspace_id, H5S_SELECT_SET, offset, NULL, count, NULL);
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

